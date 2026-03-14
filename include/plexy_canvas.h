#ifndef PLEXY_CANVAS_H
#define PLEXY_CANVAS_H



#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct PlexyCanvas PlexyCanvas;


typedef enum {
    PLEXY_CANVAS_TARGET_FBO = 0,   
    PLEXY_CANVAS_TARGET_GBM = 1,   
} PlexyCanvasTarget;


typedef enum {
    PLEXY_WIDGET_LABEL = 0,
    PLEXY_WIDGET_BUTTON,
    PLEXY_WIDGET_ICON_BUTTON,
    PLEXY_WIDGET_TOGGLE_BUTTON,
    PLEXY_WIDGET_TEXT_INPUT,
    PLEXY_WIDGET_TEXT_AREA,
    PLEXY_WIDGET_CHECKBOX,
    PLEXY_WIDGET_RADIO_BUTTON,
    PLEXY_WIDGET_SWITCH,
    PLEXY_WIDGET_SLIDER,
    PLEXY_WIDGET_PROGRESS_BAR,
    PLEXY_WIDGET_PANEL,
    PLEXY_WIDGET_SCROLL_VIEW,
    PLEXY_WIDGET_LIST_VIEW,
    PLEXY_WIDGET_ICON_VIEW,
    PLEXY_WIDGET_SEPARATOR,
    PLEXY_WIDGET_SPACER,
    PLEXY_WIDGET_ROW,     
    PLEXY_WIDGET_COLUMN,  
    PLEXY_WIDGET_TOOLBAR,       
    PLEXY_WIDGET_TOOLBAR_ITEM,  
    PLEXY_WIDGET_SIDEBAR,       
    PLEXY_WIDGET_SIDEBAR_ITEM,  
    PLEXY_WIDGET_SEARCH_FIELD,  
    PLEXY_WIDGET_BREADCRUMB,    
    PLEXY_WIDGET_SPLIT_VIEW,    
    PLEXY_WIDGET_STATUS_BAR,    
} PlexyWidgetType;


typedef enum {
    PLEXY_CANVAS_DRAWER_LEFT = 0,
    PLEXY_CANVAS_DRAWER_RIGHT = 1,
    PLEXY_CANVAS_DRAWER_TOP = 2,
    PLEXY_CANVAS_DRAWER_BOTTOM = 3,
} PlexyDrawerEdge;


typedef enum {
    PLEXY_EVENT_CLICK = 1,
    PLEXY_EVENT_VALUE_CHANGED,
    PLEXY_EVENT_TEXT_CHANGED,
    PLEXY_EVENT_KEY,
    PLEXY_EVENT_FOCUS_IN,
    PLEXY_EVENT_FOCUS_OUT,
} PlexyCanvasEventType;



PlexyCanvas* plexy_canvas_create(int width, int height, PlexyCanvasTarget target);
void         plexy_canvas_destroy(PlexyCanvas* ctx);
void         plexy_canvas_resize(PlexyCanvas* ctx, int width, int height);
void         plexy_canvas_set_font(PlexyCanvas* ctx, const char* font_path);




int plexy_canvas_init_gpu(PlexyCanvas* ctx, const char* drm_device);


int plexy_canvas_init_with_egl(PlexyCanvas* ctx, void* egl_display, void* egl_context);



int      plexy_canvas_get_dmabuf_fd(PlexyCanvas* ctx);
uint32_t plexy_canvas_get_stride(PlexyCanvas* ctx);
uint32_t plexy_canvas_get_format(PlexyCanvas* ctx);
uint64_t plexy_canvas_get_modifier(PlexyCanvas* ctx);



void     plexy_canvas_begin_frame(PlexyCanvas* ctx, float dt);
void     plexy_canvas_end_frame(PlexyCanvas* ctx);
void     plexy_canvas_swap_buffers(PlexyCanvas* ctx);  
uint32_t plexy_canvas_get_texture(PlexyCanvas* ctx);  
int      plexy_canvas_needs_render(PlexyCanvas* ctx);



void plexy_canvas_set_blur_texture(PlexyCanvas* ctx, uint32_t blur_tex);



void plexy_canvas_set_dark_mode(PlexyCanvas* ctx, int dark);
void plexy_canvas_set_glass_alpha(PlexyCanvas* ctx, float alpha);
void plexy_canvas_set_scale_factor(PlexyCanvas* ctx, float scale);  



uint32_t plexy_canvas_create_widget(PlexyCanvas* ctx, int type, uint32_t parent_id);
void     plexy_canvas_destroy_widget(PlexyCanvas* ctx, uint32_t widget_id);
uint32_t plexy_canvas_root(PlexyCanvas* ctx);



void plexy_canvas_set_text(PlexyCanvas* ctx, uint32_t id, const char* text);
void plexy_canvas_set_value(PlexyCanvas* ctx, uint32_t id, float value);
void plexy_canvas_set_visible(PlexyCanvas* ctx, uint32_t id, int visible);
void plexy_canvas_set_enabled(PlexyCanvas* ctx, uint32_t id, int enabled);
void plexy_canvas_set_checked(PlexyCanvas* ctx, uint32_t id, int checked);
void plexy_canvas_set_readonly(PlexyCanvas* ctx, uint32_t id, int readonly);



