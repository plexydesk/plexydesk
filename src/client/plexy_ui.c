

#define _POSIX_C_SOURCE 200112L


#include "../../include/plexy_ui.h"
#include "../../include/plexy_canvas.h"
#include "../../include/plexy_protocol.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <poll.h>


typedef struct PlexyConnection PlexyConnection;
typedef struct PlexyProtoWindow PlexyProtoWindow;  
typedef struct PlexyBuffer PlexyBuffer;


extern PlexyConnection* plexy_connect(const char* socket_path);
extern void plexy_disconnect(PlexyConnection* conn);
extern int plexy_get_fd(PlexyConnection* conn);
extern int plexy_dispatch(PlexyConnection* conn);
extern int plexy_is_dark_mode(PlexyConnection* conn);
typedef void (*PlexyDarkModeCallbackFn)(PlexyConnection* conn, int dark, void* user_data);
extern void plexy_set_dark_mode_callback(PlexyConnection* conn, PlexyDarkModeCallbackFn cb, void* user_data);

typedef struct {
    void (*configure)(void* window, uint32_t width, uint32_t height, uint32_t state_flags, void* user_data);
    void (*close)(void* window, void* user_data);
    void (*pointer_enter)(void* window, int32_t x, int32_t y, void* user_data);
    void (*pointer_leave)(void* window, void* user_data);
    void (*pointer_motion)(void* window, int32_t x, int32_t y, void* user_data);
    void (*pointer_button)(void* window, uint32_t button, _Bool pressed, int32_t x, int32_t y, void* user_data);
    void (*pointer_axis)(void* window, int32_t axis, int32_t value, int32_t discrete, void* user_data);
    void (*key)(void* window, uint32_t keycode, _Bool pressed, uint32_t modifiers, void* user_data);
    void (*modifiers)(void* window, uint32_t depressed, uint32_t latched, uint32_t locked, uint32_t group, void* user_data);
    void (*focus_in)(void* window, void* user_data);
    void (*focus_out)(void* window, void* user_data);
    void (*frame_done)(void* window, void* user_data);
    void (*menu_action)(void* window, uint32_t item_id, void* user_data);
    void (*scale_changed)(void* window, float scale_factor, uint32_t buffer_scale, void* user_data);
    void (*enter_output)(void* window, uint32_t output_id, void* user_data);
    void (*leave_output)(void* window, uint32_t output_id, void* user_data);
} PlexyUIWindowCallbacks;


extern void* plexy_create_window(PlexyConnection* conn, int32_t x, int32_t y,
                                  uint32_t width, uint32_t height, const char* title);
extern void plexy_destroy_window(void* window);
extern void plexy_window_set_callbacks(void* window, const void* callbacks, void* user_data);
extern int plexy_window_attach(void* window, void* buffer);
extern int plexy_window_commit(void* window);
extern int plexy_window_set_glass_blur(void* window, _Bool blur_background);
extern void* plexy_create_buffer_from_dmabuf(PlexyConnection* conn, int fd,
                                              uint32_t width, uint32_t height,
                                              uint32_t stride, uint32_t format,
                                              uint64_t modifier);
extern int plexy_buffer_update_dmabuf(void* buffer, int fd, uint32_t stride);
extern void plexy_destroy_buffer(void* buffer);
extern int plexy_window_menu_begin(void* window, _Bool supported, const char* app_title);
extern int plexy_window_menu_add(void* window, uint32_t menu_id, const char* title);
extern int plexy_window_menu_add_item(void* window, uint32_t menu_id, uint32_t item_id,
                                      const char* label, _Bool enabled);
extern int plexy_window_menu_commit(void* window);



typedef struct WidgetCBEntry {
    uint32_t id;
    PlexyClickCallback on_click;
    void* on_click_data;
    PlexyValueCallback on_value;
    void* on_value_data;
    PlexyTextCallback on_text;
    void* on_text_data;
    PlexyKeyCallback on_key;
    void* on_key_data;
} WidgetCBEntry;

#define PLEXY_UI_MAX_LISTS 64
#define PLEXY_UI_MAX_LIST_ITEMS 1024

typedef struct {
    struct PlexyWindow* win;
    uint32_t list_id;
    uint32_t index;
} ListItemClickCtx;

typedef struct {
    int kind; 
    uint32_t list_id;
    uint32_t item_ids[PLEXY_UI_MAX_LIST_ITEMS];
    char* item_text[PLEXY_UI_MAX_LIST_ITEMS];
    ListItemClickCtx* item_ctx[PLEXY_UI_MAX_LIST_ITEMS];
    uint32_t row_ids[PLEXY_UI_MAX_LIST_ITEMS];
    uint32_t row_count;
    uint32_t item_count;
    int selected_index;
    int in_use;
} ListModel;


struct PlexyWindow {
    PlexyApp* app;
    PlexyCanvas* canvas;
    uint32_t root_id;
    int width;
    int height;

    PlexyConnection* conn;
    void* proto_win;     
    void* buffer;        

    WidgetCBEntry callbacks[1024];
    uint32_t callback_count;
    PlexyMenuCallback menu_cb;
    void* menu_cb_data;

    ListModel lists[PLEXY_UI_MAX_LISTS];
};

struct PlexyApp {
    PlexyConnection* conn;
    int running;
    int dark_mode;
    char name[64];
    PlexyWindow* windows[16];
    uint32_t window_count;
};

enum {
    PLEXY_UI_MENU_ID_APP  = 1,
    PLEXY_UI_MENU_ID_FILE = 2,
    PLEXY_UI_MENU_ID_EDIT = 3,
    PLEXY_UI_MENU_ID_VIEW = 4,
};

enum {
    PLEXY_UI_MENU_ITEM_ABOUT      = 1001,
    PLEXY_UI_MENU_ITEM_QUIT       = 1002,
    PLEXY_UI_MENU_ITEM_CLOSE      = 1101,
    PLEXY_UI_MENU_ITEM_COPY       = 1201,
    PLEXY_UI_MENU_ITEM_CUT        = 1202,
    PLEXY_UI_MENU_ITEM_PASTE      = 1203,
    PLEXY_UI_MENU_ITEM_SELECT_ALL = 1204,
};


uint32_t plexy_list_append(PlexyWindow* win, uint32_t list_view_id, const char* item_text);
void plexy_list_select(PlexyWindow* win, uint32_t list_view_id, int index);
uint32_t plexy_icon_append(PlexyWindow* win, uint32_t icon_view_id, const char* item_text);
void plexy_icon_select(PlexyWindow* win, uint32_t icon_view_id, int index);



static WidgetCBEntry* find_cb(PlexyWindow* win, uint32_t id) {
    for (uint32_t i = 0; i < win->callback_count; i++) {
        if (win->callbacks[i].id == id) return &win->callbacks[i];
    }
    return NULL;
}

static WidgetCBEntry* get_or_create_cb(PlexyWindow* win, uint32_t id) {
    WidgetCBEntry* e = find_cb(win, id);
    if (e) return e;
    if (win->callback_count >= 1024) return NULL;
    e = &win->callbacks[win->callback_count++];
    memset(e, 0, sizeof(*e));
    e->id = id;
    return e;
}

static char* dup_string_local(const char* s) {
    size_t n;
    char* out;
    if (!s) return NULL;
    n = strlen(s);
    out = (char*)malloc(n + 1);
    if (!out) return NULL;
    memcpy(out, s, n + 1);
    return out;
}

