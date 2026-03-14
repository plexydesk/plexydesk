

#include "canvas_context.hpp"
#include "plexy_canvas.h"

#include <cstdlib>
#include <cstdio>

using namespace plexy::ui;



static inline CanvasContext* ctx(PlexyCanvas* c) {
    return reinterpret_cast<CanvasContext*>(c);
}

static inline PlexyCanvas* wrap(CanvasContext* c) {
    return reinterpret_cast<PlexyCanvas*>(c);
}

static Widget* find_widget(CanvasContext* c, uint32_t id) {
    
    struct StackEntry { Widget* w; };
    StackEntry stack[512];
    int sp = 0;

    Widget* root = c->root();
    if (!root) return nullptr;
    if (root->id == id) return root;

    stack[sp++] = {root};
    while (sp > 0) {
        Widget* w = stack[--sp].w;
        if (w->id == id) return w;
        for (Widget* child = w->first_child; child; child = child->next_sibling) {
            if (sp < 512) stack[sp++] = {child};
        }
    }
    return nullptr;
}

static WidgetType map_widget_type(int type) {
    switch (type) {
        case PLEXY_WIDGET_LABEL:         return WidgetType::Label;
        case PLEXY_WIDGET_BUTTON:        return WidgetType::Button;
        case PLEXY_WIDGET_ICON_BUTTON:   return WidgetType::IconButton;
        case PLEXY_WIDGET_TOGGLE_BUTTON: return WidgetType::ToggleButton;
        case PLEXY_WIDGET_TEXT_INPUT:     return WidgetType::TextInput;
        case PLEXY_WIDGET_TEXT_AREA:      return WidgetType::TextArea;
        case PLEXY_WIDGET_CHECKBOX:       return WidgetType::Checkbox;
        case PLEXY_WIDGET_RADIO_BUTTON:  return WidgetType::RadioButton;
        case PLEXY_WIDGET_SWITCH:        return WidgetType::Switch;
        case PLEXY_WIDGET_SLIDER:        return WidgetType::Slider;
        case PLEXY_WIDGET_PROGRESS_BAR:  return WidgetType::ProgressBar;
        case PLEXY_WIDGET_PANEL:         return WidgetType::Panel;
        case PLEXY_WIDGET_SCROLL_VIEW:   return WidgetType::ScrollView;
        case PLEXY_WIDGET_LIST_VIEW:     return WidgetType::List;
        case PLEXY_WIDGET_ICON_VIEW:     return WidgetType::IconView;
        case PLEXY_WIDGET_SEPARATOR:     return WidgetType::Separator;
        case PLEXY_WIDGET_SPACER:        return WidgetType::Spacer;
        case PLEXY_WIDGET_ROW:           return WidgetType::Panel;  
        case PLEXY_WIDGET_COLUMN:        return WidgetType::Panel;  
        case PLEXY_WIDGET_TOOLBAR:       return WidgetType::Toolbar;
        case PLEXY_WIDGET_TOOLBAR_ITEM:  return WidgetType::Button;  
        case PLEXY_WIDGET_SIDEBAR:       return WidgetType::Panel;   
        case PLEXY_WIDGET_SIDEBAR_ITEM:  return WidgetType::Button;  
        case PLEXY_WIDGET_SEARCH_FIELD:  return WidgetType::TextInput;
        case PLEXY_WIDGET_BREADCRUMB:    return WidgetType::Label;
        case PLEXY_WIDGET_SPLIT_VIEW:    return WidgetType::SplitView;
        case PLEXY_WIDGET_STATUS_BAR:    return WidgetType::StatusBar;
        default:                          return WidgetType::Panel;
    }
}



PlexyCanvas* plexy_canvas_create(int width, int height, PlexyCanvasTarget target) {
    CanvasContext* c = new (std::nothrow) CanvasContext();
    if (!c) return nullptr;

    CanvasTarget t = (target == PLEXY_CANVAS_TARGET_GBM) ? CanvasTarget::GBM : CanvasTarget::FBO;
    if (!c->init(width, height, t, nullptr)) {
        delete c;
        return nullptr;
    }

    return wrap(c);
}

void plexy_canvas_destroy(PlexyCanvas* canvas) {
    if (!canvas) return;
    ctx(canvas)->shutdown();
    delete ctx(canvas);
}

void plexy_canvas_resize(PlexyCanvas* canvas, int width, int height) {
    if (canvas) ctx(canvas)->resize(width, height);
}

