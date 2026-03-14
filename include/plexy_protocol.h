#pragma once

#include <stdint.h>

#define PLEXY_SOCKET_NAME "plexy.sock"

#define PLEXY_PROTOCOL_VERSION 1

enum PlexyOpcode {
    PLEXY_OP_HELLO = 1,
    PLEXY_OP_HELLO_REPLY = 2,
    PLEXY_OP_CREATE_WINDOW = 3,
    PLEXY_OP_WINDOW_CREATED = 4,
    PLEXY_OP_ATTACH_BUFFER = 5,
    PLEXY_OP_BUFFER_ATTACHED = 6,
    PLEXY_OP_COMMIT = 7,
    PLEXY_OP_FRAME_DONE = 8,
    PLEXY_OP_CONFIGURE = 9,
    PLEXY_OP_CONFIGURE_ACK = 10,
    PLEXY_OP_DESTROY_WINDOW = 11,
    PLEXY_OP_WINDOW_CLOSED = 12,
    PLEXY_OP_POINTER_ENTER = 13,
    PLEXY_OP_POINTER_LEAVE = 14,
    PLEXY_OP_POINTER_MOTION = 15,
    PLEXY_OP_POINTER_BUTTON = 16,
    PLEXY_OP_KEY = 17,
    PLEXY_OP_FOCUS_IN = 18,
    PLEXY_OP_FOCUS_OUT = 19,
    PLEXY_OP_POINTER_AXIS = 22,      
    PLEXY_OP_MODIFIERS = 23,         
    PLEXY_OP_REQUEST_MOVE = 24,      
    PLEXY_OP_REQUEST_RESIZE = 25,    
    PLEXY_OP_REQUEST_MAXIMIZE = 26,  
    PLEXY_OP_REQUEST_MINIMIZE = 27,  
    PLEXY_OP_SET_GLASS_EFFECT = 20,
    PLEXY_OP_SET_DECORATIONS = 21,  
    
    
    PLEXY_OP_CREATE_LAYER_SURFACE = 30,
    PLEXY_OP_LAYER_SURFACE_CREATED = 31,
    PLEXY_OP_DESTROY_LAYER_SURFACE = 32,
    
    
    PLEXY_OP_GET_WINDOW_LIST = 40,
    PLEXY_OP_WINDOW_LIST_REPLY = 41,
    PLEXY_OP_WINDOW_EVENT = 42,
    PLEXY_OP_RAISE_WINDOW = 43,
    PLEXY_OP_MINIMIZE_WINDOW = 44,
    PLEXY_OP_RESTORE_WINDOW = 45,
    PLEXY_OP_GET_DOCK_GEOMETRY = 46,
    PLEXY_OP_DOCK_GEOMETRY_REPLY = 47,
    
    
    PLEXY_OP_SCENE_CAPS = 50,
    PLEXY_OP_SCENE_TEXTURE_READY = 51,  
    PLEXY_OP_FRAME_READY = 52,           
    PLEXY_OP_ATTACH_LAYER_BUFFER = 53,   
    PLEXY_OP_COMMIT_LAYER_SURFACE = 54,  
    PLEXY_OP_SCENE_SHM_READY = 55,       
    
    
    PLEXY_OP_CREATE_TRANSPARENT_SURFACE = 60,
    PLEXY_OP_UPDATE_TRANSPARENT_SURFACE = 61,
    PLEXY_OP_DESTROY_TRANSPARENT_SURFACE = 62,
    PLEXY_OP_TRANSPARENT_SURFACE_CREATED = 63,
    
    
    PLEXY_OP_OUTPUT_INFO = 70,
    PLEXY_OP_OUTPUT_SCALE = 71,
    PLEXY_OP_OUTPUT_DONE = 72,
    PLEXY_OP_SURFACE_ENTER_OUTPUT = 73,
    PLEXY_OP_SURFACE_LEAVE_OUTPUT = 74,
    PLEXY_OP_SET_BUFFER_SCALE = 75,
    PLEXY_OP_PREFERRED_BUFFER_SCALE = 76,
    
    
    PLEXY_OP_ATTACH_DMABUF = 80,
    PLEXY_OP_DMABUF_ATTACHED = 81,
    
    
    PLEXY_OP_CREATE_POPUP = 90,
    PLEXY_OP_POPUP_CREATED = 91,
    PLEXY_OP_SET_INPUT_REGION_MODE = 92,
    PLEXY_OP_INPUT_REGION_OP = 93,
    PLEXY_OP_APP_MENU_RESET = 94,
    PLEXY_OP_APP_MENU_ADD = 95,
    PLEXY_OP_APP_MENU_ADD_ITEM = 96,
    PLEXY_OP_APP_MENU_COMMIT = 97,
    PLEXY_OP_APP_MENU_ACTIVATE = 98,
    PLEXY_OP_CLIPBOARD_SET_TEXT = 99,
    PLEXY_OP_CLIPBOARD_GET_TEXT = 100,
    PLEXY_OP_CLIPBOARD_TEXT = 101,
    PLEXY_OP_UPDATE_POPUP = 102,
    PLEXY_OP_SET_CURSOR_SHAPE = 103,
    PLEXY_OP_SET_WINDOW_TYPE = 104,

    
    
