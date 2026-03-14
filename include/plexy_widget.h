#ifndef PLEXY_WIDGET_H
#define PLEXY_WIDGET_H

#include <stdint.h>
#include <stdbool.h>
#include "plexy_color.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct PlexyWidget PlexyWidget;
typedef struct PlexyWidgetManager PlexyWidgetManager;
typedef struct PlexyCanvas PlexyCanvas;





typedef enum {
    PLEXY_SRV_WIDGET_LABEL,
    PLEXY_SRV_WIDGET_BUTTON,
    PLEXY_SRV_WIDGET_TEXT_INPUT,
    PLEXY_SRV_WIDGET_TEXT_AREA,
    PLEXY_SRV_WIDGET_CHECKBOX,
    PLEXY_SRV_WIDGET_RADIO_BUTTON,
    PLEXY_SRV_WIDGET_SLIDER,
    PLEXY_SRV_WIDGET_PROGRESS_BAR,
    PLEXY_SRV_WIDGET_PANEL,
    PLEXY_SRV_WIDGET_WINDOW,
    PLEXY_SRV_WIDGET_SCROLLBAR,
    PLEXY_SRV_WIDGET_LISTBOX,
    PLEXY_SRV_WIDGET_COMBOBOX,
    PLEXY_SRV_WIDGET_MENU,
    PLEXY_SRV_WIDGET_TOOLTIP
} PlexyServerWidgetType;





typedef enum {
    PLEXY_SRV_EVENT_NONE,
    PLEXY_SRV_EVENT_MOUSE_ENTER,
    PLEXY_SRV_EVENT_MOUSE_LEAVE,
    PLEXY_SRV_EVENT_MOUSE_MOVE,
    PLEXY_SRV_EVENT_MOUSE_DOWN,
    PLEXY_SRV_EVENT_MOUSE_UP,
    PLEXY_SRV_EVENT_CLICK,
    PLEXY_SRV_EVENT_DOUBLE_CLICK,
    PLEXY_SRV_EVENT_KEY_DOWN,
    PLEXY_SRV_EVENT_KEY_UP,
    PLEXY_SRV_EVENT_CHAR,
    PLEXY_SRV_EVENT_FOCUS_IN,
    PLEXY_SRV_EVENT_FOCUS_OUT,
    PLEXY_SRV_EVENT_VALUE_CHANGED
} PlexyServerEventType;

typedef struct {
    PlexyServerEventType type;
    PlexyWidget* widget;
    
    
    int32_t mouse_x;
    int32_t mouse_y;
    uint32_t button;  
    
    
    uint32_t key_code;
    uint32_t modifiers;  
    uint32_t character;  
    
    
    float value;
    const char* text;
} PlexyEvent;


typedef void (*PlexyEventCallback)(PlexyWidget* widget, PlexyEvent* event, void* user_data);






PlexyWidgetManager* plexy_widget_manager_create(void);
void plexy_widget_manager_destroy(PlexyWidgetManager* manager);


void plexy_widget_manager_update(PlexyWidgetManager* manager, float dt);


void plexy_widget_manager_render(PlexyWidgetManager* manager, PlexyCanvas* canvas);


void plexy_widget_manager_render_to_buffer(PlexyWidgetManager* manager,
                                            void* buffer,
                                            uint32_t width,
                                            uint32_t height,
                                            uint32_t stride);


void plexy_widget_manager_inject_mouse_move(PlexyWidgetManager* manager, int32_t x, int32_t y);
void plexy_widget_manager_inject_mouse_button(PlexyWidgetManager* manager, uint32_t button, bool pressed);
void plexy_widget_manager_inject_key(PlexyWidgetManager* manager, uint32_t key_code, bool pressed, uint32_t modifiers);
void plexy_widget_manager_inject_char(PlexyWidgetManager* manager, uint32_t character);


void plexy_widget_manager_set_focus(PlexyWidgetManager* manager, PlexyWidget* widget);
PlexyWidget* plexy_widget_manager_get_focused(PlexyWidgetManager* manager);


void plexy_widget_manager_add_root_widget(PlexyWidgetManager* manager, PlexyWidget* widget);


void plexy_widget_manager_set_dark_mode(PlexyWidgetManager* manager, bool dark);