void plexy_canvas_set_font(PlexyCanvas* canvas, const char* font_path) {
    if (canvas && font_path) {
        ctx(canvas)->atlas()->init(font_path, 14);
    }
}



int plexy_canvas_init_gpu(PlexyCanvas* canvas, const char* drm_device) {
    if (!canvas) return -1;
    return ctx(canvas)->init_gpu(drm_device) ? 0 : -1;
}

int plexy_canvas_init_with_egl(PlexyCanvas* canvas, void* egl_display, void* egl_context) {
    if (!canvas) return -1;
    return ctx(canvas)->init_with_egl(egl_display, egl_context) ? 0 : -1;
}



int plexy_canvas_get_dmabuf_fd(PlexyCanvas* canvas) {
    if (!canvas) return -1;
    return ctx(canvas)->get_dmabuf_fd();
}

uint32_t plexy_canvas_get_stride(PlexyCanvas* canvas) {
    if (!canvas) return 0;
    return ctx(canvas)->get_stride();
}

uint32_t plexy_canvas_get_format(PlexyCanvas* canvas) {
    if (!canvas) return 0;
    return ctx(canvas)->get_format();
}

uint64_t plexy_canvas_get_modifier(PlexyCanvas* canvas) {
    if (!canvas) return 0;
    return ctx(canvas)->get_modifier();
}



void plexy_canvas_begin_frame(PlexyCanvas* canvas, float dt) {
    if (canvas) ctx(canvas)->begin_frame(dt);
}

void plexy_canvas_end_frame(PlexyCanvas* canvas) {
    if (canvas) ctx(canvas)->end_frame();
}

void plexy_canvas_swap_buffers(PlexyCanvas* canvas) {
    if (canvas) ctx(canvas)->swap_buffers();
}

uint32_t plexy_canvas_get_texture(PlexyCanvas* canvas) {
    if (!canvas) return 0;
    return ctx(canvas)->get_texture();
}

int plexy_canvas_needs_render(PlexyCanvas* canvas) {
    if (!canvas) return 0;
    return ctx(canvas)->needs_render() ? 1 : 0;
}



void plexy_canvas_set_blur_texture(PlexyCanvas* canvas, uint32_t blur_tex) {
    if (canvas) ctx(canvas)->set_blur_texture(blur_tex);
}



void plexy_canvas_set_dark_mode(PlexyCanvas* canvas, int dark) {
    if (canvas) ctx(canvas)->set_dark_mode(dark != 0);
}

void plexy_canvas_set_glass_alpha(PlexyCanvas* canvas, float alpha) {
    if (canvas) ctx(canvas)->set_glass_alpha(alpha);
}

void plexy_canvas_set_scale_factor(PlexyCanvas* canvas, float scale) {
    if (canvas) ctx(canvas)->set_scale_factor(scale);
}



uint32_t plexy_canvas_create_widget(PlexyCanvas* canvas, int type, uint32_t parent_id) {
    if (!canvas) return 0;
    CanvasContext* c = ctx(canvas);

    Widget* parent = nullptr;
    if (parent_id != 0) {
        parent = find_widget(c, parent_id);
    }

    WidgetType wt = map_widget_type(type);
    Widget* w = c->create_widget(wt, parent);
    if (!w) return 0;

    
    if (type == PLEXY_WIDGET_ROW) {
        w->layout.direction = FlexDirection::Row;
    } else if (type == PLEXY_WIDGET_COLUMN) {
        w->layout.direction = FlexDirection::Column;
    } else if (type == PLEXY_WIDGET_TOOLBAR) {
        w->layout.direction = FlexDirection::Row;
        w->layout.height = 44.0f;
        w->layout.align = AlignItems::Center;
        w->layout.padding[0] = 4; w->layout.padding[1] = 12;
        w->layout.padding[2] = 4; w->layout.padding[3] = 12;
        w->layout.gap = 6;
    } else if (type == PLEXY_WIDGET_SIDEBAR) {
        w->layout.direction = FlexDirection::Column;
        w->layout.width = 220.0f;
        w->layout.padding[0] = 8; w->layout.padding[1] = 8;
        w->layout.padding[2] = 8; w->layout.padding[3] = 8;
        w->layout.gap = 2;
    } else if (type == PLEXY_WIDGET_SIDEBAR_ITEM) {
        w->layout.direction = FlexDirection::Row;
        w->layout.height = 28.0f;
        w->layout.align = AlignItems::Center;
        w->layout.padding[0] = 4; w->layout.padding[1] = 10;
        w->layout.padding[2] = 4; w->layout.padding[3] = 10;
        w->layout.gap = 8;
    } else if (type == PLEXY_WIDGET_TOOLBAR_ITEM) {
        w->layout.direction = FlexDirection::Column;
        w->layout.align = AlignItems::Center;
        w->layout.padding[0] = 4; w->layout.padding[1] = 10;
        w->layout.padding[2] = 4; w->layout.padding[3] = 10;
    } else if (type == PLEXY_WIDGET_SEARCH_FIELD) {
        w->layout.height = 28.0f;
        w->corner_radius = 14.0f;
    } else if (type == PLEXY_WIDGET_SPLIT_VIEW) {
        w->layout.direction = FlexDirection::Row;
        w->layout.flex_grow = 1;
    } else if (type == PLEXY_WIDGET_STATUS_BAR) {
        w->layout.direction = FlexDirection::Row;
        w->layout.height = 24.0f;
        w->layout.align = AlignItems::Center;
        w->layout.padding[0] = 2; w->layout.padding[1] = 12;
        w->layout.padding[2] = 2; w->layout.padding[3] = 12;
    }

    return w->id;
}