    PLEXY_OP_RAW_POINTER_MOTION = 110,
    PLEXY_OP_RAW_POINTER_BUTTON = 111,
    PLEXY_OP_RAW_POINTER_AXIS   = 112,
    PLEXY_OP_WINDOW_POSITION    = 113,  
    PLEXY_OP_SUBSCRIBE_RAW_INPUT = 114, 

    
    PLEXY_OP_REQUEST_FULLSCREEN  = 120, 
    PLEXY_OP_SET_WINDOW_TITLE    = 121, 
    PLEXY_OP_SET_APP_ID          = 122, 
    PLEXY_OP_WINDOW_STATE_CHANGED = 123,
    PLEXY_OP_SESSION_LOCK        = 124, 
    PLEXY_OP_SESSION_UNLOCK      = 125, 
    PLEXY_OP_CLIPBOARD_DATA_FD   = 126, 
    PLEXY_OP_DARK_MODE_CHANGED   = 127, 
    PLEXY_OP_SET_WINDOW_GEOMETRY = 128, 

    
    PLEXY_OP_UI_CREATE_WIDGET = 0x100,
    PLEXY_OP_UI_WIDGET_CREATED = 0x101,
    PLEXY_OP_UI_DESTROY_WIDGET = 0x102,
    PLEXY_OP_UI_SET_TEXT = 0x103,
    PLEXY_OP_UI_SET_VALUE = 0x104,
    PLEXY_OP_UI_SET_LAYOUT = 0x105,
    PLEXY_OP_UI_SET_STYLE = 0x106,
    PLEXY_OP_UI_SET_FLAGS = 0x107,
    PLEXY_OP_UI_SET_CALLBACKS = 0x108,
    PLEXY_OP_UI_BATCH_BEGIN = 0x109,
    PLEXY_OP_UI_BATCH_END = 0x10A,

    
    PLEXY_OP_UI_EVENT_CLICK = 0x1F0,
    PLEXY_OP_UI_EVENT_VALUE_CHANGED = 0x1F1,
    PLEXY_OP_UI_EVENT_TEXT_CHANGED = 0x1F2,
    PLEXY_OP_UI_EVENT_KEY = 0x1F3,
    PLEXY_OP_UI_EVENT_FOCUS = 0x1F4,

    PLEXY_OP_ERROR = 255
};

enum PlexyError {
    PLEXY_ERR_NONE = 0,
    PLEXY_ERR_INVALID_OPCODE = 1,
    PLEXY_ERR_INVALID_WINDOW = 2,
    PLEXY_ERR_BUFFER_FAILED = 3,
    PLEXY_ERR_OUT_OF_MEMORY = 4,
    PLEXY_ERR_PROTOCOL_VERSION = 5
};

#ifndef PLEXY_FORMAT_ARGB8888
#define PLEXY_FORMAT_ARGB8888 0
#define PLEXY_FORMAT_XRGB8888 1
#define PLEXY_FORMAT_RGBA8888 2
#endif

enum PlexyLayer {
    PLEXY_LAYER_BACKGROUND = 0,
    PLEXY_LAYER_BOTTOM = 1,
    PLEXY_LAYER_TOP = 2,
    PLEXY_LAYER_OVERLAY = 3
};
typedef enum PlexyLayer PlexyLayer;

enum PlexyAnchor {
    PLEXY_ANCHOR_NONE = 0,
    PLEXY_ANCHOR_TOP = 1,
    PLEXY_ANCHOR_BOTTOM = 2,
    PLEXY_ANCHOR_LEFT = 4,
    PLEXY_ANCHOR_RIGHT = 8
};
typedef enum PlexyAnchor PlexyAnchor;

