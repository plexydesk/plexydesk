#include "style.hpp"
#include "canvas_context.hpp"
#include "render.hpp"
#include <cmath>
#include <cstdio>
#include <cstring>

namespace plexy::ui {






extern const char* string_table_get(StringId id);



static PaintFn g_paint_table[(int)WidgetType::COUNT] = {};

static bool g_paint_table_initialized = false;


void paint_label(FrameArena*, PaintList*, const Widget*, const Theme*, float, float, GlyphAtlas*, uint32_t*);
void paint_button(FrameArena*, PaintList*, const Widget*, const Theme*, float, float, GlyphAtlas*, uint32_t*);
void paint_text_input(FrameArena*, PaintList*, const Widget*, const Theme*, float, float, GlyphAtlas*, uint32_t*);
void paint_text_area(FrameArena*, PaintList*, const Widget*, const Theme*, float, float, GlyphAtlas*, uint32_t*);
void paint_checkbox(FrameArena*, PaintList*, const Widget*, const Theme*, float, float, GlyphAtlas*, uint32_t*);
void paint_slider(FrameArena*, PaintList*, const Widget*, const Theme*, float, float, GlyphAtlas*, uint32_t*);
void paint_progress(FrameArena*, PaintList*, const Widget*, const Theme*, float, float, GlyphAtlas*, uint32_t*);
void paint_panel(FrameArena*, PaintList*, const Widget*, const Theme*, float, float, GlyphAtlas*, uint32_t*);
void paint_separator(FrameArena*, PaintList*, const Widget*, const Theme*, float, float, GlyphAtlas*, uint32_t*);
void paint_switch(FrameArena*, PaintList*, const Widget*, const Theme*, float, float, GlyphAtlas*, uint32_t*);
void paint_scroll_view(FrameArena*, PaintList*, const Widget*, const Theme*, float, float, GlyphAtlas*, uint32_t*);
void paint_list_view(FrameArena*, PaintList*, const Widget*, const Theme*, float, float, GlyphAtlas*, uint32_t*);
void paint_icon_view(FrameArena*, PaintList*, const Widget*, const Theme*, float, float, GlyphAtlas*, uint32_t*);
void paint_toolbar(FrameArena*, PaintList*, const Widget*, const Theme*, float, float, GlyphAtlas*, uint32_t*);
void paint_status_bar(FrameArena*, PaintList*, const Widget*, const Theme*, float, float, GlyphAtlas*, uint32_t*);
void paint_split_view(FrameArena*, PaintList*, const Widget*, const Theme*, float, float, GlyphAtlas*, uint32_t*);

static void init_paint_table() {
    if (g_paint_table_initialized) return;
    g_paint_table[(int)WidgetType::Label] = paint_label;
    g_paint_table[(int)WidgetType::Button] = paint_button;
    g_paint_table[(int)WidgetType::TextInput] = paint_text_input;
    g_paint_table[(int)WidgetType::TextArea] = paint_text_area;
    g_paint_table[(int)WidgetType::Checkbox] = paint_checkbox;
    g_paint_table[(int)WidgetType::Slider] = paint_slider;
    g_paint_table[(int)WidgetType::ProgressBar] = paint_progress;
    g_paint_table[(int)WidgetType::Panel] = paint_panel;
    g_paint_table[(int)WidgetType::Separator] = paint_separator;
    g_paint_table[(int)WidgetType::Switch] = paint_switch;
    g_paint_table[(int)WidgetType::ScrollView] = paint_scroll_view;
    g_paint_table[(int)WidgetType::List] = paint_list_view;
    g_paint_table[(int)WidgetType::IconView] = paint_icon_view;
    g_paint_table[(int)WidgetType::Toolbar] = paint_toolbar;
    g_paint_table[(int)WidgetType::StatusBar] = paint_status_bar;
    g_paint_table[(int)WidgetType::SplitView] = paint_split_view;
    g_paint_table_initialized = true;
}

PaintFn get_paint_fn(WidgetType type) {
    init_paint_table();
    int idx = (int)type;
    if (idx < 0 || idx >= (int)WidgetType::COUNT) return nullptr;
    return g_paint_table[idx];
}



static float widget_radius(const Widget* w, const Theme* theme) {
    if (w->corner_radius >= 0) return w->corner_radius;
    switch (w->type) {
        case WidgetType::Button: return theme->radius_medium;
        case WidgetType::TextInput: return theme->radius_medium;
        case WidgetType::TextArea: return theme->radius_medium;
        case WidgetType::Panel: return theme->radius_large;
        case WidgetType::Checkbox: return theme->radius_small;
        case WidgetType::Switch: return theme->radius_full;
        default: return theme->radius_medium;
    }
}



static Material widget_material(const Widget* w, const Theme* theme) {
    if (w->has_material_override) return w->material_override;

    if (!w->enabled()) return theme->control_disabled;
    if (w->pressed()) return theme->control_pressed;
    if (w->hovered()) return theme->control_hover;

    return theme->control_background;
}

static uint16_t resolve_font_size(const Widget* w, const Theme* theme, uint16_t theme_size) {
    float scale = get_scale_factor_global();
    float base = (w->font_size > 0.0f) ? w->font_size : (float)theme_size;
    if (base < 1.0f) base = 1.0f;
    return (uint16_t)(base * scale);
}

static Color resolve_text_color(const Widget* w, const Theme* theme) {
    if (w->has_text_color_override) return w->text_color;
    return w->enabled() ? theme->content_primary : theme->content_disabled;
}

static bool is_sidebar_panel(const Widget* w) {
    return w && w->type == WidgetType::Panel && w->layout.width >= 180.0f;
}

static const char* sidebar_icon_for_label(const char* label) {
    if (!label || !label[0]) return nullptr;
    if (!strcmp(label, "Home")) return "folder";
    if (!strcmp(label, "Computer")) return "file-manager";
    if (!strcmp(label, "Text Edit")) return "text-editor";
    if (!strcmp(label, "Recents")) return "preferences-system-time";
    if (!strcmp(label, "Shared")) return "folder";
    if (!strcmp(label, "Desktop")) return "folder";
    if (!strcmp(label, "Documents")) return "folder-documents";
    if (!strcmp(label, "Downloads")) return "folder-download";
    if (!strcmp(label, "Pictures")) return "folder-pictures";
    if (!strcmp(label, "Music")) return "folder-music";
    if (!strcmp(label, "Videos")) return "folder-videos";
    if (!strcmp(label, "iCloud Drive")) return "folder";
    if (!strcmp(label, "Macintosh HD")) return "folder";
    return nullptr;
}

static void sidebar_parse_text(const char* raw,
                               char* out_label,
                               size_t out_label_cap,
                               char* out_icon,
                               size_t out_icon_cap) {
    const char* p;
    const char* end;

    if (!out_label || out_label_cap == 0) return;
    out_label[0] = '\0';
    if (out_icon && out_icon_cap > 0) out_icon[0] = '\0';
    if (!raw || !raw[0]) return;

    
    if (raw[0] == '[') {
        p = raw + 1;
        end = strchr(p, ']');
        if (end) {
            size_t icon_len = (size_t)(end - p);
            if (icon_len > 0 && out_icon && out_icon_cap > 0) {
                if (icon_len > 5 && strncmp(p, "icon=", 5) == 0) {
                    p += 5;
                    icon_len -= 5;
                }
                if (icon_len >= out_icon_cap) icon_len = out_icon_cap - 1;
                memcpy(out_icon, p, icon_len);
                out_icon[icon_len] = '\0';
            }
            snprintf(out_label, out_label_cap, "%s", end + 1);
            return;
        }
    }

    snprintf(out_label, out_label_cap, "%s", raw);
}

static void truncate_middle_to_width(const char* input, char* out, size_t out_cap,
                                     float max_w, GlyphAtlas* atlas, uint16_t font_size) {
    const char* ell = "...";
    char work[1024];
    float tw, th;

    if (!out || out_cap == 0) return;
    out[0] = '\0';
    if (!input || !input[0]) return;

    snprintf(out, out_cap, "%s", input);
    atlas->measure(out, font_size, &tw, &th);
    if (tw <= max_w) return;

    size_t n = strlen(input);
    if (n <= 4) {
        snprintf(out, out_cap, "%s", ell);
        return;
    }

    
    size_t lo = 2, hi = n - 2;
    size_t best_total = 2; 

    while (lo <= hi) {
        size_t mid = (lo + hi) / 2;
        size_t left = mid / 2;
        size_t right = mid - left;
        if (left < 1) left = 1;
        if (right < 1) right = 1;

        snprintf(work, sizeof(work), "%.*s%s%s",
                 (int)left, input, ell, input + (n - right));
        atlas->measure(work, font_size, &tw, &th);
        if (tw <= max_w) {
            best_total = mid;
            lo = mid + 1;
        } else {
            if (mid == 0) break;
            hi = mid - 1;
        }
    }

    size_t left = best_total / 2;
    size_t right = best_total - left;
    if (left < 1) left = 1;
    if (right < 1) right = 1;
    snprintf(out, out_cap, "%.*s%s%s",
             (int)left, input, ell, input + (n - right));
}



static void emit_elevation(PaintList* list, const Widget* w, const Theme* theme,
                           float x, float y, float width, float height, float radius) {
    int level = (int)w->elevation;
    if (level <= 0 || level > 3) return;

    const ElevationLevel& el = theme->elevation[level];

    if (el.shadow1.color.a > 0) {
        list->push_shadow(x, y, width, height, radius, el.shadow1);
    }
    if (el.shadow2.color.a > 0) {
        list->push_shadow(x, y, width, height, radius, el.shadow2);
    }
}



void paint_label(FrameArena* arena, PaintList* list,
                 const Widget* w, const Theme* theme,
                 float ax, float ay, GlyphAtlas* atlas, uint32_t* next_run_id) {
    const char* text = string_table_get(w->text);
    if (!text || !text[0]) {
        return;
    }

    bool in_toolbar = (w->parent && w->parent->type == WidgetType::Toolbar) ||
                      (w->parent && w->parent->parent &&
                       w->parent->parent->type == WidgetType::Toolbar);
    bool sidebar_section = is_sidebar_panel(w->parent) && (w->font_size > 0.0f && w->font_size <= 11.5f);
    float bw = w->layout.computed_w;
    float bh = w->layout.computed_h;
    bool breadcrumb_capsule = (!in_toolbar &&
                               w->corner_radius >= 0.0f &&
                               bw > 180.0f &&
                               bh >= 30.0f &&
                               bh <= 52.0f);
    uint16_t font_size = resolve_font_size(w, theme, sidebar_section ? theme->font_caption.size : theme->font_body.size);
    Color color = sidebar_section ? theme->content_tertiary : resolve_text_color(w, theme);
    float text_x = ax;
    float baseline_y = ay + font_size;
    const char* draw_text = text;
    char clipped[512];

    if ((in_toolbar && bw > 140.0f && bh <= 42.0f) || breadcrumb_capsule) {
        Material capsule = theme->surface_primary;
        capsule.inner_highlight = 0.12f;
        capsule.inner_shadow = 0.03f;
        capsule.border_color = Color::rgba(0.80f, 0.86f, 0.93f, 0.76f);
        capsule.border_width = 0.8f;
        float cap_r = (w->corner_radius >= 0.0f) ? w->corner_radius : 10.0f;
        list->push_shadow(ax, ay + 1.0f, bw, bh, cap_r,
                          Shadow::make(0, 4.0f, 14.0f, Color::rgba(0.58f, 0.68f, 0.80f, 0.12f), 0));
        list->push_rounded_rect(ax, ay, bw, bh, cap_r, capsule);
        list->push_rounded_rect(ax + 1.0f, ay + 1.0f, bw - 2.0f, 1.0f, cap_r,
                                Material::solid(Color::rgba(1.0f, 1.0f, 1.0f, 0.42f)));

        truncate_middle_to_width(text, clipped, sizeof(clipped), bw - 36.0f, atlas, font_size);
        draw_text = clipped;
        color = theme->content_secondary;

        float tw, th;
        atlas->measure(draw_text, font_size, &tw, &th);
        text_x = ax + 16.0f;
        baseline_y = ay + bh * 0.5f + th * 0.35f;

        if (in_toolbar && strchr(text, '/')) {
            float crumb_dot = ax + bw - 16.0f;
            Material dot = Material::solid(theme->content_tertiary.with_alpha(0.85f));
            list->push_rounded_rect(crumb_dot - 2.0f, ay + bh * 0.5f - 2.0f, 4.0f, 4.0f, 2.0f, dot);
        }
    } else if (sidebar_section) {
        color = theme->content_secondary.with_alpha(0.72f);
        baseline_y = ay + bh * 0.5f + font_size * 0.33f;
        text_x = ax + 6.0f;
    }

    uint32_t run_id = (*next_run_id)++;
    GlyphRun* run = atlas->layout_text(arena, draw_text, font_size, text_x, baseline_y, run_id);
    
    if (run && run->glyph_count > 0) {
        store_glyph_run_global(run);
        list->push_text(text_x, baseline_y, run_id, color, (float)font_size);
    }
}



void paint_button(FrameArena* arena, PaintList* list,
                  const Widget* w, const Theme* theme,
                  float ax, float ay, GlyphAtlas* atlas, uint32_t* next_run_id) {
    float bw = w->layout.computed_w;
    float bh = w->layout.computed_h;
    float radius = widget_radius(w, theme);
    bool in_toolbar = (w->parent && w->parent->type == WidgetType::Toolbar) ||
                      (w->parent && w->parent->parent &&
                       w->parent->parent->type == WidgetType::Toolbar);
    bool in_sidebar = (w->parent && is_sidebar_panel(w->parent) &&
                       w->layout.computed_h <= 52.0f);

    
    emit_elevation(list, w, theme, ax, ay, bw, bh, radius);

    
    Material mat;
    if (!w->enabled()) {
        mat = theme->control_disabled;
    } else if (in_sidebar) {
        
        mat = Material::solid(Color::none());
    } else if (in_toolbar) {
        if (w->pressed()) {
            mat = Material::solid(Color::rgba(0.87f, 0.91f, 0.97f, 0.96f));
        } else if (w->hovered()) {
            mat = Material::solid(Color::rgba(0.95f, 0.97f, 1.0f, 0.98f));
        } else {
            mat = Material::solid(Color::rgba(0.97f, 0.98f, 1.0f, 0.92f));
        }
        mat.border_color = Color::rgba(0.79f, 0.85f, 0.92f, 0.84f);
        mat.border_width = 0.8f;
        mat.inner_highlight = 0.12f;
        mat.inner_shadow = 0.02f;
    } else if (w->has_material_override) {
        mat = w->material_override;
    } else {
        mat = widget_material(w, theme);
    }
    if (in_toolbar && mat.border_width <= 0.0f) {
        mat.border_color = Color::rgba(0.72f, 0.74f, 0.79f, 0.85f);
        mat.border_width = 0.8f;
    }
    list->push_rounded_rect(ax, ay, bw, bh, radius, mat);

    
    if (w->focused()) {
        Material focus_mat = {};
        focus_mat.type = Material::Type::Solid;
        focus_mat.color = Color::none();
        focus_mat.border_color = theme->focus_ring_color;
        focus_mat.border_width = theme->focus_ring_width;
        list->push_rounded_rect(ax - 2, ay - 2, bw + 4, bh + 4, radius + 2, focus_mat);
    }

    
    const char* text = string_table_get(w->text);
    if (text && text[0]) {
        uint16_t default_size = (in_toolbar || in_sidebar) ? theme->font_body.size
                                                            : theme->font_body_bold.size;
        uint16_t font_size = resolve_font_size(w, theme, default_size);
        const char* draw_text = text;
        char clipped[256];
        char parsed_label[256];
        char parsed_icon[96];
        float left_pad = 16.0f;

        sidebar_parse_text(text, parsed_label, sizeof(parsed_label), parsed_icon, sizeof(parsed_icon));
        if (parsed_label[0] || parsed_icon[0]) {
            draw_text = parsed_label;
        }

        if (in_sidebar || parsed_icon[0]) {
            const char* icon_name = parsed_icon[0] ? parsed_icon : sidebar_icon_for_label(draw_text);
            bool active_item = false;
            if (w->has_material_override && w->material_override.type == Material::Type::Glass) {
                active_item = w->material_override.opacity >= 0.93f;
            }
            if (!active_item && (w->hovered() || w->pressed())) {
                active_item = true;
            }

            if (icon_name) {
                uint32_t icon_tex = canvas_get_theme_icon_texture(icon_name);
                if (in_sidebar && !icon_tex && strcmp(icon_name, "folder")) {
                    icon_tex = canvas_get_theme_icon_texture("folder");
                }
                if (icon_tex) {
                    bool icon_centered = in_toolbar || (!in_sidebar && parsed_icon[0]);
                    float icon_size = icon_centered ? 16.0f : 18.0f;
                    float ix = icon_centered ? (ax + (bw - icon_size) * 0.5f) : (ax + 13.0f);
                    float iy = ay + (bh - icon_size) * 0.5f;
                    Color icon_tint = icon_centered
                        ? Color::rgba(0.30f, 0.38f, 0.48f, 0.98f)
                        : Color::rgba(0.10f, 0.50f, 0.96f, 1.0f);
                    list->push_icon(ix, iy, icon_size, icon_size, icon_tex, icon_tint);
                    left_pad = icon_centered ? bw : 40.0f;
                }
            }
        }
        if (in_sidebar) {
            truncate_middle_to_width(draw_text, clipped, sizeof(clipped), bw - left_pad - 10.0f, atlas, font_size);
            draw_text = clipped;
        }
        
        float tw, th;
        atlas->measure(draw_text, font_size, &tw, &th);
        float tx = in_sidebar ? (ax + left_pad) : (ax + (bw - tw) * 0.5f);
        float ty = ay + (bh - th) * 0.5f + th * 0.8f;

        Color text_color = resolve_text_color(w, theme);
        if (in_toolbar) {
            text_color = theme->content_secondary.with_alpha(0.95f);
        } else if (in_sidebar) {
            text_color = text_color.with_alpha(w->pressed() ? 1.0f : 0.96f);
        }

        uint32_t run_id = (*next_run_id)++;
        GlyphRun* run = atlas->layout_text(arena, draw_text, font_size, tx, ty, run_id);
        if (run && run->glyph_count > 0) {
            store_glyph_run_global(run);
            list->push_text(tx, ty, run_id, text_color, (float)font_size);
        }
    }
}



void paint_text_input(FrameArena* arena, PaintList* list,
                      const Widget* w, const Theme* theme,
                      float ax, float ay, GlyphAtlas* atlas, uint32_t* next_run_id) {
    float bw = w->layout.computed_w;
    float bh = w->layout.computed_h;
    float radius = widget_radius(w, theme);
    bool in_toolbar = (w->parent && w->parent->type == WidgetType::Toolbar);

    Material mat = w->focused() ? theme->text_input_focused : theme->text_input_background;
    if (!w->enabled()) mat = theme->control_disabled;
    if (in_toolbar) {
        mat.inner_highlight = 0.16f;
        mat.inner_shadow = 0.03f;
        mat.border_color = w->focused()
            ? Color::rgba(0.46f, 0.72f, 0.98f, 0.76f)
            : Color::rgba(0.79f, 0.85f, 0.93f, 0.78f);
        mat.border_width = 0.8f;
        list->push_shadow(ax, ay + 1.0f, bw, bh, radius,
                          Shadow::make(0, 4.0f, 12.0f, Color::rgba(0.61f, 0.71f, 0.82f, 0.10f), 0));
    }

    list->push_rounded_rect(ax, ay, bw, bh, radius, mat);

    
    if (w->focused()) {
        Material focus_mat = {};
        focus_mat.type = Material::Type::Solid;
        focus_mat.color = Color::none();
        focus_mat.border_color = theme->focus_ring_color;
        focus_mat.border_width = theme->focus_ring_width;
        list->push_rounded_rect(ax - 2, ay - 2, bw + 4, bh + 4, radius + 2, focus_mat);
    }

    
    const char* text = string_table_get(w->text);
    float padding_left = theme->spacing_sm;
    uint16_t font_size = resolve_font_size(w, theme, theme->font_body.size);
    
    if (text && text[0]) {
        Color text_color = resolve_text_color(w, theme);
        float ty = ay + bh * 0.5f + font_size * 0.35f;

        uint32_t run_id = (*next_run_id)++;
        GlyphRun* run = atlas->layout_text(arena, text, font_size, ax + padding_left, ty, run_id);
        if (run && run->glyph_count > 0) {
            store_glyph_run_global(run);
            list->push_text(ax + padding_left, ty, run_id, text_color, (float)font_size);
        }
    }

    
    if (w->focused()) {
        float cursor_x;
        if (text && text[0]) {
            float tw, th;
            atlas->measure(text, font_size, &tw, &th);
            cursor_x = ax + padding_left + tw;
        } else {
            cursor_x = ax + padding_left;
        }
        Material cursor_mat = Material::solid(theme->content_primary);
        list->push_rounded_rect(cursor_x, ay + 4, 1.5f, bh - 8, 1.0f, cursor_mat);
    }
}

void paint_text_area(FrameArena* arena, PaintList* list,
                     const Widget* w, const Theme* theme,
                     float ax, float ay, GlyphAtlas* atlas, uint32_t* next_run_id) {
    float bw = w->layout.computed_w;
    float bh = w->layout.computed_h;
    float radius = widget_radius(w, theme);

    Material mat = w->focused() ? theme->text_input_focused : theme->text_input_background;
    if (!w->enabled()) mat = theme->control_disabled;

    list->push_rounded_rect(ax, ay, bw, bh, radius, mat);

    
    if (w->focused()) {
        Material focus_mat = {};
        focus_mat.type = Material::Type::Solid;
        focus_mat.color = Color::none();
        focus_mat.border_color = theme->focus_ring_color;
        focus_mat.border_width = theme->focus_ring_width;
        list->push_rounded_rect(ax - 2, ay - 2, bw + 4, bh + 4, radius + 2, focus_mat);
    }

    
    const char* text = string_table_get(w->text);
    float padding = theme->spacing_sm;
    uint16_t font_size = resolve_font_size(w, theme, theme->font_body.size);
    float line_height = font_size * 1.4f;
    
    if (text && text[0]) {
        Color text_color = resolve_text_color(w, theme);
        float ty = ay + padding + line_height * 0.7f;
        float tx = ax + padding;

        
        size_t text_len = strlen(text);
        const char* start_pos = text;
        if (text_len > 10000) {
            start_pos = text + (text_len - 10000);
            
            while (*start_pos && *start_pos != '\n') start_pos++;
            if (*start_pos == '\n') start_pos++;
        }

        
        const char* line_start = start_pos;
        const char* p = start_pos;
        int line_num = 0;

        while (*p && ty < (ay + bh - padding)) {
            if (*p == '\n' || *(p+1) == '\0') {
                
                size_t len = (*p == '\n') ? (p - line_start) : (p - line_start + 1);
                if (len > 0 && len < 1024) {
                    char line_buf[1024];
                    memcpy(line_buf, line_start, len);
                    line_buf[len] = '\0';
                    
                    uint32_t run_id = (*next_run_id)++;
                    GlyphRun* run = atlas->layout_text(arena, line_buf, font_size, tx, ty, run_id);
                    if (run && run->glyph_count > 0) {
                        store_glyph_run_global(run);
                        list->push_text(tx, ty, run_id, text_color, (float)font_size);
                    }
                }
                
                ty += line_height;
                line_num++;
                
                if (*p == '\n') {
                    line_start = p + 1;
                }
            }
            if (*p) p++;
        }
    }
}




void paint_checkbox(FrameArena* arena, PaintList* list,
                    const Widget* w, const Theme* theme,
                    float ax, float ay, GlyphAtlas* atlas, uint32_t* next_run_id) {
    float box_size = 18.0f;
    float radius = widget_radius(w, theme);
    float by = ay + (w->layout.computed_h - box_size) * 0.5f;

    
    Material mat = w->checked() ? theme->checkbox_checked : theme->checkbox_background;
    if (!w->enabled()) mat = theme->control_disabled;
    list->push_rounded_rect(ax, by, box_size, box_size, radius, mat);

    
    if (w->checked()) {
        Color check_color = theme->content_on_accent;
        Material check_mat = Material::solid(check_color);
        
        float cx = ax + box_size * 0.3f;
        float cy = by + box_size * 0.3f;
        list->push_rounded_rect(cx, cy, box_size * 0.4f, box_size * 0.4f, 2.0f, check_mat);
    }

    
    const char* text = string_table_get(w->text);
    if (text && text[0]) {
        uint16_t font_size = resolve_font_size(w, theme, theme->font_body.size);
        
        float tx = ax + box_size + theme->spacing_sm;
        float ty = ay + w->layout.computed_h * 0.5f + font_size * 0.35f;
        Color text_color = resolve_text_color(w, theme);

        uint32_t run_id = (*next_run_id)++;
        GlyphRun* run = atlas->layout_text(arena, text, font_size, tx, ty, run_id);
        if (run && run->glyph_count > 0) {
            store_glyph_run_global(run);
            list->push_text(tx, ty, run_id, text_color, (float)font_size);
        }
    }
}



void paint_slider(FrameArena* arena, PaintList* list,
                  const Widget* w, const Theme* theme,
                  float ax, float ay, GlyphAtlas* atlas, uint32_t* next_run_id) {
    float bw = w->layout.computed_w;
    float bh = w->layout.computed_h;
    float track_h = 4.0f;
    float track_y = ay + (bh - track_h) * 0.5f;
    float radius = track_h * 0.5f;

    
    list->push_rounded_rect(ax, track_y, bw, track_h, radius, theme->slider_track);

    
    float t = (w->value - w->value_min) / (w->value_max - w->value_min);
    if (t < 0) t = 0;
    if (t > 1) t = 1;
    float fill_w = bw * t;
    if (fill_w > 0) {
        list->push_rounded_rect(ax, track_y, fill_w, track_h, radius, theme->slider_fill);
    }

    
    float thumb_size = 20.0f;
    float thumb_x = ax + fill_w - thumb_size * 0.5f;
    float thumb_y = ay + (bh - thumb_size) * 0.5f;

    
    Shadow thumb_shadow = Shadow::make(0, 1, 3, Color::rgba(0, 0, 0, 0.2f), 0);
    list->push_shadow(thumb_x, thumb_y, thumb_size, thumb_size, thumb_size * 0.5f, thumb_shadow);

    list->push_rounded_rect(thumb_x, thumb_y, thumb_size, thumb_size,
                            thumb_size * 0.5f, theme->slider_thumb);
}



void paint_progress(FrameArena* arena, PaintList* list,
                    const Widget* w, const Theme* theme,
                    float ax, float ay, GlyphAtlas* atlas, uint32_t* next_run_id) {
    float bw = w->layout.computed_w;
    float bh = w->layout.computed_h;
    float bar_h = 6.0f;
    float bar_y = ay + (bh - bar_h) * 0.5f;
    float radius = bar_h * 0.5f;

    
    list->push_rounded_rect(ax, bar_y, bw, bar_h, radius, theme->progress_track);

    
    float t = (w->value - w->value_min) / (w->value_max - w->value_min);
    if (t < 0) t = 0;
    if (t > 1) t = 1;
    float fill_w = bw * t;
    if (fill_w > 0) {
        list->push_rounded_rect(ax, bar_y, fill_w, bar_h, radius, theme->progress_fill);
    }
}



void paint_panel(FrameArena* arena, PaintList* list,
                 const Widget* w, const Theme* theme,
                 float ax, float ay, GlyphAtlas* atlas, uint32_t* next_run_id) {
    
    
    if (!w->has_material_override && w->elevation <= 0 && w->corner_radius < 0) {
        return;
    }
    
    float bw = w->layout.computed_w;
    float bh = w->layout.computed_h;
    float radius = widget_radius(w, theme);

    emit_elevation(list, w, theme, ax, ay, bw, bh, radius);

    Material mat = w->has_material_override ? w->material_override : theme->surface_primary;
    list->push_rounded_rect(ax, ay, bw, bh, radius, mat);
}



void paint_separator(FrameArena* arena, PaintList* list,
                     const Widget* w, const Theme* theme,
                     float ax, float ay, GlyphAtlas* atlas, uint32_t* next_run_id) {
    float bw = w->layout.computed_w;
    list->push_rounded_rect(ax, ay, bw, 1.0f, 0, theme->separator);
}



void paint_switch(FrameArena* arena, PaintList* list,
                  const Widget* w, const Theme* theme,
                  float ax, float ay, GlyphAtlas* atlas, uint32_t* next_run_id) {
    float track_w = 44.0f;
    float track_h = 24.0f;
    float track_y = ay + (w->layout.computed_h - track_h) * 0.5f;
    float radius = track_h * 0.5f;

    
    Material track_mat = w->checked() ?
        Material::solid(theme->accent) :
        theme->checkbox_background;
    if (!w->enabled()) track_mat = theme->control_disabled;
    list->push_rounded_rect(ax, track_y, track_w, track_h, radius, track_mat);

    
    float thumb_size = 20.0f;
    float thumb_pad = 2.0f;
    float thumb_x = w->checked() ?
        ax + track_w - thumb_size - thumb_pad :
        ax + thumb_pad;
    float thumb_y = track_y + thumb_pad;

    Shadow thumb_shadow = Shadow::make(0, 1, 2, Color::rgba(0, 0, 0, 0.15f), 0);
    list->push_shadow(thumb_x, thumb_y, thumb_size, thumb_size, thumb_size * 0.5f, thumb_shadow);

    Material thumb_mat = Material::solid(Color::rgba(1, 1, 1, 1));
    list->push_rounded_rect(thumb_x, thumb_y, thumb_size, thumb_size,
                            thumb_size * 0.5f, thumb_mat);

    
    const char* text = string_table_get(w->text);
    if (text && text[0]) {
        uint16_t font_size = resolve_font_size(w, theme, theme->font_body.size);
        
        float tx = ax + track_w + theme->spacing_sm;
        float ty = ay + w->layout.computed_h * 0.5f + font_size * 0.35f;
        Color text_color = resolve_text_color(w, theme);

        uint32_t run_id = (*next_run_id)++;
        GlyphRun* run = atlas->layout_text(arena, text, font_size, tx, ty, run_id);
        if (run && run->glyph_count > 0) {
            store_glyph_run_global(run);
            list->push_text(tx, ty, run_id, text_color, (float)font_size);
        }
    }
}



void paint_scroll_view(FrameArena* arena, PaintList* list,
                       const Widget* w, const Theme* theme,
                       float ax, float ay, GlyphAtlas* atlas, uint32_t* next_run_id) {
    
    
    
    float bw = w->layout.computed_w;
    float bh = w->layout.computed_h;

    
    float scrollbar_w = 6.0f;
    float scrollbar_x = ax + bw - scrollbar_w - 2.0f;

    
    float thumb_h = bh * 0.3f;  
    float thumb_y = ay + w->value * (bh - thumb_h);  

    Material scrollbar_mat = Material::solid(Color::rgba(0, 0, 0, 0.2f));
    list->push_rounded_rect(scrollbar_x, thumb_y, scrollbar_w, thumb_h,
                            scrollbar_w * 0.5f, scrollbar_mat);
}



void paint_list_view(FrameArena* arena, PaintList* list,
                     const Widget* w, const Theme* theme,
                     float ax, float ay, GlyphAtlas* atlas, uint32_t* next_run_id) {
    float bw = w->layout.computed_w;
    float bh = w->layout.computed_h;
    float radius = (w->corner_radius >= 0.0f) ? w->corner_radius : 0.0f;

    Material panel = theme->surface_primary;
    panel.inner_highlight = 0.10f;
    panel.inner_shadow = 0.02f;
    panel.border_color = Color::rgba(0.82f, 0.87f, 0.94f, 0.82f);
    panel.border_width = 0.9f;
    list->push_rounded_rect(ax, ay, bw, bh, radius, panel);

    const char* text = string_table_get(w->text);
    if (!text || !text[0]) return;

    uint16_t font_size = resolve_font_size(w, theme, theme->font_body.size);
    float line_h = font_size * 1.5f;
    float pad = theme->spacing_sm;
    float y = ay + pad + font_size * 0.8f;
    int selected = (int)(w->value + 0.5f);
    int row = 0;

    const char* line_start = text;
    const char* p = text;
    while (1) {
        if (*p == '\n' || *p == '\0') {
            size_t len = (size_t)(p - line_start);
            if (len > 0 && (y + line_h * 0.5f) <= (ay + bh - pad)) {
                if (row == selected) {
                    list->push_rounded_rect(ax + 2.0f, y - font_size,
                                            bw - 4.0f, line_h,
                                            theme->radius_small, theme->control_selected);
                }
                if (len < 1024) {
                    char line_buf[1024];
                    char draw_buf[1024];
                    memcpy(line_buf, line_start, len);
                    line_buf[len] = '\0';
                    truncate_middle_to_width(line_buf, draw_buf, sizeof(draw_buf),
                                             bw - pad * 2.0f - 8.0f, atlas, font_size);

                    uint32_t run_id = (*next_run_id)++;
                    GlyphRun* run = atlas->layout_text(arena, draw_buf, font_size,
                                                       ax + pad, y, run_id);
                    if (run && run->glyph_count > 0) {
                        store_glyph_run_global(run);
                        list->push_text(ax + pad, y, run_id,
                                        resolve_text_color(w, theme), (float)font_size);
                    }
                }
            }
            row++;
            y += line_h;
            if (*p == '\0' || y > (ay + bh - pad)) break;
            line_start = p + 1;
        }
        p++;
    }
}



void paint_icon_view(FrameArena* arena, PaintList* list,
                     const Widget* w, const Theme* theme,
                     float ax, float ay, GlyphAtlas* atlas, uint32_t* next_run_id) {
    float bw = w->layout.computed_w;
    float bh = w->layout.computed_h;

    

    const char* text = string_table_get(w->text);
    if (!text || !text[0]) return;

    float pad = 18.0f;
    float gap = 16.0f;
    float cell_w = 176.0f;
    float cell_h = 168.0f;
    int cols = (int)((bw - pad * 2 + gap) / (cell_w + gap));
    if (cols < 1) cols = 1;

    
    int item_count = 0;
    for (const char* c = text; *c; c++) {
        if (*c == '\n') item_count++;
    }
    if (text[0] && text[strlen(text) - 1] != '\n') item_count++;

    int total_rows = (item_count + cols - 1) / cols;
    float content_h = pad * 2 + total_rows * (cell_h + gap) - gap;
    float max_scroll = content_h - bh;
    if (max_scroll < 0) max_scroll = 0;

    
    Widget* mw = const_cast<Widget*>(w);
    if (mw->scroll_y > max_scroll) mw->scroll_y = max_scroll;
    float scroll_y = mw->scroll_y;

    const bool dark_theme = (theme == &kPlexyDarkTheme);
    uint16_t font_size = resolve_font_size(w, theme, theme->font_body_bold.size);
    int selected = (int)(w->value + 0.5f);
    int idx = 0;

    const char* line_start = text;
    const char* p = text;
    while (1) {
        if (*p == '\n' || *p == '\0') {
            size_t len = (size_t)(p - line_start);
            if (len > 0) {
                int col = idx % cols;
                int row = idx / cols;
                float cx = ax + pad + col * (cell_w + gap);
                float cy = ay + pad + row * (cell_h + gap) - scroll_y;
                
                if (cy + cell_h < ay) { idx++; if (*p == '\0') break; line_start = p + 1; p++; continue; }
                if (cy > ay + bh) break;

                if (idx == selected) {
                    Material tile = theme->control_selected;
                    tile.inner_highlight = 0.0f;
                    tile.inner_shadow = 0.0f;
                    list->push_rounded_rect(cx, cy, cell_w, cell_h, theme->radius_medium, tile);
                }

                char name[256];
                if (len >= sizeof(name)) len = sizeof(name) - 1;
                memcpy(name, line_start, len);
                name[len] = '\0';
                size_t nlen = strlen(name);
                bool is_dir = (nlen > 0 && name[nlen - 1] == '/');
                if (is_dir) name[nlen - 1] = '\0';

                const char* icon_name = "text-x-generic";
                if (is_dir) {
                    icon_name = "folder";
                    if (!strcmp(name, "Documents")) icon_name = "folder-documents";
                    else if (!strcmp(name, "Downloads")) icon_name = "folder-download";
                    else if (!strcmp(name, "Pictures")) icon_name = "folder-pictures";
                    else if (!strcmp(name, "Videos")) icon_name = "folder-videos";
                    else if (!strcmp(name, "Music")) icon_name = "folder-music";
                } else {
                    
                    const char* dot = strrchr(name, '.');
                    if (dot) {
                        dot++; 
                        if (!strcmp(dot,"png") || !strcmp(dot,"jpg") || !strcmp(dot,"jpeg") ||
                            !strcmp(dot,"gif") || !strcmp(dot,"bmp") || !strcmp(dot,"webp") ||
                            !strcmp(dot,"svg") || !strcmp(dot,"ico") || !strcmp(dot,"tiff"))
                            icon_name = "image-x-generic";
                        else if (!strcmp(dot,"mp3") || !strcmp(dot,"flac") || !strcmp(dot,"wav") ||
                                 !strcmp(dot,"ogg") || !strcmp(dot,"aac") || !strcmp(dot,"m4a") ||
                                 !strcmp(dot,"wma") || !strcmp(dot,"opus"))
                            icon_name = "audio-x-generic";
                        else if (!strcmp(dot,"mp4") || !strcmp(dot,"mkv") || !strcmp(dot,"avi") ||
                                 !strcmp(dot,"mov") || !strcmp(dot,"webm") || !strcmp(dot,"wmv") ||
                                 !strcmp(dot,"flv"))
                            icon_name = "video-x-generic";
                        else if (!strcmp(dot,"pdf"))
                            icon_name = "application-pdf";
                        else if (!strcmp(dot,"zip") || !strcmp(dot,"tar") || !strcmp(dot,"gz") ||
                                 !strcmp(dot,"bz2") || !strcmp(dot,"xz") || !strcmp(dot,"7z") ||
                                 !strcmp(dot,"rar") || !strcmp(dot,"zst"))
                            icon_name = "application-zip";
                        else if (!strcmp(dot,"py") || !strcmp(dot,"pyw"))
                            icon_name = "text-x-python";
                        else if (!strcmp(dot,"c") || !strcmp(dot,"h"))
                            icon_name = "text-x-c";
                        else if (!strcmp(dot,"cpp") || !strcmp(dot,"cxx") || !strcmp(dot,"cc") ||
                                 !strcmp(dot,"hpp") || !strcmp(dot,"hxx"))
                            icon_name = "text-x-cpp";
                        else if (!strcmp(dot,"cs"))
                            icon_name = "text-x-csharp";
                        else if (!strcmp(dot,"html") || !strcmp(dot,"htm"))
                            icon_name = "text-html";
                        else if (!strcmp(dot,"sh") || !strcmp(dot,"bash") || !strcmp(dot,"zsh") ||
                                 !strcmp(dot,"fish"))
                            icon_name = "text-x-script";
                        else if (!strcmp(dot,"bin") || !strcmp(dot,"AppImage"))
                            icon_name = "application-x-executable";
                    }
                }
                uint32_t tex = canvas_get_theme_icon_texture(icon_name);
                if (tex != 0) {
                    float icon_size = is_dir ? 100.0f : 88.0f;
                    float ix = cx + (cell_w - icon_size) * 0.5f;
                    float iy = cy + 18.0f;
                    list->push_icon(ix, iy, icon_size, icon_size, tex, Color::rgba(1, 1, 1, 1));
                }

                if (name[0]) {
                    char draw_name[256];
                    float tw, th;
                    truncate_middle_to_width(name, draw_name, sizeof(draw_name),
                                              cell_w - 18.0f, atlas, font_size);
                    atlas->measure(draw_name, font_size, &tw, &th);
                    float tx = cx + (cell_w - tw) * 0.5f;
                    float ty = cy + cell_h - 22.0f;

                    uint32_t run_id = (*next_run_id)++;
                    GlyphRun* run = atlas->layout_text(arena, draw_name, font_size, tx, ty, run_id);
                    if (run && run->glyph_count > 0) {
                        store_glyph_run_global(run);
                        Color label_color = resolve_text_color(w, theme);
                        if (dark_theme) {
                            label_color = Color::rgba(0.97f, 0.98f, 1.0f, 1.0f);
                        }
                        if (idx == selected) {
                            label_color = dark_theme ? Color::rgba(1.0f, 1.0f, 1.0f, 1.0f)
                                                     : theme->content_primary;
                        }
                        if (dark_theme) {
                            
                            list->push_text(tx + 0.6f, ty, run_id, label_color, (float)font_size);
                        }
                        list->push_text(tx, ty, run_id, label_color, (float)font_size);
                    }
                }
                idx++;
            }
            if (*p == '\0') break;
            line_start = p + 1;
        }
        p++;
    }
}




void paint_toolbar(FrameArena* arena, PaintList* list,
                   const Widget* w, const Theme* theme,
                   float ax, float ay, GlyphAtlas* atlas, uint32_t* next_run_id) {
    float bw = w->layout.computed_w;
    float bh = w->layout.computed_h;

    Material mat = w->has_material_override ? w->material_override : theme->surface_secondary;
    mat.border_color = Color::none();
    mat.border_width = 0.0f;
    mat.inner_highlight = 0.14f;
    mat.inner_shadow = 0.025f;
    list->push_rounded_rect(ax, ay, bw, bh, 0.0f, mat);
    list->push_rounded_rect(ax, ay, bw, 1.0f, 0, Material::solid(Color::rgba(1, 1, 1, 0.55f)));
    list->push_rounded_rect(ax, ay + bh - 1.0f, bw, 1.0f, 0,
                            Material::solid(Color::rgba(0.78f, 0.84f, 0.91f, 0.62f)));
}



void paint_status_bar(FrameArena* arena, PaintList* list,
                      const Widget* w, const Theme* theme,
                      float ax, float ay, GlyphAtlas* atlas, uint32_t* next_run_id) {
    float bw = w->layout.computed_w;
    float bh = w->layout.computed_h;

    
    
    if (w->has_material_override) {
        Material mat = w->material_override;
        mat.border_color = Color::rgba(0.80f, 0.86f, 0.93f, 0.46f);
        mat.border_width = 0.5f;
        list->push_rounded_rect(ax, ay, bw, bh, 0.0f, mat);
    }

    
    list->push_rounded_rect(ax, ay, bw, 1.0f, 0, Material::solid(Color::rgba(0.78f, 0.84f, 0.91f, 0.48f)));

    
    const char* text = string_table_get(w->text);
    if (text && text[0]) {
        uint16_t font_size = resolve_font_size(w, theme, theme->font_caption.size);
        Color color = theme->content_secondary.with_alpha(0.90f);

        uint32_t run_id = (*next_run_id)++;
        GlyphRun* run = atlas->layout_text(arena, text, font_size,
                                            ax + 12.0f, ay + bh * 0.5f + font_size * 0.35f, run_id);
        if (run && run->glyph_count > 0) {
            store_glyph_run_global(run);
            list->push_text(ax + 12.0f, ay + bh * 0.5f + font_size * 0.35f,
                           run_id, color, (float)font_size);
        }
    }
}




void paint_split_view(FrameArena* arena, PaintList* list,
                      const Widget* w, const Theme* theme,
                      float ax, float ay, GlyphAtlas* atlas, uint32_t* next_run_id) {
    
    if (w->has_material_override) {
        float bw = w->layout.computed_w;
        float bh = w->layout.computed_h;
        list->push_rounded_rect(ax, ay, bw, bh, 0, w->material_override);
    }

    
    Widget* first = w->first_child;
    if (first) {
        float divider_x = ax + first->layout.computed_x + first->layout.computed_w;
        float bh = w->layout.computed_h;
        list->push_rounded_rect(divider_x, ay + 12.0f, 1.0f, bh - 24.0f, 0,
                                Material::solid(Color::rgba(0.80f, 0.86f, 0.93f, 0.52f)));
    }
}

} 