void plexy_canvas_destroy_widget(PlexyCanvas* canvas, uint32_t widget_id) {
    if (!canvas || widget_id == 0) return;
    Widget* w = find_widget(ctx(canvas), widget_id);
    if (w) ctx(canvas)->destroy_widget(w);
}

uint32_t plexy_canvas_root(PlexyCanvas* canvas) {
    if (!canvas) return 0;
    Widget* r = ctx(canvas)->root();
    return r ? r->id : 0;
}



void plexy_canvas_set_text(PlexyCanvas* canvas, uint32_t id, const char* text) {
    if (!canvas || !text) return;
    CanvasContext* c = ctx(canvas);
    Widget* w = find_widget(c, id);
    if (!w) return;

    if (w->text.valid()) {
        c->update_string(w->text, text);
    } else {
        w->text = c->intern_string(text);
    }
    w->mark_dirty();
}

void plexy_canvas_set_value(PlexyCanvas* canvas, uint32_t id, float value) {
    if (!canvas) return;
    Widget* w = find_widget(ctx(canvas), id);
    if (w) {
        w->value = value;
        if (w->type == WidgetType::IconView) w->scroll_y = 0;
        w->mark_dirty();
    }
}

void plexy_canvas_set_visible(PlexyCanvas* canvas, uint32_t id, int visible) {
    if (!canvas) return;
    Widget* w = find_widget(ctx(canvas), id);
    if (!w) return;
    if (visible) w->set_flag(WIDGET_VISIBLE);
    else w->clear_flag(WIDGET_VISIBLE);
    w->mark_dirty();
    ctx(canvas)->mark_layout_dirty();
}

void plexy_canvas_set_enabled(PlexyCanvas* canvas, uint32_t id, int enabled) {
    if (!canvas) return;
    Widget* w = find_widget(ctx(canvas), id);
    if (!w) return;
    if (enabled) w->set_flag(WIDGET_ENABLED);
    else w->clear_flag(WIDGET_ENABLED);
    w->mark_dirty();
}

void plexy_canvas_set_readonly(PlexyCanvas* canvas, uint32_t id, int readonly) {
    if (!canvas) return;
    Widget* w = find_widget(ctx(canvas), id);
    if (!w) return;
    if (readonly) w->set_flag(WIDGET_READONLY);
    else w->clear_flag(WIDGET_READONLY);
}

void plexy_canvas_set_checked(PlexyCanvas* canvas, uint32_t id, int checked) {
    if (!canvas) return;
    Widget* w = find_widget(ctx(canvas), id);
    if (!w) return;
    if (checked) w->set_flag(WIDGET_CHECKED);
    else w->clear_flag(WIDGET_CHECKED);
    w->mark_dirty();
}



void plexy_canvas_set_flex_direction(PlexyCanvas* canvas, uint32_t id, int row) {
    if (!canvas) return;
    Widget* w = find_widget(ctx(canvas), id);
    if (w) {
        w->layout.direction = row ? FlexDirection::Row : FlexDirection::Column;
        ctx(canvas)->mark_layout_dirty();
    }
}

void plexy_canvas_set_flex_grow(PlexyCanvas* canvas, uint32_t id, float grow) {
    if (!canvas) return;
    Widget* w = find_widget(ctx(canvas), id);
    if (w) {
        w->layout.flex_grow = grow;
        ctx(canvas)->mark_layout_dirty();
    }
}