enum PlexyWindowEventType {
    PLEXY_WINDOW_EVENT_CREATED = 0,
    PLEXY_WINDOW_EVENT_DESTROYED = 1,
    PLEXY_WINDOW_EVENT_MINIMIZED = 2,
    PLEXY_WINDOW_EVENT_RESTORED = 3,
    PLEXY_WINDOW_EVENT_FOCUSED = 4,
    PLEXY_WINDOW_EVENT_UNFOCUSED = 5,
    PLEXY_WINDOW_EVENT_TITLE_CHANGED = 6
};
typedef enum PlexyWindowEventType PlexyWindowEventType;

#pragma pack(push, 1)

typedef struct {
    uint16_t opcode;
    uint16_t length;
} PlexyHeader;

typedef struct {
    PlexyHeader header;
    uint32_t version;
    char name[64];
} PlexyHello;

typedef struct {
    PlexyHeader header;
    uint32_t version;
    uint32_t client_id;
    float ui_scale;
    uint32_t screen_width;
    uint32_t screen_height;
    uint32_t dark_mode;      
} PlexyHelloReply;

typedef struct {
    PlexyHeader header;
    int32_t x;                        
    int32_t y;
    uint32_t width;                   
    uint32_t height;
    uint32_t buffer_scale;            
    char title[128];
} PlexyCreateWindow;

typedef struct {
    PlexyHeader header;
    uint32_t window_id;
    uint32_t width;                   
    uint32_t height;
    float scale_factor;               
    uint32_t buffer_scale;            
} PlexyWindowCreated;

typedef struct {
    PlexyHeader header;
    uint32_t window_id;
    uint32_t width;
    uint32_t height;
    uint32_t stride;
    uint32_t format;
    uint32_t shm_size;
} PlexyAttachBuffer;

typedef struct {
    PlexyHeader header;
    uint32_t window_id;
    uint32_t buffer_id;
} PlexyBufferAttached;

typedef struct {
    PlexyHeader header;
    uint32_t window_id;
    int32_t cursor_cell_x;
    int32_t cursor_cell_y;
    int32_t cursor_cols;
    int32_t cursor_rows;
    
    int32_t damage_x;
    int32_t damage_y;
    int32_t damage_width;
    int32_t damage_height;
} PlexyCommit;

typedef struct {
    PlexyHeader header;
    uint32_t window_id;
    uint64_t frame_time_ns;
} PlexyFrameDone;

typedef struct {
    PlexyHeader header;
    uint32_t window_id;
    uint32_t width;                   
    uint32_t height;
    float scale_factor;               
    uint32_t buffer_scale;            
    uint32_t serial;
    uint32_t state_flags;             
} PlexyConfigure;

enum PlexyWindowStateFlags {
    PLEXY_STATE_MAXIMIZED   = 1u << 0,
    PLEXY_STATE_FULLSCREEN  = 1u << 1,
    PLEXY_STATE_RESIZING    = 1u << 2,
    PLEXY_STATE_ACTIVATED   = 1u << 3,
    PLEXY_STATE_TILED_LEFT  = 1u << 4,
    PLEXY_STATE_TILED_RIGHT = 1u << 5,
    PLEXY_STATE_SUSPENDED   = 1u << 6,
};

typedef struct {
    PlexyHeader header;
    uint32_t window_id;
    uint32_t serial;
} PlexyConfigureAck;

typedef struct {
    PlexyHeader header;
    uint32_t window_id;
} PlexyDestroyWindow;

typedef struct {
    PlexyHeader header;
    uint32_t window_id;
} PlexyWindowClosed;

typedef struct {
    PlexyHeader header;
    uint32_t window_id;
    int32_t x;
    int32_t y;
} PlexyPointerEnter;

typedef struct {
    PlexyHeader header;
    uint32_t window_id;
} PlexyPointerLeave;

typedef struct {
    PlexyHeader header;
    uint32_t window_id;
    int32_t x;
    int32_t y;
} PlexyPointerMotion;

typedef struct {
    PlexyHeader header;
    uint32_t window_id;
    uint32_t button;
    uint32_t state;
    int32_t x;
    int32_t y;
} PlexyPointerButton;

typedef struct {
    PlexyHeader header;
    uint32_t window_id;
    uint32_t keycode;
    uint32_t state;
    uint32_t modifiers;
} PlexyKey;

