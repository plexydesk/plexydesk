#ifndef PLEXY_H
#define PLEXY_H

#include <stdint.h>
#include <stdbool.h>
#include "plexy_protocol.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PlexyConnection PlexyConnection;
typedef struct PlexyWindow PlexyWindow;
typedef struct PlexyBuffer PlexyBuffer;
typedef struct PlexyLayerSurface PlexyLayerSurface;
typedef void (*PlexyMenuActionCallback)(PlexyWindow* window, uint32_t item_id, void* user_data);
typedef void (*PlexyClipboardTextCallback)(PlexyConnection* conn, const char* text, void* user_data);

typedef struct {
    void (*configure)(PlexyWindow* window, uint32_t width, uint32_t height, uint32_t state_flags, void* user_data);
    void (*close)(PlexyWindow* window, void* user_data);
    void (*pointer_enter)(PlexyWindow* window, int32_t x, int32_t y, void* user_data);
    void (*pointer_leave)(PlexyWindow* window, void* user_data);
    void (*pointer_motion)(PlexyWindow* window, int32_t x, int32_t y, void* user_data);
    void (*pointer_button)(PlexyWindow* window, uint32_t button, bool pressed, int32_t x, int32_t y, void* user_data);
    void (*pointer_axis)(PlexyWindow* window, int32_t axis, int32_t value, int32_t discrete, void* user_data);
    void (*key)(PlexyWindow* window, uint32_t keycode, bool pressed, uint32_t modifiers, void* user_data);
    void (*modifiers)(PlexyWindow* window, uint32_t depressed, uint32_t latched, uint32_t locked, uint32_t group, void* user_data);
    void (*focus_in)(PlexyWindow* window, void* user_data);
    void (*focus_out)(PlexyWindow* window, void* user_data);
    void (*frame_done)(PlexyWindow* window, void* user_data);
    void (*menu_action)(PlexyWindow* window, uint32_t item_id, void* user_data);
    void (*scale_changed)(PlexyWindow* window, float scale_factor, uint32_t buffer_scale, void* user_data);
    void (*enter_output)(PlexyWindow* window, uint32_t output_id, void* user_data);
    void (*leave_output)(PlexyWindow* window, uint32_t output_id, void* user_data);
} PlexyWindowCallbacks;

PlexyConnection* plexy_connect(const char* socket_path);

void plexy_disconnect(PlexyConnection* conn);

int plexy_get_fd(PlexyConnection* conn);

float plexy_get_ui_scale(PlexyConnection* conn);

void plexy_get_screen_size(PlexyConnection* conn, uint32_t* width, uint32_t* height);


int plexy_is_dark_mode(PlexyConnection* conn);

typedef void (*PlexyDarkModeCallback)(PlexyConnection* conn, int dark, void* user_data);
void plexy_set_dark_mode_callback(PlexyConnection* conn, PlexyDarkModeCallback cb, void* user_data);

int plexy_dispatch(PlexyConnection* conn);

int plexy_dispatch_pending(PlexyConnection* conn);

int plexy_flush(PlexyConnection* conn);

int plexy_roundtrip(PlexyConnection* conn);

PlexyWindow* plexy_create_window(PlexyConnection* conn, 
                                  int32_t x, int32_t y,
                                  uint32_t width, uint32_t height,
                                  const char* title);


PlexyWindow* plexy_create_popup(PlexyConnection* conn,
                                PlexyWindow* parent,
                                int32_t x, int32_t y,
                                uint32_t width, uint32_t height);

int plexy_popup_update(PlexyWindow* popup, int32_t x, int32_t y, uint32_t flags);

void plexy_destroy_window(PlexyWindow* window);

uint32_t plexy_window_get_id(PlexyWindow* window);

void plexy_window_get_size(PlexyWindow* window, uint32_t* width, uint32_t* height);

void plexy_window_set_callbacks(PlexyWindow* window, const PlexyWindowCallbacks* callbacks, void* user_data);