static ListModel* find_list_model(PlexyWindow* win, uint32_t list_id) {
    uint32_t i;
    if (!win) return NULL;
    for (i = 0; i < PLEXY_UI_MAX_LISTS; i++) {
        if (win->lists[i].in_use && win->lists[i].list_id == list_id) return &win->lists[i];
    }
    return NULL;
}

static ListModel* create_list_model(PlexyWindow* win, uint32_t list_id) {
    uint32_t i;
    if (!win || list_id == 0) return NULL;
    for (i = 0; i < PLEXY_UI_MAX_LISTS; i++) {
        if (!win->lists[i].in_use) {
            memset(&win->lists[i], 0, sizeof(win->lists[i]));
            win->lists[i].in_use = 1;
            win->lists[i].list_id = list_id;
            win->lists[i].selected_index = -1;
            return &win->lists[i];
        }
    }
    return NULL;
}

static ListModel* create_model_with_kind(PlexyWindow* win, uint32_t id, int kind) {
    ListModel* lm = create_list_model(win, id);
    if (lm) lm->kind = kind;
    return lm;
}

static void list_apply_item_style(PlexyWindow* win, uint32_t item_id, int selected) {
    int dark;
    if (!win || !win->canvas || item_id == 0) return;
    dark = win->app ? win->app->dark_mode : 0;
    if (selected) {
        if (dark) {
            plexy_canvas_set_fill_color(win->canvas, item_id, 0.04f, 0.52f, 1.0f, 0.90f);
            plexy_canvas_set_text_color(win->canvas, item_id, 1.0f, 1.0f, 1.0f, 1.0f);
        } else {
            plexy_canvas_set_fill_color(win->canvas, item_id, 0.78f, 0.86f, 0.98f, 1.0f);
            plexy_canvas_set_text_color(win->canvas, item_id, 0.12f, 0.18f, 0.28f, 1.0f);
        }
    } else {
        if (dark) {
            plexy_canvas_set_fill_color(win->canvas, item_id, 0.22f, 0.22f, 0.24f, 0.85f);
            plexy_canvas_set_text_color(win->canvas, item_id, 1.0f, 1.0f, 1.0f, 1.0f);
        } else {
            plexy_canvas_set_fill_color(win->canvas, item_id, 0.94f, 0.95f, 0.97f, 1.0f);
            plexy_canvas_set_text_color(win->canvas, item_id, 0.18f, 0.20f, 0.24f, 1.0f);
        }
    }
    plexy_canvas_set_corner_radius(win->canvas, item_id, 8.0f);
}

static void icon_apply_item_style(PlexyWindow* win, uint32_t item_id, int selected) {
    int dark;
    if (!win || !win->canvas || item_id == 0) return;
    dark = win->app ? win->app->dark_mode : 0;
    if (selected) {
        if (dark) {
            plexy_canvas_set_fill_color(win->canvas, item_id, 0.04f, 0.52f, 1.0f, 0.90f);
            plexy_canvas_set_text_color(win->canvas, item_id, 1.0f, 1.0f, 1.0f, 1.0f);
        } else {
            plexy_canvas_set_fill_color(win->canvas, item_id, 0.78f, 0.86f, 0.98f, 1.0f);
            plexy_canvas_set_text_color(win->canvas, item_id, 0.12f, 0.18f, 0.28f, 1.0f);
        }
    } else {
        if (dark) {
            plexy_canvas_set_fill_color(win->canvas, item_id, 0.22f, 0.22f, 0.24f, 0.85f);
            plexy_canvas_set_text_color(win->canvas, item_id, 1.0f, 1.0f, 1.0f, 1.0f);
        } else {
            plexy_canvas_set_fill_color(win->canvas, item_id, 0.95f, 0.96f, 0.98f, 1.0f);
            plexy_canvas_set_text_color(win->canvas, item_id, 0.18f, 0.20f, 0.24f, 1.0f);
        }
    }
    plexy_canvas_set_corner_radius(win->canvas, item_id, 10.0f);
}

static void list_set_selected(PlexyWindow* win, ListModel* list, int index) {
    uint32_t i;
    if (!win || !list) return;
    if (index < 0 || (uint32_t)index >= list->item_count) index = -1;
    if (list->selected_index == index) {
        plexy_canvas_set_value(win->canvas, list->list_id, (index >= 0) ? (float)index : 0.0f);
        return;
    }
    list->selected_index = index;
    plexy_canvas_set_value(win->canvas, list->list_id, (index >= 0) ? (float)index : 0.0f);
    for (i = 0; i < list->item_count; i++) {
        if (list->kind == 1) icon_apply_item_style(win, list->item_ids[i], ((int)i == index));
        else list_apply_item_style(win, list->item_ids[i], ((int)i == index));
    }
}

static void list_model_clear(PlexyWindow* win, ListModel* list) {
    uint32_t i;
    if (!win || !list) return;
    if (list->kind == 1) {
        for (i = 0; i < list->row_count; i++) {
            if (list->row_ids[i]) plexy_canvas_destroy_widget(win->canvas, list->row_ids[i]);
            list->row_ids[i] = 0;
        }
    } else {
        for (i = 0; i < list->item_count; i++) {
            if (list->item_ids[i]) plexy_canvas_destroy_widget(win->canvas, list->item_ids[i]);
            list->item_ids[i] = 0;
        }
    }
    list->row_count = 0;
    for (i = 0; i < list->item_count; i++) {
        free(list->item_text[i]);
        list->item_text[i] = NULL;
        free(list->item_ctx[i]);
        list->item_ctx[i] = NULL;
        list->item_ids[i] = 0;
    }
    list->item_count = 0;
    list->selected_index = -1;
}

static void cleanup_window_lists(PlexyWindow* win) {
    uint32_t i;
    if (!win) return;
    for (i = 0; i < PLEXY_UI_MAX_LISTS; i++) {
        if (win->lists[i].in_use) {
            list_model_clear(win, &win->lists[i]);
            win->lists[i].in_use = 0;
            win->lists[i].list_id = 0;
        }
    }
}

static void inject_ctrl_shortcut(PlexyWindow* win, uint32_t keycode) {
    const uint32_t mod_ctrl = 1u << 1;
    if (!win || !win->canvas) return;
    plexy_canvas_inject_key(win->canvas, keycode, 1, mod_ctrl);
    plexy_canvas_inject_key(win->canvas, keycode, 0, mod_ctrl);
}