typedef struct {
    PlexyHeader header;
    uint32_t window_id;
} PlexyFocusIn;

typedef struct {
    PlexyHeader header;
    uint32_t window_id;
} PlexyFocusOut;


typedef struct {
    PlexyHeader header;
    uint32_t window_id;
    int32_t axis;           
    int32_t value;          
    int32_t discrete;       
} PlexyPointerAxis;


typedef struct {
    PlexyHeader header;
    uint32_t window_id;
    uint32_t depressed;     
    uint32_t latched;       
    uint32_t locked;        
    uint32_t group;         
} PlexyModifiers;



typedef struct {
    PlexyHeader header;
    int32_t screen_x;       
    int32_t screen_y;       
    int32_t dx;             
    int32_t dy;             
    uint32_t window_id;     
} PlexyRawPointerMotion;

typedef struct {
    PlexyHeader header;
    uint32_t button;        
    uint32_t state;         
} PlexyRawPointerButton;

typedef struct {
    PlexyHeader header;
    int32_t axis;           
    int32_t value;          
    int32_t discrete;       
} PlexyRawPointerAxis;

typedef struct {
    PlexyHeader header;
    uint32_t window_id;
    int32_t  screen_x;      
    int32_t  screen_y;      
    int32_t  width;          
    int32_t  height;         
    uint8_t  visible;        
    uint8_t  pad[3];
} PlexyWindowPosition;

enum PlexyRawInputEventMask {
    PLEXY_RAW_INPUT_EVENT_POINTER_MOTION = 1u << 0,
    PLEXY_RAW_INPUT_EVENT_POINTER_BUTTON = 1u << 1,
    PLEXY_RAW_INPUT_EVENT_POINTER_AXIS = 1u << 2,
    PLEXY_RAW_INPUT_EVENT_ALL =
        PLEXY_RAW_INPUT_EVENT_POINTER_MOTION |
        PLEXY_RAW_INPUT_EVENT_POINTER_BUTTON |
        PLEXY_RAW_INPUT_EVENT_POINTER_AXIS
};

typedef struct {
    PlexyHeader header;
    uint32_t event_mask; 
} PlexySubscribeRawInput;



typedef struct {
    PlexyHeader header;
    uint32_t window_id;
    uint32_t fullscreen;  
} PlexyRequestFullscreen;

typedef struct {
    PlexyHeader header;
    uint32_t window_id;
    char title[256];
} PlexySetWindowTitle;

typedef struct {
    PlexyHeader header;
    uint32_t window_id;
    char app_id[256];
} PlexySetAppId;

typedef struct {
    PlexyHeader header;
    uint32_t window_id;
    uint32_t state_flags;  
} PlexyWindowStateChanged;

typedef struct {
    PlexyHeader header;
    uint32_t locked;       
} PlexySessionLock;

typedef struct {
    PlexyHeader header;
} PlexySessionUnlock;

typedef struct {
    PlexyHeader header;
    uint32_t dark;  
} PlexyDarkModeChanged;

typedef struct {
    PlexyHeader header;
    uint32_t window_id;
    uint32_t data_size;    
    char mime_type[64];    
} PlexyClipboardDataFd;

typedef struct {
    PlexyHeader header;
    uint32_t window_id;
    int32_t min_width;
    int32_t min_height;
    int32_t max_width;     
    int32_t max_height;    
} PlexySetWindowGeometry;

typedef struct {
    PlexyHeader header;
    uint32_t error_code;
    char message[128];
} PlexyErrorMsg;

typedef struct {
    PlexyHeader header;
    uint32_t window_id;
    uint32_t blur_background;    
    uint32_t reserved[2];        
} PlexySetGlassEffect;

typedef struct {
    PlexyHeader header;
    uint32_t window_id;
    uint32_t enabled;  
} PlexySetDecorations;

typedef struct {
    PlexyHeader header;
    uint32_t window_id;
    uint32_t enabled;  
} PlexySetInputRegionMode;

typedef struct {
    PlexyHeader header;
    uint32_t window_id;
    int32_t x;
    int32_t y;
    int32_t width;
    int32_t height;
    uint32_t add;  
} PlexyInputRegionOp;

typedef struct {
    PlexyHeader header;
    uint32_t layer;              
    uint32_t anchor;             
    uint32_t width;              
    uint32_t height;             
    int32_t exclusive_zone;      
    uint32_t keyboard_interactivity; 
} PlexyCreateLayerSurface;