void plexy_canvas_set_size(PlexyCanvas* canvas, uint32_t id, float w_val, float h_val) {
    if (!canvas) return;
    Widget* w = find_widget(ctx(canvas), id);
    if (w) {
        w->layout.width = w_val;
        w->layout.height = h_val;
        ctx(canvas)->mark_layout_dirty();
    }
}

void plexy_canvas_set_margin(PlexyCanvas* canvas, uint32_t id,
                              float t, float r, float b, float l) {
    if (!canvas) return;
    Widget* w = find_widget(ctx(canvas), id);
    if (w) {
        w->layout.margin[0] = t;
        w->layout.margin[1] = r;
        w->layout.margin[2] = b;
        w->layout.margin[3] = l;
        ctx(canvas)->mark_layout_dirty();
    }
}

void plexy_canvas_set_padding(PlexyCanvas* canvas, uint32_t id,
                               float t, float r, float b, float l) {
    if (!canvas) return;
    Widget* w = find_widget(ctx(canvas), id);
    if (w) {
        w->layout.padding[0] = t;
        w->layout.padding[1] = r;
        w->layout.padding[2] = b;
        w->layout.padding[3] = l;
        ctx(canvas)->mark_layout_dirty();
    }
}

void plexy_canvas_set_gap(PlexyCanvas* canvas, uint32_t id, float gap) {
    if (!canvas) return;
    Widget* w = find_widget(ctx(canvas), id);
    if (w) {
        w->layout.gap = gap;
        ctx(canvas)->mark_layout_dirty();
    }
}

void plexy_canvas_set_justify(PlexyCanvas* canvas, uint32_t id, int justify) {
    if (!canvas) return;
    Widget* w = find_widget(ctx(canvas), id);
    if (w) {
        w->layout.justify = static_cast<JustifyContent>(justify);
        ctx(canvas)->mark_layout_dirty();
    }
}

void plexy_canvas_set_align(PlexyCanvas* canvas, uint32_t id, int align) {
    if (!canvas) return;
    Widget* w = find_widget(ctx(canvas), id);
    if (w) {
        w->layout.align = static_cast<AlignItems>(align);
        ctx(canvas)->mark_layout_dirty();
    }
}



void plexy_canvas_set_drawer(PlexyCanvas* canvas, uint32_t id, int edge) {
    if (!canvas) return;
    Widget* w = find_widget(ctx(canvas), id);
    if (!w) return;

    w->is_drawer = true;
    switch (edge) {
        case PLEXY_CANVAS_DRAWER_RIGHT:  w->drawer_edge = DrawerEdge::Right; break;
        case PLEXY_CANVAS_DRAWER_TOP:    w->drawer_edge = DrawerEdge::Top; break;
        case PLEXY_CANVAS_DRAWER_BOTTOM: w->drawer_edge = DrawerEdge::Bottom; break;
        case PLEXY_CANVAS_DRAWER_LEFT:
        default:
            w->drawer_edge = DrawerEdge::Left;
            break;
    }
    w->mark_dirty();
    ctx(canvas)->mark_layout_dirty();
}

void plexy_canvas_set_drawer_open(PlexyCanvas* canvas, uint32_t id, int open, int animated) {
    if (!canvas) return;
    CanvasContext* c = ctx(canvas);
    Widget* w = find_widget(c, id);
    if (!w) return;

    if (!w->is_drawer) {
        w->is_drawer = true;
        w->drawer_edge = DrawerEdge::Left;
    }

    const float target = open ? 1.0f : 0.0f;
    if (animated) {
        float duration = (w->drawer_anim_duration > 0.0f) ? w->drawer_anim_duration : 0.22f;
        c->animations()->animate(&w->drawer_open, target, duration, ease_out_cubic, w->id);
    } else {
        c->animations()->cancel(&w->drawer_open);
        w->drawer_open = target;
    }

    w->mark_dirty();
    c->mark_layout_dirty();
}

void plexy_canvas_toggle_drawer(PlexyCanvas* canvas, uint32_t id, int animated) {
    if (!canvas) return;
    Widget* w = find_widget(ctx(canvas), id);
    if (!w) return;
    int open = (w->drawer_open < 0.5f) ? 1 : 0;
    plexy_canvas_set_drawer_open(canvas, id, open, animated);
}