void plexy_window_set_user_data(PlexyWindow* window, void* user_data);

void* plexy_window_get_user_data(PlexyWindow* window);

uint32_t plexy_get_output_count(PlexyConnection* conn);

const PlexyOutputInfo* plexy_get_output_info(PlexyConnection* conn, uint32_t index);

float plexy_get_output_scale(PlexyConnection* conn, uint32_t output_id);

uint64_t plexy_get_output_generation(PlexyConnection* conn);

PlexyWindow* plexy_create_window_scaled(PlexyConnection* conn,
                                        int32_t x, int32_t y,
                                        uint32_t width, uint32_t height,
                                        uint32_t buffer_scale,
                                        const char* title);

float plexy_window_get_scale(PlexyWindow* window);

uint32_t plexy_window_get_buffer_scale(PlexyWindow* window);

void plexy_window_get_logical_size(PlexyWindow* window, uint32_t* width, uint32_t* height);

void plexy_window_get_buffer_size(PlexyWindow* window, uint32_t* width, uint32_t* height);

int plexy_window_set_buffer_scale(PlexyWindow* window, uint32_t scale);

uint32_t plexy_window_get_output_count(PlexyWindow* window);

const uint32_t* plexy_window_get_outputs(PlexyWindow* window);

PlexyBuffer* plexy_create_buffer(PlexyConnection* conn, uint32_t width, uint32_t height, uint32_t format);

void plexy_destroy_buffer(PlexyBuffer* buffer);

void* plexy_buffer_get_data(PlexyBuffer* buffer);

uint32_t plexy_buffer_get_stride(PlexyBuffer* buffer);

uint32_t plexy_buffer_get_width(PlexyBuffer* buffer);

uint32_t plexy_buffer_get_height(PlexyBuffer* buffer);

PlexyBuffer* plexy_create_buffer_scaled(PlexyConnection* conn,
                                        uint32_t logical_width,
                                        uint32_t logical_height,
                                        uint32_t buffer_scale,
                                        uint32_t format);

void plexy_buffer_get_logical_size(PlexyBuffer* buffer, uint32_t* width, uint32_t* height);

uint32_t plexy_buffer_get_scale(PlexyBuffer* buffer);


PlexyBuffer* plexy_create_buffer_from_dmabuf(PlexyConnection* conn,
                                              int fd,
                                              uint32_t width,
                                              uint32_t height,
                                              uint32_t stride,
                                              uint32_t format,
                                              uint64_t modifier);


int plexy_buffer_update_dmabuf(PlexyBuffer* buffer, int fd, uint32_t stride);

int plexy_window_attach(PlexyWindow* window, PlexyBuffer* buffer);

int plexy_window_commit(PlexyWindow* window);

int plexy_window_commit_with_cursor(PlexyWindow* window, int32_t cursor_x, int32_t cursor_y, int32_t cols, int32_t rows);
int plexy_window_commit_damage(PlexyWindow* window, int32_t x, int32_t y, int32_t width, int32_t height,
                                int32_t cursor_x, int32_t cursor_y, int32_t cols, int32_t rows);

int plexy_window_set_glass_blur(PlexyWindow* window, bool blur_background);

int plexy_window_set_decorations(PlexyWindow* window, bool enable);

int plexy_window_set_type(PlexyWindow* window, uint32_t type);

int plexy_window_set_input_region_mode(PlexyWindow* window, bool enabled);
int plexy_window_input_region_op(PlexyWindow* window,
                                 int32_t x, int32_t y,
                                 int32_t width, int32_t height,
                                 bool add);
int plexy_window_menu_begin(PlexyWindow* window, bool supported, const char* app_title);
int plexy_window_menu_add(PlexyWindow* window, uint32_t menu_id, const char* title);
int plexy_window_menu_add_item(PlexyWindow* window, uint32_t menu_id, uint32_t item_id,
                               const char* label, bool enabled);
