#pragma once

#include "types.hpp"
#include "widget.hpp"
#include "text.hpp"

namespace plexy::ui {



struct FontStyle {
    uint16_t size = 14;
    float weight = 400;
    float letter_spacing = 0;
    float line_height = 1.2f;
};



struct AnimCurve {
    float duration = 0.2f;
    float (*easing)(float t) = nullptr;
};



struct ElevationLevel {
    Shadow shadow1;
    Shadow shadow2;
};



struct Theme {
    Material surface_primary;
    Material surface_secondary;
    Material surface_tertiary;
    Material surface_glass;
    Material surface_vibrancy;

    Color content_primary;
    Color content_secondary;
    Color content_tertiary;
    Color content_disabled;
    Color content_on_accent;

    Color accent;
    Color accent_hover;
    Color accent_pressed;

    Material control_background;
    Material control_hover;
    Material control_pressed;
    Material control_disabled;
    Material control_selected;

    Material text_input_background;
    Material text_input_focused;
    Material checkbox_background;
    Material checkbox_checked;
    Material slider_track;
    Material slider_fill;
    Material slider_thumb;
    Material progress_track;
    Material progress_fill;
    Material separator;

    ElevationLevel elevation[4];

    float radius_small;
    float radius_medium;
    float radius_large;
    float radius_full;

    float spacing_xs;
    float spacing_sm;
    float spacing_md;
    float spacing_lg;
    float spacing_xl;

    FontStyle font_body;
    FontStyle font_body_bold;
    FontStyle font_caption;
    FontStyle font_heading;
    FontStyle font_title;

    AnimCurve anim_hover;
    AnimCurve anim_press;
    AnimCurve anim_focus;
    AnimCurve anim_layout;

    float focus_ring_width;
    Color focus_ring_color;
    float border_width;
};

extern const Theme kPlexyLightTheme;
extern const Theme kPlexyDarkTheme;



typedef void (*PaintFn)(FrameArena* arena, PaintList* list,
                        const Widget* widget, const Theme* theme,
                        float abs_x, float abs_y,
                        GlyphAtlas* atlas, uint32_t* next_run_id);

PaintFn get_paint_fn(WidgetType type);

void paint_label(FrameArena* arena, PaintList* list,
                 const Widget* w, const Theme* theme,
                 float ax, float ay, GlyphAtlas* atlas, uint32_t* next_run_id);

void paint_button(FrameArena* arena, PaintList* list,
                  const Widget* w, const Theme* theme,
                  float ax, float ay, GlyphAtlas* atlas, uint32_t* next_run_id);

void paint_text_input(FrameArena* arena, PaintList* list,
                      const Widget* w, const Theme* theme,
                      float ax, float ay, GlyphAtlas* atlas, uint32_t* next_run_id);

void paint_checkbox(FrameArena* arena, PaintList* list,
                    const Widget* w, const Theme* theme,
                    float ax, float ay, GlyphAtlas* atlas, uint32_t* next_run_id);

void paint_slider(FrameArena* arena, PaintList* list,
                  const Widget* w, const Theme* theme,
                  float ax, float ay, GlyphAtlas* atlas, uint32_t* next_run_id);

void paint_progress(FrameArena* arena, PaintList* list,
                    const Widget* w, const Theme* theme,
                    float ax, float ay, GlyphAtlas* atlas, uint32_t* next_run_id);

void paint_panel(FrameArena* arena, PaintList* list,
                 const Widget* w, const Theme* theme,
                 float ax, float ay, GlyphAtlas* atlas, uint32_t* next_run_id);

void paint_separator(FrameArena* arena, PaintList* list,
                     const Widget* w, const Theme* theme,
                     float ax, float ay, GlyphAtlas* atlas, uint32_t* next_run_id);

void paint_switch(FrameArena* arena, PaintList* list,
                  const Widget* w, const Theme* theme,
                  float ax, float ay, GlyphAtlas* atlas, uint32_t* next_run_id);

void paint_scroll_view(FrameArena* arena, PaintList* list,
                       const Widget* w, const Theme* theme,
                       float ax, float ay, GlyphAtlas* atlas, uint32_t* next_run_id);

void paint_list_view(FrameArena* arena, PaintList* list,
                     const Widget* w, const Theme* theme,
                     float ax, float ay, GlyphAtlas* atlas, uint32_t* next_run_id);

void paint_icon_view(FrameArena* arena, PaintList* list,
                     const Widget* w, const Theme* theme,
                     float ax, float ay, GlyphAtlas* atlas, uint32_t* next_run_id);

void paint_toolbar(FrameArena* arena, PaintList* list,
                   const Widget* w, const Theme* theme,
                   float ax, float ay, GlyphAtlas* atlas, uint32_t* next_run_id);

void paint_status_bar(FrameArena* arena, PaintList* list,
                      const Widget* w, const Theme* theme,
                      float ax, float ay, GlyphAtlas* atlas, uint32_t* next_run_id);

void paint_split_view(FrameArena* arena, PaintList* list,
                      const Widget* w, const Theme* theme,
                      float ax, float ay, GlyphAtlas* atlas, uint32_t* next_run_id);

} 