void plexy_canvas_set_flex_direction(PlexyCanvas* ctx, uint32_t id, int row);
void plexy_canvas_set_flex_grow(PlexyCanvas* ctx, uint32_t id, float grow);
void plexy_canvas_set_size(PlexyCanvas* ctx, uint32_t id, float w, float h);
void plexy_canvas_set_margin(PlexyCanvas* ctx, uint32_t id, float t, float r, float b, float l);
void plexy_canvas_set_padding(PlexyCanvas* ctx, uint32_t id, float t, float r, float b, float l);
void plexy_canvas_set_gap(PlexyCanvas* ctx, uint32_t id, float gap);
void plexy_canvas_set_justify(PlexyCanvas* ctx, uint32_t id, int justify);
void plexy_canvas_set_align(PlexyCanvas* ctx, uint32_t id, int align);



void plexy_canvas_set_drawer(PlexyCanvas* ctx, uint32_t id, int edge);
void plexy_canvas_set_drawer_open(PlexyCanvas* ctx, uint32_t id, int open, int animated);
void plexy_canvas_toggle_drawer(PlexyCanvas* ctx, uint32_t id, int animated);
int  plexy_canvas_is_drawer_open(PlexyCanvas* ctx, uint32_t id);



void plexy_canvas_set_corner_radius(PlexyCanvas* ctx, uint32_t id, float radius);
void plexy_canvas_set_elevation(PlexyCanvas* ctx, uint32_t id, float elevation);
void plexy_canvas_set_fill_color(PlexyCanvas* ctx, uint32_t id, float r, float g, float b, float a);
void plexy_canvas_set_glass_material(PlexyCanvas* ctx, uint32_t id, float opacity);
void plexy_canvas_set_surface_material(PlexyCanvas* ctx, uint32_t id);
void plexy_canvas_set_border(PlexyCanvas* ctx, uint32_t id, float r, float g, float b, float a, float width);
void plexy_canvas_set_text_color(PlexyCanvas* ctx, uint32_t id, float r, float g, float b, float a);
void plexy_canvas_set_font_size(PlexyCanvas* ctx, uint32_t id, float size);



void plexy_canvas_begin_path(PlexyCanvas* ctx);
void plexy_canvas_move_to(PlexyCanvas* ctx, float x, float y);
void plexy_canvas_line_to(PlexyCanvas* ctx, float x, float y);
void plexy_canvas_quadratic_to(PlexyCanvas* ctx, float cx, float cy, float x, float y);
void plexy_canvas_bezier_to(PlexyCanvas* ctx,
                            float cp1x, float cp1y,
                            float cp2x, float cp2y,
                            float x, float y);
void plexy_canvas_close_path(PlexyCanvas* ctx);
void plexy_canvas_set_path_stroke_color(PlexyCanvas* ctx, float r, float g, float b, float a);
void plexy_canvas_set_path_stroke_width(PlexyCanvas* ctx, float width);
void plexy_canvas_stroke_path(PlexyCanvas* ctx);
void plexy_canvas_clear_paths(PlexyCanvas* ctx);



void plexy_canvas_inject_mouse_move(PlexyCanvas* ctx, float x, float y);
void plexy_canvas_inject_mouse_button(PlexyCanvas* ctx, int button, int pressed, float x, float y);
void plexy_canvas_inject_key(PlexyCanvas* ctx, uint32_t keycode, int pressed, uint32_t mods);
void plexy_canvas_inject_char(PlexyCanvas* ctx, uint32_t codepoint);



typedef void (*PlexyCanvasEventCallback)(PlexyCanvas* ctx, uint32_t widget_id,
                                          int event_type, void* userdata);
void plexy_canvas_set_event_callback(PlexyCanvas* ctx, PlexyCanvasEventCallback cb, void* userdata);

typedef void (*PlexyCanvasWidgetCallback)(uint32_t widget_id, void* userdata);
typedef void (*PlexyCanvasValueCallback)(uint32_t widget_id, float value, void* userdata);
typedef void (*PlexyCanvasTextCallback)(uint32_t widget_id, const char* text, void* userdata);
typedef void (*PlexyCanvasKeyCallback)(uint32_t widget_id, uint32_t keycode, uint32_t mods, void* userdata);

void plexy_canvas_on_click(PlexyCanvas* ctx, uint32_t id, PlexyCanvasWidgetCallback cb, void* userdata);
void plexy_canvas_on_value_changed(PlexyCanvas* ctx, uint32_t id, PlexyCanvasValueCallback cb, void* userdata);
void plexy_canvas_on_text_changed(PlexyCanvas* ctx, uint32_t id, PlexyCanvasTextCallback cb, void* userdata);
void plexy_canvas_on_key(PlexyCanvas* ctx, uint32_t id, PlexyCanvasKeyCallback cb, void* userdata);
void plexy_canvas_set_focus(PlexyCanvas* ctx, uint32_t id);


int  plexy_canvas_get_widget_bounds(PlexyCanvas* ctx, uint32_t id,
                                    float* x, float* y, float* width, float* height);

#ifdef __cplusplus
}
#endif

#endif 
