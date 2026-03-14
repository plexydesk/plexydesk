#include "canvas_context.hpp"
#include <cstdio>
#include <cstring>
#include <cmath>

namespace plexy::ui {



static CanvasContext* g_active_context = nullptr;

const char* string_table_get(StringId id) {
    if (!g_active_context) return "";
    return g_active_context->get_string(id);
}

void string_table_update(StringId id, const char* text) {
    if (g_active_context) {
        g_active_context->update_string(id, text);
    }
}

void store_glyph_run_global(GlyphRun* run) {
    if (g_active_context) {
        g_active_context->store_glyph_run(run);
    }
}

float get_scale_factor_global() {
    return g_active_context ? g_active_context->scale_factor() : 1.0f;
}



void CanvasContext::create_quad_vao() {
    float v[] = {
        -1.f,-1.f, 0.f,0.f,  1.f,-1.f, 1.f,0.f,  1.f, 1.f, 1.f,1.f,
        -1.f,-1.f, 0.f,0.f,  1.f, 1.f, 1.f,1.f, -1.f, 1.f, 0.f,1.f
    };
    glGenVertexArrays(1, &quad_vao_);
    glGenBuffers(1, &quad_vbo_);
    glBindVertexArray(quad_vao_);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(v), v, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
    glBindVertexArray(0);
}

void CanvasContext::destroy_quad_vao() {
    if (quad_vbo_) { glDeleteBuffers(1, &quad_vbo_); quad_vbo_ = 0; }
    if (quad_vao_) { glDeleteVertexArrays(1, &quad_vao_); quad_vao_ = 0; }
}



void CanvasContext::create_fbo(int w, int h) {
    glGenFramebuffers(1, &fbo_);
    glGenTextures(1, &fbo_color_tex_);
    glGenRenderbuffers(1, &fbo_depth_rbo_);

    glBindTexture(GL_TEXTURE_2D, fbo_color_tex_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindRenderbuffer(GL_RENDERBUFFER, fbo_depth_rbo_);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_color_tex_, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fbo_depth_rbo_);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        fprintf(stderr, "[Canvas] FBO incomplete: 0x%X\n", status);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void CanvasContext::destroy_fbo() {
    if (fbo_color_tex_) { glDeleteTextures(1, &fbo_color_tex_); fbo_color_tex_ = 0; }
    if (fbo_depth_rbo_) { glDeleteRenderbuffers(1, &fbo_depth_rbo_); fbo_depth_rbo_ = 0; }
    if (fbo_) { glDeleteFramebuffers(1, &fbo_); fbo_ = 0; }
}



bool CanvasContext::init(int width, int height, CanvasTarget target, const char* font_path) {
    width_ = width;
    height_ = height;
    target_ = target;
    font_path_ = font_path;  

    frame_arena_.init(512 * 1024);
    widget_pool_.init();
    grid_.init(width, height);

    
    root_ = widget_pool_.alloc();
    *root_ = Widget{};
    root_->id = 1;
    root_->type = WidgetType::Panel;
    root_->flags = WIDGET_VISIBLE | WIDGET_ENABLED;
    root_->layout.direction = FlexDirection::Column;
    root_->layout.width = (float)width;
    root_->layout.height = (float)height;
    root_->corner_radius = 0;  

    g_active_context = this;

    
    
    return true;
}

void CanvasContext::init_gl() {
    if (gl_initialized_) return;
    
    
    if (font_path_) {
        atlas_.init(font_path_, 14);
    }
    
    renderer_.init(width_, height_);
    create_quad_vao();

    if (target_ == CanvasTarget::FBO) {
        create_fbo(width_, height_);
    }
    
    gl_initialized_ = true;
    printf("[Canvas] GL resources initialized (%dx%d, target=%s)\n",
           width_, height_, target_ == CanvasTarget::FBO ? "FBO" : "GBM");
}

void CanvasContext::shutdown() {
    root_ = nullptr;
    widget_pool_.shutdown();

    grid_.shutdown();
    renderer_.shutdown();
    atlas_.shutdown();
    frame_arena_.shutdown();

    destroy_quad_vao();
    destroy_fbo();

    
    if (gpu_) {
        
    }

    if (g_active_context == this) g_active_context = nullptr;
}

void CanvasContext::resize(int width, int height) {
    width_ = width;
    height_ = height;
    renderer_.resize(width, height);
    grid_.shutdown();
    grid_.init(width, height);

    if (target_ == CanvasTarget::FBO) {
        destroy_fbo();
        create_fbo(width, height);
    } else if (target_ == CanvasTarget::GBM) {
        resize_gpu(width, height);
    }

    if (root_) {
        root_->layout.width = (float)width;
        root_->layout.height = (float)height;
    }
    layout_dirty_ = true;
}



void CanvasContext::begin_frame(float dt) {
    
    bool anim_active = anim_.tick(dt);
    if (anim_active) {
        render_dirty_ = true;
        if (root_) {
            
            grid_.rebuild(root_);
        }
    }

    
    bool had_events = !events_.empty();
    bool state_changed = event_proc_.process_all(&events_, &grid_, &focus_, root_);

    if (had_events && state_changed) {
        render_dirty_ = true;
    }

    
    if (layout_dirty_ && root_) {
        layout_.solve(root_, (float)width_, (float)height_);
        grid_.rebuild(root_);
        layout_dirty_ = false;
        render_dirty_ = true;
    }

    
    if (render_dirty_ || anim_.has_active()) {
        if (target_ == CanvasTarget::FBO && fbo_) {
            glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
            glViewport(0, 0, width_, height_);
            glClearColor(0, 0, 0, 0);
            glClear(GL_COLOR_BUFFER_BIT);
        } else if (target_ == CanvasTarget::GBM) {
            glViewport(0, 0, width_, height_);
            glClearColor(0, 0, 0, 0);
            glClear(GL_COLOR_BUFFER_BIT);
        }
    }
}

void CanvasContext::end_frame() {
    if (!root_) {
        if (target_ == CanvasTarget::FBO) {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
        return;
    }

    
    theme_ = dark_mode_ ? &kPlexyDarkTheme : &kPlexyLightTheme;

    
    frame_arena_.reset();

    
    PaintList paint_list;
    paint_list.init(&frame_arena_, 512);

    uint32_t next_run_id = 1;
    glyph_run_count_ = 0;
    glyph_runs_ = frame_arena_.alloc_array<GlyphRun>(MAX_GLYPH_RUNS);

    
    paint_widget_tree(root_, 0, 0, &paint_list, &next_run_id);

    
    for (const PathStrokeLine& line : custom_stroke_lines_) {
        paint_list.push_line(line.x1, line.y1, line.x2, line.y2, line.width, line.color);
    }

    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, atlas_.texture());

    
    renderer_.render(paint_list, 0, blur_texture_,
                     quad_vao_, dark_mode_, glass_alpha_,
                     &atlas_, glyph_runs_, glyph_run_count_);

    render_dirty_ = false;

    
    if (target_ == CanvasTarget::FBO) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    
}

void CanvasContext::paint_widget_tree(Widget* w, float parent_x, float parent_y,
                                      PaintList* list, uint32_t* next_run_id) {
    if (!w || !w->visible()) return;

    float ax = 0.0f, ay = 0.0f;
    widget_effective_rect(w, &ax, &ay, nullptr, nullptr);

    PaintFn paint_fn = get_paint_fn(w->type);

    if (paint_fn) {
        paint_fn(&frame_arena_, list, w, theme_, ax, ay, &atlas_, next_run_id);
    }

    
    for (Widget* c = w->first_child; c; c = c->next_sibling) {
        if (!c->is_drawer)
            paint_widget_tree(c, ax, ay, list, next_run_id);
    }
    for (Widget* c = w->first_child; c; c = c->next_sibling) {
        if (c->is_drawer)
            paint_widget_tree(c, ax, ay, list, next_run_id);
    }
}



void CanvasContext::inject_mouse_move(float x, float y) {
    UIEvent ev{}; ev.type = UIEventType::MouseMove; ev.x = x; ev.y = y;
    events_.push(ev);
}

void CanvasContext::inject_mouse_button(int button, bool pressed, float x, float y) {
    UIEvent ev{}; ev.type = UIEventType::MouseButton;
    ev.button = button; ev.pressed = pressed; ev.x = x; ev.y = y;
    events_.push(ev);
}

void CanvasContext::inject_mouse_scroll(float dx, float dy, float x, float y) {
    UIEvent ev{}; ev.type = UIEventType::MouseScroll;
    ev.scroll_dx = dx; ev.scroll_dy = dy; ev.x = x; ev.y = y;
    events_.push(ev);
}

void CanvasContext::inject_key(uint32_t keycode, uint32_t mods, bool pressed) {
    UIEvent ev{}; ev.type = pressed ? UIEventType::KeyDown : UIEventType::KeyUp;
    ev.keycode = keycode; ev.modifiers = mods;
    events_.push(ev);
}

void CanvasContext::inject_char(uint32_t codepoint) {
    UIEvent ev{}; ev.type = UIEventType::CharInput; ev.character = codepoint;
    events_.push(ev);
}



bool CanvasContext::hit_test(float x, float y) {
    return grid_.hit_test(x, y) != nullptr;
}

bool CanvasContext::needs_render() const {
    return render_dirty_ || layout_dirty_ || anim_.has_active();
}



void CanvasContext::path_begin() {
    active_path_.clear();
}

void CanvasContext::path_move_to(float x, float y) {
    PathCmd cmd{};
    cmd.type = PathCmd::Type::Move;
    cmd.p1 = {x, y};
    active_path_.push_back(cmd);
}

void CanvasContext::path_line_to(float x, float y) {
    PathCmd cmd{};
    cmd.type = PathCmd::Type::Line;
    cmd.p1 = {x, y};
    active_path_.push_back(cmd);
}

void CanvasContext::path_quadratic_to(float cx, float cy, float x, float y) {
    PathCmd cmd{};
    cmd.type = PathCmd::Type::Quadratic;
    cmd.p1 = {cx, cy};
    cmd.p2 = {x, y};
    active_path_.push_back(cmd);
}

void CanvasContext::path_cubic_to(float cp1x, float cp1y, float cp2x, float cp2y, float x, float y) {
    PathCmd cmd{};
    cmd.type = PathCmd::Type::Cubic;
    cmd.p1 = {cp1x, cp1y};
    cmd.p2 = {cp2x, cp2y};
    cmd.p3 = {x, y};
    active_path_.push_back(cmd);
}

void CanvasContext::path_close() {
    PathCmd cmd{};
    cmd.type = PathCmd::Type::Close;
    active_path_.push_back(cmd);
}

void CanvasContext::set_path_stroke_color(float r, float g, float b, float a) {
    path_stroke_color_ = {r, g, b, a};
}

void CanvasContext::set_path_stroke_width(float width) {
    path_stroke_width_ = (width > 0.0f) ? width : 1.0f;
}

void CanvasContext::append_line_segment(float x1, float y1, float x2, float y2) {
    PathStrokeLine line{};
    line.x1 = x1;
    line.y1 = y1;
    line.x2 = x2;
    line.y2 = y2;
    line.width = path_stroke_width_;
    line.color = path_stroke_color_;
    custom_stroke_lines_.push_back(line);
}

void CanvasContext::path_stroke() {
    if (active_path_.empty()) return;

    Vec2 current = {0.0f, 0.0f};
    Vec2 subpath_start = {0.0f, 0.0f};
    bool has_current = false;
    bool has_subpath_start = false;

    for (const PathCmd& cmd : active_path_) {
        switch (cmd.type) {
            case PathCmd::Type::Move: {
                current = cmd.p1;
                subpath_start = cmd.p1;
                has_current = true;
                has_subpath_start = true;
                break;
            }
            case PathCmd::Type::Line: {
                if (!has_current) {
                    current = cmd.p1;
                    subpath_start = cmd.p1;
                    has_current = true;
                    has_subpath_start = true;
                    break;
                }
                append_line_segment(current.x, current.y, cmd.p1.x, cmd.p1.y);
                current = cmd.p1;
                break;
            }
            case PathCmd::Type::Quadratic: {
                if (!has_current) {
                    current = cmd.p2;
                    subpath_start = cmd.p2;
                    has_current = true;
                    has_subpath_start = true;
                    break;
                }
                const Vec2 p0 = current;
                const Vec2 p1 = cmd.p1;
                const Vec2 p2 = cmd.p2;
                const float approx =
                    std::hypot(p1.x - p0.x, p1.y - p0.y) +
                    std::hypot(p2.x - p1.x, p2.y - p1.y);
                int segments = (int)(approx / 12.0f);
                if (segments < 8) segments = 8;
                if (segments > 64) segments = 64;

                Vec2 prev = p0;
                for (int i = 1; i <= segments; ++i) {
                    const float t = (float)i / (float)segments;
                    const float mt = 1.0f - t;
                    Vec2 pt{
                        mt * mt * p0.x + 2.0f * mt * t * p1.x + t * t * p2.x,
                        mt * mt * p0.y + 2.0f * mt * t * p1.y + t * t * p2.y
                    };
                    append_line_segment(prev.x, prev.y, pt.x, pt.y);
                    prev = pt;
                }
                current = p2;
                break;
            }
            case PathCmd::Type::Cubic: {
                if (!has_current) {
                    current = cmd.p3;
                    subpath_start = cmd.p3;
                    has_current = true;
                    has_subpath_start = true;
                    break;
                }
                const Vec2 p0 = current;
                const Vec2 p1 = cmd.p1;
                const Vec2 p2 = cmd.p2;
                const Vec2 p3 = cmd.p3;
                const float approx =
                    std::hypot(p1.x - p0.x, p1.y - p0.y) +
                    std::hypot(p2.x - p1.x, p2.y - p1.y) +
                    std::hypot(p3.x - p2.x, p3.y - p2.y);
                int segments = (int)(approx / 12.0f);
                if (segments < 12) segments = 12;
                if (segments > 96) segments = 96;

                Vec2 prev = p0;
                for (int i = 1; i <= segments; ++i) {
                    const float t = (float)i / (float)segments;
                    const float mt = 1.0f - t;
                    Vec2 pt{
                        mt * mt * mt * p0.x +
                        3.0f * mt * mt * t * p1.x +
                        3.0f * mt * t * t * p2.x +
                        t * t * t * p3.x,
                        mt * mt * mt * p0.y +
                        3.0f * mt * mt * t * p1.y +
                        3.0f * mt * t * t * p2.y +
                        t * t * t * p3.y
                    };
                    append_line_segment(prev.x, prev.y, pt.x, pt.y);
                    prev = pt;
                }
                current = p3;
                break;
            }
            case PathCmd::Type::Close: {
                if (has_current && has_subpath_start) {
                    append_line_segment(current.x, current.y, subpath_start.x, subpath_start.y);
                    current = subpath_start;
                }
                break;
            }
        }
    }

    render_dirty_ = true;
}

void CanvasContext::clear_paths() {
    active_path_.clear();
    custom_stroke_lines_.clear();
    render_dirty_ = true;
}



Widget* CanvasContext::create_widget(WidgetType type, Widget* parent) {
    Widget* w = widget_pool_.alloc();
    if (!w) return nullptr;

    static uint32_t next_id = 2;
    *w = Widget{};
    w->id = next_id++;
    w->type = type;
    w->flags = WIDGET_VISIBLE | WIDGET_ENABLED | WIDGET_DIRTY;

    if (!parent) parent = root_;

    w->parent = parent;
    if (parent) {
        if (!parent->first_child) {
            parent->first_child = w;
        } else {
            Widget* last = parent->first_child;
            while (last->next_sibling) last = last->next_sibling;
            last->next_sibling = w;
        }
    }

    layout_dirty_ = true;
    return w;
}

void CanvasContext::unlink_widget(Widget* widget) {
    Widget* parent = widget->parent;
    if (!parent) return;

    if (parent->first_child == widget) {
        parent->first_child = widget->next_sibling;
    } else {
        Widget* prev = parent->first_child;
        while (prev && prev->next_sibling != widget) prev = prev->next_sibling;
        if (prev) prev->next_sibling = widget->next_sibling;
    }
    widget->parent = nullptr;
    widget->next_sibling = nullptr;
}

void CanvasContext::destroy_widget(Widget* widget) {
    if (!widget || widget == root_) return;

    while (widget->first_child) {
        destroy_widget(widget->first_child);
    }

    if (focus_.focused() == widget) focus_.clear_focus();

    unlink_widget(widget);
    widget_pool_.free(widget);
    layout_dirty_ = true;
}



StringId CanvasContext::intern_string(const char* text) {
    if (!text) return {};
    uint32_t len = (uint32_t)strlen(text);
    if (string_pool_used_ + len + 1 > STRING_POOL_SIZE) return {};
    uint32_t id = next_string_id_;
    if (id >= MAX_STRINGS) return {};

    string_entries_[id].offset = string_pool_used_;
    string_entries_[id].length = len;
    string_entries_[id].used = true;

    memcpy(string_pool_ + string_pool_used_, text, len + 1);
    string_pool_used_ += len + 1;
    next_string_id_++;

    return {id};
}

void CanvasContext::update_string(StringId id, const char* text) {
    if (!id.valid() || id.id >= MAX_STRINGS || !string_entries_[id.id].used) return;
    uint32_t len = (uint32_t)strlen(text);
    if (len <= string_entries_[id.id].length) {
        memcpy(string_pool_ + string_entries_[id.id].offset, text, len + 1);
        string_entries_[id.id].length = len;
    } else {
        if (string_pool_used_ + len + 1 > STRING_POOL_SIZE) return;
        string_entries_[id.id].offset = string_pool_used_;
        string_entries_[id.id].length = len;
        memcpy(string_pool_ + string_pool_used_, text, len + 1);
        string_pool_used_ += len + 1;
    }
}

const char* CanvasContext::get_string(StringId id) const {
    if (!id.valid() || id.id >= MAX_STRINGS || !string_entries_[id.id].used) return "";
    return string_pool_ + string_entries_[id.id].offset;
}

void CanvasContext::store_glyph_run(GlyphRun* run) {
    if (!run || glyph_run_count_ >= MAX_GLYPH_RUNS) return;
    glyph_runs_[glyph_run_count_++] = *run;
}



} 
