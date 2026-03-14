#pragma once

#include "arena.hpp"
#include <cstdint>
#include <cstring>
#include <cmath>

namespace plexy::ui {



struct Color {
    float r, g, b, a;

    static Color rgba(float r, float g, float b, float a = 1.0f) { return {r, g, b, a}; }
    static Color hex(uint32_t argb) {
        return {
            ((argb >> 16) & 0xFF) / 255.0f,
            ((argb >> 8) & 0xFF) / 255.0f,
            (argb & 0xFF) / 255.0f,
            ((argb >> 24) & 0xFF) / 255.0f,
        };
    }
    static Color none() { return {0, 0, 0, 0}; }

    Color with_alpha(float a_) const { return {r, g, b, a_}; }

    Color lerp(const Color& to, float t) const {
        return {
            r + (to.r - r) * t,
            g + (to.g - g) * t,
            b + (to.b - b) * t,
            a + (to.a - a) * t,
        };
    }
};

struct Vec2 { float x, y; };



struct Material {
    enum class Type : uint8_t {
        Solid,
        LinearGradient,
        Glass,
        Vibrancy,
    };

    Type type = Type::Solid;

    
    Color color = {1, 1, 1, 1};
    Color color_end = {1, 1, 1, 1};  
    float gradient_angle = 0;

    
    float blur_radius = 0;
    float opacity = 1.0f;
    Color tint = {1, 1, 1, 0};
    float saturation = 1.0f;
    float noise = 0;

    
    Color border_color = Color::none();
    float border_width = 0;

    
    float inner_highlight = 0;
    float inner_shadow = 0;

    static Material solid(Color c) {
        Material m;
        m.type = Type::Solid;
        m.color = c;
        return m;
    }

    static Material glass(float blur, float opacity_, Color tint_,
                          float sat = 1.3f, float highlight = 0.25f, float shadow = 0.08f) {
        Material m;
        m.type = Type::Glass;
        m.blur_radius = blur;
        m.opacity = opacity_;
        m.tint = tint_;
        m.saturation = sat;
        m.inner_highlight = highlight;
        m.inner_shadow = shadow;
        return m;
    }
};



struct Shadow {
    Vec2 offset = {0, 0};
    float blur_sigma = 0;
    float spread = 0;
    Color color = Color::none();

    static Shadow none() { return {}; }
    static Shadow make(float ox, float oy, float blur, Color c, float spread_ = 0) {
        return {{ox, oy}, blur, spread_, c};
    }
};



enum class PaintOp : uint8_t {
    RoundedRect,
    Text,
    Shadow,
    Line,
    Icon,
    ClipPush,
    ClipPop,
    BlurRect,
};

struct PaintCmd {
    PaintOp op;
    uint16_t z_order;
    float x, y, w, h;   
    Material material;

    union {
        struct { float tl, tr, br, bl; } radii;
        struct { uint32_t glyph_run_id; Color color; float font_size; } text;
        struct { float sigma; float ox, oy; Color color; float spread; float radius; } shadow;
        struct { float x1, y1, x2, y2, thickness; Color color; } line;
        struct { uint32_t texture_id; Color tint; float u0, v0, u1, v1; } icon;
        struct { float blur_radius; } blur;
    };
};



struct PaintList {
    PaintCmd* cmds = nullptr;
    uint32_t count = 0;
    uint32_t capacity = 0;
    FrameArena* arena = nullptr;

    void init(FrameArena* a, uint32_t initial_cap = 256) {
        arena = a;
        cmds = arena->alloc_array<PaintCmd>(initial_cap);
        capacity = initial_cap;
        count = 0;
    }

    void push(const PaintCmd& cmd) {
        if (count >= capacity) {
            uint32_t new_cap = capacity * 2;
            PaintCmd* new_cmds = arena->alloc_array<PaintCmd>(new_cap);
            std::memcpy(new_cmds, cmds, sizeof(PaintCmd) * count);
            cmds = new_cmds;
            capacity = new_cap;
        }
        cmds[count++] = cmd;
    }