int plexy_canvas_is_drawer_open(PlexyCanvas* canvas, uint32_t id) {
    if (!canvas) return 0;
    Widget* w = find_widget(ctx(canvas), id);
    if (!w || !w->is_drawer) return 0;
    return (w->drawer_open >= 0.999f) ? 1 : 0;
}



void plexy_canvas_set_corner_radius(PlexyCanvas* canvas, uint32_t id, float radius) {
    if (!canvas) return;
    Widget* w = find_widget(ctx(canvas), id);
    if (w) { w->corner_radius = radius; w->mark_dirty(); }
}

void plexy_canvas_set_elevation(PlexyCanvas* canvas, uint32_t id, float elevation) {
    if (!canvas) return;
    Widget* w = find_widget(ctx(canvas), id);
    if (w) { w->elevation = elevation; w->mark_dirty(); }
}

void plexy_canvas_set_fill_color(PlexyCanvas* canvas, uint32_t id,
                                  float r, float g, float b, float a) {
    if (!canvas) return;
    Widget* w = find_widget(ctx(canvas), id);
    if (w) {
        w->material_override = Material::solid(Color::rgba(r, g, b, a));
        w->has_material_override = true;
        w->mark_dirty();
    }
}

void plexy_canvas_set_glass_material(PlexyCanvas* canvas, uint32_t id, float opacity) {
    if (!canvas) return;
    Widget* w = find_widget(ctx(canvas), id);
    if (!w) return;

    float clamped = opacity;
    if (clamped < 0.0f) clamped = 0.0f;
    if (clamped > 1.0f) clamped = 1.0f;

    Material glass = Material::glass(
        22.0f,
        clamped,
        Color::rgba(1.0f, 1.0f, 1.0f, 0.94f),
        0.0f,
        0.42f,
        0.02f
    );
    glass.border_color = Color::rgba(1.0f, 1.0f, 1.0f, 0.86f);
    glass.border_width = 1.2f;
    glass.noise = 0.0f;

    w->material_override = glass;
    w->has_material_override = true;
    w->mark_dirty();
}

void plexy_canvas_set_surface_material(PlexyCanvas* canvas, uint32_t id) {
    if (!canvas) return;
    Widget* w = find_widget(ctx(canvas), id);
    if (!w) return;

    
    w->has_material_override = false;
    w->mark_dirty();
}

void plexy_canvas_set_border(PlexyCanvas* canvas, uint32_t id,
                              float r, float g, float b, float a, float width) {
    if (!canvas) return;
    Widget* w = find_widget(ctx(canvas), id);
    if (!w) return;

    
    if (!w->has_material_override) {
        w->material_override = Material::solid(Color::none());
        w->has_material_override = true;
    }
    w->material_override.border_color = Color::rgba(r, g, b, a);
    w->material_override.border_width = width;
    w->mark_dirty();
}

void plexy_canvas_set_text_color(PlexyCanvas* canvas, uint32_t id,
                                  float r, float g, float b, float a) {
    if (!canvas) return;
    Widget* w = find_widget(ctx(canvas), id);
    if (w) {
        w->text_color = Color::rgba(r, g, b, a);
        w->has_text_color_override = true;
        w->mark_dirty();
    }
}

void plexy_canvas_set_font_size(PlexyCanvas* canvas, uint32_t id, float size) {
    if (!canvas) return;
    Widget* w = find_widget(ctx(canvas), id);
    if (w) {
        w->font_size = size;
        w->mark_dirty();
    }
}



void plexy_canvas_begin_path(PlexyCanvas* canvas) {
    if (canvas) ctx(canvas)->path_begin();
}

void plexy_canvas_move_to(PlexyCanvas* canvas, float x, float y) {
    if (canvas) ctx(canvas)->path_move_to(x, y);
}

void plexy_canvas_line_to(PlexyCanvas* canvas, float x, float y) {
    if (canvas) ctx(canvas)->path_line_to(x, y);
}

void plexy_canvas_quadratic_to(PlexyCanvas* canvas, float cx, float cy, float x, float y) {
    if (canvas) ctx(canvas)->path_quadratic_to(cx, cy, x, y);
}

void plexy_canvas_bezier_to(PlexyCanvas* canvas,
                            float cp1x, float cp1y,
                            float cp2x, float cp2y,
                            float x, float y) {
    if (canvas) ctx(canvas)->path_cubic_to(cp1x, cp1y, cp2x, cp2y, x, y);
}