static void publish_default_menubar(PlexyWindow* win, const char* title) {
    const char* app_name = (title && title[0]) ? title : "App";
    if (!win || !win->proto_win) return;

    if (plexy_window_menu_begin(win->proto_win, 1, app_name) < 0) return;

    plexy_window_menu_add(win->proto_win, PLEXY_UI_MENU_ID_APP, app_name);
    plexy_window_menu_add_item(win->proto_win, PLEXY_UI_MENU_ID_APP, PLEXY_UI_MENU_ITEM_ABOUT, "About", 1);
    plexy_window_menu_add_item(win->proto_win, PLEXY_UI_MENU_ID_APP, PLEXY_UI_MENU_ITEM_QUIT, "Quit", 1);

    plexy_window_menu_add(win->proto_win, PLEXY_UI_MENU_ID_FILE, "File");
    plexy_window_menu_add_item(win->proto_win, PLEXY_UI_MENU_ID_FILE, PLEXY_UI_MENU_ITEM_CLOSE, "Close Window", 1);

    plexy_window_menu_add(win->proto_win, PLEXY_UI_MENU_ID_EDIT, "Edit");
    plexy_window_menu_add_item(win->proto_win, PLEXY_UI_MENU_ID_EDIT, PLEXY_UI_MENU_ITEM_COPY, "Copy", 1);
    plexy_window_menu_add_item(win->proto_win, PLEXY_UI_MENU_ID_EDIT, PLEXY_UI_MENU_ITEM_CUT, "Cut", 1);
    plexy_window_menu_add_item(win->proto_win, PLEXY_UI_MENU_ID_EDIT, PLEXY_UI_MENU_ITEM_PASTE, "Paste", 1);
    plexy_window_menu_add_item(win->proto_win, PLEXY_UI_MENU_ID_EDIT, PLEXY_UI_MENU_ITEM_SELECT_ALL, "Select All", 1);

    plexy_window_menu_add(win->proto_win, PLEXY_UI_MENU_ID_VIEW, "View");
    plexy_window_menu_commit(win->proto_win);
}

static void on_list_item_click(uint32_t widget_id, void* userdata) {
    ListItemClickCtx* ctx = (ListItemClickCtx*)userdata;
    ListModel* list;
    WidgetCBEntry* cb;
    (void)widget_id;
    if (!ctx || !ctx->win) return;
    list = find_list_model(ctx->win, ctx->list_id);
    if (!list) return;
    if (ctx->index >= list->item_count) return;

    list_set_selected(ctx->win, list, (int)ctx->index);
    cb = find_cb(ctx->win, ctx->list_id);
    if (cb && cb->on_value) cb->on_value(ctx->list_id, (float)ctx->index, cb->on_value_data);
    if (cb && cb->on_text && list->item_text[ctx->index]) {
        cb->on_text(ctx->list_id, list->item_text[ctx->index], cb->on_text_data);
    }
}


static void bridge_click(uint32_t widget_id, void* userdata) {
    PlexyWindow* win = (PlexyWindow*)userdata;
    WidgetCBEntry* e = find_cb(win, widget_id);
    if (e && e->on_click) e->on_click(widget_id, e->on_click_data);
}


static void bridge_value(uint32_t widget_id, float value, void* userdata) {
    PlexyWindow* win = (PlexyWindow*)userdata;
    WidgetCBEntry* e = find_cb(win, widget_id);
    if (e && e->on_value) e->on_value(widget_id, value, e->on_value_data);
}


static void bridge_text(uint32_t widget_id, const char* text, void* userdata) {
    PlexyWindow* win = (PlexyWindow*)userdata;
    WidgetCBEntry* e = find_cb(win, widget_id);
    if (e && e->on_text) e->on_text(widget_id, text ? text : "", e->on_text_data);
}


static void bridge_key(uint32_t widget_id, uint32_t keycode, uint32_t mods, void* userdata) {
    PlexyWindow* win = (PlexyWindow*)userdata;
    WidgetCBEntry* e = find_cb(win, widget_id);
    if (e && e->on_key) e->on_key(widget_id, keycode, mods, e->on_key_data);
}



static void on_ptr_motion(void* pw, int32_t x, int32_t y, void* ud) {
    PlexyWindow* win = (PlexyWindow*)ud;
    (void)pw;
    if (win->canvas) plexy_canvas_inject_mouse_move(win->canvas, (float)x, (float)y);
}

static void on_ptr_leave(void* pw, void* ud) {
    PlexyWindow* win = (PlexyWindow*)ud;
    (void)pw;
    
    if (win->canvas) plexy_canvas_inject_mouse_move(win->canvas, -1.0f, -1.0f);
}

static void on_ptr_button(void* pw, uint32_t button, _Bool pressed,
                           int32_t x, int32_t y, void* ud) {
    PlexyWindow* win = (PlexyWindow*)ud;
    (void)pw;
    if (!win->canvas) return;
    int btn = 0;
    if (button == 0x110) btn = 0;
    else if (button == 0x111) btn = 1;
    else if (button == 0x112) btn = 2;
    plexy_canvas_inject_mouse_button(win->canvas, btn, pressed ? 1 : 0, (float)x, (float)y);
}


static uint32_t keycode_to_char(uint32_t keycode, uint32_t modifiers) {
    int shift = (modifiers & 1) != 0;  
    
    
    static const char letters[] = "qwertyuiopasdfghjklzxcvbnm";
    static const int letter_codes[] = {16,17,18,19,20,21,22,23,24,25, 30,31,32,33,34,35,36,37,38, 44,45,46,47,48,49,50};
    for (int i = 0; i < 26; i++) {
        if (keycode == (uint32_t)letter_codes[i]) {
            char c = letters[i];
            return shift ? (c - 32) : c;  
        }
    }
    
    
    static const char numbers[] = "1234567890";
    static const char shifted[] = "!@#$%^&*()";
    if (keycode >= 2 && keycode <= 11) {
        int idx = keycode - 2;
        return shift ? shifted[idx] : numbers[idx];
    }
    
    
    if (keycode == 57) return ' ';
    
    
    if (keycode == 14) return 8;
    
    
    if (keycode == 12) return shift ? '_' : '-';  
    if (keycode == 13) return shift ? '+' : '=';  
    if (keycode == 26) return shift ? '{' : '[';  
    if (keycode == 27) return shift ? '}' : ']';  
    if (keycode == 39) return shift ? ':' : ';';  
    if (keycode == 40) return shift ? '"' : '\''; 
    if (keycode == 41) return shift ? '~' : '`';  
    if (keycode == 43) return shift ? '|' : '\\'; 
    if (keycode == 51) return shift ? '<' : ',';  
    if (keycode == 52) return shift ? '>' : '.';  
    if (keycode == 53) return shift ? '?' : '/';  
    
    return 0;  
}

static void on_key_event(void* pw, uint32_t keycode, _Bool pressed,
                          uint32_t modifiers, void* ud) {
    PlexyWindow* win = (PlexyWindow*)ud;
    (void)pw;
    if (win->canvas) {
        plexy_canvas_inject_key(win->canvas, keycode, pressed ? 1 : 0, modifiers);
        
        
        if (pressed) {
            uint32_t ch = keycode_to_char(keycode, modifiers);
            if (ch != 0) {
                plexy_canvas_inject_char(win->canvas, ch);
            }
        }
    }
}

static void on_close(void* pw, void* ud) {
    PlexyWindow* win = (PlexyWindow*)ud;
    (void)pw;
    if (win && win->app) {
        plexy_app_quit(win->app);
    }
}

static void on_menu_action(void* pw, uint32_t item_id, void* ud) {
    PlexyWindow* win = (PlexyWindow*)ud;
    (void)pw;
    if (!win) return;

    if (win->menu_cb) {
        win->menu_cb(item_id, win->menu_cb_data);
        return;
    }

    switch (item_id) {
        case PLEXY_UI_MENU_ITEM_QUIT:
            if (win->app) plexy_app_quit(win->app);
            break;
        case PLEXY_UI_MENU_ITEM_CLOSE:
            if (win->app) plexy_app_quit(win->app);
            break;
        case PLEXY_UI_MENU_ITEM_COPY:
            inject_ctrl_shortcut(win, 46);  
            break;
        case PLEXY_UI_MENU_ITEM_CUT:
            inject_ctrl_shortcut(win, 45);  
            break;
        case PLEXY_UI_MENU_ITEM_PASTE:
            inject_ctrl_shortcut(win, 47);  
            break;
        case PLEXY_UI_MENU_ITEM_SELECT_ALL:
            inject_ctrl_shortcut(win, 30);  
            break;
        default:
            break;
    }
}