int plexy_window_menu_commit(PlexyWindow* window);
int plexy_set_clipboard_text(PlexyConnection* conn, PlexyWindow* window, const char* text);
int plexy_request_clipboard_text(PlexyConnection* conn, PlexyWindow* window);
void plexy_set_clipboard_text_callback(PlexyConnection* conn,
                                       PlexyClipboardTextCallback callback,
                                       void* user_data);

typedef struct {
    void (*created)(PlexyLayerSurface* surface, uint32_t surface_id, void* user_data);
    void (*closed)(PlexyLayerSurface* surface, void* user_data);
    void (*pointer_enter)(PlexyLayerSurface* surface, int32_t x, int32_t y, void* user_data);
    void (*pointer_leave)(PlexyLayerSurface* surface, void* user_data);
    void (*pointer_motion)(PlexyLayerSurface* surface, int32_t x, int32_t y, void* user_data);
    void (*pointer_button)(PlexyLayerSurface* surface, uint32_t button, bool pressed, int32_t x, int32_t y, void* user_data);
} PlexyLayerSurfaceCallbacks;

PlexyLayerSurface* plexy_create_layer_surface(PlexyConnection* conn,
                                               PlexyLayer layer,
                                               PlexyAnchor anchor,
                                               uint32_t width,
                                               uint32_t height,
                                               uint32_t exclusive_zone,
                                               int32_t margin_x,
                                               int32_t margin_y);

void plexy_destroy_layer_surface(PlexyLayerSurface* surface);

uint32_t plexy_layer_surface_get_id(PlexyLayerSurface* surface);

void plexy_layer_surface_get_size(PlexyLayerSurface* surface, uint32_t* width, uint32_t* height);

void plexy_layer_surface_set_callbacks(PlexyLayerSurface* surface, const PlexyLayerSurfaceCallbacks* callbacks, void* user_data);

void plexy_layer_surface_set_user_data(PlexyLayerSurface* surface, void* user_data);

void* plexy_layer_surface_get_user_data(PlexyLayerSurface* surface);

int plexy_layer_surface_attach(PlexyLayerSurface* surface, PlexyBuffer* buffer);

int plexy_layer_surface_commit(PlexyLayerSurface* surface);

typedef void (*PlexyWindowEventCallback)(PlexyConnection* conn, 
                                          uint32_t window_id,
                                          PlexyWindowEventType event_type,
                                          const char* title,
                                          void* user_data);

void plexy_set_window_event_callback(PlexyConnection* conn, PlexyWindowEventCallback callback, void* user_data);

int plexy_get_window_list(PlexyConnection* conn);

int plexy_raise_window(PlexyConnection* conn, uint32_t window_id);
int plexy_activate_window(PlexyConnection* conn, uint32_t window_id);

int plexy_set_cursor_shape(PlexyConnection* conn, uint32_t window_id, uint32_t cursor_shape);

int plexy_minimize_window(PlexyConnection* conn, uint32_t window_id);

int plexy_restore_window(PlexyConnection* conn, uint32_t window_id);

int plexy_window_request_move(PlexyWindow* window);
int plexy_window_request_resize(PlexyWindow* window, uint32_t edges);
int plexy_window_request_maximize(PlexyWindow* window, bool maximized);
int plexy_window_request_minimize(PlexyWindow* window);
int plexy_window_request_fullscreen(PlexyWindow* window, bool fullscreen);
int plexy_window_set_title(PlexyWindow* window, const char* title);
int plexy_window_set_app_id(PlexyWindow* window, const char* app_id);
int plexy_window_set_geometry_hints(PlexyWindow* window, int32_t min_w, int32_t min_h, int32_t max_w, int32_t max_h);
int plexy_session_lock(PlexyConnection* conn);
int plexy_session_unlock(PlexyConnection* conn);

typedef void (*PlexySceneCapsCallback)(PlexyConnection* conn,
                                        uint32_t surface_id,
                                        bool dma_buf_supported,
                                        bool modifiers_supported,
                                        void* user_data);