void plexy_widget_set_position(PlexyWidget* widget, int32_t x, int32_t y);
void plexy_widget_set_size(PlexyWidget* widget, uint32_t width, uint32_t height);
void plexy_widget_get_position(PlexyWidget* widget, int32_t* x, int32_t* y);
void plexy_widget_get_size(PlexyWidget* widget, uint32_t* width, uint32_t* height);

void plexy_widget_set_visible(PlexyWidget* widget, bool visible);
bool plexy_widget_is_visible(PlexyWidget* widget);

void plexy_widget_set_enabled(PlexyWidget* widget, bool enabled);
bool plexy_widget_is_enabled(PlexyWidget* widget);


void plexy_widget_add_child(PlexyWidget* parent, PlexyWidget* child);
void plexy_widget_remove_child(PlexyWidget* parent, PlexyWidget* child);
PlexyWidget* plexy_widget_get_parent(PlexyWidget* widget);


void plexy_widget_set_event_callback(PlexyWidget* widget, PlexyEventCallback callback, void* user_data);


void plexy_widget_destroy(PlexyWidget* widget);





PlexyWidget* plexy_label_create(PlexyWidgetManager* manager, const char* text);
void plexy_label_set_text(PlexyWidget* label, const char* text);
const char* plexy_label_get_text(PlexyWidget* label);
void plexy_label_set_font_size(PlexyWidget* label, float size);
void plexy_label_set_text_color(PlexyWidget* label, uint32_t color);
void plexy_label_set_text_color_hex(PlexyWidget* label, const char* hex);





PlexyWidget* plexy_button_create(PlexyWidgetManager* manager, const char* text);
void plexy_button_set_text(PlexyWidget* button, const char* text);
const char* plexy_button_get_text(PlexyWidget* button);





PlexyWidget* plexy_text_input_create(PlexyWidgetManager* manager);
void plexy_text_input_set_text(PlexyWidget* input, const char* text);
const char* plexy_text_input_get_text(PlexyWidget* input);
void plexy_text_input_set_placeholder(PlexyWidget* input, const char* placeholder);
void plexy_text_input_set_max_length(PlexyWidget* input, uint32_t max_length);





PlexyWidget* plexy_checkbox_create(PlexyWidgetManager* manager, const char* label);
void plexy_checkbox_set_checked(PlexyWidget* checkbox, bool checked);
bool plexy_checkbox_is_checked(PlexyWidget* checkbox);
void plexy_checkbox_set_label(PlexyWidget* checkbox, const char* label);





PlexyWidget* plexy_slider_create(PlexyWidgetManager* manager, float min, float max);
void plexy_slider_set_value(PlexyWidget* slider, float value);
float plexy_slider_get_value(PlexyWidget* slider);
void plexy_slider_set_range(PlexyWidget* slider, float min, float max);





PlexyWidget* plexy_progress_bar_create(PlexyWidgetManager* manager);
void plexy_progress_bar_set_value(PlexyWidget* progress, float value); 
float plexy_progress_bar_get_value(PlexyWidget* progress);





PlexyWidget* plexy_panel_create(PlexyWidgetManager* manager);
void plexy_panel_set_background_color(PlexyWidget* panel, uint32_t color);
void plexy_panel_set_background_color_hex(PlexyWidget* panel, const char* hex);
void plexy_panel_set_border_color(PlexyWidget* panel, uint32_t color);
void plexy_panel_set_border_color_hex(PlexyWidget* panel, const char* hex);
void plexy_panel_set_border_width(PlexyWidget* panel, float width);


typedef enum {
    PLEXY_LAYOUT_NONE,      
    PLEXY_LAYOUT_VERTICAL,  
    PLEXY_LAYOUT_HORIZONTAL 
} PlexyLayoutMode;


typedef enum {
    PLEXY_SIZE_FIXED,       
    PLEXY_SIZE_EXPAND,      
    PLEXY_SIZE_FILL         
} PlexySizePolicy;

void plexy_panel_set_layout(PlexyWidget* panel, PlexyLayoutMode mode);
void plexy_panel_set_padding(PlexyWidget* panel, int32_t padding);
void plexy_panel_set_spacing(PlexyWidget* panel, int32_t spacing);


void plexy_widget_set_size_policy(PlexyWidget* widget, PlexySizePolicy horizontal, PlexySizePolicy vertical);
void plexy_widget_set_layout_weight(PlexyWidget* widget, float weight);

#ifdef __cplusplus
}
#endif

#endif 