typedef struct {
    PlexyHeader header;
    uint32_t surface_id;
} PlexyLayerSurfaceCreated;

typedef struct {
    PlexyHeader header;
    uint32_t surface_id;
} PlexyDestroyLayerSurface;

typedef struct {
    PlexyHeader header;
} PlexyGetWindowList;

typedef struct {
    PlexyHeader header;
    uint32_t count;
    
} PlexyWindowListReply;

typedef struct {
    uint32_t window_id;
    uint32_t state;              
    char title[128];
} PlexyWindowInfo;

typedef struct {
    PlexyHeader header;
    uint32_t event_type;         
    uint32_t window_id;
    uint32_t state;              
    char title[128];             
} PlexyWindowEvent;

#define PLEXY_RAISE_FLAG_ACTIVATE  0x1  

typedef struct {
    PlexyHeader header;
    uint32_t window_id;
    uint32_t flags;
} PlexyRaiseWindow;

typedef struct {
    PlexyHeader header;
    uint32_t window_id;
} PlexyMinimizeWindow;

typedef struct {
    PlexyHeader header;
    uint32_t window_id;
} PlexyRestoreWindow;

typedef struct {
    PlexyHeader header;
    uint32_t window_id;
} PlexyRequestMove;

typedef struct {
    PlexyHeader header;
    uint32_t window_id;
    uint32_t edges;      
} PlexyRequestResize;

typedef struct {
    PlexyHeader header;
    uint32_t window_id;
    uint32_t maximized;  
} PlexyRequestMaximize;

typedef struct {
    PlexyHeader header;
    uint32_t window_id;
} PlexyRequestMinimize;

typedef struct {
    PlexyHeader header;
} PlexyGetDockGeometry;

typedef struct {
    PlexyHeader header;
    float center_x;              
    float center_y;
    float width;
    float height;
} PlexyDockGeometryReply;

typedef struct {
    PlexyHeader header;
    uint32_t dma_buf_supported;  
    uint32_t modifiers_supported; 
    uint32_t reserved[2];
} PlexySceneCaps;

typedef struct {
    PlexyHeader header;
    uint32_t width;
    uint32_t height;
    uint32_t format;             
    uint32_t stride;
    uint32_t offset;             
    uint64_t modifier;           
    
} PlexySceneTextureReady;

typedef struct {
    PlexyHeader header;
    uint32_t frame_number;       
    uint32_t reserved;
} PlexyFrameReady;

typedef struct {
    PlexyHeader header;
    uint32_t width;
    uint32_t height;
    uint32_t stride;             
    uint32_t format;             
    uint32_t shm_size;           
    
} PlexySceneShmReady;

typedef struct {
    PlexyHeader header;
    uint32_t surface_id;         
    uint32_t width;
    uint32_t height;
    uint32_t stride;
    uint32_t format;
    uint32_t shm_size;
    
} PlexyAttachLayerBuffer;

typedef struct {
    PlexyHeader header;
    uint32_t surface_id;
} PlexyCommitLayerSurface;

typedef struct {
    PlexyHeader header;
    uint32_t surface_id;
    uint32_t width;
    uint32_t height;
    uint32_t stride;
    uint32_t format;             
    int32_t fd;                  
    float x;                     
    float y;
    float blur_radius;           
    float refraction_strength;   
    uint32_t chromatic_aberration; 
    uint32_t needs_glass_effect;  
} PlexyTransparentSurfaceCreate;

typedef struct {
    PlexyHeader header;
    uint32_t surface_id;
    float x;                     
    float y;
    float blur_radius;
    float refraction_strength;
    uint32_t chromatic_aberration;
} PlexyUpdateTransparentSurface;

typedef struct {
    PlexyHeader header;
    uint32_t surface_id;
} PlexyDestroyTransparentSurface;

typedef struct {
    PlexyHeader header;
    uint32_t surface_id;
    uint32_t result;             
} PlexyTransparentSurfaceCreated;

typedef struct {
    PlexyHeader header;
    uint32_t output_id;
    int32_t x;                        
    int32_t y;
    uint32_t physical_width_mm;       
    uint32_t physical_height_mm;
    uint32_t pixel_width;             
    uint32_t pixel_height;
    float scale_factor;               
    uint32_t subpixel;                
    uint32_t transform;               
    char make[64];                    
    char model[64];                   
} PlexyOutputInfo;