static void on_configure(void* pw, uint32_t width, uint32_t height, uint32_t state_flags, void* ud) {
    PlexyWindow* win = (PlexyWindow*)ud;
    (void)pw;
    (void)state_flags;
    if (win->canvas && ((int)width != win->width || (int)height != win->height)) {
        win->width = (int)width;
        win->height = (int)height;
        plexy_canvas_resize(win->canvas, (int)width, (int)height);
        
        
        if (win->buffer) {
            plexy_destroy_buffer(win->buffer);
            win->buffer = NULL;
        }
    }
}



static void submit_frame(PlexyWindow* win) {
    if (!win->canvas || !win->conn) return;

    int dmabuf_fd = plexy_canvas_get_dmabuf_fd(win->canvas);
    if (dmabuf_fd < 0) return;

    uint32_t stride = plexy_canvas_get_stride(win->canvas);
    uint32_t format = plexy_canvas_get_format(win->canvas);
    uint64_t modifier = plexy_canvas_get_modifier(win->canvas);

    if (win->buffer) {
        plexy_buffer_update_dmabuf(win->buffer, dmabuf_fd, stride);
    } else {
        win->buffer = plexy_create_buffer_from_dmabuf(
            win->conn, dmabuf_fd,
            (uint32_t)win->width, (uint32_t)win->height,
            stride, format, modifier);
    }

    if (win->buffer && win->proto_win) {
        plexy_window_attach(win->proto_win, win->buffer);
        plexy_window_commit(win->proto_win);
    }
}



static void on_dark_mode_changed(PlexyConnection* conn, int dark, void* user_data) {
    PlexyApp* app = (PlexyApp*)user_data;
    uint32_t i, j;
    (void)conn;
    if (!app) return;
    app->dark_mode = dark;
    for (i = 0; i < app->window_count; i++) {
        PlexyWindow* win = app->windows[i];
        if (!win) continue;
        if (win->canvas) {
            plexy_canvas_set_dark_mode(win->canvas, dark);
        }
        
        for (j = 0; j < PLEXY_UI_MAX_LISTS; j++) {
            ListModel* lm = &win->lists[j];
            uint32_t k;
            if (!lm->in_use) continue;
            for (k = 0; k < lm->item_count; k++) {
                if (lm->item_ids[k] == 0) continue;
                if (lm->kind == 1)
                    icon_apply_item_style(win, lm->item_ids[k], ((int)k == lm->selected_index));
                else
                    list_apply_item_style(win, lm->item_ids[k], ((int)k == lm->selected_index));
            }
        }
    }
}



PlexyApp* plexy_app_create(const char* name) {
    PlexyApp* app = (PlexyApp*)calloc(1, sizeof(PlexyApp));
    if (!app) return NULL;
    strncpy(app->name, name, sizeof(app->name) - 1);

    app->conn = plexy_connect(NULL);
    if (!app->conn) {
        fprintf(stderr, "[PlexyUI] Failed to connect to server\n");
        free(app);
        return NULL;
    }
    app->dark_mode = plexy_is_dark_mode(app->conn);
    plexy_set_dark_mode_callback(app->conn, on_dark_mode_changed, app);
    return app;
}

void plexy_app_run(PlexyApp* app) {
    if (!app) return;
    app->running = 1;

    int fd = plexy_get_fd(app->conn);
    struct pollfd pfd;
    pfd.fd = fd;
    pfd.events = POLLIN;

    while (app->running) {
        int ret = poll(&pfd, 1, 16);
        if (ret < 0) break;

        if (ret > 0 && (pfd.revents & POLLIN)) {
            if (plexy_dispatch(app->conn) < 0) break;
        }

        
        for (uint32_t i = 0; i < app->window_count; i++) {
            PlexyWindow* win = app->windows[i];
            if (win->canvas) {
                
                plexy_canvas_begin_frame(win->canvas, 1.0f / 60.0f);

                
                if (plexy_canvas_needs_render(win->canvas)) {
                    plexy_canvas_end_frame(win->canvas);
                    plexy_canvas_swap_buffers(win->canvas);
                    submit_frame(win);
                }
            }
        }
    }
}

void plexy_app_quit(PlexyApp* app) {
    if (app) app->running = 0;
}

void plexy_app_destroy(PlexyApp* app) {
    if (!app) return;
    for (uint32_t i = 0; i < app->window_count; i++) {
        PlexyWindow* win = app->windows[i];
        cleanup_window_lists(win);
        if (win->canvas) plexy_canvas_destroy(win->canvas);
        if (win->buffer) plexy_destroy_buffer(win->buffer);
        if (win->proto_win) plexy_destroy_window(win->proto_win);
        free(win);
    }
    if (app->conn) plexy_disconnect(app->conn);
    free(app);
}

int plexy_app_is_dark_mode(PlexyApp* app) {
    return app ? app->dark_mode : 0;
}

int plexy_window_is_dark_mode(PlexyWindow* win) {
    return (win && win->app) ? win->app->dark_mode : 0;
}



PlexyWindow* plexy_window_create(PlexyApp* app, uint32_t width, uint32_t height, const char* title) {
    if (!app || app->window_count >= 16) return NULL;

    PlexyWindow* win = (PlexyWindow*)calloc(1, sizeof(PlexyWindow));
    if (!win) return NULL;

    win->app = app;
    win->conn = app->conn;
    win->width = (int)width;
    win->height = (int)height;

    
    win->proto_win = plexy_create_window(app->conn, -1, -1, width, height, title);
    if (!win->proto_win) {
        fprintf(stderr, "[PlexyUI] Failed to create window\n");
        free(win);
        return NULL;
    }

    
    PlexyUIWindowCallbacks cbs;
    memset(&cbs, 0, sizeof(cbs));
    cbs.pointer_motion = on_ptr_motion;
    cbs.pointer_leave = on_ptr_leave;
    cbs.pointer_button = on_ptr_button;
    cbs.key = on_key_event;
    cbs.configure = on_configure;
    cbs.close = on_close;
    cbs.menu_action = on_menu_action;
    plexy_window_set_callbacks(win->proto_win, &cbs, win);
    publish_default_menubar(win, title);

    
    win->canvas = plexy_canvas_create((int)width, (int)height, PLEXY_CANVAS_TARGET_GBM);
    if (!win->canvas) {
        fprintf(stderr, "[PlexyUI] Failed to create canvas\n");
        plexy_destroy_window(win->proto_win);
        free(win);
        return NULL;
    }

    
    if (plexy_canvas_init_gpu(win->canvas, NULL) < 0) {
        fprintf(stderr, "[PlexyUI] Failed to init GPU\n");
        plexy_canvas_destroy(win->canvas);
        plexy_destroy_window(win->proto_win);
        free(win);
        return NULL;
    }

    
    static const char* font_paths[] = {
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
        "/usr/share/fonts/truetype/freefont/FreeSans.ttf",
        "/usr/share/fonts/TTF/DejaVuSans.ttf",
        NULL
    };
    for (int i = 0; font_paths[i]; i++) {
        if (access(font_paths[i], R_OK) == 0) {
            plexy_canvas_set_font(win->canvas, font_paths[i]);
            break;
        }
    }

    
    plexy_canvas_set_dark_mode(win->canvas, app->dark_mode);

    
    
    float scale = 1.0f;
    const char* scale_env = getenv("PLEXY_FONT_SCALE");
    if (scale_env) {
        scale = (float)atof(scale_env);
        if (scale < 1.0f) scale = 1.0f;
        if (scale > 4.0f) scale = 4.0f;
    }
    plexy_canvas_set_scale_factor(win->canvas, scale);

    
    win->root_id = plexy_canvas_root(win->canvas);

    
    plexy_canvas_set_flex_direction(win->canvas, win->root_id, 0);
    plexy_canvas_set_size(win->canvas, win->root_id, (float)width, (float)height);

    
    plexy_canvas_begin_frame(win->canvas, 0.0f);
    plexy_canvas_end_frame(win->canvas);
    plexy_canvas_swap_buffers(win->canvas);
    submit_frame(win);

    app->windows[app->window_count++] = win;
    return win;
}

