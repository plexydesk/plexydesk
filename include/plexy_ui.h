

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "plexy_color.h"
#include <stdint.h>



typedef struct PlexyApp PlexyApp;
typedef struct PlexyWindow PlexyWindow;



typedef void (*PlexyClickCallback)(uint32_t widget_id, void* userdata);
typedef void (*PlexyValueCallback)(uint32_t widget_id, float value, void* userdata);
typedef void (*PlexyTextCallback)(uint32_t widget_id, const char* text, void* userdata);
typedef void (*PlexyKeyCallback)(uint32_t widget_id, uint32_t keycode, uint32_t mods, void* userdata);
typedef void (*PlexyMenuCallback)(uint32_t item_id, void* userdata);



PlexyApp* plexy_app_create(const char* name);
void      plexy_app_run(PlexyApp* app);
void      plexy_app_quit(PlexyApp* app);
void      plexy_app_destroy(PlexyApp* app);


int plexy_app_is_dark_mode(PlexyApp* app);
int plexy_window_is_dark_mode(PlexyWindow* win);



PlexyWindow* plexy_window_create(PlexyApp* app, uint32_t width, uint32_t height, const char* title);
uint32_t     plexy_window_root(PlexyWindow* win);
void         plexy_window_destroy(PlexyWindow* win);



uint32_t plexy_label(PlexyWindow* win, uint32_t parent, const char* text);
uint32_t plexy_button(PlexyWindow* win, uint32_t parent, const char* text);
uint32_t plexy_text_input(PlexyWindow* win, uint32_t parent, const char* placeholder);
uint32_t plexy_text_area(PlexyWindow* win, uint32_t parent, const char* initial_text);
uint32_t plexy_checkbox(PlexyWindow* win, uint32_t parent, const char* label);
uint32_t plexy_switch_widget(PlexyWindow* win, uint32_t parent, const char* label);
uint32_t plexy_slider(PlexyWindow* win, uint32_t parent, float min, float max, float initial);
uint32_t plexy_progress(PlexyWindow* win, uint32_t parent, float value);
uint32_t plexy_separator(PlexyWindow* win, uint32_t parent);
uint32_t plexy_spacer(PlexyWindow* win, uint32_t parent);
uint32_t plexy_panel(PlexyWindow* win, uint32_t parent);
uint32_t plexy_scroll_view(PlexyWindow* win, uint32_t parent);
uint32_t plexy_list_view(PlexyWindow* win, uint32_t parent, const char* items_text);
uint32_t plexy_icon_view(PlexyWindow* win, uint32_t parent, const char* items_text);
void     plexy_list_clear(PlexyWindow* win, uint32_t list_view_id);
uint32_t plexy_list_append(PlexyWindow* win, uint32_t list_view_id, const char* item_text);
void     plexy_list_select(PlexyWindow* win, uint32_t list_view_id, int index);
void     plexy_icon_clear(PlexyWindow* win, uint32_t icon_view_id);
uint32_t plexy_icon_append(PlexyWindow* win, uint32_t icon_view_id, const char* item_text);
void     plexy_icon_select(PlexyWindow* win, uint32_t icon_view_id, int index);
void     plexy_destroy_widget(PlexyWindow* win, uint32_t widget_id);



uint32_t plexy_row(PlexyWindow* win, uint32_t parent);
uint32_t plexy_column(PlexyWindow* win, uint32_t parent);



uint32_t plexy_toolbar(PlexyWindow* win, uint32_t parent);
uint32_t plexy_toolbar_item(PlexyWindow* win, uint32_t parent, const char* label);
uint32_t plexy_sidebar(PlexyWindow* win, uint32_t parent);
uint32_t plexy_sidebar_item(PlexyWindow* win, uint32_t parent, const char* label);
uint32_t plexy_sidebar_item_with_icon(PlexyWindow* win, uint32_t parent, const char* label, const char* icon_name);
uint32_t plexy_sidebar_section(PlexyWindow* win, uint32_t parent, const char* title);
uint32_t plexy_search_field(PlexyWindow* win, uint32_t parent, const char* placeholder);
uint32_t plexy_breadcrumb(PlexyWindow* win, uint32_t parent, const char* path);
uint32_t plexy_split_view(PlexyWindow* win, uint32_t parent);
uint32_t plexy_status_bar(PlexyWindow* win, uint32_t parent, const char* text);
uint32_t plexy_drawer(PlexyWindow* win, uint32_t parent, int edge, float size);
void     plexy_drawer_set_open(PlexyWindow* win, uint32_t drawer_id, int open, int animated);
void     plexy_drawer_toggle(PlexyWindow* win, uint32_t drawer_id, int animated);
int      plexy_drawer_is_open(PlexyWindow* win, uint32_t drawer_id);



