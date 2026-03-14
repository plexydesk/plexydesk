#pragma once

#include "widget.hpp"

namespace plexy::ui {

class LayoutEngine {
public:
    void solve(Widget* root, float avail_w, float avail_h);
    void mark_dirty(Widget* widget);
    uint32_t generation() const { return generation_; }

private:
    uint32_t generation_ = 0;
    void solve_flex(Widget* container, float avail_w, float avail_h);
    void measure_intrinsic(Widget* widget, float* out_w, float* out_h);
    uint32_t count_children(const Widget* parent);
    float clamp_width(const LayoutNode& node, float w);
    float clamp_height(const LayoutNode& node, float h);
};

} 