uint32_t plexy_window_root(PlexyWindow* win) {
    return win ? win->root_id : 0;
}

void plexy_window_destroy(PlexyWindow* win) {
    if (!win) return;
    if (win->canvas) { plexy_canvas_destroy(win->canvas); win->canvas = NULL; }
    if (win->buffer) { plexy_destroy_buffer(win->buffer); win->buffer = NULL; }
    if (win->proto_win) { plexy_destroy_window(win->proto_win); win->proto_win = NULL; }
}



static uint32_t create_local(PlexyWindow* win, uint32_t parent, int type) {
    if (!win || !win->canvas) return 0;
    uint32_t p = parent ? parent : win->root_id;
    return plexy_canvas_create_widget(win->canvas, type, p);
}

static uint32_t create_local_text(PlexyWindow* win, uint32_t parent, int type, const char* text) {
    uint32_t id = create_local(win, parent, type);
    if (id && text) plexy_canvas_set_text(win->canvas, id, text);
    return id;
}

uint32_t plexy_label(PlexyWindow* win, uint32_t parent, const char* text) {
    return create_local_text(win, parent, PLEXY_WIDGET_LABEL, text);
}

uint32_t plexy_button(PlexyWindow* win, uint32_t parent, const char* text) {
    return create_local_text(win, parent, PLEXY_WIDGET_BUTTON, text);
}

uint32_t plexy_text_input(PlexyWindow* win, uint32_t parent, const char* placeholder) {
    return create_local_text(win, parent, PLEXY_WIDGET_TEXT_INPUT, placeholder);
}

uint32_t plexy_text_area(PlexyWindow* win, uint32_t parent, const char* initial_text) {
    return create_local_text(win, parent, PLEXY_WIDGET_TEXT_AREA, initial_text ? initial_text : "");
}

uint32_t plexy_checkbox(PlexyWindow* win, uint32_t parent, const char* label) {
    return create_local_text(win, parent, PLEXY_WIDGET_CHECKBOX, label);
}

uint32_t plexy_switch_widget(PlexyWindow* win, uint32_t parent, const char* label) {
    return create_local_text(win, parent, PLEXY_WIDGET_SWITCH, label);
}

uint32_t plexy_slider(PlexyWindow* win, uint32_t parent, float min, float max, float initial) {
    uint32_t id = create_local(win, parent, PLEXY_WIDGET_SLIDER);
    if (id) plexy_canvas_set_value(win->canvas, id, initial);
    return id;
}

uint32_t plexy_progress(PlexyWindow* win, uint32_t parent, float value) {
    uint32_t id = create_local(win, parent, PLEXY_WIDGET_PROGRESS_BAR);
    if (id) plexy_canvas_set_value(win->canvas, id, value);
    return id;
}

uint32_t plexy_separator(PlexyWindow* win, uint32_t parent) {
    return create_local(win, parent, PLEXY_WIDGET_SEPARATOR);
}

uint32_t plexy_spacer(PlexyWindow* win, uint32_t parent) {
    uint32_t id = create_local(win, parent, PLEXY_WIDGET_SPACER);
    if (id) plexy_canvas_set_flex_grow(win->canvas, id, 1.0f);
    return id;
}

uint32_t plexy_panel(PlexyWindow* win, uint32_t parent) {
    return create_local(win, parent, PLEXY_WIDGET_PANEL);
}

uint32_t plexy_scroll_view(PlexyWindow* win, uint32_t parent) {
    return create_local(win, parent, PLEXY_WIDGET_SCROLL_VIEW);
}

uint32_t plexy_list_view(PlexyWindow* win, uint32_t parent, const char* items_text) {
    return create_local_text(win, parent, PLEXY_WIDGET_LIST_VIEW, items_text ? items_text : "");
}

uint32_t plexy_icon_view(PlexyWindow* win, uint32_t parent, const char* items_text) {
    return create_local_text(win, parent, PLEXY_WIDGET_ICON_VIEW, items_text ? items_text : "");
}

void plexy_destroy_widget(PlexyWindow* win, uint32_t widget_id) {
    ListModel* lm;
    if (!win || !win->canvas) return;
    lm = find_list_model(win, widget_id);
    if (lm) {
        list_model_clear(win, lm);
        lm->in_use = 0;
        lm->list_id = 0;
    }
    plexy_canvas_destroy_widget(win->canvas, widget_id);
}

void plexy_list_clear(PlexyWindow* win, uint32_t list_view_id) {
    ListModel* lm = find_list_model(win, list_view_id);
    if (!win || !lm) return;
    list_model_clear(win, lm);
}

uint32_t plexy_list_append(PlexyWindow* win, uint32_t list_view_id, const char* item_text) {
    ListModel* lm = find_list_model(win, list_view_id);
    ListItemClickCtx* ctx;
    uint32_t item_id;
    uint32_t idx;
    if (!win || !lm || !item_text) return 0;
    if (lm->item_count >= PLEXY_UI_MAX_LIST_ITEMS) return 0;

    idx = lm->item_count;
    item_id = create_local_text(win, list_view_id, PLEXY_WIDGET_BUTTON, item_text);
    if (!item_id) return 0;

    plexy_canvas_set_size(win->canvas, item_id, -1.0f, 34.0f);
    list_apply_item_style(win, item_id, 0);

    lm->item_text[idx] = dup_string_local(item_text);
    if (!lm->item_text[idx]) {
        plexy_canvas_destroy_widget(win->canvas, item_id);
        return 0;
    }

    ctx = (ListItemClickCtx*)calloc(1, sizeof(*ctx));
    if (!ctx) {
        free(lm->item_text[idx]);
        lm->item_text[idx] = NULL;
        plexy_canvas_destroy_widget(win->canvas, item_id);
        return 0;
    }
    ctx->win = win;
    ctx->list_id = list_view_id;
    ctx->index = idx;

    lm->item_ids[idx] = item_id;
    lm->item_ctx[idx] = ctx;
    lm->item_count++;

    plexy_canvas_on_click(win->canvas, item_id, on_list_item_click, ctx);
    return item_id;
}