void plexy_flex_direction(PlexyWindow* win, uint32_t widget_id, int direction); 
void plexy_justify(PlexyWindow* win, uint32_t widget_id, int justify);
void plexy_align(PlexyWindow* win, uint32_t widget_id, int align);
void plexy_flex_grow(PlexyWindow* win, uint32_t widget_id, float grow);
void plexy_size(PlexyWindow* win, uint32_t widget_id, float width, float height);
void plexy_margin(PlexyWindow* win, uint32_t widget_id, float top, float right, float bottom, float left);
void plexy_padding(PlexyWindow* win, uint32_t widget_id, float top, float right, float bottom, float left);
void plexy_gap(PlexyWindow* win, uint32_t widget_id, float gap);



void plexy_set_text(PlexyWindow* win, uint32_t widget_id, const char* text);
void plexy_set_value(PlexyWindow* win, uint32_t widget_id, float value);
void plexy_set_visible(PlexyWindow* win, uint32_t widget_id, int visible);
void plexy_set_enabled(PlexyWindow* win, uint32_t widget_id, int enabled);
void plexy_set_readonly(PlexyWindow* win, uint32_t widget_id, int readonly);



void plexy_set_corner_radius(PlexyWindow* win, uint32_t widget_id, float radius);
void plexy_set_elevation(PlexyWindow* win, uint32_t widget_id, float level);
void plexy_set_fill_color(PlexyWindow* win, uint32_t widget_id, float r, float g, float b, float a);
void plexy_set_fill_color_hex(PlexyWindow* win, uint32_t widget_id, const char* hex);
void plexy_set_text_color(PlexyWindow* win, uint32_t widget_id, float r, float g, float b, float a);
void plexy_set_text_color_hex(PlexyWindow* win, uint32_t widget_id, const char* hex);
void plexy_set_font_size(PlexyWindow* win, uint32_t widget_id, float size);
void plexy_set_border(PlexyWindow* win, uint32_t widget_id, float r, float g, float b, float a, float width);
void plexy_set_glass(PlexyWindow* win, uint32_t widget_id, float opacity);
void plexy_enable_blur_background(PlexyWindow* win, int enable);
void plexy_set_glass_material(PlexyWindow* win, uint32_t widget_id, float opacity);
void plexy_set_surface_material(PlexyWindow* win, uint32_t widget_id);



void plexy_on_click(PlexyWindow* win, uint32_t widget_id, PlexyClickCallback cb, void* userdata);
void plexy_on_value_changed(PlexyWindow* win, uint32_t widget_id, PlexyValueCallback cb, void* userdata);
void plexy_on_text_changed(PlexyWindow* win, uint32_t widget_id, PlexyTextCallback cb, void* userdata);
void plexy_on_key(PlexyWindow* win, uint32_t widget_id, PlexyKeyCallback cb, void* userdata);
void plexy_set_focus(PlexyWindow* win, uint32_t widget_id);
void plexy_on_menu(PlexyWindow* win, PlexyMenuCallback cb, void* userdata);


int  plexy_menu_begin(PlexyWindow* win, int supported, const char* app_title);
int  plexy_menu_add(PlexyWindow* win, uint32_t menu_id, const char* title);
int  plexy_menu_add_item(PlexyWindow* win, uint32_t menu_id, uint32_t item_id,
                         const char* label, int enabled);
int  plexy_menu_commit(PlexyWindow* win);



void plexy_batch_begin(PlexyWindow* win);
void plexy_batch_end(PlexyWindow* win);


void plexy_path_clear(PlexyWindow* win);
void plexy_path_begin(PlexyWindow* win);
void plexy_path_move_to(PlexyWindow* win, float x, float y);
void plexy_path_line_to(PlexyWindow* win, float x, float y);
void plexy_path_bezier_to(PlexyWindow* win,
                          float cp1x, float cp1y,
                          float cp2x, float cp2y,
                          float x, float y);
void plexy_path_set_stroke(PlexyWindow* win, float r, float g, float b, float a, float width);
void plexy_path_stroke(PlexyWindow* win);
int  plexy_widget_bounds(PlexyWindow* win, uint32_t widget_id,
                         float* x, float* y, float* width, float* height);




#define PLEXY_JUSTIFY_START 0
#define PLEXY_JUSTIFY_END 1
#define PLEXY_JUSTIFY_CENTER 2
#define PLEXY_JUSTIFY_SPACE_BETWEEN 3
#define PLEXY_JUSTIFY_SPACE_AROUND 4
#define PLEXY_JUSTIFY_SPACE_EVENLY 5


#define PLEXY_ALIGN_START 0
#define PLEXY_ALIGN_END 1
#define PLEXY_ALIGN_CENTER 2
#define PLEXY_ALIGN_STRETCH 3


#define PLEXY_FLEX_ROW 0
#define PLEXY_FLEX_COLUMN 1


#define PLEXY_DRAWER_LEFT 0
#define PLEXY_DRAWER_RIGHT 1
#define PLEXY_DRAWER_TOP 2
#define PLEXY_DRAWER_BOTTOM 3

#ifdef __cplusplus
}
#endif
