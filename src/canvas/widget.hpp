#pragma once

#include "types.hpp"
#include <cstdint>

namespace plexy::ui {



enum class FlexDirection : uint8_t { Row, Column };
enum class JustifyContent : uint8_t { Start, End, Center, SpaceBetween, SpaceAround, SpaceEvenly };
enum class AlignItems : uint8_t { Start, End, Center, Stretch };
enum class AlignSelf : uint8_t { Auto, Start, End, Center, Stretch };
enum class DrawerEdge : uint8_t { Left, Right, Top, Bottom };

struct LayoutNode {
    FlexDirection direction = FlexDirection::Column;
    JustifyContent justify = JustifyContent::Start;
    AlignItems align = AlignItems::Stretch;
    AlignSelf self_align = AlignSelf::Auto;

    float flex_grow = 0;
    float flex_shrink = 1;
    float flex_basis = -1;  

    float width = -1;   
    float height = -1;
    float min_width = 0;
    float min_height = 0;
    float max_width = 1e6f;
    float max_height = 1e6f;

    float margin[4] = {};    
    float padding[4] = {};   
    float gap = 0;

    
    float computed_x = 0, computed_y = 0;
    float computed_w = 0, computed_h = 0;

    uint32_t generation = 0;  
};



struct Widget {
    uint32_t id = 0;
    WidgetType type = WidgetType::Label;
    uint32_t flags = WIDGET_VISIBLE | WIDGET_ENABLED;

    
    StringId text = {};
    float value = 0;             
    float value_min = 0;
    float value_max = 1;
    float font_size = 0;         
    Color text_color = Color::none();
    bool has_text_color_override = false;
    float scroll_y = 0;          

    
    float corner_radius = -1;    
    Material material_override = {};
    bool has_material_override = false;
    float elevation = 0;         
    bool is_drawer = false;
    DrawerEdge drawer_edge = DrawerEdge::Left;
    float drawer_open = 1.0f;    
    float drawer_slide_size = -1.0f;  
    float drawer_anim_duration = 0.22f;

    
    LayoutNode layout;

    
    Widget* parent = nullptr;
    Widget* first_child = nullptr;
    Widget* next_sibling = nullptr;

    
    WidgetCallback on_click = nullptr;
    void* on_click_data = nullptr;

    WidgetValueCallback on_value_change = nullptr;
    void* on_value_change_data = nullptr;

    WidgetTextCallback on_text_change = nullptr;
    void* on_text_change_data = nullptr;

    WidgetKeyCallback on_key = nullptr;
    void* on_key_data = nullptr;

    
    uint32_t client_id = 0;

    

    bool has_flag(uint32_t f) const { return (flags & f) != 0; }
    void set_flag(uint32_t f) { flags |= f; }
    void clear_flag(uint32_t f) { flags &= ~f; }
    void toggle_flag(uint32_t f) { flags ^= f; }

    bool visible() const { return has_flag(WIDGET_VISIBLE); }
    bool enabled() const { return has_flag(WIDGET_ENABLED); }
    bool focused() const { return has_flag(WIDGET_FOCUSED); }
    bool hovered() const { return has_flag(WIDGET_HOVERED); }
    bool pressed() const { return has_flag(WIDGET_PRESSED); }
    bool checked() const { return has_flag(WIDGET_CHECKED); }
    bool dirty() const { return has_flag(WIDGET_DIRTY); }

    void mark_dirty() { set_flag(WIDGET_DIRTY); }
    void clear_dirty() { clear_flag(WIDGET_DIRTY); }
};

inline float widget_drawer_extent(const Widget* w) {
    if (!w) return 0.0f;
    if (w->drawer_slide_size > 0.0f) return w->drawer_slide_size;
    if (w->drawer_edge == DrawerEdge::Left || w->drawer_edge == DrawerEdge::Right) {
        return w->layout.computed_w;
    }
    return w->layout.computed_h;
}

inline void widget_effective_offset(const Widget* w, float* out_x, float* out_y) {
    float ox = 0.0f;
    float oy = 0.0f;
    for (const Widget* cur = w; cur; cur = cur->parent) {
        if (!cur->is_drawer) continue;
        float open = cur->drawer_open;
        if (open < 0.0f) open = 0.0f;
        if (open > 1.0f) open = 1.0f;
        float closed = 1.0f - open;
        float extent = widget_drawer_extent(cur);
        switch (cur->drawer_edge) {
            case DrawerEdge::Left:   ox -= extent * closed; break;
            case DrawerEdge::Right:  ox += extent * closed; break;
            case DrawerEdge::Top:    oy -= extent * closed; break;
            case DrawerEdge::Bottom: oy += extent * closed; break;
        }
    }
    if (out_x) *out_x = ox;
    if (out_y) *out_y = oy;
}

inline void widget_effective_rect(const Widget* w, float* x, float* y, float* width, float* height) {
    if (!w) return;
    float ox = 0.0f, oy = 0.0f;
    widget_effective_offset(w, &ox, &oy);
    if (x) *x = w->layout.computed_x + ox;
    if (y) *y = w->layout.computed_y + oy;
    if (width) *width = w->layout.computed_w;
    if (height) *height = w->layout.computed_h;
}

} 