void plexy_list_select(PlexyWindow* win, uint32_t list_view_id, int index) {
    ListModel* lm = find_list_model(win, list_view_id);
    if (!win || !lm) return;
    list_set_selected(win, lm, index);
}

void plexy_icon_clear(PlexyWindow* win, uint32_t icon_view_id) {
    ListModel* lm = find_list_model(win, icon_view_id);
    if (!win || !lm || lm->kind != 1) return;
    list_model_clear(win, lm);
}

uint32_t plexy_icon_append(PlexyWindow* win, uint32_t icon_view_id, const char* item_text) {
    ListModel* lm = find_list_model(win, icon_view_id);
    ListItemClickCtx* ctx;
    uint32_t row_id;
    uint32_t item_id;
    uint32_t icon_box;
    uint32_t icon_top;
    uint32_t icon_body;
    uint32_t name_id;
    uint32_t idx;
    uint32_t col_count = 4;
    int is_dir;
    char caption[256];
    size_t n;

    if (!win || !lm || lm->kind != 1 || !item_text) return 0;
    if (lm->item_count >= PLEXY_UI_MAX_LIST_ITEMS) return 0;

    idx = lm->item_count;
    if ((idx % col_count) == 0) {
        row_id = create_local(win, icon_view_id, PLEXY_WIDGET_ROW);
        if (!row_id) return 0;
        plexy_canvas_set_gap(win->canvas, row_id, 8.0f);
        lm->row_ids[lm->row_count++] = row_id;
    } else {
        row_id = lm->row_ids[lm->row_count - 1];
    }

    item_id = create_local(win, row_id, PLEXY_WIDGET_PANEL);
    if (!item_id) return 0;

    plexy_canvas_set_size(win->canvas, item_id, 128.0f, 116.0f);
    plexy_canvas_set_flex_direction(win->canvas, item_id, 0);
    plexy_canvas_set_align(win->canvas, item_id, PLEXY_ALIGN_CENTER);
    plexy_canvas_set_gap(win->canvas, item_id, 6.0f);
    plexy_canvas_set_padding(win->canvas, item_id, 8.0f, 8.0f, 8.0f, 8.0f);
    icon_apply_item_style(win, item_id, 0);

    lm->item_text[idx] = dup_string_local(item_text);
    if (!lm->item_text[idx]) {
        plexy_canvas_destroy_widget(win->canvas, item_id);
        return 0;
    }

    is_dir = 0;
    n = strlen(item_text);
    if (n > 0 && item_text[n - 1] == '/') is_dir = 1;
    if (n >= sizeof(caption)) n = sizeof(caption) - 1;
    memcpy(caption, item_text, n);
    caption[n] = '\0';
    if (is_dir && n > 0) caption[n - 1] = '\0';

    icon_box = create_local(win, item_id, PLEXY_WIDGET_PANEL);
    if (!icon_box) {
        free(lm->item_text[idx]);
        lm->item_text[idx] = NULL;
        plexy_canvas_destroy_widget(win->canvas, item_id);
        return 0;
    }
    plexy_canvas_set_enabled(win->canvas, icon_box, 0);
    plexy_canvas_set_flex_direction(win->canvas, icon_box, 0);
    plexy_canvas_set_align(win->canvas, icon_box, PLEXY_ALIGN_START);
    plexy_canvas_set_gap(win->canvas, icon_box, 0.0f);
    plexy_canvas_set_size(win->canvas, icon_box, 64.0f, 52.0f);

    icon_top = create_local(win, icon_box, PLEXY_WIDGET_PANEL);
    icon_body = create_local(win, icon_box, PLEXY_WIDGET_PANEL);
    if (!icon_top || !icon_body) {
        free(lm->item_text[idx]);
        lm->item_text[idx] = NULL;
        plexy_canvas_destroy_widget(win->canvas, item_id);
        return 0;
    }
    plexy_canvas_set_enabled(win->canvas, icon_top, 0);
    plexy_canvas_set_enabled(win->canvas, icon_body, 0);

    if (is_dir) {
        plexy_canvas_set_size(win->canvas, icon_top, 24.0f, 9.0f);
        plexy_canvas_set_size(win->canvas, icon_body, 62.0f, 40.0f);
        plexy_canvas_set_fill_color(win->canvas, icon_top, 0.78f, 0.67f, 0.46f, 0.95f);
        plexy_canvas_set_fill_color(win->canvas, icon_body, 0.85f, 0.76f, 0.57f, 0.95f);
    } else {
        plexy_canvas_set_size(win->canvas, icon_top, 0.0f, 0.0f);
        plexy_canvas_set_size(win->canvas, icon_body, 46.0f, 52.0f);
        plexy_canvas_set_fill_color(win->canvas, icon_body, 0.88f, 0.90f, 0.93f, 0.95f);
    }
    plexy_canvas_set_corner_radius(win->canvas, icon_top, 4.0f);
    plexy_canvas_set_corner_radius(win->canvas, icon_body, 6.0f);

    name_id = create_local_text(win, item_id, PLEXY_WIDGET_LABEL, caption);
    if (name_id) {
        plexy_canvas_set_font_size(win->canvas, name_id, 12.0f);
    }

    ctx = (ListItemClickCtx*)calloc(1, sizeof(*ctx));
    if (!ctx) {
        free(lm->item_text[idx]);
        lm->item_text[idx] = NULL;
        plexy_canvas_destroy_widget(win->canvas, item_id);
        return 0;
    }
    ctx->win = win;
    ctx->list_id = icon_view_id;
    ctx->index = idx;

    lm->item_ids[idx] = item_id;
    lm->item_ctx[idx] = ctx;
    lm->item_count++;

    plexy_canvas_on_click(win->canvas, item_id, on_list_item_click, ctx);
    return item_id;
}

void plexy_icon_select(PlexyWindow* win, uint32_t icon_view_id, int index) {
    ListModel* lm = find_list_model(win, icon_view_id);
    if (!win || !lm || lm->kind != 1) return;
    list_set_selected(win, lm, index);
}



uint32_t plexy_row(PlexyWindow* win, uint32_t parent) {
    return create_local(win, parent, PLEXY_WIDGET_ROW);
}

uint32_t plexy_column(PlexyWindow* win, uint32_t parent) {
    return create_local(win, parent, PLEXY_WIDGET_COLUMN);
}



uint32_t plexy_toolbar(PlexyWindow* win, uint32_t parent) {
    return create_local(win, parent, PLEXY_WIDGET_TOOLBAR);
}

uint32_t plexy_toolbar_item(PlexyWindow* win, uint32_t parent, const char* label) {
    uint32_t id = create_local_text(win, parent, PLEXY_WIDGET_TOOLBAR_ITEM, label);
    if (id) {
        plexy_canvas_set_corner_radius(win->canvas, id, 10.0f);
        plexy_canvas_set_font_size(win->canvas, id, 13.0f);
    }
    return id;
}

uint32_t plexy_sidebar(PlexyWindow* win, uint32_t parent) {
    uint32_t id = create_local(win, parent, PLEXY_WIDGET_SIDEBAR);
    if (id) {
        
        plexy_canvas_set_corner_radius(win->canvas, id, 14.0f);
        plexy_canvas_set_elevation(win->canvas, id, 0.0f);
    }
    return id;
}

