#define _DEFAULT_SOURCE
#define _POSIX_C_SOURCE 200809L
#include "../../include/plexy.h"
#include "../../include/plexy_protocol.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>

struct PlexyBuffer {
    uint32_t id;
    uint32_t width;
    uint32_t height;
    uint32_t stride;
    uint32_t format;
    uint32_t buffer_scale;
    uint32_t logical_width;
    uint32_t logical_height;
    void* data;
    size_t size;
    int shm_fd;
    char shm_name[64];
    PlexyConnection* conn;
    int is_dmabuf;           
    int dmabuf_fd;           
    uint64_t dmabuf_modifier;
};

struct PlexyWindow {
    uint32_t id;
    uint32_t width;
    uint32_t height;
    float scale_factor;
    uint32_t buffer_scale;
    uint32_t output_ids[4];
    uint32_t output_count;
    PlexyConnection* conn;
    PlexyBuffer* attached_buffer;
    PlexyWindowCallbacks callbacks;
    void* user_data;
    uint32_t pending_serial;
};

struct PlexyLayerSurface {
    uint32_t id;
    uint32_t width;
    uint32_t height;
    PlexyConnection* conn;
    PlexyBuffer* attached_buffer;
    PlexyLayerSurfaceCallbacks callbacks;
    void* user_data;
};

struct PlexyTransparentSurface {
    uint32_t id;
    PlexyConnection* conn;
    PlexyBuffer* buffer;
};

struct PlexyConnection {
    int fd;
    uint32_t client_id;
    float ui_scale;
    uint32_t screen_width;
    uint32_t screen_height;
    PlexyWindow** windows;
    uint32_t window_count;
    uint32_t window_capacity;
    PlexyLayerSurface** layer_surfaces;
    uint32_t layer_surface_count;
    uint32_t layer_surface_capacity;
    PlexyTransparentSurface** transparent_surfaces;
    uint32_t transparent_surface_count;
    uint32_t transparent_surface_capacity;
    uint32_t next_buffer_id;
    uint32_t next_transparent_surface_id;
    char recv_buf[4096];
    size_t recv_len;
    PlexyOutputInfo* outputs;
    uint32_t output_count;
    uint32_t output_capacity;
    uint64_t output_generation;
    int scene_shm_fd;
    void* scene_shm_data;
    uint32_t scene_shm_size;
    PlexyWindowEventCallback window_event_cb;
    void* window_event_user_data;
    PlexySceneCapsCallback scene_caps_cb;
    void* scene_caps_user_data;
    PlexySceneTextureCallback scene_texture_cb;
    void* scene_texture_user_data;
    PlexySceneShmCallback scene_shm_cb;
    void* scene_shm_user_data;
    PlexyFrameReadyCallback frame_ready_cb;
    void* frame_ready_user_data;
    PlexyClipboardTextCallback clipboard_text_cb;
    void* clipboard_text_user_data;
    char clipboard_text[1024];
    PlexyRawPointerMotionCallback raw_pointer_motion_cb;
    void* raw_pointer_motion_user_data;
    PlexyRawPointerButtonCallback raw_pointer_button_cb;
    void* raw_pointer_button_user_data;
    PlexyRawPointerAxisCallback raw_pointer_axis_cb;
    void* raw_pointer_axis_user_data;
    PlexyWindowPositionCallback window_position_cb;
    void* window_position_user_data;
    int dark_mode;
    PlexyDarkModeCallback dark_mode_cb;
    void* dark_mode_user_data;
};

enum {
    PLEXY_MENU_ID_APP  = 1,
    PLEXY_MENU_ID_FILE = 2,
    PLEXY_MENU_ID_EDIT = 3,
    PLEXY_MENU_ID_VIEW = 4,
};

enum {
    PLEXY_MENU_ITEM_ABOUT      = 1001,
    PLEXY_MENU_ITEM_QUIT       = 1002,
    PLEXY_MENU_ITEM_CLOSE      = 1101,
    PLEXY_MENU_ITEM_COPY       = 1201,
    PLEXY_MENU_ITEM_CUT        = 1202,
    PLEXY_MENU_ITEM_PASTE      = 1203,
    PLEXY_MENU_ITEM_SELECT_ALL = 1204,
};

static int send_message(PlexyConnection* conn, const void* data, size_t len) {
    ssize_t sent = send(conn->fd, data, len, MSG_NOSIGNAL);
    if (sent != (ssize_t)len) {
        return -1;
    }
    return 0;
}

static int update_raw_input_subscription(PlexyConnection* conn) {
    if (!conn) return -1;

    uint32_t mask = 0;
    if (conn->raw_pointer_motion_cb) mask |= PLEXY_RAW_INPUT_EVENT_POINTER_MOTION;
    if (conn->raw_pointer_button_cb) mask |= PLEXY_RAW_INPUT_EVENT_POINTER_BUTTON;
    if (conn->raw_pointer_axis_cb) mask |= PLEXY_RAW_INPUT_EVENT_POINTER_AXIS;

    PlexySubscribeRawInput msg;
    memset(&msg, 0, sizeof(msg));
    msg.header.opcode = PLEXY_OP_SUBSCRIBE_RAW_INPUT;
    msg.header.length = sizeof(msg);
    msg.event_mask = mask;
    return send_message(conn, &msg, sizeof(msg));
}

static int send_message_with_fd(PlexyConnection* conn, const void* data, size_t len, int fd) {
    struct msghdr msg = {0};
    struct iovec iov;
    char cmsgbuf[CMSG_SPACE(sizeof(int))];
    
    iov.iov_base = (void*)data;
    iov.iov_len = len;
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    
    if (fd >= 0) {
        msg.msg_control = cmsgbuf;
        msg.msg_controllen = sizeof(cmsgbuf);
        struct cmsghdr* cmsg = CMSG_FIRSTHDR(&msg);
        cmsg->cmsg_level = SOL_SOCKET;
        cmsg->cmsg_type = SCM_RIGHTS;
        cmsg->cmsg_len = CMSG_LEN(sizeof(int));
        memcpy(CMSG_DATA(cmsg), &fd, sizeof(int));
    }
    
    ssize_t sent = sendmsg(conn->fd, &msg, MSG_NOSIGNAL);
    if (sent != (ssize_t)len) {
        return -1;
    }
    return 0;
}

static void publish_default_window_menu(PlexyWindow* window, const char* title) {
    if (!window) return;

    const char* app_name = (title && title[0]) ? title : "App";
    (void)plexy_window_menu_begin(window, true, app_name);

    (void)plexy_window_menu_add(window, PLEXY_MENU_ID_APP, app_name);
    (void)plexy_window_menu_add_item(window, PLEXY_MENU_ID_APP, PLEXY_MENU_ITEM_ABOUT, "About", true);
    (void)plexy_window_menu_add_item(window, PLEXY_MENU_ID_APP, PLEXY_MENU_ITEM_QUIT, "Quit", true);

    (void)plexy_window_menu_add(window, PLEXY_MENU_ID_FILE, "File");
    (void)plexy_window_menu_add_item(window, PLEXY_MENU_ID_FILE, PLEXY_MENU_ITEM_CLOSE, "Close Window", true);

    (void)plexy_window_menu_add(window, PLEXY_MENU_ID_EDIT, "Edit");
    (void)plexy_window_menu_add_item(window, PLEXY_MENU_ID_EDIT, PLEXY_MENU_ITEM_COPY, "Copy", true);
    (void)plexy_window_menu_add_item(window, PLEXY_MENU_ID_EDIT, PLEXY_MENU_ITEM_CUT, "Cut", true);
    (void)plexy_window_menu_add_item(window, PLEXY_MENU_ID_EDIT, PLEXY_MENU_ITEM_PASTE, "Paste", true);
    (void)plexy_window_menu_add_item(window, PLEXY_MENU_ID_EDIT, PLEXY_MENU_ITEM_SELECT_ALL, "Select All", true);

    (void)plexy_window_menu_add(window, PLEXY_MENU_ID_VIEW, "View");
    (void)plexy_window_menu_commit(window);
}

static PlexyWindow* find_window(PlexyConnection* conn, uint32_t id) {
    for (uint32_t i = 0; i < conn->window_count; i++) {
        if (conn->windows[i] && conn->windows[i]->id == id) {
            return conn->windows[i];
        }
    }
    return NULL;
}

static void add_window(PlexyConnection* conn, PlexyWindow* window) {
    if (conn->window_count >= conn->window_capacity) {
        uint32_t new_cap = conn->window_capacity ? conn->window_capacity * 2 : 4;
        PlexyWindow** new_arr = realloc(conn->windows, new_cap * sizeof(PlexyWindow*));
        if (!new_arr) return;
        conn->windows = new_arr;
        conn->window_capacity = new_cap;
    }
    conn->windows[conn->window_count++] = window;
}

static void remove_window(PlexyConnection* conn, PlexyWindow* window) {
    for (uint32_t i = 0; i < conn->window_count; i++) {
        if (conn->windows[i] == window) {
            conn->windows[i] = conn->windows[--conn->window_count];
            return;
        }
    }
}

static PlexyLayerSurface* find_layer_surface(PlexyConnection* conn, uint32_t surface_id) {
    for (uint32_t i = 0; i < conn->layer_surface_count; i++) {
        if (conn->layer_surfaces[i]->id == surface_id) {
            return conn->layer_surfaces[i];
        }
    }
    return NULL;
}