    void push_rounded_rect(float x, float y, float w, float h,
                           float radius, const Material& mat) {
        PaintCmd cmd{};
        cmd.op = PaintOp::RoundedRect;
        cmd.x = x; cmd.y = y; cmd.w = w; cmd.h = h;
        cmd.material = mat;
        cmd.radii = {radius, radius, radius, radius};
        push(cmd);
    }

    void push_rounded_rect4(float x, float y, float w, float h,
                            float tl, float tr, float br, float bl,
                            const Material& mat) {
        PaintCmd cmd{};
        cmd.op = PaintOp::RoundedRect;
        cmd.x = x; cmd.y = y; cmd.w = w; cmd.h = h;
        cmd.material = mat;
        cmd.radii = {tl, tr, br, bl};
        push(cmd);
    }

    void push_shadow(float x, float y, float w, float h,
                     float radius, const Shadow& s) {
        PaintCmd cmd{};
        cmd.op = PaintOp::Shadow;
        cmd.x = x; cmd.y = y; cmd.w = w; cmd.h = h;
        cmd.shadow.sigma = s.blur_sigma;
        cmd.shadow.ox = s.offset.x;
        cmd.shadow.oy = s.offset.y;
        cmd.shadow.color = s.color;
        cmd.shadow.spread = s.spread;
        cmd.shadow.radius = radius;
        push(cmd);
    }

    void push_text(float x, float y, uint32_t glyph_run_id, Color color, float font_size) {
        PaintCmd cmd{};
        cmd.op = PaintOp::Text;
        cmd.x = x; cmd.y = y;
        cmd.text.glyph_run_id = glyph_run_id;
        cmd.text.color = color;
        cmd.text.font_size = font_size;
        push(cmd);
    }

    void push_icon(float x, float y, float w, float h, uint32_t texture_id, Color tint) {
        PaintCmd cmd{};
        cmd.op = PaintOp::Icon;
        cmd.x = x; cmd.y = y; cmd.w = w; cmd.h = h;
        cmd.icon.texture_id = texture_id;
        cmd.icon.tint = tint;
        cmd.icon.u0 = 0.0f; cmd.icon.v0 = 0.0f;
        cmd.icon.u1 = 1.0f; cmd.icon.v1 = 1.0f;
        push(cmd);
    }

    void push_line(float x1, float y1, float x2, float y2, float thickness, Color color) {
        PaintCmd cmd{};
        cmd.op = PaintOp::Line;
        cmd.x = (x1 < x2) ? x1 : x2;
        cmd.y = (y1 < y2) ? y1 : y2;
        cmd.w = std::fabs(x2 - x1);
        cmd.h = std::fabs(y2 - y1);
        cmd.line = {x1, y1, x2, y2, thickness, color};
        push(cmd);
    }
};



enum WidgetFlags : uint32_t {
    WIDGET_VISIBLE   = 1 << 0,
    WIDGET_ENABLED   = 1 << 1,
    WIDGET_FOCUSED   = 1 << 2,
    WIDGET_HOVERED   = 1 << 3,
    WIDGET_PRESSED   = 1 << 4,
    WIDGET_CHECKED   = 1 << 5,
    WIDGET_DIRTY     = 1 << 6,
    WIDGET_READONLY  = 1 << 7,  
};

enum class WidgetType : uint8_t {
    Label,
    Button,
    IconButton,
    ToggleButton,
    TextInput,
    TextArea,
    Checkbox,
    RadioButton,
    Switch,
    Slider,
    ProgressBar,
    Panel,
    ScrollView,
    Separator,
    Spacer,
    Dropdown,
    Menu,
    ContextMenu,
    Tooltip,
    Popover,
    Dialog,
    TabBar,
    TabView,
    List,
    IconView,
    Table,
    TreeView,
    Toolbar,
    StatusBar,
    SplitView,
    COUNT,
};

struct StringId {
    uint32_t id = 0;
    bool valid() const { return id != 0; }
};


typedef void (*WidgetCallback)(uint32_t widget_id, void* userdata);
typedef void (*WidgetValueCallback)(uint32_t widget_id, float value, void* userdata);
typedef void (*WidgetTextCallback)(uint32_t widget_id, const char* text, void* userdata);
typedef void (*WidgetKeyCallback)(uint32_t widget_id, uint32_t key, uint32_t mods, void* userdata);

} 