typedef struct {
    PlexyHeader header;
    uint32_t output_id;
    float scale_factor;               
} PlexyOutputScale;

typedef struct {
    PlexyHeader header;
    uint32_t output_id;
} PlexyOutputDone;

typedef struct {
    PlexyHeader header;
    uint32_t window_id;
    uint32_t output_id;
} PlexySurfaceEnterOutput;

typedef struct {
    PlexyHeader header;
    uint32_t window_id;
    uint32_t output_id;
} PlexySurfaceLeaveOutput;

typedef struct {
    PlexyHeader header;
    uint32_t window_id;
    uint32_t scale;                   
} PlexySetBufferScale;

typedef struct {
    PlexyHeader header;
    uint32_t window_id;
    uint32_t preferred_scale;         
    float exact_scale;                
} PlexyPreferredBufferScale;


typedef struct {
    PlexyHeader header;
    uint32_t window_id;
    uint32_t width;
    uint32_t height;
    uint32_t stride;
    uint32_t format;                  
    uint64_t modifier;                
    uint32_t offset;                  
    uint32_t flags;                   
} PlexyAttachDmaBuf;

typedef struct {
    PlexyHeader header;
    uint32_t window_id;
    uint32_t buffer_id;
    uint32_t result;                  
} PlexyDmaBufAttached;


typedef struct {
    PlexyHeader header;
    uint32_t parent_window_id;        
    int32_t x;                        
    int32_t y;                        
    uint32_t width;                   
    uint32_t height;                  
    uint32_t buffer_scale;            
    uint32_t flags;                   
} PlexyCreatePopup;

enum PlexyPopupFlags {
    PLEXY_POPUP_FLAG_NONE = 0,
    PLEXY_POPUP_FLAG_BEHIND_PARENT = 1u << 0,
};

enum PlexyWindowType {
    PLEXY_WINDOW_TYPE_NORMAL = 0,
    PLEXY_WINDOW_TYPE_DIALOG = 1,
    PLEXY_WINDOW_TYPE_SPLASH = 2,
    PLEXY_WINDOW_TYPE_TOOLBAR = 3,
    PLEXY_WINDOW_TYPE_UTILITY = 4,
    PLEXY_WINDOW_TYPE_DOCK = 5,
    PLEXY_WINDOW_TYPE_DESKTOP = 6,
    PLEXY_WINDOW_TYPE_MENU = 7,
    PLEXY_WINDOW_TYPE_POPUP_MENU = 8,
    PLEXY_WINDOW_TYPE_DROPDOWN_MENU = 9,
    PLEXY_WINDOW_TYPE_TOOLTIP = 10,
    PLEXY_WINDOW_TYPE_NOTIFICATION = 11,
    PLEXY_WINDOW_TYPE_COMBO = 12,
    PLEXY_WINDOW_TYPE_DND = 13,
};

typedef struct {
    PlexyHeader header;
    uint32_t window_id;
    uint32_t window_type;   
} PlexySetWindowType;

typedef struct {
    PlexyHeader header;
    uint32_t window_id;
    int32_t x;
    int32_t y;
    uint32_t flags;
} PlexyUpdatePopup;

typedef struct {
    PlexyHeader header;
    uint32_t window_id;               
    uint32_t width;                   
    uint32_t height;                  
    float scale_factor;               
    uint32_t buffer_scale;            
} PlexyPopupCreated;

typedef struct {
    PlexyHeader header;
    uint32_t window_id;
    uint32_t supported;
    char app_title[64];
} PlexyAppMenuReset;

typedef struct {
    PlexyHeader header;
    uint32_t window_id;
    uint32_t menu_id;
    char title[64];
} PlexyAppMenuAdd;

typedef struct {
    PlexyHeader header;
    uint32_t window_id;
    uint32_t menu_id;
    uint32_t item_id;
    uint32_t enabled;
    char label[64];
} PlexyAppMenuAddItem;

typedef struct {
    PlexyHeader header;
    uint32_t window_id;
} PlexyAppMenuCommit;

typedef struct {
    PlexyHeader header;
    uint32_t window_id;
    uint32_t item_id;
} PlexyAppMenuActivate;

typedef struct {
    PlexyHeader header;
    uint32_t window_id;
    char text[1024];
} PlexyClipboardSetText;

typedef struct {
    PlexyHeader header;
    uint32_t window_id;
} PlexyClipboardGetText;