static void handle_message(PlexyConnection* conn, const PlexyHeader* header, const void* payload, int fd) {
    switch (header->opcode) {
        case PLEXY_OP_WINDOW_CREATED: {
            break;
        }
        case PLEXY_OP_CONFIGURE: {
            const PlexyConfigure* cfg = (const PlexyConfigure*)payload;
            PlexyWindow* win = find_window(conn, cfg->window_id);
            if (win) {
                win->pending_serial = cfg->serial;
                win->width = cfg->width;
                win->height = cfg->height;
                
                if (cfg->scale_factor != win->scale_factor || cfg->buffer_scale != win->buffer_scale) {
                    win->scale_factor = cfg->scale_factor;
                    win->buffer_scale = cfg->buffer_scale;
                    
                    if (win->callbacks.scale_changed) {
                        win->callbacks.scale_changed(win, win->scale_factor, win->buffer_scale, win->user_data);
                    }
                }
                
                if (win->callbacks.configure) {
                    win->callbacks.configure(win, cfg->width, cfg->height, cfg->state_flags, win->user_data);
                }
            }
            break;
        }
        case PLEXY_OP_WINDOW_CLOSED: {
            const PlexyWindowClosed* closed = (const PlexyWindowClosed*)payload;
            PlexyWindow* win = find_window(conn, closed->window_id);
            if (win && win->callbacks.close) {
                win->callbacks.close(win, win->user_data);
            }
            break;
        }
        case PLEXY_OP_POINTER_ENTER: {
            const PlexyPointerEnter* evt = (const PlexyPointerEnter*)payload;
            PlexyWindow* win = find_window(conn, evt->window_id);
            if (win && win->callbacks.pointer_enter) {
                win->callbacks.pointer_enter(win, evt->x, evt->y, win->user_data);
            }
            break;
        }
        case PLEXY_OP_POINTER_LEAVE: {
            const PlexyPointerLeave* evt = (const PlexyPointerLeave*)payload;
            PlexyWindow* win = find_window(conn, evt->window_id);
            if (win && win->callbacks.pointer_leave) {
                win->callbacks.pointer_leave(win, win->user_data);
            }
            break;
        }
        case PLEXY_OP_POINTER_MOTION: {
            const PlexyPointerMotion* evt = (const PlexyPointerMotion*)payload;
            PlexyWindow* win = find_window(conn, evt->window_id);
            if (win && win->callbacks.pointer_motion) {
                win->callbacks.pointer_motion(win, evt->x, evt->y, win->user_data);
            }
            break;
        }
        case PLEXY_OP_POINTER_BUTTON: {
            const PlexyPointerButton* evt = (const PlexyPointerButton*)payload;
            PlexyWindow* win = find_window(conn, evt->window_id);
            if (win && win->callbacks.pointer_button) {
                win->callbacks.pointer_button(win, evt->button, evt->state != 0, evt->x, evt->y, win->user_data);
            }
            break;
        }
        case PLEXY_OP_POINTER_AXIS: {
            const PlexyPointerAxis* evt = (const PlexyPointerAxis*)payload;
            PlexyWindow* win = find_window(conn, evt->window_id);
            if (win && win->callbacks.pointer_axis) {
                win->callbacks.pointer_axis(win, evt->axis, evt->value, evt->discrete, win->user_data);
            }
            break;
        }
        case PLEXY_OP_KEY: {
            const PlexyKey* evt = (const PlexyKey*)payload;
            PlexyWindow* win = find_window(conn, evt->window_id);
            if (win && win->callbacks.key) {
                win->callbacks.key(win, evt->keycode, evt->state != 0, evt->modifiers, win->user_data);
            }
            break;
        }
        case PLEXY_OP_MODIFIERS: {
            const PlexyModifiers* evt = (const PlexyModifiers*)payload;
            PlexyWindow* win = find_window(conn, evt->window_id);
            if (win && win->callbacks.modifiers) {
                win->callbacks.modifiers(win, evt->depressed, evt->latched, evt->locked, evt->group, win->user_data);
            }
            break;
        }
        case PLEXY_OP_FOCUS_IN: {
            const PlexyFocusIn* evt = (const PlexyFocusIn*)payload;
            PlexyWindow* win = find_window(conn, evt->window_id);
            if (win && win->callbacks.focus_in) {
                win->callbacks.focus_in(win, win->user_data);
            }
            break;
        }
        case PLEXY_OP_FOCUS_OUT: {
            const PlexyFocusOut* evt = (const PlexyFocusOut*)payload;
            PlexyWindow* win = find_window(conn, evt->window_id);
            if (win && win->callbacks.focus_out) {
                win->callbacks.focus_out(win, win->user_data);
            }
            break;
        }
        case PLEXY_OP_FRAME_DONE: {
            const PlexyFrameDone* evt = (const PlexyFrameDone*)payload;
            PlexyWindow* win = find_window(conn, evt->window_id);
            if (win && win->callbacks.frame_done) {
                win->callbacks.frame_done(win, win->user_data);
            }
            break;
        }
        case PLEXY_OP_APP_MENU_ACTIVATE: {
            const PlexyAppMenuActivate* evt = (const PlexyAppMenuActivate*)payload;
            PlexyWindow* win = find_window(conn, evt->window_id);
            if (!win) break;
            if (win->callbacks.menu_action) {
                win->callbacks.menu_action(win, evt->item_id, win->user_data);
                break;
            }

            if (evt->item_id == PLEXY_MENU_ITEM_QUIT || evt->item_id == PLEXY_MENU_ITEM_CLOSE) {
                if (win->callbacks.close) {
                    win->callbacks.close(win, win->user_data);
                }
                break;
            }

            if (win->callbacks.key) {
                uint32_t keycode = 0;
                switch (evt->item_id) {
                    case PLEXY_MENU_ITEM_COPY: keycode = 46; break;       
                    case PLEXY_MENU_ITEM_CUT: keycode = 45; break;        
                    case PLEXY_MENU_ITEM_PASTE: keycode = 47; break;      
                    case PLEXY_MENU_ITEM_SELECT_ALL: keycode = 30; break; 
                    default: break;
                }
                if (keycode) {
                    win->callbacks.key(win, keycode, true, PLEXY_MOD_CTRL, win->user_data);
                    win->callbacks.key(win, keycode, false, PLEXY_MOD_CTRL, win->user_data);
                }
            }
            break;
        }
        case PLEXY_OP_CLIPBOARD_TEXT: {
            const PlexyClipboardText* evt = (const PlexyClipboardText*)payload;
            strncpy(conn->clipboard_text, evt->text, sizeof(conn->clipboard_text) - 1);
            conn->clipboard_text[sizeof(conn->clipboard_text) - 1] = '\0';
            if (conn->clipboard_text_cb) {
                conn->clipboard_text_cb(conn, conn->clipboard_text, conn->clipboard_text_user_data);
            }
            break;
        }
        case PLEXY_OP_ERROR: {
            const PlexyErrorMsg* err = (const PlexyErrorMsg*)payload;
            fprintf(stderr, "Plexy error %d: %s\n", err->error_code, err->message);
            break;
        }
        case PLEXY_OP_LAYER_SURFACE_CREATED: {
            const PlexyLayerSurfaceCreated* msg = (const PlexyLayerSurfaceCreated*)payload;
            
            for (uint32_t i = 0; i < conn->layer_surface_count; i++) {
                PlexyLayerSurface* surface = conn->layer_surfaces[i];
                if (surface->id == 0) {
                    surface->id = msg->surface_id;
                    if (surface->callbacks.created) {
                        surface->callbacks.created(surface, msg->surface_id, surface->user_data);
                    }
                    break;
                }
            }
            break;
        }
        case PLEXY_OP_WINDOW_EVENT: {
            const PlexyWindowEvent* evt = (const PlexyWindowEvent*)payload;
            if (conn->window_event_cb) {
                conn->window_event_cb(conn, evt->window_id, evt->event_type, 
                                     evt->title, conn->window_event_user_data);
            }
            break;
        }
        case PLEXY_OP_SCENE_CAPS: {
            const PlexySceneCaps* caps = (const PlexySceneCaps*)payload;
            if (conn->scene_caps_cb) {
                
                conn->scene_caps_cb(conn, 0,
                                   caps->dma_buf_supported != 0,
                                   caps->modifiers_supported != 0,
                                   conn->scene_caps_user_data);
            }
            break;
        }
        case PLEXY_OP_SCENE_TEXTURE_READY: {
            const PlexySceneTextureReady* tex = (const PlexySceneTextureReady*)payload;
            if (conn->scene_texture_cb) {
                conn->scene_texture_cb(conn, tex->width, tex->height, tex->format,
                                      tex->modifier, tex->stride, tex->offset, fd,
                                      conn->scene_texture_user_data);
            }
            break;
        }
        case PLEXY_OP_SCENE_SHM_READY: {
            const PlexySceneShmReady* shm = (const PlexySceneShmReady*)payload;
            
            if (conn->scene_shm_data) {
                munmap(conn->scene_shm_data, conn->scene_shm_size);
                conn->scene_shm_data = NULL;
            }
            if (conn->scene_shm_fd >= 0) {
                close(conn->scene_shm_fd);
                conn->scene_shm_fd = -1;
            }
            
            if (fd >= 0) {
                conn->scene_shm_fd = fd;
                conn->scene_shm_size = shm->shm_size;
                conn->scene_shm_data = mmap(NULL, shm->shm_size, PROT_READ, MAP_SHARED, fd, 0);
                if (conn->scene_shm_data == MAP_FAILED) {
                    fprintf(stderr, "[plexy.c] Failed to mmap scene SHM: %s\n", strerror(errno));
                    conn->scene_shm_data = NULL;
                    close(fd);
                    conn->scene_shm_fd = -1;
                } else {
                    if (conn->scene_shm_cb) {
                        conn->scene_shm_cb(conn, shm->width, shm->height, shm->stride,
                                          shm->format, conn->scene_shm_data, shm->shm_size,
                                          conn->scene_shm_user_data);
                    }
                }
            }
            break;
        }
        case PLEXY_OP_FRAME_READY: {
            const PlexyFrameReady* frame = (const PlexyFrameReady*)payload;
            if (conn->frame_ready_cb) {
                conn->frame_ready_cb(conn, frame->frame_number, conn->frame_ready_user_data);
            }
            break;
        }
        case PLEXY_OP_OUTPUT_INFO: {
            const PlexyOutputInfo* info = (const PlexyOutputInfo*)payload;
            
            PlexyOutputInfo* output = NULL;
            for (uint32_t i = 0; i < conn->output_count; i++) {
                if (conn->outputs[i].output_id == info->output_id) {
                    output = &conn->outputs[i];
                    break;
                }
            }
            
            if (!output) {
                if (conn->output_count >= conn->output_capacity) {
                    uint32_t new_cap = conn->output_capacity ? conn->output_capacity * 2 : 4;
                    PlexyOutputInfo* new_outputs = realloc(conn->outputs, new_cap * sizeof(PlexyOutputInfo));
                    if (!new_outputs) break;
                    conn->outputs = new_outputs;
                    conn->output_capacity = new_cap;
                }
                output = &conn->outputs[conn->output_count++];
            }
            
            memcpy(output, info, sizeof(PlexyOutputInfo));
            break;
        }
        case PLEXY_OP_OUTPUT_SCALE: {
            const PlexyOutputScale* scale = (const PlexyOutputScale*)payload;
            
            for (uint32_t i = 0; i < conn->output_count; i++) {
                if (conn->outputs[i].output_id == scale->output_id) {
                    conn->outputs[i].scale_factor = scale->scale_factor;
                    break;
                }
            }
            break;
        }
        case PLEXY_OP_OUTPUT_DONE: {
            conn->output_generation++;
            break;
        }
        case PLEXY_OP_SURFACE_ENTER_OUTPUT: {
            const PlexySurfaceEnterOutput* evt = (const PlexySurfaceEnterOutput*)payload;
            PlexyWindow* win = find_window(conn, evt->window_id);
            if (win && win->output_count < 4) {
                win->output_ids[win->output_count++] = evt->output_id;
                if (win->callbacks.enter_output) {
                    win->callbacks.enter_output(win, evt->output_id, win->user_data);
                }
            }
            break;
        }
        case PLEXY_OP_SURFACE_LEAVE_OUTPUT: {
            const PlexySurfaceLeaveOutput* evt = (const PlexySurfaceLeaveOutput*)payload;
            PlexyWindow* win = find_window(conn, evt->window_id);
            if (win) {
                
                for (uint32_t i = 0; i < win->output_count; i++) {
                    if (win->output_ids[i] == evt->output_id) {
                        memmove(&win->output_ids[i], &win->output_ids[i + 1],
                               (win->output_count - i - 1) * sizeof(uint32_t));
                        win->output_count--;
                        break;
                    }
                }
                if (win->callbacks.leave_output) {
                    win->callbacks.leave_output(win, evt->output_id, win->user_data);
                }
            }
            break;
        }
        case PLEXY_OP_PREFERRED_BUFFER_SCALE: {
            const PlexyPreferredBufferScale* scale = (const PlexyPreferredBufferScale*)payload;
            PlexyWindow* win = find_window(conn, scale->window_id);
            if (win) {
                float old_scale = win->scale_factor;
                uint32_t old_buffer_scale = win->buffer_scale;
                win->scale_factor = scale->exact_scale;
                win->buffer_scale = scale->preferred_scale;
                
                if ((old_scale != win->scale_factor || old_buffer_scale != win->buffer_scale) &&
                    win->callbacks.scale_changed) {
                    win->callbacks.scale_changed(win, win->scale_factor, win->buffer_scale, win->user_data);
                }
            }
            break;
        }
        case PLEXY_OP_RAW_POINTER_MOTION: {
            const PlexyRawPointerMotion* evt = (const PlexyRawPointerMotion*)payload;
            if (conn->raw_pointer_motion_cb) {
                conn->raw_pointer_motion_cb(conn, evt->screen_x, evt->screen_y,
                                            evt->dx, evt->dy, evt->window_id,
                                            conn->raw_pointer_motion_user_data);
            }
            break;
        }
        case PLEXY_OP_RAW_POINTER_BUTTON: {
            const PlexyRawPointerButton* evt = (const PlexyRawPointerButton*)payload;
            if (conn->raw_pointer_button_cb) {
                conn->raw_pointer_button_cb(conn, evt->button, evt->state,
                                            conn->raw_pointer_button_user_data);
            }
            break;
        }
        case PLEXY_OP_RAW_POINTER_AXIS: {
            const PlexyRawPointerAxis* evt = (const PlexyRawPointerAxis*)payload;
            if (conn->raw_pointer_axis_cb) {
                conn->raw_pointer_axis_cb(conn, evt->axis, evt->value, evt->discrete,
                                          conn->raw_pointer_axis_user_data);
            }
            break;
        }
        case PLEXY_OP_WINDOW_POSITION: {
            const PlexyWindowPosition* evt = (const PlexyWindowPosition*)payload;
            if (conn->window_position_cb) {
                conn->window_position_cb(conn, evt->window_id, evt->screen_x, evt->screen_y,
                                         evt->width, evt->height, evt->visible,
                                         conn->window_position_user_data);
            }
            break;
        }
        case PLEXY_OP_WINDOW_STATE_CHANGED: {
            const PlexyWindowStateChanged* msg = (const PlexyWindowStateChanged*)payload;
            PlexyWindow* win = find_window(conn, msg->window_id);
            if (win && win->callbacks.configure) {
                
                win->callbacks.configure(win, 0, 0, msg->state_flags, win->user_data);
            }
            break;
        }
        case PLEXY_OP_DARK_MODE_CHANGED: {
            const PlexyDarkModeChanged* msg = (const PlexyDarkModeChanged*)payload;
            conn->dark_mode = (int)msg->dark;
            if (conn->dark_mode_cb) {
                conn->dark_mode_cb(conn, conn->dark_mode, conn->dark_mode_user_data);
            }
            break;
        }
        default:
            break;
    }
}

