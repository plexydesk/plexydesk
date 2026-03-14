#include "layout.hpp"
#include <cmath>
#include <algorithm>
#include <cstdio>

namespace plexy::ui {

uint32_t LayoutEngine::count_children(const Widget* parent) {
    uint32_t n = 0;
    for (Widget* c = parent->first_child; c; c = c->next_sibling) {
        if (c->visible()) n++;
    }
    return n;
}

float LayoutEngine::clamp_width(const LayoutNode& node, float w) {
    if (w < node.min_width) w = node.min_width;
    if (w > node.max_width) w = node.max_width;
    return w;
}

float LayoutEngine::clamp_height(const LayoutNode& node, float h) {
    if (h < node.min_height) h = node.min_height;
    if (h > node.max_height) h = node.max_height;
    return h;
}

void LayoutEngine::measure_intrinsic(Widget* widget, float* out_w, float* out_h) {
    switch (widget->type) {
        case WidgetType::Label:       *out_w = 100; *out_h = 20; break;
        case WidgetType::Button:      *out_w = 80;  *out_h = 36; break;
        case WidgetType::TextInput:   *out_w = 200; *out_h = 36; break;
        case WidgetType::TextArea:    *out_w = 400; *out_h = 200; break;
        case WidgetType::Checkbox:
        case WidgetType::RadioButton: *out_w = 120; *out_h = 24; break;
        case WidgetType::Switch:      *out_w = 150; *out_h = 28; break;
        case WidgetType::Slider:      *out_w = 200; *out_h = 28; break;
        case WidgetType::ProgressBar: *out_w = 200; *out_h = 20; break;
        case WidgetType::List:        *out_w = 320; *out_h = 260; break;
        case WidgetType::IconView:    *out_w = 320; *out_h = 260; break;
        case WidgetType::Separator:   *out_w = 0;   *out_h = 1;  break;
        case WidgetType::Spacer:      *out_w = 0;   *out_h = 0;  break;
        case WidgetType::Panel:
        case WidgetType::ScrollView: {
            
            bool is_row = (widget->layout.direction == FlexDirection::Row);
            float total_main = 0, max_cross = 0;
            int child_count = 0;
            
            for (Widget* c = widget->first_child; c; c = c->next_sibling) {
                if (!c->visible()) continue;
                
                float cw, ch;
                if (c->layout.width >= 0) cw = c->layout.width;
                else { float tw, th; measure_intrinsic(c, &tw, &th); cw = tw; }
                if (c->layout.height >= 0) ch = c->layout.height;
                else { float tw, th; measure_intrinsic(c, &tw, &th); ch = th; }
                
                if (is_row) {
                    total_main += cw;
                    if (ch > max_cross) max_cross = ch;
                } else {
                    total_main += ch;
                    if (cw > max_cross) max_cross = cw;
                }
                child_count++;
            }
            
            
            if (child_count > 1) {
                total_main += widget->layout.gap * (child_count - 1);
            }
            
            
            float pad_h = widget->layout.padding[0] + widget->layout.padding[2];
            float pad_w = widget->layout.padding[1] + widget->layout.padding[3];
            
            if (is_row) {
                *out_w = total_main + pad_w;
                *out_h = max_cross + pad_h;
            } else {
                *out_w = max_cross + pad_w;
                *out_h = total_main + pad_h;
            }
            break;
        }
        default:                      *out_w = 0;   *out_h = 0;  break;
    }
}

void LayoutEngine::mark_dirty(Widget* widget) {
    while (widget) {
        widget->mark_dirty();
        widget = widget->parent;
    }
}

void LayoutEngine::solve(Widget* root, float avail_w, float avail_h) {
    if (!root) return;
    generation_++;

    root->layout.computed_x = 0;
    root->layout.computed_y = 0;
    root->layout.computed_w = (root->layout.width >= 0) ? root->layout.width : avail_w;
    root->layout.computed_h = (root->layout.height >= 0) ? root->layout.height : avail_h;
    root->layout.computed_w = clamp_width(root->layout, root->layout.computed_w);
    root->layout.computed_h = clamp_height(root->layout, root->layout.computed_h);
    root->layout.generation = generation_;

    solve_flex(root, root->layout.computed_w, root->layout.computed_h);
}

void LayoutEngine::solve_flex(Widget* container, float avail_w, float avail_h) {
    const LayoutNode& ln = container->layout;
    bool is_row = (ln.direction == FlexDirection::Row);

    float pad_top = ln.padding[0], pad_right = ln.padding[1];
    float pad_bottom = ln.padding[2], pad_left = ln.padding[3];
    float inner_w = avail_w - pad_left - pad_right;
    float inner_h = avail_h - pad_top - pad_bottom;

    struct ChildInfo {
        Widget* widget;
        float base_main, base_cross;
        float flex_grow, flex_shrink;
        float margin_before, margin_after;
        float margin_cross_before, margin_cross_after;
        float final_main, final_cross;
    };

    static constexpr int MAX_CHILDREN = 256;
    ChildInfo children[MAX_CHILDREN];
    uint32_t child_count = 0;

    for (Widget* c = container->first_child; c && child_count < MAX_CHILDREN; c = c->next_sibling) {
        if (!c->visible()) continue;

        ChildInfo& ci = children[child_count];
        ci.widget = c;
        ci.flex_grow = c->layout.flex_grow;
        ci.flex_shrink = c->layout.flex_shrink;

        float intrinsic_w, intrinsic_h;
        measure_intrinsic(c, &intrinsic_w, &intrinsic_h);

        float child_w = (c->layout.width >= 0) ? c->layout.width :
                         (c->layout.flex_basis >= 0) ? c->layout.flex_basis : intrinsic_w;
        float child_h = (c->layout.height >= 0) ? c->layout.height : intrinsic_h;
        child_w = clamp_width(c->layout, child_w);
        child_h = clamp_height(c->layout, child_h);

        if (is_row) {
            ci.base_main = child_w; ci.base_cross = child_h;
            ci.margin_before = c->layout.margin[3]; ci.margin_after = c->layout.margin[1];
            ci.margin_cross_before = c->layout.margin[0]; ci.margin_cross_after = c->layout.margin[2];
        } else {
            ci.base_main = child_h; ci.base_cross = child_w;
            ci.margin_before = c->layout.margin[0]; ci.margin_after = c->layout.margin[2];
            ci.margin_cross_before = c->layout.margin[3]; ci.margin_cross_after = c->layout.margin[1];
        }

        ci.final_main = ci.base_main;
        ci.final_cross = ci.base_cross;
        child_count++;
    }

    if (child_count == 0) return;

    
    float max_cross = 0;
    for (uint32_t i = 0; i < child_count; i++) {
        float child_cross = children[i].base_cross + 
                           children[i].margin_cross_before + 
                           children[i].margin_cross_after;
        if (child_cross > max_cross) max_cross = child_cross;
    }

    float main_avail = is_row ? inner_w : inner_h;
    float cross_avail = is_row ? inner_h : inner_w;
    
    
    if (cross_avail < max_cross) {
        cross_avail = max_cross;
        
        if (is_row) {
            container->layout.computed_h = max_cross + pad_top + pad_bottom;
        } else {
            container->layout.computed_w = max_cross + pad_left + pad_right;
        }
    }

    float total_main = 0, total_grow = 0, total_shrink = 0;
    for (uint32_t i = 0; i < child_count; i++) {
        total_main += children[i].base_main + children[i].margin_before + children[i].margin_after;
        total_grow += children[i].flex_grow;
        total_shrink += children[i].flex_shrink;
        if (i > 0) total_main += ln.gap;
    }

    float free_space = main_avail - total_main;

    if (free_space > 0 && total_grow > 0) {
        for (uint32_t i = 0; i < child_count; i++) {
            children[i].final_main = children[i].base_main +
                                     (children[i].flex_grow / total_grow) * free_space;
        }
    } else if (free_space < 0 && total_shrink > 0) {
        float shrink_amount = -free_space;
        for (uint32_t i = 0; i < child_count; i++) {
            float shrink = (children[i].flex_shrink / total_shrink) * shrink_amount;
            children[i].final_main = std::max(0.0f, children[i].base_main - shrink);
        }
    }

    for (uint32_t i = 0; i < child_count; i++) {
        if (is_row) {
            children[i].final_main = clamp_width(children[i].widget->layout, children[i].final_main);
        } else {
            children[i].final_main = clamp_height(children[i].widget->layout, children[i].final_main);
        }
    }

    float actual_total = 0;
    for (uint32_t i = 0; i < child_count; i++) {
        actual_total += children[i].final_main + children[i].margin_before + children[i].margin_after;
        if (i > 0) actual_total += ln.gap;
    }
    float remaining = main_avail - actual_total;

    float main_offset = 0, justify_gap = 0;
    switch (ln.justify) {
        case JustifyContent::Start: main_offset = 0; break;
        case JustifyContent::End: main_offset = remaining; break;
        case JustifyContent::Center: main_offset = remaining * 0.5f; break;
        case JustifyContent::SpaceBetween:
            if (child_count > 1) justify_gap = remaining / (float)(child_count - 1);
            break;
        case JustifyContent::SpaceAround:
            if (child_count > 0) {
                float space = remaining / (float)child_count;
                main_offset = space * 0.5f; justify_gap = space;
            }
            break;
        case JustifyContent::SpaceEvenly:
            if (child_count > 0) {
                float space = remaining / (float)(child_count + 1);
                main_offset = space; justify_gap = space;
            }
            break;
    }

    float cursor = main_offset;
    for (uint32_t i = 0; i < child_count; i++) {
        ChildInfo& ci = children[i];
        Widget* c = ci.widget;
        cursor += ci.margin_before;

        AlignItems align = ln.align;
        if (ci.widget->layout.self_align != AlignSelf::Auto) {
            align = (AlignItems)((int)ci.widget->layout.self_align - 1);
        }

        float cross_size = ci.final_cross;
        float cross_pos = ci.margin_cross_before;

        switch (align) {
            case AlignItems::Start: cross_pos = ci.margin_cross_before; break;
            case AlignItems::End: cross_pos = cross_avail - cross_size - ci.margin_cross_after; break;
            case AlignItems::Center: cross_pos = (cross_avail - cross_size) * 0.5f; break;
            case AlignItems::Stretch:
                
                if (cross_avail > ci.final_cross) {
                    cross_size = cross_avail - ci.margin_cross_before - ci.margin_cross_after;
                }
                cross_pos = ci.margin_cross_before;
                break;
        }

        if (is_row) cross_size = clamp_height(c->layout, cross_size);
        else cross_size = clamp_width(c->layout, cross_size);

        if (is_row) {
            c->layout.computed_x = container->layout.computed_x + pad_left + cursor;
            c->layout.computed_y = container->layout.computed_y + pad_top + cross_pos;
            c->layout.computed_w = ci.final_main;
            c->layout.computed_h = cross_size;
        } else {
            c->layout.computed_x = container->layout.computed_x + pad_left + cross_pos;
            c->layout.computed_y = container->layout.computed_y + pad_top + cursor;
            c->layout.computed_w = cross_size;
            c->layout.computed_h = ci.final_main;
        }

        c->layout.generation = generation_;
        cursor += ci.final_main + ci.margin_after + ln.gap + justify_gap;

        if (c->first_child) {
            solve_flex(c, c->layout.computed_w, c->layout.computed_h);
        }
    }
}

} 