typedef void (*PlexySceneTextureCallback)(PlexyConnection* conn,
                                           uint32_t width,
                                           uint32_t height,
                                           uint32_t format,
                                           uint64_t modifier,
                                           uint32_t stride,
                                           uint32_t offset,
                                           int fd,
                                           void* user_data);

typedef void (*PlexySceneShmCallback)(PlexyConnection* conn,
                                       uint32_t width,
                                       uint32_t height,
                                       uint32_t stride,
                                       uint32_t format,
                                       void* shm_data,   
                                       uint32_t shm_size,
                                       void* user_data);

typedef void (*PlexyFrameReadyCallback)(PlexyConnection* conn,
                                         uint32_t frame_number,
                                         void* user_data);

void plexy_set_scene_caps_callback(PlexyConnection* conn, PlexySceneCapsCallback callback, void* user_data);

void plexy_set_scene_texture_callback(PlexyConnection* conn, PlexySceneTextureCallback callback, void* user_data);

void plexy_set_scene_shm_callback(PlexyConnection* conn, PlexySceneShmCallback callback, void* user_data);

void plexy_set_frame_ready_callback(PlexyConnection* conn, PlexyFrameReadyCallback callback, void* user_data);


typedef void (*PlexyRawPointerMotionCallback)(PlexyConnection* conn,
    int32_t screen_x, int32_t screen_y, int32_t dx, int32_t dy,
    uint32_t window_id, void* user_data);
typedef void (*PlexyRawPointerButtonCallback)(PlexyConnection* conn,
    uint32_t button, uint32_t state, void* user_data);
typedef void (*PlexyRawPointerAxisCallback)(PlexyConnection* conn,
    int32_t axis, int32_t value, int32_t discrete, void* user_data);

void plexy_set_raw_pointer_motion_callback(PlexyConnection* conn, PlexyRawPointerMotionCallback cb, void* user_data);
void plexy_set_raw_pointer_button_callback(PlexyConnection* conn, PlexyRawPointerButtonCallback cb, void* user_data);
void plexy_set_raw_pointer_axis_callback(PlexyConnection* conn, PlexyRawPointerAxisCallback cb, void* user_data);

typedef void (*PlexyWindowPositionCallback)(PlexyConnection* conn,
    uint32_t window_id, int32_t screen_x, int32_t screen_y,
    int32_t width, int32_t height, uint8_t visible, void* user_data);
void plexy_set_window_position_callback(PlexyConnection* conn, PlexyWindowPositionCallback cb, void* user_data);

typedef struct PlexyTransparentSurface PlexyTransparentSurface;

PlexyTransparentSurface* plexy_create_transparent_surface(
    PlexyConnection* conn,
    PlexyBuffer* buffer,
    float x, float y,
    float blur_radius,
    float refraction_strength,
    bool chromatic_aberration);

int plexy_update_transparent_surface(
    PlexyTransparentSurface* surface,
    float x, float y,
    float blur_radius,
    float refraction_strength,
    bool chromatic_aberration);

void plexy_destroy_transparent_surface(PlexyTransparentSurface* surface);

uint32_t plexy_transparent_surface_get_id(PlexyTransparentSurface* surface);

#define PLEXY_FORMAT_ARGB8888 0
#define PLEXY_FORMAT_XRGB8888 1
#define PLEXY_FORMAT_RGBA8888 2

#define PLEXY_BTN_LEFT   0x110
#define PLEXY_BTN_RIGHT  0x111
#define PLEXY_BTN_MIDDLE 0x112

#define PLEXY_MOD_SHIFT   (1 << 0)
#define PLEXY_MOD_CTRL    (1 << 1)
#define PLEXY_MOD_ALT     (1 << 2)
#define PLEXY_MOD_SUPER   (1 << 3)

#ifdef __cplusplus
}
#endif

#endif
