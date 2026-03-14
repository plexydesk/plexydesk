#include "style.hpp"

namespace plexy::ui {



static float ease_out_cubic(float t) {
    float f = t - 1.0f;
    return f * f * f + 1.0f;
}

static float ease_out_quart(float t) {
    float f = t - 1.0f;
    float f2 = f * f;
    return 1.0f - f2 * f2;
}





const Theme kPlexyLightTheme = {
    
    .surface_primary = [] {
        Material m = Material::solid(Color::rgba(0.985f, 0.99f, 1.0f, 1.0f));
        m.inner_highlight = 0.16f;
        m.inner_shadow = 0.025f;
        m.border_color = Color::rgba(0.81f, 0.87f, 0.94f, 0.78f);
        m.border_width = 0.7f;
        return m;
    }(),
    .surface_secondary = [] {
        Material m = Material::solid(Color::rgba(0.95f, 0.97f, 0.995f, 1.0f));
        m.inner_highlight = 0.12f;
        m.inner_shadow = 0.02f;
        m.border_color = Color::rgba(0.81f, 0.87f, 0.94f, 0.60f);
        m.border_width = 0.6f;
        return m;
    }(),
    .surface_tertiary = [] {
        Material m = Material::solid(Color::rgba(0.95f, 0.97f, 0.995f, 1.0f));
        m.border_color = Color::rgba(0.83f, 0.88f, 0.94f, 0.45f);
        m.border_width = 0.5f;
        return m;
    }(),
    .surface_glass = Material::glass(16.0f, 0.42f,
                                      Color::rgba(0.98f, 0.99f, 1.0f, 0.18f),
                                      1.18f, 0.18f, 0.04f),
    .surface_vibrancy = [] {
        Material m = Material::glass(14.0f, 0.62f,
                                      Color::rgba(0.96f, 0.98f, 1.0f, 0.16f),
                                      1.18f, 0.12f, 0.04f);
        m.type = Material::Type::Vibrancy;
        return m;
    }(),

    
    .content_primary = Color::rgba(0.27f, 0.36f, 0.48f, 0.96f),
    .content_secondary = Color::rgba(0.46f, 0.55f, 0.66f, 0.92f),
    .content_tertiary = Color::rgba(0.60f, 0.67f, 0.76f, 0.84f),
    .content_disabled = Color::rgba(0.46f, 0.55f, 0.66f, 0.40f),
    .content_on_accent = Color::rgba(1.0f, 1.0f, 1.0f, 1.0f),

    
    .accent = Color::rgba(0.37f, 0.67f, 0.98f, 1.0f),
    .accent_hover = Color::rgba(0.33f, 0.63f, 0.95f, 1.0f),
    .accent_pressed = Color::rgba(0.28f, 0.57f, 0.89f, 1.0f),

    
    .control_background = [] {
        Material m = Material::solid(Color::rgba(0.97f, 0.98f, 0.995f, 0.90f));
        m.inner_highlight = 0.18f;
        m.inner_shadow = 0.03f;
        m.border_color = Color::rgba(0.77f, 0.83f, 0.91f, 0.70f);
        m.border_width = 0.7f;
        return m;
    }(),
    .control_hover = [] {
        Material m = Material::solid(Color::rgba(0.985f, 0.99f, 1.0f, 0.98f));
        m.inner_highlight = 0.24f;
        m.inner_shadow = 0.02f;
        m.border_color = Color::rgba(0.76f, 0.83f, 0.91f, 0.78f);
        m.border_width = 0.7f;
        return m;
    }(),
    .control_pressed = [] {
        Material m = Material::solid(Color::rgba(0.91f, 0.94f, 0.98f, 0.98f));
        m.inner_highlight = 0.06f;
        m.inner_shadow = 0.08f;
        m.border_color = Color::rgba(0.72f, 0.79f, 0.88f, 0.82f);
        m.border_width = 0.7f;
        return m;
    }(),
    .control_disabled = [] {
        Material m = Material::solid(Color::rgba(0.96f, 0.97f, 0.98f, 0.52f));
        m.border_color = Color::rgba(0.78f, 0.83f, 0.90f, 0.24f);
        m.border_width = 0.5f;
        return m;
    }(),
    .control_selected = [] {
        Material m = Material::solid(Color::rgba(0.80f, 0.90f, 1.0f, 0.88f));
        m.inner_highlight = 0.20f;
        m.inner_shadow = 0.03f;
        m.border_color = Color::rgba(0.51f, 0.73f, 0.96f, 0.90f);
        m.border_width = 0.8f;
        return m;
    }(),

    
    .text_input_background = [] {
        Material m = Material::solid(Color::rgba(0.985f, 0.99f, 1.0f, 0.98f));
        m.inner_highlight = 0.14f;
        m.border_color = Color::rgba(0.78f, 0.84f, 0.92f, 0.78f);
        m.border_width = 0.9f;
        return m;
    }(),
    .text_input_focused = [] {
        Material m = Material::solid(Color::rgba(0.99f, 0.995f, 1.0f, 1.0f));
        m.border_color = Color::rgba(0.42f, 0.69f, 0.98f, 0.72f);
        m.border_width = 1.0f;
        return m;
    }(),
    .checkbox_background = [] {
        Material m = Material::solid(Color::rgba(1.0f, 1.0f, 1.0f, 1.0f));
        m.border_color = Color::rgba(0, 0, 0, 0.2f);
        m.border_width = 1.0f;
        return m;
    }(),
    .checkbox_checked = [] {
        Material m = Material::solid(Color::rgba(0.0f, 0.478f, 1.0f, 1.0f));
        m.inner_highlight = 0.1f;
        return m;
    }(),
    .slider_track = Material::solid(Color::rgba(0.77f, 0.84f, 0.92f, 0.30f)),
    .slider_fill = Material::solid(Color::rgba(0.37f, 0.67f, 0.98f, 1.0f)),
    .slider_thumb = [] {
        Material m = Material::solid(Color::rgba(1.0f, 1.0f, 1.0f, 1.0f));
        m.inner_highlight = 0.3f;
        m.inner_shadow = 0.05f;
        m.border_color = Color::rgba(0, 0, 0, 0.06f);
        m.border_width = 0.5f;
        return m;
    }(),
    .progress_track = Material::solid(Color::rgba(0.77f, 0.84f, 0.92f, 0.22f)),
    .progress_fill = Material::solid(Color::rgba(0.37f, 0.67f, 0.98f, 1.0f)),
    .separator = Material::solid(Color::rgba(0.78f, 0.84f, 0.91f, 0.58f)),

    
    .elevation = {
        
        { Shadow::none(), Shadow::none() },
        
        {
            Shadow::make(0, 1.0f, 3.0f, Color::rgba(0.40f, 0.52f, 0.67f, 0.12f), 0),
            Shadow::make(0, 2.0f, 8.0f, Color::rgba(0.63f, 0.73f, 0.84f, 0.10f), 0),
        },
        
        {
            Shadow::make(0, 2.0f, 8.0f, Color::rgba(0, 0, 0, 0.18f), 0),
            Shadow::make(0, 6.0f, 20.0f, Color::rgba(0, 0, 0, 0.12f), 0),
        },
        
        {
            Shadow::make(0, 14.0f, 32.0f, Color::rgba(0.44f, 0.56f, 0.70f, 0.18f), 0),
            Shadow::make(0, 26.0f, 56.0f, Color::rgba(0.69f, 0.77f, 0.87f, 0.14f), 0),
        },
    },

    
    .radius_small = 4.0f,
    .radius_medium = 10.0f,
    .radius_large = 16.0f,
    .radius_full = 999.0f,

    
    .spacing_xs = 4.0f,
    .spacing_sm = 8.0f,
    .spacing_md = 12.0f,
    .spacing_lg = 16.0f,
    .spacing_xl = 24.0f,

    
    .font_body = { .size = 14, .weight = 450, .letter_spacing = 0, .line_height = 1.3f },
    .font_body_bold = { .size = 14, .weight = 650, .letter_spacing = 0, .line_height = 1.3f },
    .font_caption = { .size = 12, .weight = 500, .letter_spacing = 0.02f, .line_height = 1.2f },
    .font_heading = { .size = 20, .weight = 700, .letter_spacing = -0.01f, .line_height = 1.25f },
    .font_title = { .size = 28, .weight = 700, .letter_spacing = -0.02f, .line_height = 1.15f },

    
    .anim_hover = { .duration = 0.15f, .easing = ease_out_cubic },
    .anim_press = { .duration = 0.08f, .easing = ease_out_quart },
    .anim_focus = { .duration = 0.2f, .easing = ease_out_cubic },
    .anim_layout = { .duration = 0.3f, .easing = ease_out_cubic },

    
    .focus_ring_width = 2.0f,
    .focus_ring_color = Color::rgba(0.42f, 0.69f, 0.98f, 0.42f),
    .border_width = 0.5f,
};



const Theme kPlexyDarkTheme = {
    
    .surface_primary = [] {
        Material m = Material::solid(Color::rgba(0.15f, 0.15f, 0.16f, 0.95f));
        m.inner_highlight = 0.08f;
        m.inner_shadow = 0.06f;
        m.border_color = Color::rgba(1, 1, 1, 0.06f);
        m.border_width = 0.5f;
        return m;
    }(),
    .surface_secondary = [] {
        Material m = Material::solid(Color::rgba(0.18f, 0.18f, 0.19f, 0.92f));
        m.border_color = Color::rgba(1, 1, 1, 0.05f);
        m.border_width = 0.5f;
        return m;
    }(),
    .surface_tertiary = Material::solid(Color::rgba(0.22f, 0.22f, 0.23f, 0.88f)),
    .surface_glass = Material::glass(20.0f, 0.72f,
                                      Color::rgba(0.1f, 0.1f, 0.1f, 0.35f),
                                      1.2f, 0.08f, 0.1f),
    .surface_vibrancy = [] {
        Material m = Material::glass(12.0f, 0.6f,
                                      Color::rgba(0.1f, 0.1f, 0.1f, 0.2f),
                                      1.15f, 0.05f, 0.08f);
        m.type = Material::Type::Vibrancy;
        return m;
    }(),

    
    .content_primary = Color::rgba(1.0f, 1.0f, 1.0f, 0.88f),
    .content_secondary = Color::rgba(1.0f, 1.0f, 1.0f, 0.55f),
    .content_tertiary = Color::rgba(1.0f, 1.0f, 1.0f, 0.35f),
    .content_disabled = Color::rgba(1.0f, 1.0f, 1.0f, 0.2f),
    .content_on_accent = Color::rgba(1.0f, 1.0f, 1.0f, 1.0f),

    
    .accent = Color::rgba(0.04f, 0.52f, 1.0f, 1.0f),
    .accent_hover = Color::rgba(0.1f, 0.56f, 1.0f, 1.0f),
    .accent_pressed = Color::rgba(0.0f, 0.44f, 0.88f, 1.0f),

    
    .control_background = [] {
        Material m = Material::solid(Color::rgba(0.25f, 0.25f, 0.27f, 0.8f));
        m.inner_highlight = 0.1f;
        m.inner_shadow = 0.06f;
        m.border_color = Color::rgba(1, 1, 1, 0.08f);
        m.border_width = 0.5f;
        return m;
    }(),
    .control_hover = [] {
        Material m = Material::solid(Color::rgba(0.3f, 0.3f, 0.32f, 0.85f));
        m.inner_highlight = 0.12f;
        m.inner_shadow = 0.04f;
        m.border_color = Color::rgba(1, 1, 1, 0.1f);
        m.border_width = 0.5f;
        return m;
    }(),
    .control_pressed = [] {
        Material m = Material::solid(Color::rgba(0.2f, 0.2f, 0.22f, 0.9f));
        m.inner_highlight = 0.04f;
        m.inner_shadow = 0.12f;
        m.border_color = Color::rgba(1, 1, 1, 0.12f);
        m.border_width = 0.5f;
        return m;
    }(),
    .control_disabled = [] {
        Material m = Material::solid(Color::rgba(0.2f, 0.2f, 0.2f, 0.4f));
        m.border_color = Color::rgba(1, 1, 1, 0.04f);
        m.border_width = 0.5f;
        return m;
    }(),
    .control_selected = [] {
        Material m = Material::solid(Color::rgba(0.04f, 0.52f, 1.0f, 1.0f));
        m.inner_highlight = 0.1f;
        m.inner_shadow = 0.06f;
        return m;
    }(),

    
    .text_input_background = [] {
        Material m = Material::solid(Color::rgba(0.1f, 0.1f, 0.11f, 1.0f));
        m.border_color = Color::rgba(1, 1, 1, 0.1f);
        m.border_width = 1.0f;
        return m;
    }(),
    .text_input_focused = [] {
        Material m = Material::solid(Color::rgba(0.1f, 0.1f, 0.11f, 1.0f));
        m.border_color = Color::rgba(0.04f, 0.52f, 1.0f, 0.5f);
        m.border_width = 1.0f;
        return m;
    }(),
    .checkbox_background = [] {
        Material m = Material::solid(Color::rgba(0.2f, 0.2f, 0.22f, 1.0f));
        m.border_color = Color::rgba(1, 1, 1, 0.15f);
        m.border_width = 1.0f;
        return m;
    }(),
    .checkbox_checked = [] {
        Material m = Material::solid(Color::rgba(0.04f, 0.52f, 1.0f, 1.0f));
        m.inner_highlight = 0.08f;
        return m;
    }(),
    .slider_track = Material::solid(Color::rgba(1.0f, 1.0f, 1.0f, 0.08f)),
    .slider_fill = Material::solid(Color::rgba(0.04f, 0.52f, 1.0f, 1.0f)),
    .slider_thumb = [] {
        Material m = Material::solid(Color::rgba(0.95f, 0.95f, 0.95f, 1.0f));
        m.inner_highlight = 0.15f;
        m.inner_shadow = 0.08f;
        m.border_color = Color::rgba(0, 0, 0, 0.2f);
        m.border_width = 0.5f;
        return m;
    }(),
    .progress_track = Material::solid(Color::rgba(1.0f, 1.0f, 1.0f, 0.06f)),
    .progress_fill = Material::solid(Color::rgba(0.04f, 0.52f, 1.0f, 1.0f)),
    .separator = Material::solid(Color::rgba(1.0f, 1.0f, 1.0f, 0.08f)),

    
    .elevation = {
        { Shadow::none(), Shadow::none() },
        {
            Shadow::make(0, 0.5f, 3.0f, Color::rgba(0, 0, 0, 0.3f), 0),
            Shadow::make(0, 1.0f, 6.0f, Color::rgba(0, 0, 0, 0.15f), 0),
        },
        {
            Shadow::make(0, 2.0f, 10.0f, Color::rgba(0, 0, 0, 0.3f), 0),
            Shadow::make(0, 4.0f, 20.0f, Color::rgba(0, 0, 0, 0.15f), 0),
        },
        {
            Shadow::make(0, 8.0f, 28.0f, Color::rgba(0, 0, 0, 0.35f), 0),
            Shadow::make(0, 16.0f, 52.0f, Color::rgba(0, 0, 0, 0.18f), 0),
        },
    },

    
    .radius_small = 4.0f,
    .radius_medium = 8.0f,
    .radius_large = 12.0f,
    .radius_full = 999.0f,

    
    .spacing_xs = 4.0f,
    .spacing_sm = 8.0f,
    .spacing_md = 12.0f,
    .spacing_lg = 16.0f,
    .spacing_xl = 24.0f,

    
    .font_body = { .size = 14, .weight = 400, .letter_spacing = 0, .line_height = 1.3f },
    .font_body_bold = { .size = 14, .weight = 700, .letter_spacing = 0, .line_height = 1.3f },
    .font_caption = { .size = 12, .weight = 400, .letter_spacing = 0.02f, .line_height = 1.2f },
    .font_heading = { .size = 20, .weight = 700, .letter_spacing = -0.01f, .line_height = 1.25f },
    .font_title = { .size = 28, .weight = 700, .letter_spacing = -0.02f, .line_height = 1.15f },

    
    .anim_hover = { .duration = 0.15f, .easing = ease_out_cubic },
    .anim_press = { .duration = 0.08f, .easing = ease_out_quart },
    .anim_focus = { .duration = 0.2f, .easing = ease_out_cubic },
    .anim_layout = { .duration = 0.3f, .easing = ease_out_cubic },

    
    .focus_ring_width = 2.0f,
    .focus_ring_color = Color::rgba(0.04f, 0.52f, 1.0f, 0.4f),
    .border_width = 0.5f,
};

} 