PlexyConnection* plexy_connect(const char* socket_path) {
    char default_path[256];
    
    if (!socket_path) {
        const char* env_socket = getenv("PLEXY_SOCKET");
        if (env_socket && strlen(env_socket) > 0) {
            socket_path = env_socket;
        } else {
            const char* runtime_dir = getenv("XDG_RUNTIME_DIR");
            if (runtime_dir && strlen(runtime_dir) > 0) {
                snprintf(default_path, sizeof(default_path), "%s/plexy.sock", runtime_dir);
            } else {
                snprintf(default_path, sizeof(default_path), "/tmp/plexy-%d.sock", getuid());
            }
            socket_path = default_path;
        }
    }
    
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) {
        return NULL;
    }
    
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);
    
    if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(fd);
        return NULL;
    }
    
    PlexyConnection* conn = calloc(1, sizeof(PlexyConnection));
    if (!conn) {
        close(fd);
        return NULL;
    }
    
    conn->fd = fd;
    conn->next_buffer_id = 1;
    conn->next_transparent_surface_id = 1;
    conn->scene_shm_fd = -1;
    
    PlexyHello hello;
    memset(&hello, 0, sizeof(hello));
    hello.header.opcode = PLEXY_OP_HELLO;
    hello.header.length = sizeof(hello);
    hello.version = PLEXY_PROTOCOL_VERSION;
    strncpy(hello.name, "plexy-client", sizeof(hello.name) - 1);
    
    if (send_message(conn, &hello, sizeof(hello)) < 0) {
        close(fd);
        free(conn);
        return NULL;
    }
    
    PlexyHelloReply reply;
    ssize_t n = recv(fd, &reply, sizeof(reply), 0);
    if (n != sizeof(reply) || reply.header.opcode != PLEXY_OP_HELLO_REPLY) {
        close(fd);
        free(conn);
        return NULL;
    }
    
    conn->client_id = reply.client_id;
    conn->ui_scale = reply.ui_scale;
    conn->screen_width = reply.screen_width;
    conn->screen_height = reply.screen_height;
    conn->dark_mode = (int)reply.dark_mode;
    
    return conn;
}

void plexy_disconnect(PlexyConnection* conn) {
    if (!conn) return;
    
    for (uint32_t i = 0; i < conn->window_count; i++) {
        if (conn->windows[i]) {
            free(conn->windows[i]);
        }
    }
    free(conn->windows);
    
    free(conn->outputs);
    
    if (conn->fd >= 0) {
        close(conn->fd);
    }
    
    free(conn);
}

int plexy_get_fd(PlexyConnection* conn) {
    return conn ? conn->fd : -1;
}

float plexy_get_ui_scale(PlexyConnection* conn) {
    return conn ? conn->ui_scale : 1.0f;
}

void plexy_get_screen_size(PlexyConnection* conn, uint32_t* width, uint32_t* height) {
    if (!conn) {
        if (width) *width = 1920;
        if (height) *height = 1080;
        return;
    }
    if (width) *width = conn->screen_width;
    if (height) *height = conn->screen_height;
}

int plexy_is_dark_mode(PlexyConnection* conn) {
    return conn ? conn->dark_mode : 0;
}

void plexy_set_dark_mode_callback(PlexyConnection* conn, PlexyDarkModeCallback cb, void* user_data) {
    if (!conn) return;
    conn->dark_mode_cb = cb;
    conn->dark_mode_user_data = user_data;
}