typedef struct {
    PlexyHeader header;
    uint32_t window_id;
    char text[1024];
} PlexyClipboardText;



enum PlexyUIWidgetType {
    PLEXY_UI_LABEL = 0,
    PLEXY_UI_BUTTON = 1,
    PLEXY_UI_TEXT_INPUT = 2,
    PLEXY_UI_CHECKBOX = 3,
    PLEXY_UI_SWITCH = 4,
    PLEXY_UI_SLIDER = 5,
    PLEXY_UI_PROGRESS = 6,
    PLEXY_UI_PANEL = 7,
    PLEXY_UI_SEPARATOR = 8,
    PLEXY_UI_SPACER = 9,
    PLEXY_UI_SCROLL_VIEW = 10,
    PLEXY_UI_ROW = 11,    
    PLEXY_UI_COLUMN = 12, 
};

enum PlexyUIFlexDir { PLEXY_UI_FLEX_ROW = 0, PLEXY_UI_FLEX_COLUMN = 1 };
enum PlexyUIJustify {
    PLEXY_UI_JUSTIFY_START = 0, PLEXY_UI_JUSTIFY_END = 1,
    PLEXY_UI_JUSTIFY_CENTER = 2, PLEXY_UI_JUSTIFY_SPACE_BETWEEN = 3,
    PLEXY_UI_JUSTIFY_SPACE_AROUND = 4, PLEXY_UI_JUSTIFY_SPACE_EVENLY = 5,
};
enum PlexyUIAlign {
    PLEXY_UI_ALIGN_START = 0, PLEXY_UI_ALIGN_END = 1,
    PLEXY_UI_ALIGN_CENTER = 2, PLEXY_UI_ALIGN_STRETCH = 3,
};

typedef struct {
    PlexyHeader header;
    uint32_t widget_type;
    uint32_t parent_id;      
    uint32_t request_id;     
} PlexyUICreateWidget;

typedef struct {
    PlexyHeader header;
    uint32_t widget_id;
    uint32_t request_id;
} PlexyUIWidgetCreated;

typedef struct {
    PlexyHeader header;
    uint32_t widget_id;
} PlexyUIDestroyWidget;

typedef struct {
    PlexyHeader header;
    uint32_t widget_id;
    char text[256];
} PlexyUISetText;

typedef struct {
    PlexyHeader header;
    uint32_t widget_id;
    float value;
    float value_min;
    float value_max;
} PlexyUISetValue;

typedef struct {
    PlexyHeader header;
    uint32_t widget_id;
    uint8_t direction;       
    uint8_t justify;         
    uint8_t align;           
    uint8_t reserved;
    float flex_grow;
    float flex_shrink;
    float width;             
    float height;            
    float margin[4];         
    float padding[4];        
    float gap;
} PlexyUISetLayout;

typedef struct {
    PlexyHeader header;
    uint32_t widget_id;
    float corner_radius;     
    float elevation;         
    float fill_r, fill_g, fill_b, fill_a;
    float border_r, border_g, border_b, border_a;
    float border_width;
    uint32_t material_type;  
} PlexyUISetStyle;

typedef struct {
    PlexyHeader header;
    uint32_t widget_id;
    uint32_t flags;          
} PlexyUISetFlags;

typedef struct {
    PlexyHeader header;
    uint32_t widget_id;
    uint32_t event_mask;     
} PlexyUISetCallbacks;

typedef struct {
    PlexyHeader header;
} PlexyUIBatchBegin;

typedef struct {
    PlexyHeader header;
} PlexyUIBatchEnd;


typedef struct {
    PlexyHeader header;
    uint32_t widget_id;
} PlexyUIEventClick;

typedef struct {
    PlexyHeader header;
    uint32_t widget_id;
    float value;
} PlexyUIEventValueChanged;

typedef struct {
    PlexyHeader header;
    uint32_t widget_id;
    char text[256];
} PlexyUIEventTextChanged;

typedef struct {
    PlexyHeader header;
    uint32_t widget_id;
    uint32_t keycode;
    uint32_t modifiers;
    uint32_t pressed;
} PlexyUIEventKey;

typedef struct {
    PlexyHeader header;
    uint32_t widget_id;
    uint32_t focused;        
} PlexyUIEventFocus;

typedef struct {
    PlexyHeader header;
    uint32_t window_id;
    uint32_t cursor_shape;   
} PlexySetCursorShape;

#pragma pack(pop)