uint32_t plexy_sidebar_item(PlexyWindow* win, uint32_t parent, const char* label) {
    uint32_t id = create_local_text(win, parent, PLEXY_WIDGET_SIDEBAR_ITEM, label);
    if (id) {
        plexy_canvas_set_corner_radius(win->canvas, id, 10.0f);
        plexy_canvas_set_font_size(win->canvas, id, 13.0f);
    }
    return id;
}

uint32_t plexy_sidebar_item_with_icon(PlexyWindow* win, uint32_t parent, const char* label, const char* icon_name) {
    char decorated[320];
    uint32_t id;

    if (icon_name && icon_name[0]) {
        snprintf(decorated, sizeof(decorated), "[icon=%s]%s", icon_name, label ? label : "");
        id = create_local_text(win, parent, PLEXY_WIDGET_SIDEBAR_ITEM, decorated);
    } else {
        id = create_local_text(win, parent, PLEXY_WIDGET_SIDEBAR_ITEM, label ? label : "");
    }

    if (id) {
        plexy_canvas_set_corner_radius(win->canvas, id, 10.0f);
        plexy_canvas_set_font_size(win->canvas, id, 13.0f);
    }
    return id;
}

uint32_t plexy_sidebar_section(PlexyWindow* win, uint32_t parent, const char* title) {
    uint32_t id = create_local_text(win, parent, PLEXY_WIDGET_LABEL, title);
    if (id) {
        
        plexy_canvas_set_font_size(win->canvas, id, 11.0f);
        plexy_canvas_set_margin(win->canvas, id, 12.0f, 0.0f, 4.0f, 2.0f);
    }
    return id;
}

uint32_t plexy_search_field(PlexyWindow* win, uint32_t parent, const char* placeholder) {
    uint32_t id = create_local_text(win, parent, PLEXY_WIDGET_SEARCH_FIELD, placeholder);
    return id;
}

uint32_t plexy_breadcrumb(PlexyWindow* win, uint32_t parent, const char* path) {
    uint32_t id = create_local_text(win, parent, PLEXY_WIDGET_BREADCRUMB, path);
    if (id) {
        
        plexy_canvas_set_font_size(win->canvas, id, 13.0f);
    }
    return id;
}

uint32_t plexy_split_view(PlexyWindow* win, uint32_t parent) {
    return create_local(win, parent, PLEXY_WIDGET_SPLIT_VIEW);
}

uint32_t plexy_status_bar(PlexyWindow* win, uint32_t parent, const char* text) {
    return create_local_text(win, parent, PLEXY_WIDGET_STATUS_BAR, text ? text : "");
}

uint32_t plexy_drawer(PlexyWindow* win, uint32_t parent, int edge, float size) {
    uint32_t id = create_local(win, parent, PLEXY_WIDGET_PANEL);
    if (!id || !win || !win->canvas) return id;

    plexy_canvas_set_drawer(win->canvas, id, edge);

    if (edge == PLEXY_DRAWER_LEFT || edge == PLEXY_DRAWER_RIGHT) {
        float w = size > 0.0f ? size : 260.0f;
        plexy_canvas_set_size(win->canvas, id, w, -1.0f);
        
        if (edge == PLEXY_DRAWER_LEFT)
            plexy_canvas_set_margin(win->canvas, id, 0.0f, -w, 0.0f, 0.0f);
        else
            plexy_canvas_set_margin(win->canvas, id, 0.0f, 0.0f, 0.0f, -w);
    } else {
        float h = size > 0.0f ? size : 220.0f;
        plexy_canvas_set_size(win->canvas, id, -1.0f, h);
        if (edge == PLEXY_DRAWER_TOP)
            plexy_canvas_set_margin(win->canvas, id, 0.0f, 0.0f, -h, 0.0f);
        else
            plexy_canvas_set_margin(win->canvas, id, -h, 0.0f, 0.0f, 0.0f);
    }

    
    plexy_canvas_set_corner_radius(win->canvas, id, 0.0f);
    plexy_canvas_set_elevation(win->canvas, id, 2.0f);
    plexy_canvas_set_drawer_open(win->canvas, id, 0, 0);
    return id;
}

void plexy_drawer_set_open(PlexyWindow* win, uint32_t drawer_id, int open, int animated) {
    if (win && win->canvas) {
        plexy_canvas_set_drawer_open(win->canvas, drawer_id, open, animated);
    }
}

void plexy_drawer_toggle(PlexyWindow* win, uint32_t drawer_id, int animated) {
    if (win && win->canvas) {
        plexy_canvas_toggle_drawer(win->canvas, drawer_id, animated);
    }
}

int plexy_drawer_is_open(PlexyWindow* win, uint32_t drawer_id) {
    if (!win || !win->canvas) return 0;
    return plexy_canvas_is_drawer_open(win->canvas, drawer_id);
}



void plexy_flex_direction(PlexyWindow* win, uint32_t id, int direction) {
    if (win && win->canvas) plexy_canvas_set_flex_direction(win->canvas, id, direction == 0);
}

void plexy_justify(PlexyWindow* win, uint32_t id, int justify) {
    if (win && win->canvas) plexy_canvas_set_justify(win->canvas, id, justify);
}

void plexy_align(PlexyWindow* win, uint32_t id, int align) {
    if (win && win->canvas) plexy_canvas_set_align(win->canvas, id, align);
}

void plexy_flex_grow(PlexyWindow* win, uint32_t id, float grow) {
    if (win && win->canvas) plexy_canvas_set_flex_grow(win->canvas, id, grow);
}

void plexy_size(PlexyWindow* win, uint32_t id, float width, float height) {
    if (win && win->canvas) plexy_canvas_set_size(win->canvas, id, width, height);
}

void plexy_margin(PlexyWindow* win, uint32_t id, float top, float right, float bottom, float left) {
    if (win && win->canvas) plexy_canvas_set_margin(win->canvas, id, top, right, bottom, left);
}

void plexy_padding(PlexyWindow* win, uint32_t id, float top, float right, float bottom, float left) {
    if (win && win->canvas) plexy_canvas_set_padding(win->canvas, id, top, right, bottom, left);
}

void plexy_gap(PlexyWindow* win, uint32_t id, float gap) {
    if (win && win->canvas) plexy_canvas_set_gap(win->canvas, id, gap);
}



void plexy_set_text(PlexyWindow* win, uint32_t id, const char* text) {
    if (win && win->canvas && text) plexy_canvas_set_text(win->canvas, id, text);
}

void plexy_set_value(PlexyWindow* win, uint32_t id, float value) {
    ListModel* lm = find_list_model(win, id);
    if (!win || !win->canvas) return;
    if (lm) {
        list_set_selected(win, lm, (int)(value + 0.5f));
        return;
    }
    plexy_canvas_set_value(win->canvas, id, value);
}

void plexy_set_visible(PlexyWindow* win, uint32_t id, int visible) {
    if (win && win->canvas) plexy_canvas_set_visible(win->canvas, id, visible);
}

void plexy_set_enabled(PlexyWindow* win, uint32_t id, int enabled) {
    if (win && win->canvas) plexy_canvas_set_enabled(win->canvas, id, enabled);
}

void plexy_set_readonly(PlexyWindow* win, uint32_t id, int readonly) {
    if (win && win->canvas) plexy_canvas_set_readonly(win->canvas, id, readonly);
}