int plexy_dispatch(PlexyConnection* conn) {
    if (!conn) return -1;

    PlexyHeader hdr;
    ssize_t peeked = recv(conn->fd, &hdr, sizeof(hdr), MSG_PEEK);
    if (peeked <= 0) {
        if (peeked < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
            return 0;  
        return peeked == 0 ? 0 : -1;
    }
    if (peeked < (ssize_t)sizeof(hdr)) {
        return 1; 
    }
    if (hdr.length < sizeof(PlexyHeader) || hdr.length > sizeof(conn->recv_buf)) {
        fprintf(stderr, "[plexy.c] ERROR: Invalid message length %u\n", hdr.length);
        return -1;
    }

    char cmsgbuf[CMSG_SPACE(sizeof(int) * 8)];
    struct msghdr msg = {0};
    struct iovec iov;

    iov.iov_base = conn->recv_buf;
    iov.iov_len = hdr.length;
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_control = cmsgbuf;
    msg.msg_controllen = sizeof(cmsgbuf);

    ssize_t n = recvmsg(conn->fd, &msg, 0);
    if (n <= 0) {
        if (n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
            return 0;
        return n == 0 ? 0 : -1;
    }
    if (n < (ssize_t)sizeof(PlexyHeader)) {
        return 1;
    }

    int received_fd = -1;
    for (struct cmsghdr* cmsg = CMSG_FIRSTHDR(&msg);
         cmsg != NULL;
         cmsg = CMSG_NXTHDR(&msg, cmsg)) {
        if (cmsg->cmsg_level == SOL_SOCKET && cmsg->cmsg_type == SCM_RIGHTS) {
            int* fds = (int*)CMSG_DATA(cmsg);
            int count = (cmsg->cmsg_len - CMSG_LEN(0)) / sizeof(int);
            if (count > 0) {
                received_fd = fds[0];
            }
        }
    }

    PlexyHeader* header = (PlexyHeader*)conn->recv_buf;
    
    bool fd_expected = (header->opcode == PLEXY_OP_SCENE_TEXTURE_READY) ||
                       (header->opcode == PLEXY_OP_SCENE_SHM_READY) ||
                       (header->opcode == PLEXY_OP_ATTACH_BUFFER) ||
                       (header->opcode == PLEXY_OP_ATTACH_LAYER_BUFFER);

    handle_message(conn, header, conn->recv_buf, received_fd);

    if (received_fd >= 0 && !fd_expected) {
        close(received_fd);
    }

    return 1;
}

int plexy_dispatch_pending(PlexyConnection* conn) {
    if (!conn) return -1;
    
    int flags = fcntl(conn->fd, F_GETFL, 0);
    fcntl(conn->fd, F_SETFL, flags | O_NONBLOCK);
    
    int result;
    while ((result = plexy_dispatch(conn)) > 0)
        ;
    
    fcntl(conn->fd, F_SETFL, flags);
    
    return result >= 0 ? 0 : -1;
}

int plexy_flush(PlexyConnection* conn) {
    return conn ? 0 : -1;
}

int plexy_roundtrip(PlexyConnection* conn) {
    if (!conn) return -1;
    plexy_flush(conn);
    return plexy_dispatch(conn);
}

PlexyWindow* plexy_create_window(PlexyConnection* conn, 
                                  int32_t x, int32_t y,
                                  uint32_t width, uint32_t height,
                                  const char* title) {
    if (!conn) return NULL;
    
    PlexyWindow* window = calloc(1, sizeof(PlexyWindow));
    if (!window) return NULL;
    
    window->conn = conn;
    window->width = width;
    window->height = height;
    window->scale_factor = conn->ui_scale;
    window->buffer_scale = 1;  
    
    PlexyCreateWindow msg;
    memset(&msg, 0, sizeof(msg));
    msg.header.opcode = PLEXY_OP_CREATE_WINDOW;
    msg.header.length = sizeof(msg);
    msg.x = x;
    msg.y = y;
    msg.width = width;
    msg.height = height;
    msg.buffer_scale = 0;  
    if (title) {
        strncpy(msg.title, title, sizeof(msg.title) - 1);
    }
    
    if (send_message(conn, &msg, sizeof(msg)) < 0) {
        free(window);
        return NULL;
    }

    
    PlexyWindowCreated reply;
    int max_attempts = 100;  
    while (max_attempts-- > 0) {
        
        PlexyHeader hdr;
        ssize_t peeked = recv(conn->fd, &hdr, sizeof(hdr), MSG_PEEK);
        if (peeked < (ssize_t)sizeof(hdr)) {
            free(window);
            return NULL;
        }

        if (hdr.opcode == PLEXY_OP_WINDOW_CREATED) {
            
            ssize_t n = recv(conn->fd, &reply, sizeof(reply), 0);
            if (n != sizeof(reply)) {
                free(window);
                return NULL;
            }
            break;
        } else {
            
            char skip_buf[512];
            size_t to_read = hdr.length < sizeof(skip_buf) ? hdr.length : sizeof(skip_buf);
            ssize_t skipped = recv(conn->fd, skip_buf, to_read, 0);
            if (skipped <= 0) {
                free(window);
                return NULL;
            }
            
            while ((size_t)skipped < hdr.length) {
                size_t remaining = hdr.length - skipped;
                size_t chunk = remaining < sizeof(skip_buf) ? remaining : sizeof(skip_buf);
                ssize_t r = recv(conn->fd, skip_buf, chunk, 0);
                if (r <= 0) {
                    free(window);
                    return NULL;
                }
                skipped += r;
            }
        }
    }

    if (max_attempts <= 0) {
        free(window);
        return NULL;
    }

    window->id = reply.window_id;
    window->width = reply.width;
    window->height = reply.height;
    window->scale_factor = reply.scale_factor;
    window->buffer_scale = reply.buffer_scale;
    
    add_window(conn, window);
    publish_default_window_menu(window, title);
    
    return window;
}

void plexy_destroy_window(PlexyWindow* window) {
    if (!window) return;
    
    PlexyDestroyWindow msg;
    msg.header.opcode = PLEXY_OP_DESTROY_WINDOW;
    msg.header.length = sizeof(msg);
    msg.window_id = window->id;
    
    send_message(window->conn, &msg, sizeof(msg));
    
    remove_window(window->conn, window);
    free(window);
}

PlexyWindow* plexy_create_popup(PlexyConnection* conn,
                                PlexyWindow* parent,
                                int32_t x, int32_t y,
                                uint32_t width, uint32_t height) {
    if (!conn) return NULL;
    
    PlexyWindow* window = calloc(1, sizeof(PlexyWindow));
    if (!window) return NULL;
    
    window->conn = conn;
    window->width = width;
    window->height = height;
    window->scale_factor = conn->ui_scale;
    window->buffer_scale = 1;
    
    PlexyCreatePopup msg;
    memset(&msg, 0, sizeof(msg));
    msg.header.opcode = PLEXY_OP_CREATE_POPUP;
    msg.header.length = sizeof(msg);
    msg.parent_window_id = parent ? parent->id : 0;
    msg.x = x;
    msg.y = y;
    msg.width = width;
    msg.height = height;
    msg.buffer_scale = 1;
    msg.flags = PLEXY_POPUP_FLAG_NONE;
    
    if (send_message(conn, &msg, sizeof(msg)) < 0) {
        free(window);
        return NULL;
    }

    
    PlexyPopupCreated reply;
    int max_attempts = 100;  
    while (max_attempts-- > 0) {
        
        PlexyHeader hdr;
        ssize_t peeked = recv(conn->fd, &hdr, sizeof(hdr), MSG_PEEK);
        if (peeked < (ssize_t)sizeof(hdr)) {
            free(window);
            return NULL;
        }

        if (hdr.opcode == PLEXY_OP_POPUP_CREATED) {
            
            ssize_t n = recv(conn->fd, &reply, sizeof(reply), 0);
            if (n != sizeof(reply)) {
                free(window);
                return NULL;
            }
            break;
        } else {
            
            char skip_buf[512];
            size_t to_read = hdr.length < sizeof(skip_buf) ? hdr.length : sizeof(skip_buf);
            ssize_t skipped = recv(conn->fd, skip_buf, to_read, 0);
            if (skipped <= 0) {
                free(window);
                return NULL;
            }
            
            while ((size_t)skipped < hdr.length) {
                size_t remaining = hdr.length - skipped;
                size_t chunk = remaining < sizeof(skip_buf) ? remaining : sizeof(skip_buf);
                ssize_t r = recv(conn->fd, skip_buf, chunk, 0);
                if (r <= 0) {
                    free(window);
                    return NULL;
                }
                skipped += r;
            }
        }
    }

    if (max_attempts <= 0) {
        free(window);
        return NULL;
    }

    window->id = reply.window_id;
    window->width = reply.width;
    window->height = reply.height;
    window->scale_factor = reply.scale_factor;
    window->buffer_scale = reply.buffer_scale;
    
    add_window(conn, window);
    (void)plexy_window_menu_begin(window, false, NULL);
    (void)plexy_window_menu_commit(window);
    
    return window;
}

int plexy_popup_update(PlexyWindow* popup, int32_t x, int32_t y, uint32_t flags) {
    if (!popup || !popup->conn) return -1;

    PlexyUpdatePopup msg;
    memset(&msg, 0, sizeof(msg));
    msg.header.opcode = PLEXY_OP_UPDATE_POPUP;
    msg.header.length = sizeof(msg);
    msg.window_id = popup->id;
    msg.x = x;
    msg.y = y;
    msg.flags = flags;

    return send_message(popup->conn, &msg, sizeof(msg));
}

uint32_t plexy_window_get_id(PlexyWindow* window) {
    return window ? window->id : 0;
}

void plexy_window_get_size(PlexyWindow* window, uint32_t* width, uint32_t* height) {
    if (window) {
        if (width) *width = window->width;
        if (height) *height = window->height;
    }
}

void plexy_window_set_callbacks(PlexyWindow* window, const PlexyWindowCallbacks* callbacks, void* user_data) {
    if (!window) return;
    if (callbacks) {
        window->callbacks = *callbacks;
    } else {
        memset(&window->callbacks, 0, sizeof(window->callbacks));
    }
    window->user_data = user_data;
}

void plexy_window_set_user_data(PlexyWindow* window, void* user_data) {
    if (window) window->user_data = user_data;
}

void* plexy_window_get_user_data(PlexyWindow* window) {
    return window ? window->user_data : NULL;
}

PlexyBuffer* plexy_create_buffer(PlexyConnection* conn, uint32_t width, uint32_t height, uint32_t format) {
    if (!conn) return NULL;
    
    PlexyBuffer* buffer = calloc(1, sizeof(PlexyBuffer));
    if (!buffer) return NULL;
    
    buffer->conn = conn;
    buffer->id = conn->next_buffer_id++;
    buffer->width = width;
    buffer->height = height;
    buffer->format = format;
    buffer->stride = width * 4;
    buffer->size = buffer->stride * height;
    
    snprintf(buffer->shm_name, sizeof(buffer->shm_name), "/plexy-buf-%d-%d", getpid(), buffer->id);
    
    buffer->shm_fd = shm_open(buffer->shm_name, O_RDWR | O_CREAT | O_EXCL, 0600);
    if (buffer->shm_fd < 0) {
        free(buffer);
        return NULL;
    }
    
    if (ftruncate(buffer->shm_fd, buffer->size) < 0) {
        close(buffer->shm_fd);
        shm_unlink(buffer->shm_name);
        free(buffer);
        return NULL;
    }
    
    buffer->data = mmap(NULL, buffer->size, PROT_READ | PROT_WRITE, MAP_SHARED, buffer->shm_fd, 0);
    if (buffer->data == MAP_FAILED) {
        close(buffer->shm_fd);
        shm_unlink(buffer->shm_name);
        free(buffer);
        return NULL;
    }
    
    memset(buffer->data, 0, buffer->size);
    
    return buffer;
}

PlexyBuffer* plexy_create_buffer_from_dmabuf(PlexyConnection* conn,
                                              int fd,
                                              uint32_t width,
                                              uint32_t height,
                                              uint32_t stride,
                                              uint32_t format,
                                              uint64_t modifier) {
    if (!conn || fd < 0) return NULL;
    
    PlexyBuffer* buffer = calloc(1, sizeof(PlexyBuffer));
    if (!buffer) return NULL;
    
    int dup_fd = dup(fd);
    if (dup_fd < 0) {
        free(buffer);
        return NULL;
    }
    
    buffer->conn = conn;
    buffer->id = conn->next_buffer_id++;
    buffer->width = width;
    buffer->height = height;
    buffer->format = format;
    buffer->stride = stride;
    buffer->is_dmabuf = 1;
    buffer->dmabuf_fd = dup_fd;
    buffer->dmabuf_modifier = modifier;
    buffer->shm_fd = -1;       
    buffer->data = NULL;       
    
    return buffer;
}

int plexy_buffer_update_dmabuf(PlexyBuffer* buffer, int fd, uint32_t stride) {
    if (!buffer || !buffer->is_dmabuf || fd < 0) return -1;
    
    int dup_fd = dup(fd);
    if (dup_fd < 0) return -1;
    
    if (buffer->dmabuf_fd >= 0) {
        close(buffer->dmabuf_fd);
    }
    
    buffer->dmabuf_fd = dup_fd;
    buffer->stride = stride;
    
    return 0;
}

void plexy_destroy_buffer(PlexyBuffer* buffer) {
    if (!buffer) return;
    
    if (buffer->data && buffer->data != MAP_FAILED) {
        munmap(buffer->data, buffer->size);
    }
    if (buffer->shm_fd >= 0) {
        close(buffer->shm_fd);
        shm_unlink(buffer->shm_name);
    }
    if (buffer->is_dmabuf && buffer->dmabuf_fd >= 0) {
        close(buffer->dmabuf_fd);
    }
    
    free(buffer);
}

void* plexy_buffer_get_data(PlexyBuffer* buffer) {
    return buffer ? buffer->data : NULL;
}

uint32_t plexy_buffer_get_stride(PlexyBuffer* buffer) {
    return buffer ? buffer->stride : 0;
}

uint32_t plexy_buffer_get_width(PlexyBuffer* buffer) {
    return buffer ? buffer->width : 0;
}

uint32_t plexy_buffer_get_height(PlexyBuffer* buffer) {
    return buffer ? buffer->height : 0;
}

int plexy_window_attach(PlexyWindow* window, PlexyBuffer* buffer) {
    if (!window || !buffer) return -1;
    
    if (buffer->is_dmabuf) {
        
        PlexyAttachDmaBuf msg;
        memset(&msg, 0, sizeof(msg));
        msg.header.opcode = PLEXY_OP_ATTACH_DMABUF;
        msg.header.length = sizeof(msg);
        msg.window_id = window->id;
        msg.width = buffer->width;
        msg.height = buffer->height;
        msg.stride = buffer->stride;
        msg.format = buffer->format;
        msg.modifier = buffer->dmabuf_modifier;
        msg.offset = 0;
        msg.flags = 0;
        
        if (send_message_with_fd(window->conn, &msg, sizeof(msg), buffer->dmabuf_fd) < 0) {
            return -1;
        }
    } else {
        
        PlexyAttachBuffer msg;
        memset(&msg, 0, sizeof(msg));
        msg.header.opcode = PLEXY_OP_ATTACH_BUFFER;
        msg.header.length = sizeof(msg);
        msg.window_id = window->id;
        msg.width = buffer->width;
        msg.height = buffer->height;
        msg.stride = buffer->stride;
        msg.format = buffer->format;
        msg.shm_size = buffer->size;
        
        if (send_message_with_fd(window->conn, &msg, sizeof(msg), buffer->shm_fd) < 0) {
            return -1;
        }
    }
    
    window->attached_buffer = buffer;
    return 0;
}

int plexy_window_commit(PlexyWindow* window) {
    if (!window) return -1;
    
    PlexyCommit msg;
    memset(&msg, 0, sizeof(msg));
    msg.header.opcode = PLEXY_OP_COMMIT;
    msg.header.length = sizeof(msg);
    msg.window_id = window->id;
    msg.cursor_cell_x = -1;
    msg.cursor_cell_y = -1;
    msg.cursor_cols = 0;
    msg.cursor_rows = 0;
    
    msg.damage_x = 0;
    msg.damage_y = 0;
    msg.damage_width = 0;
    msg.damage_height = 0;
    
    return send_message(window->conn, &msg, sizeof(msg));
}

int plexy_window_commit_with_cursor(PlexyWindow* window, int32_t cursor_x, int32_t cursor_y, int32_t cols, int32_t rows) {
    if (!window) return -1;
    
    PlexyCommit msg;
    memset(&msg, 0, sizeof(msg));
    msg.header.opcode = PLEXY_OP_COMMIT;
    msg.header.length = sizeof(msg);
    msg.window_id = window->id;
    msg.cursor_cell_x = cursor_x;
    msg.cursor_cell_y = cursor_y;
    msg.cursor_cols = cols;
    msg.cursor_rows = rows;
    
    msg.damage_x = 0;
    msg.damage_y = 0;
    msg.damage_width = 0;
    msg.damage_height = 0;
    
    return send_message(window->conn, &msg, sizeof(msg));
}

int plexy_window_commit_damage(PlexyWindow* window, int32_t x, int32_t y, int32_t width, int32_t height,
                                int32_t cursor_x, int32_t cursor_y, int32_t cols, int32_t rows) {
    if (!window) return -1;
    
    PlexyCommit msg;
    memset(&msg, 0, sizeof(msg));
    msg.header.opcode = PLEXY_OP_COMMIT;
    msg.header.length = sizeof(msg);
    msg.window_id = window->id;
    msg.cursor_cell_x = cursor_x;
    msg.cursor_cell_y = cursor_y;
    msg.cursor_cols = cols;
    msg.cursor_rows = rows;
    msg.damage_x = x;
    msg.damage_y = y;
    msg.damage_width = width;
    msg.damage_height = height;
    
    return send_message(window->conn, &msg, sizeof(msg));
}

int plexy_window_set_glass_blur(PlexyWindow* window, bool blur_background) {
    if (!window) return -1;
    
    PlexySetGlassEffect msg;
    memset(&msg, 0, sizeof(msg));
    msg.header.opcode = PLEXY_OP_SET_GLASS_EFFECT;
    msg.header.length = sizeof(msg);
    msg.window_id = window->id;
    msg.blur_background = blur_background ? 1 : 0;
    
    return send_message(window->conn, &msg, sizeof(msg));
}

int plexy_window_set_decorations(PlexyWindow* window, bool enable) {
    if (!window) return -1;
    
    PlexySetDecorations msg;
    memset(&msg, 0, sizeof(msg));
    msg.header.opcode = PLEXY_OP_SET_DECORATIONS;
    msg.header.length = sizeof(msg);
    msg.window_id = window->id;
    msg.enabled = enable ? 1 : 0;
    
    return send_message(window->conn, &msg, sizeof(msg));
}

int plexy_window_set_type(PlexyWindow* window, uint32_t type) {
    if (!window) return -1;

    PlexySetWindowType msg;
    memset(&msg, 0, sizeof(msg));
    msg.header.opcode = PLEXY_OP_SET_WINDOW_TYPE;
    msg.header.length = sizeof(msg);
    msg.window_id = window->id;
    msg.window_type = type;

    return send_message(window->conn, &msg, sizeof(msg));
}

int plexy_window_set_input_region_mode(PlexyWindow* window, bool enabled) {
    if (!window) return -1;

    PlexySetInputRegionMode msg;
    memset(&msg, 0, sizeof(msg));
    msg.header.opcode = PLEXY_OP_SET_INPUT_REGION_MODE;
    msg.header.length = sizeof(msg);
    msg.window_id = window->id;
    msg.enabled = enabled ? 1u : 0u;

    return send_message(window->conn, &msg, sizeof(msg));
}

int plexy_window_input_region_op(PlexyWindow* window,
                                 int32_t x, int32_t y,
                                 int32_t width, int32_t height,
                                 bool add) {
    if (!window) return -1;
    if (width <= 0 || height <= 0) return 0;

    PlexyInputRegionOp msg;
    memset(&msg, 0, sizeof(msg));
    msg.header.opcode = PLEXY_OP_INPUT_REGION_OP;
    msg.header.length = sizeof(msg);
    msg.window_id = window->id;
    msg.x = x;
    msg.y = y;
    msg.width = width;
    msg.height = height;
    msg.add = add ? 1u : 0u;

    return send_message(window->conn, &msg, sizeof(msg));
}

int plexy_window_menu_begin(PlexyWindow* window, bool supported, const char* app_title) {
    if (!window) return -1;

    PlexyAppMenuReset msg;
    memset(&msg, 0, sizeof(msg));
    msg.header.opcode = PLEXY_OP_APP_MENU_RESET;
    msg.header.length = sizeof(msg);
    msg.window_id = window->id;
    msg.supported = supported ? 1u : 0u;
    if (app_title) {
        strncpy(msg.app_title, app_title, sizeof(msg.app_title) - 1);
    }
    return send_message(window->conn, &msg, sizeof(msg));
}

int plexy_window_menu_add(PlexyWindow* window, uint32_t menu_id, const char* title) {
    if (!window) return -1;

    PlexyAppMenuAdd msg;
    memset(&msg, 0, sizeof(msg));
    msg.header.opcode = PLEXY_OP_APP_MENU_ADD;
    msg.header.length = sizeof(msg);
    msg.window_id = window->id;
    msg.menu_id = menu_id;
    if (title) {
        strncpy(msg.title, title, sizeof(msg.title) - 1);
    }
    return send_message(window->conn, &msg, sizeof(msg));
}

int plexy_window_menu_add_item(PlexyWindow* window, uint32_t menu_id, uint32_t item_id,
                               const char* label, bool enabled) {
    if (!window) return -1;

    PlexyAppMenuAddItem msg;
    memset(&msg, 0, sizeof(msg));
    msg.header.opcode = PLEXY_OP_APP_MENU_ADD_ITEM;
    msg.header.length = sizeof(msg);
    msg.window_id = window->id;
    msg.menu_id = menu_id;
    msg.item_id = item_id;
    msg.enabled = enabled ? 1u : 0u;
    if (label) {
        strncpy(msg.label, label, sizeof(msg.label) - 1);
    }
    return send_message(window->conn, &msg, sizeof(msg));
}

int plexy_window_menu_commit(PlexyWindow* window) {
    if (!window) return -1;

    PlexyAppMenuCommit msg;
    memset(&msg, 0, sizeof(msg));
    msg.header.opcode = PLEXY_OP_APP_MENU_COMMIT;
    msg.header.length = sizeof(msg);
    msg.window_id = window->id;
    return send_message(window->conn, &msg, sizeof(msg));
}

int plexy_set_clipboard_text(PlexyConnection* conn, PlexyWindow* window, const char* text) {
    if (!conn) return -1;

    PlexyClipboardSetText msg;
    memset(&msg, 0, sizeof(msg));
    msg.header.opcode = PLEXY_OP_CLIPBOARD_SET_TEXT;
    msg.header.length = sizeof(msg);
    msg.window_id = window ? window->id : 0;
    if (text) {
        strncpy(msg.text, text, sizeof(msg.text) - 1);
    }

    return send_message(conn, &msg, sizeof(msg));
}

int plexy_request_clipboard_text(PlexyConnection* conn, PlexyWindow* window) {
    if (!conn) return -1;

    PlexyClipboardGetText msg;
    memset(&msg, 0, sizeof(msg));
    msg.header.opcode = PLEXY_OP_CLIPBOARD_GET_TEXT;
    msg.header.length = sizeof(msg);
    msg.window_id = window ? window->id : 0;

    return send_message(conn, &msg, sizeof(msg));
}

void plexy_set_clipboard_text_callback(PlexyConnection* conn,
                                       PlexyClipboardTextCallback callback,
                                       void* user_data) {
    if (!conn) return;
    conn->clipboard_text_cb = callback;
    conn->clipboard_text_user_data = user_data;
}

PlexyLayerSurface* plexy_create_layer_surface(PlexyConnection* conn,
                                               PlexyLayer layer,
                                               PlexyAnchor anchor,
                                               uint32_t width,
                                               uint32_t height,
                                               uint32_t exclusive_zone,
                                               int32_t margin_x,
                                               int32_t margin_y) {
    if (!conn) return NULL;
    
    PlexyLayerSurface* surface = calloc(1, sizeof(PlexyLayerSurface));
    if (!surface) return NULL;
    
    surface->conn = conn;
    surface->width = width;
    surface->height = height;
    surface->id = 0; 
    
    
    if (conn->layer_surface_count >= conn->layer_surface_capacity) {
        uint32_t new_cap = conn->layer_surface_capacity == 0 ? 4 : conn->layer_surface_capacity * 2;
        PlexyLayerSurface** new_list = realloc(conn->layer_surfaces, new_cap * sizeof(PlexyLayerSurface*));
        if (!new_list) {
            free(surface);
            return NULL;
        }
        conn->layer_surfaces = new_list;
        conn->layer_surface_capacity = new_cap;
    }
    
    conn->layer_surfaces[conn->layer_surface_count++] = surface;
    
    
    PlexyCreateLayerSurface msg;
    memset(&msg, 0, sizeof(msg));
    msg.header.opcode = PLEXY_OP_CREATE_LAYER_SURFACE;
    msg.header.length = sizeof(msg);
    msg.layer = layer;
    msg.anchor = anchor;
    msg.width = width;
    msg.height = height;
    msg.exclusive_zone = exclusive_zone;
    msg.keyboard_interactivity = 0;
    
    if (send_message(conn, &msg, sizeof(msg)) < 0) {
        conn->layer_surface_count--;
        free(surface);
        return NULL;
    }
    
    return surface;
}

void plexy_destroy_layer_surface(PlexyLayerSurface* surface) {
    if (!surface) return;
    
    PlexyConnection* conn = surface->conn;
    
    
    PlexyDestroyLayerSurface msg;
    memset(&msg, 0, sizeof(msg));
    msg.header.opcode = PLEXY_OP_DESTROY_LAYER_SURFACE;
    msg.header.length = sizeof(msg);
    msg.surface_id = surface->id;
    
    send_message(conn, &msg, sizeof(msg));
    
    
    for (uint32_t i = 0; i < conn->layer_surface_count; i++) {
        if (conn->layer_surfaces[i] == surface) {
            conn->layer_surfaces[i] = conn->layer_surfaces[conn->layer_surface_count - 1];
            conn->layer_surface_count--;
            break;
        }
    }
    
    free(surface);
}

uint32_t plexy_layer_surface_get_id(PlexyLayerSurface* surface) {
    return surface ? surface->id : 0;
}

void plexy_layer_surface_get_size(PlexyLayerSurface* surface, uint32_t* width, uint32_t* height) {
    if (!surface) return;
    if (width) *width = surface->width;
    if (height) *height = surface->height;
}

void plexy_layer_surface_set_callbacks(PlexyLayerSurface* surface, 
                                        const PlexyLayerSurfaceCallbacks* callbacks, 
                                        void* user_data) {
    if (!surface) return;
    if (callbacks) {
        surface->callbacks = *callbacks;
    } else {
        memset(&surface->callbacks, 0, sizeof(surface->callbacks));
    }
    surface->user_data = user_data;
}

void plexy_layer_surface_set_user_data(PlexyLayerSurface* surface, void* user_data) {
    if (surface) surface->user_data = user_data;
}

void* plexy_layer_surface_get_user_data(PlexyLayerSurface* surface) {
    return surface ? surface->user_data : NULL;
}

int plexy_layer_surface_attach(PlexyLayerSurface* surface, PlexyBuffer* buffer) {
    if (!surface || !buffer) return -1;
    
    PlexyAttachLayerBuffer msg;
    memset(&msg, 0, sizeof(msg));
    msg.header.opcode = PLEXY_OP_ATTACH_LAYER_BUFFER;
    msg.header.length = sizeof(msg);
    msg.surface_id = surface->id;
    msg.width = buffer->width;
    msg.height = buffer->height;
    msg.stride = buffer->stride;
    msg.format = buffer->format;
    msg.shm_size = buffer->size;
    
    if (send_message_with_fd(surface->conn, &msg, sizeof(msg), buffer->shm_fd) < 0) {
        return -1;
    }
    
    surface->attached_buffer = buffer;
    return 0;
}

int plexy_layer_surface_commit(PlexyLayerSurface* surface) {
    if (!surface) return -1;
    
    PlexyCommitLayerSurface msg;
    memset(&msg, 0, sizeof(msg));
    msg.header.opcode = PLEXY_OP_COMMIT_LAYER_SURFACE;
    msg.header.length = sizeof(msg);
    msg.surface_id = surface->id;
    
    return send_message(surface->conn, &msg, sizeof(msg));
}

void plexy_set_window_event_callback(PlexyConnection* conn, 
                                      PlexyWindowEventCallback callback, 
                                      void* user_data) {
    if (!conn) return;
    conn->window_event_cb = callback;
    conn->window_event_user_data = user_data;
}

int plexy_get_window_list(PlexyConnection* conn) {
    if (!conn) return -1;
    
    PlexyGetWindowList msg;
    memset(&msg, 0, sizeof(msg));
    msg.header.opcode = PLEXY_OP_GET_WINDOW_LIST;
    msg.header.length = sizeof(msg);
    
    return send_message(conn, &msg, sizeof(msg));
}

int plexy_raise_window(PlexyConnection* conn, uint32_t window_id) {
    if (!conn) return -1;
    
    PlexyRaiseWindow msg;
    memset(&msg, 0, sizeof(msg));
    msg.header.opcode = PLEXY_OP_RAISE_WINDOW;
    msg.header.length = sizeof(msg);
    msg.window_id = window_id;
    msg.flags = 0;
    
    return send_message(conn, &msg, sizeof(msg));
}

int plexy_activate_window(PlexyConnection* conn, uint32_t window_id) {
    if (!conn) return -1;
    
    PlexyRaiseWindow msg;
    memset(&msg, 0, sizeof(msg));
    msg.header.opcode = PLEXY_OP_RAISE_WINDOW;
    msg.header.length = sizeof(msg);
    msg.window_id = window_id;
    msg.flags = PLEXY_RAISE_FLAG_ACTIVATE;
    
    return send_message(conn, &msg, sizeof(msg));
}

int plexy_set_cursor_shape(PlexyConnection* conn, uint32_t window_id, uint32_t cursor_shape) {
    if (!conn) return -1;

    PlexySetCursorShape msg;
    memset(&msg, 0, sizeof(msg));
    msg.header.opcode = PLEXY_OP_SET_CURSOR_SHAPE;
    msg.header.length = sizeof(msg);
    msg.window_id = window_id;
    msg.cursor_shape = cursor_shape;

    return send_message(conn, &msg, sizeof(msg));
}

int plexy_minimize_window(PlexyConnection* conn, uint32_t window_id) {
    if (!conn) return -1;

    PlexyMinimizeWindow msg;
    memset(&msg, 0, sizeof(msg));
    msg.header.opcode = PLEXY_OP_MINIMIZE_WINDOW;
    msg.header.length = sizeof(msg);
    msg.window_id = window_id;

    return send_message(conn, &msg, sizeof(msg));
}

int plexy_restore_window(PlexyConnection* conn, uint32_t window_id) {
    if (!conn) return -1;
    
    PlexyRestoreWindow msg;
    memset(&msg, 0, sizeof(msg));
    msg.header.opcode = PLEXY_OP_RESTORE_WINDOW;
    msg.header.length = sizeof(msg);
    msg.window_id = window_id;
    
    return send_message(conn, &msg, sizeof(msg));
}

int plexy_window_request_move(PlexyWindow* window) {
    if (!window || !window->conn) return -1;

    PlexyRequestMove msg;
    memset(&msg, 0, sizeof(msg));
    msg.header.opcode = PLEXY_OP_REQUEST_MOVE;
    msg.header.length = sizeof(msg);
    msg.window_id = window->id;

    return send_message(window->conn, &msg, sizeof(msg));
}

int plexy_window_request_resize(PlexyWindow* window, uint32_t edges) {
    if (!window || !window->conn) return -1;

    PlexyRequestResize msg;
    memset(&msg, 0, sizeof(msg));
    msg.header.opcode = PLEXY_OP_REQUEST_RESIZE;
    msg.header.length = sizeof(msg);
    msg.window_id = window->id;
    msg.edges = edges;

    return send_message(window->conn, &msg, sizeof(msg));
}

int plexy_window_request_maximize(PlexyWindow* window, bool maximized) {
    if (!window || !window->conn) return -1;

    PlexyRequestMaximize msg;
    memset(&msg, 0, sizeof(msg));
    msg.header.opcode = PLEXY_OP_REQUEST_MAXIMIZE;
    msg.header.length = sizeof(msg);
    msg.window_id = window->id;
    msg.maximized = maximized ? 1u : 0u;

    return send_message(window->conn, &msg, sizeof(msg));
}

int plexy_window_request_minimize(PlexyWindow* window) {
    if (!window || !window->conn) return -1;

    PlexyRequestMinimize msg;
    memset(&msg, 0, sizeof(msg));
    msg.header.opcode = PLEXY_OP_REQUEST_MINIMIZE;
    msg.header.length = sizeof(msg);
    msg.window_id = window->id;

    return send_message(window->conn, &msg, sizeof(msg));
}

int plexy_window_request_fullscreen(PlexyWindow* window, bool fullscreen) {
    if (!window || !window->conn) return -1;

    PlexyRequestFullscreen msg;
    memset(&msg, 0, sizeof(msg));
    msg.header.opcode = PLEXY_OP_REQUEST_FULLSCREEN;
    msg.header.length = sizeof(msg);
    msg.window_id = window->id;
    msg.fullscreen = fullscreen ? 1u : 0u;

    return send_message(window->conn, &msg, sizeof(msg));
}

int plexy_window_set_title(PlexyWindow* window, const char* title) {
    if (!window || !window->conn) return -1;

    PlexySetWindowTitle msg;
    memset(&msg, 0, sizeof(msg));
    msg.header.opcode = PLEXY_OP_SET_WINDOW_TITLE;
    msg.header.length = sizeof(msg);
    msg.window_id = window->id;
    if (title) snprintf(msg.title, sizeof(msg.title), "%s", title);

    return send_message(window->conn, &msg, sizeof(msg));
}

int plexy_window_set_app_id(PlexyWindow* window, const char* app_id) {
    if (!window || !window->conn) return -1;

    PlexySetAppId msg;
    memset(&msg, 0, sizeof(msg));
    msg.header.opcode = PLEXY_OP_SET_APP_ID;
    msg.header.length = sizeof(msg);
    msg.window_id = window->id;
    if (app_id) snprintf(msg.app_id, sizeof(msg.app_id), "%s", app_id);

    return send_message(window->conn, &msg, sizeof(msg));
}

int plexy_window_set_geometry_hints(PlexyWindow* window, int32_t min_w, int32_t min_h, int32_t max_w, int32_t max_h) {
    if (!window || !window->conn) return -1;

    PlexySetWindowGeometry msg;
    memset(&msg, 0, sizeof(msg));
    msg.header.opcode = PLEXY_OP_SET_WINDOW_GEOMETRY;
    msg.header.length = sizeof(msg);
    msg.window_id = window->id;
    msg.min_width = min_w;
    msg.min_height = min_h;
    msg.max_width = max_w;
    msg.max_height = max_h;

    return send_message(window->conn, &msg, sizeof(msg));
}

int plexy_session_lock(PlexyConnection* conn) {
    if (!conn) return -1;

    PlexySessionLock msg;
    memset(&msg, 0, sizeof(msg));
    msg.header.opcode = PLEXY_OP_SESSION_LOCK;
    msg.header.length = sizeof(msg);
    msg.locked = 1;

    return send_message(conn, &msg, sizeof(msg));
}

int plexy_session_unlock(PlexyConnection* conn) {
    if (!conn) return -1;

    PlexySessionUnlock msg;
    memset(&msg, 0, sizeof(msg));
    msg.header.opcode = PLEXY_OP_SESSION_UNLOCK;
    msg.header.length = sizeof(msg);

    return send_message(conn, &msg, sizeof(msg));
}

void plexy_set_scene_caps_callback(PlexyConnection* conn, 
                                    PlexySceneCapsCallback callback, 
                                    void* user_data) {
    if (!conn) return;
    conn->scene_caps_cb = callback;
    conn->scene_caps_user_data = user_data;
}

void plexy_set_scene_texture_callback(PlexyConnection* conn, 
                                       PlexySceneTextureCallback callback, 
                                       void* user_data) {
    if (!conn) return;
    conn->scene_texture_cb = callback;
    conn->scene_texture_user_data = user_data;
}

void plexy_set_scene_shm_callback(PlexyConnection* conn,
                                   PlexySceneShmCallback callback,
                                   void* user_data) {
    if (!conn) return;
    conn->scene_shm_cb = callback;
    conn->scene_shm_user_data = user_data;
}

void plexy_set_frame_ready_callback(PlexyConnection* conn,
                                     PlexyFrameReadyCallback callback,
                                     void* user_data) {
    if (!conn) return;
    conn->frame_ready_cb = callback;
    conn->frame_ready_user_data = user_data;
}

void plexy_set_raw_pointer_motion_callback(PlexyConnection* conn,
                                           PlexyRawPointerMotionCallback cb,
                                           void* user_data) {
    if (!conn) return;
    conn->raw_pointer_motion_cb = cb;
    conn->raw_pointer_motion_user_data = user_data;
    (void)update_raw_input_subscription(conn);
}

void plexy_set_raw_pointer_button_callback(PlexyConnection* conn,
                                           PlexyRawPointerButtonCallback cb,
                                           void* user_data) {
    if (!conn) return;
    conn->raw_pointer_button_cb = cb;
    conn->raw_pointer_button_user_data = user_data;
    (void)update_raw_input_subscription(conn);
}

void plexy_set_raw_pointer_axis_callback(PlexyConnection* conn,
                                         PlexyRawPointerAxisCallback cb,
                                         void* user_data) {
    if (!conn) return;
    conn->raw_pointer_axis_cb = cb;
    conn->raw_pointer_axis_user_data = user_data;
    (void)update_raw_input_subscription(conn);
}

void plexy_set_window_position_callback(PlexyConnection* conn,
                                        PlexyWindowPositionCallback cb,
                                        void* user_data) {
    if (!conn) return;
    conn->window_position_cb = cb;
    conn->window_position_user_data = user_data;
}

PlexyTransparentSurface* plexy_create_transparent_surface(
    PlexyConnection* conn,
    PlexyBuffer* buffer,
    float x, float y,
    float blur_radius,
    float refraction_strength,
    bool chromatic_aberration)
{
    if (!conn || !buffer) {
        return NULL;
    }
    
    PlexyTransparentSurface* surface = calloc(1, sizeof(PlexyTransparentSurface));
    if (!surface) {
        return NULL;
    }
    
    surface->id = conn->next_transparent_surface_id++;
    surface->conn = conn;
    surface->buffer = buffer;
    
    PlexyTransparentSurfaceCreate msg;
    memset(&msg, 0, sizeof(msg));
    msg.header.opcode = PLEXY_OP_CREATE_TRANSPARENT_SURFACE;
    msg.header.length = sizeof(msg);
    msg.surface_id = surface->id;
    msg.width = buffer->width;
    msg.height = buffer->height;
    msg.stride = buffer->stride;
    msg.format = buffer->format;
    msg.x = x;
    msg.y = y;
    msg.blur_radius = blur_radius;
    msg.refraction_strength = refraction_strength;
    msg.chromatic_aberration = chromatic_aberration ? 1 : 0;
    msg.needs_glass_effect = 1;
    
    if (send_message_with_fd(conn, &msg, sizeof(msg), buffer->shm_fd) < 0) {
        free(surface);
        return NULL;
    }
    
    if (conn->transparent_surface_count >= conn->transparent_surface_capacity) {
        uint32_t new_cap = conn->transparent_surface_capacity == 0 ? 4 : conn->transparent_surface_capacity * 2;
        PlexyTransparentSurface** new_surfaces = realloc(conn->transparent_surfaces, 
                                                          new_cap * sizeof(PlexyTransparentSurface*));
        if (!new_surfaces) {
            free(surface);
            return NULL;
        }
        conn->transparent_surfaces = new_surfaces;
        conn->transparent_surface_capacity = new_cap;
    }
    
    conn->transparent_surfaces[conn->transparent_surface_count++] = surface;
    
    return surface;
}

int plexy_update_transparent_surface(
    PlexyTransparentSurface* surface,
    float x, float y,
    float blur_radius,
    float refraction_strength,
    bool chromatic_aberration)
{
    if (!surface) {
        return -1;
    }
    
    PlexyUpdateTransparentSurface msg;
    memset(&msg, 0, sizeof(msg));
    msg.header.opcode = PLEXY_OP_UPDATE_TRANSPARENT_SURFACE;
    msg.header.length = sizeof(msg);
    msg.surface_id = surface->id;
    msg.x = x;
    msg.y = y;
    msg.blur_radius = blur_radius;
    msg.refraction_strength = refraction_strength;
    msg.chromatic_aberration = chromatic_aberration ? 1 : 0;
    
    return send_message(surface->conn, &msg, sizeof(msg));
}

void plexy_destroy_transparent_surface(PlexyTransparentSurface* surface) {
    if (!surface) {
        return;
    }
    
    PlexyDestroyTransparentSurface msg;
    memset(&msg, 0, sizeof(msg));
    msg.header.opcode = PLEXY_OP_DESTROY_TRANSPARENT_SURFACE;
    msg.header.length = sizeof(msg);
    msg.surface_id = surface->id;
    
    send_message(surface->conn, &msg, sizeof(msg));
    
    
    PlexyConnection* conn = surface->conn;
    for (uint32_t i = 0; i < conn->transparent_surface_count; i++) {
        if (conn->transparent_surfaces[i] == surface) {
            conn->transparent_surfaces[i] = conn->transparent_surfaces[--conn->transparent_surface_count];
            break;
        }
    }
    
    free(surface);
}

uint32_t plexy_transparent_surface_get_id(PlexyTransparentSurface* surface) {
    return surface ? surface->id : 0;
}

uint32_t plexy_get_output_count(PlexyConnection* conn) {
    return conn ? conn->output_count : 0;
}

const PlexyOutputInfo* plexy_get_output_info(PlexyConnection* conn, uint32_t index) {
    if (!conn || index >= conn->output_count) {
        return NULL;
    }
    return &conn->outputs[index];
}

float plexy_get_output_scale(PlexyConnection* conn, uint32_t output_id) {
    if (!conn) return 1.0f;
    
    for (uint32_t i = 0; i < conn->output_count; i++) {
        if (conn->outputs[i].output_id == output_id) {
            return conn->outputs[i].scale_factor;
        }
    }
    return 1.0f;
}

uint64_t plexy_get_output_generation(PlexyConnection* conn) {
    return conn ? conn->output_generation : 0;
}

PlexyWindow* plexy_create_window_scaled(PlexyConnection* conn,
                                        int32_t x, int32_t y,
                                        uint32_t width, uint32_t height,
                                        uint32_t buffer_scale,
                                        const char* title) {
    if (!conn) return NULL;
    
    PlexyWindow* window = calloc(1, sizeof(PlexyWindow));
    if (!window) return NULL;
    
    window->conn = conn;
    window->width = width;
    window->height = height;
    window->scale_factor = conn->ui_scale;
    window->buffer_scale = buffer_scale ? buffer_scale : (uint32_t)(conn->ui_scale + 0.5f);
    
    if (conn->window_count >= conn->window_capacity) {
        uint32_t new_cap = conn->window_capacity ? conn->window_capacity * 2 : 8;
        PlexyWindow** new_windows = realloc(conn->windows, new_cap * sizeof(PlexyWindow*));
        if (!new_windows) {
            free(window);
            return NULL;
        }
        conn->windows = new_windows;
        conn->window_capacity = new_cap;
    }
    
    conn->windows[conn->window_count++] = window;
    
    PlexyCreateWindow msg;
    memset(&msg, 0, sizeof(msg));
    msg.header.opcode = PLEXY_OP_CREATE_WINDOW;
    msg.header.length = sizeof(msg);
    msg.x = x;
    msg.y = y;
    msg.width = width;
    msg.height = height;
    msg.buffer_scale = buffer_scale;
    strncpy(msg.title, title ? title : "", sizeof(msg.title) - 1);
    
    if (send_message(conn, &msg, sizeof(msg)) < 0) {
        conn->window_count--;
        free(window);
        return NULL;
    }
    
    PlexyWindowCreated reply;
    ssize_t n = recv(conn->fd, &reply, sizeof(reply), 0);
    if (n != sizeof(reply) || reply.header.opcode != PLEXY_OP_WINDOW_CREATED) {
        conn->window_count--;
        free(window);
        return NULL;
    }
    
    window->id = reply.window_id;
    window->width = reply.width;
    window->height = reply.height;
    window->scale_factor = reply.scale_factor;
    window->buffer_scale = reply.buffer_scale;
    
    return window;
}

float plexy_window_get_scale(PlexyWindow* window) {
    return window ? window->scale_factor : 1.0f;
}

uint32_t plexy_window_get_buffer_scale(PlexyWindow* window) {
    return window ? window->buffer_scale : 1;
}

void plexy_window_get_logical_size(PlexyWindow* window, uint32_t* width, uint32_t* height) {
    if (!window) return;
    if (width) *width = window->width;
    if (height) *height = window->height;
}

void plexy_window_get_buffer_size(PlexyWindow* window, uint32_t* width, uint32_t* height) {
    if (!window) return;
    if (width) *width = window->width * window->buffer_scale;
    if (height) *height = window->height * window->buffer_scale;
}

int plexy_window_set_buffer_scale(PlexyWindow* window, uint32_t scale) {
    if (!window || scale == 0) return -1;
    
    PlexySetBufferScale msg;
    memset(&msg, 0, sizeof(msg));
    msg.header.opcode = PLEXY_OP_SET_BUFFER_SCALE;
    msg.header.length = sizeof(msg);
    msg.window_id = window->id;
    msg.scale = scale;
    
    return send_message(window->conn, &msg, sizeof(msg));
}

uint32_t plexy_window_get_output_count(PlexyWindow* window) {
    return window ? window->output_count : 0;
}

const uint32_t* plexy_window_get_outputs(PlexyWindow* window) {
    return window ? window->output_ids : NULL;
}

PlexyBuffer* plexy_create_buffer_scaled(PlexyConnection* conn,
                                        uint32_t logical_width,
                                        uint32_t logical_height,
                                        uint32_t buffer_scale,
                                        uint32_t format) {
    if (!conn || buffer_scale == 0) return NULL;
    
    uint32_t buffer_width = logical_width * buffer_scale;
    uint32_t buffer_height = logical_height * buffer_scale;
    
    PlexyBuffer* buffer = plexy_create_buffer(conn, buffer_width, buffer_height, format);
    if (!buffer) return NULL;
    
    buffer->buffer_scale = buffer_scale;
    buffer->logical_width = logical_width;
    buffer->logical_height = logical_height;
    
    return buffer;
}

void plexy_buffer_get_logical_size(PlexyBuffer* buffer, uint32_t* width, uint32_t* height) {
    if (!buffer) return;
    if (width) *width = buffer->logical_width ? buffer->logical_width : buffer->width;
    if (height) *height = buffer->logical_height ? buffer->logical_height : buffer->height;
}

uint32_t plexy_buffer_get_scale(PlexyBuffer* buffer) {
    return buffer ? (buffer->buffer_scale ? buffer->buffer_scale : 1) : 1;
}