void plexy_canvas_close_path(PlexyCanvas* canvas) {
    if (canvas) ctx(canvas)->path_close();
}

void plexy_canvas_set_path_stroke_color(PlexyCanvas* canvas, float r, float g, float b, float a) {
    if (canvas) ctx(canvas)->set_path_stroke_color(r, g, b, a);
}

void plexy_canvas_set_path_stroke_width(PlexyCanvas* canvas, float width) {
    if (canvas) ctx(canvas)->set_path_stroke_width(width);
}

void plexy_canvas_stroke_path(PlexyCanvas* canvas) {
    if (canvas) ctx(canvas)->path_stroke();
}

void plexy_canvas_clear_paths(PlexyCanvas* canvas) {
    if (canvas) ctx(canvas)->clear_paths();
}



void plexy_canvas_inject_mouse_move(PlexyCanvas* canvas, float x, float y) {
    if (canvas) ctx(canvas)->inject_mouse_move(x, y);
}

void plexy_canvas_inject_mouse_button(PlexyCanvas* canvas, int button, int pressed, float x, float y) {
    if (canvas) ctx(canvas)->inject_mouse_button(button, pressed != 0, x, y);
}

void plexy_canvas_inject_key(PlexyCanvas* canvas, uint32_t keycode, int pressed, uint32_t mods) {
    if (canvas) {
        ctx(canvas)->inject_key(keycode, mods, pressed != 0);
    }
}

void plexy_canvas_inject_char(PlexyCanvas* canvas, uint32_t codepoint) {
    if (canvas) ctx(canvas)->inject_char(codepoint);
}



void plexy_canvas_set_event_callback(PlexyCanvas* canvas, PlexyCanvasEventCallback cb, void* userdata) {
    if (!canvas) return;
    ctx(canvas)->set_event_callback(
        reinterpret_cast<CanvasContext::EventCallbackFn>(cb), userdata);
}

void plexy_canvas_on_click(PlexyCanvas* canvas, uint32_t id,
                            PlexyCanvasWidgetCallback cb, void* userdata) {
    if (!canvas) return;
    Widget* w = find_widget(ctx(canvas), id);
    if (w) {
        w->on_click = cb;
        w->on_click_data = userdata;
    }
}

void plexy_canvas_on_value_changed(PlexyCanvas* canvas, uint32_t id,
                                    PlexyCanvasValueCallback cb, void* userdata) {
    if (!canvas) return;
    Widget* w = find_widget(ctx(canvas), id);
    if (w) {
        w->on_value_change = reinterpret_cast<void(*)(uint32_t, float, void*)>(cb);
        w->on_value_change_data = userdata;
    }
}

void plexy_canvas_on_text_changed(PlexyCanvas* canvas, uint32_t id,
                                   PlexyCanvasTextCallback cb, void* userdata) {
    if (!canvas) return;
    Widget* w = find_widget(ctx(canvas), id);
    if (w) {
        w->on_text_change = reinterpret_cast<void(*)(uint32_t, const char*, void*)>(cb);
        w->on_text_change_data = userdata;
    }
}

void plexy_canvas_on_key(PlexyCanvas* canvas, uint32_t id,
                          PlexyCanvasKeyCallback cb, void* userdata) {
    if (!canvas) return;
    Widget* w = find_widget(ctx(canvas), id);
    if (w) {
        w->on_key = cb;
        w->on_key_data = userdata;
    }
}

void plexy_canvas_set_focus(PlexyCanvas* canvas, uint32_t id) {
    if (!canvas) return;
    Widget* w = find_widget(ctx(canvas), id);
    if (w) {
        ctx(canvas)->set_focus(w);
    }
}

int plexy_canvas_get_widget_bounds(PlexyCanvas* canvas, uint32_t id,
                                   float* x, float* y, float* width, float* height) {
    if (!canvas) return 0;
    Widget* w = find_widget(ctx(canvas), id);
    if (!w) return 0;

    float ax = 0.0f;
    float ay = 0.0f;
    for (Widget* cur = w; cur; cur = cur->parent) {
        ax += cur->layout.computed_x;
        ay += cur->layout.computed_y;
    }
    float ox = 0.0f;
    float oy = 0.0f;
    widget_effective_offset(w, &ox, &oy);
    ax += ox;
    ay += oy;

    if (x) *x = ax;
    if (y) *y = ay;
    if (width) *width = w->layout.computed_w;
    if (height) *height = w->layout.computed_h;
    return 1;
}