void plexy_set_corner_radius(PlexyWindow* win, uint32_t id, float radius) {
    if (win && win->canvas) plexy_canvas_set_corner_radius(win->canvas, id, radius);
}

void plexy_set_elevation(PlexyWindow* win, uint32_t id, float level) {
    if (win && win->canvas) plexy_canvas_set_elevation(win->canvas, id, level);
}

void plexy_set_fill_color(PlexyWindow* win, uint32_t id, float r, float g, float b, float a) {
    if (win && win->canvas) plexy_canvas_set_fill_color(win->canvas, id, r, g, b, a);
}

void plexy_set_fill_color_hex(PlexyWindow* win, uint32_t id, const char* hex) {
    float r, g, b, a;
    if (!win || !win->canvas) return;
    if (!plexy_parse_hex_color_rgba(hex, &r, &g, &b, &a)) return;
    plexy_canvas_set_fill_color(win->canvas, id, r, g, b, a);
}

void plexy_set_text_color(PlexyWindow* win, uint32_t id, float r, float g, float b, float a) {
    if (win && win->canvas) plexy_canvas_set_text_color(win->canvas, id, r, g, b, a);
}

void plexy_set_text_color_hex(PlexyWindow* win, uint32_t id, const char* hex) {
    float r, g, b, a;
    if (!win || !win->canvas) return;
    if (!plexy_parse_hex_color_rgba(hex, &r, &g, &b, &a)) return;
    plexy_canvas_set_text_color(win->canvas, id, r, g, b, a);
}

void plexy_set_font_size(PlexyWindow* win, uint32_t id, float size) {
    if (win && win->canvas) plexy_canvas_set_font_size(win->canvas, id, size);
}

void plexy_set_border(PlexyWindow* win, uint32_t id, float r, float g, float b, float a, float width) {
    if (win && win->canvas) plexy_canvas_set_border(win->canvas, id, r, g, b, a, width);
}

void plexy_set_glass(PlexyWindow* win, uint32_t id, float opacity) {
    if (win && win->canvas) plexy_canvas_set_fill_color(win->canvas, id, 1.f, 1.f, 1.f, opacity);
}

void plexy_set_glass_material(PlexyWindow* win, uint32_t id, float opacity) {
    if (win && win->canvas) plexy_canvas_set_glass_material(win->canvas, id, opacity);
}

void plexy_set_surface_material(PlexyWindow* win, uint32_t id) {
    if (win && win->canvas) plexy_canvas_set_surface_material(win->canvas, id);
}

void plexy_enable_blur_background(PlexyWindow* win, int enable) {
    if (win && win->proto_win) {
        plexy_window_set_glass_blur(win->proto_win, enable != 0);
    }
}



void plexy_on_click(PlexyWindow* win, uint32_t id, PlexyClickCallback cb, void* userdata) {
    if (!win || !win->canvas) return;
    WidgetCBEntry* e = get_or_create_cb(win, id);
    if (e) { e->on_click = cb; e->on_click_data = userdata; }
    plexy_canvas_on_click(win->canvas, id, bridge_click, win);
}

void plexy_on_value_changed(PlexyWindow* win, uint32_t id, PlexyValueCallback cb, void* userdata) {
    if (!win || !win->canvas) return;
    WidgetCBEntry* e = get_or_create_cb(win, id);
    if (e) { e->on_value = cb; e->on_value_data = userdata; }
    plexy_canvas_on_value_changed(win->canvas, id, bridge_value, win);
}

void plexy_on_text_changed(PlexyWindow* win, uint32_t id, PlexyTextCallback cb, void* userdata) {
    if (!win || !win->canvas) return;
    WidgetCBEntry* e = get_or_create_cb(win, id);
    if (e) { e->on_text = cb; e->on_text_data = userdata; }
    plexy_canvas_on_text_changed(win->canvas, id, bridge_text, win);
}

void plexy_on_key(PlexyWindow* win, uint32_t id, PlexyKeyCallback cb, void* userdata) {
    if (!win || !win->canvas) return;
    WidgetCBEntry* e = get_or_create_cb(win, id);
    if (e) { e->on_key = cb; e->on_key_data = userdata; }
    plexy_canvas_on_key(win->canvas, id, (PlexyCanvasKeyCallback)bridge_key, win);
}

void plexy_set_focus(PlexyWindow* win, uint32_t id) {
    if (!win || !win->canvas) return;
    plexy_canvas_set_focus(win->canvas, id);
}

void plexy_on_menu(PlexyWindow* win, PlexyMenuCallback cb, void* userdata) {
    if (!win) return;
    win->menu_cb = cb;
    win->menu_cb_data = userdata;
}

int plexy_menu_begin(PlexyWindow* win, int supported, const char* app_title) {
    if (!win || !win->proto_win) return -1;
    return plexy_window_menu_begin(win->proto_win, supported != 0, app_title);
}

int plexy_menu_add(PlexyWindow* win, uint32_t menu_id, const char* title) {
    if (!win || !win->proto_win) return -1;
    return plexy_window_menu_add(win->proto_win, menu_id, title);
}

int plexy_menu_add_item(PlexyWindow* win, uint32_t menu_id, uint32_t item_id,
                        const char* label, int enabled) {
    if (!win || !win->proto_win) return -1;
    return plexy_window_menu_add_item(win->proto_win, menu_id, item_id, label, enabled != 0);
}

int plexy_menu_commit(PlexyWindow* win) {
    if (!win || !win->proto_win) return -1;
    return plexy_window_menu_commit(win->proto_win);
}



void plexy_batch_begin(PlexyWindow* win) { (void)win; }
void plexy_batch_end(PlexyWindow* win) { (void)win; }



void plexy_path_clear(PlexyWindow* win) {
    if (win && win->canvas) plexy_canvas_clear_paths(win->canvas);
}

void plexy_path_begin(PlexyWindow* win) {
    if (win && win->canvas) plexy_canvas_begin_path(win->canvas);
}

void plexy_path_move_to(PlexyWindow* win, float x, float y) {
    if (win && win->canvas) plexy_canvas_move_to(win->canvas, x, y);
}

void plexy_path_line_to(PlexyWindow* win, float x, float y) {
    if (win && win->canvas) plexy_canvas_line_to(win->canvas, x, y);
}

void plexy_path_bezier_to(PlexyWindow* win,
                          float cp1x, float cp1y,
                          float cp2x, float cp2y,
                          float x, float y) {
    if (win && win->canvas) {
        plexy_canvas_bezier_to(win->canvas, cp1x, cp1y, cp2x, cp2y, x, y);
    }
}

void plexy_path_set_stroke(PlexyWindow* win, float r, float g, float b, float a, float width) {
    if (!win || !win->canvas) return;
    plexy_canvas_set_path_stroke_color(win->canvas, r, g, b, a);
    plexy_canvas_set_path_stroke_width(win->canvas, width);
}

void plexy_path_stroke(PlexyWindow* win) {
    if (win && win->canvas) plexy_canvas_stroke_path(win->canvas);
}

int plexy_widget_bounds(PlexyWindow* win, uint32_t widget_id,
                        float* x, float* y, float* width, float* height) {
    if (!win || !win->canvas || widget_id == 0) return 0;
    return plexy_canvas_get_widget_bounds(win->canvas, widget_id, x, y, width, height);
}
