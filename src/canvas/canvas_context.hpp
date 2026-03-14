#pragma once

#include "arena.hpp"
#include "types.hpp"
#include "widget.hpp"
#include "render.hpp"
#include "text.hpp"
#include "layout.hpp"
#include "style.hpp"
#include "events.hpp"
#include "anim.hpp"

#include <GL/glew.h>
#include <vector>

namespace plexy::ui {

enum class CanvasTarget {
    FBO,     
    GBM,     
};

class CanvasContext {
public:
    
    bool init(int width, int height, CanvasTarget target, const char* font_path);
    void init_gl();  
    void shutdown();
    void resize(int width, int height);
    void resize_gpu(int width, int height);  

    
    bool init_gpu(const char* drm_device);                        
    bool init_with_egl(void* egl_display, void* egl_context);    

    
    void begin_frame(float dt);
    void end_frame();
    void swap_buffers();  

    
    GLuint get_texture() const { return fbo_color_tex_; }

    
    int get_dmabuf_fd() const;
    uint32_t get_stride() const;
    uint32_t get_format() const;
    uint64_t get_modifier() const;

    
    void set_blur_texture(GLuint tex) { blur_texture_ = tex; }

    
    void set_dark_mode(bool dark) {
        if (dark_mode_ != dark) {
            dark_mode_ = dark;
            render_dirty_ = true;
        }
    }
    void set_glass_alpha(float alpha) { glass_alpha_ = alpha; }
    void set_scale_factor(float scale) { scale_factor_ = scale; }
    float scale_factor() const { return scale_factor_; }

    
    void inject_mouse_move(float x, float y);
    void inject_mouse_button(int button, bool pressed, float x, float y);
    void inject_mouse_scroll(float dx, float dy, float x, float y);
    void inject_key(uint32_t keycode, uint32_t mods, bool pressed);
    void inject_char(uint32_t codepoint);

    
    bool hit_test(float x, float y);

    
    bool needs_render() const;
    void mark_layout_dirty() { layout_dirty_ = true; }

    
    Widget* create_widget(WidgetType type, Widget* parent = nullptr);
    void destroy_widget(Widget* widget);
    Widget* root() const { return root_; }

    
    StringId intern_string(const char* text);
    void update_string(StringId id, const char* text);
    const char* get_string(StringId id) const;

    
    void store_glyph_run(GlyphRun* run);
    GlyphRun* glyph_runs_array() { return glyph_runs_; }
    uint32_t glyph_run_count() const { return glyph_run_count_; }

    
    GlyphAtlas* atlas() { return &atlas_; }
    const Theme* theme() const { return theme_; }
    AnimationEngine* animations() { return &anim_; }
    FocusManager* focus() { return &focus_; }
    void set_focus(Widget* w) { focus_.set_focus(w); }

    
    void path_begin();
    void path_move_to(float x, float y);
    void path_line_to(float x, float y);
    void path_quadratic_to(float cx, float cy, float x, float y);
    void path_cubic_to(float cp1x, float cp1y, float cp2x, float cp2y, float x, float y);
    void path_close();
    void set_path_stroke_color(float r, float g, float b, float a);
    void set_path_stroke_width(float width);
    void path_stroke();
    void clear_paths();

    
    typedef void (*EventCallbackFn)(void* ctx, uint32_t widget_id, int event_type, void* userdata);
    void set_event_callback(EventCallbackFn cb, void* userdata) {
        event_callback_ = cb;
        event_callback_data_ = userdata;
    }

private:
    CanvasTarget target_ = CanvasTarget::FBO;

    
    FrameArena frame_arena_;
    PoolAllocator<Widget, 64> widget_pool_;
    GlyphAtlas atlas_;
    RenderBackend renderer_;
    LayoutEngine layout_;
    EventQueue events_;
    SpatialGrid grid_;
    FocusManager focus_;
    EventProcessor event_proc_;
    AnimationEngine anim_;

    
    const Theme* theme_ = &kPlexyLightTheme;
    bool dark_mode_ = false;
    float glass_alpha_ = 0.8f;
    float scale_factor_ = 1.0f;

    
    Widget* root_ = nullptr;

    
    int width_ = 0, height_ = 0;

    
    bool layout_dirty_ = true;
    bool render_dirty_ = true;

    
    static constexpr uint32_t MAX_GLYPH_RUNS = 512;
    GlyphRun* glyph_runs_ = nullptr;
    uint32_t glyph_run_count_ = 0;

    
    static constexpr uint32_t MAX_STRINGS = 1024;
    static constexpr uint32_t STRING_POOL_SIZE = 64 * 1024;
    struct StringEntry {
        uint32_t offset;
        uint32_t length;
        bool used;
    };
    StringEntry string_entries_[MAX_STRINGS] = {};
    char string_pool_[STRING_POOL_SIZE] = {};
    uint32_t string_pool_used_ = 0;
    uint32_t next_string_id_ = 1;

    
    GLuint fbo_ = 0;
    GLuint fbo_color_tex_ = 0;
    GLuint fbo_depth_rbo_ = 0;

    
    GLuint quad_vao_ = 0;
    GLuint quad_vbo_ = 0;

    
    GLuint blur_texture_ = 0;

    
    struct GpuState;
    GpuState* gpu_ = nullptr;
    
    
    const char* font_path_ = nullptr;
    bool gl_initialized_ = false;

    
    EventCallbackFn event_callback_ = nullptr;
    void* event_callback_data_ = nullptr;

    
    struct PathCmd {
        enum class Type : uint8_t { Move, Line, Quadratic, Cubic, Close };
        Type type = Type::Move;
        Vec2 p1 = {0, 0};
        Vec2 p2 = {0, 0};
        Vec2 p3 = {0, 0};
    };

    struct PathStrokeLine {
        float x1 = 0.0f, y1 = 0.0f, x2 = 0.0f, y2 = 0.0f;
        float width = 1.0f;
        Color color = {1, 1, 1, 1};
    };

    std::vector<PathCmd> active_path_;
    std::vector<PathStrokeLine> custom_stroke_lines_;
    Color path_stroke_color_ = {1.0f, 1.0f, 1.0f, 1.0f};
    float path_stroke_width_ = 1.0f;

    
    void create_fbo(int w, int h);
    void destroy_fbo();
    void create_quad_vao();
    void destroy_quad_vao();
    void paint_widget_tree(Widget* w, float parent_x, float parent_y,
                           PaintList* list, uint32_t* next_run_id);
    void unlink_widget(Widget* widget);
    void append_line_segment(float x1, float y1, float x2, float y2);
};


extern const char* string_table_get(StringId id);


extern void store_glyph_run_global(GlyphRun* run);


extern float get_scale_factor_global();

} 
