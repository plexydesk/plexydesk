#pragma once

#include <GL/glew.h>
#include "types.hpp"

namespace plexy::ui {

class GlyphAtlas;  

struct GlyphRunInstance {
    float x, y, w, h;       
    float u0, v0, u1, v1;   
};

struct GlyphRun {
    uint32_t id;
    uint32_t glyph_count;
    GlyphRunInstance* glyphs;  
};

class RenderBackend {
public:
    void init(int screen_width, int screen_height);
    void shutdown();
    void resize(int screen_width, int screen_height);

    
    void render(const PaintList& cmds,
                GLuint scene_texture,
                GLuint blur_quarter_texture,
                GLuint quad_vao,
                bool dark_mode,
                float glass_alpha,
                GlyphAtlas* atlas,
                const GlyphRun* glyph_runs,
                uint32_t glyph_run_count);

private:
    GLuint prog_sdf_ = 0;
    GLuint prog_text_ = 0;
    GLuint prog_image_ = 0;
    GLuint prog_glass_ = 0;
    GLuint prog_line_ = 0;

    
    GLint u_sdf_rect_, u_sdf_screen_size_, u_sdf_radii_;
    GLint u_sdf_fill_color_, u_sdf_border_color_, u_sdf_border_width_;
    GLint u_sdf_inner_highlight_, u_sdf_inner_shadow_;
    GLint u_sdf_shadow_color_, u_sdf_shadow_offset_, u_sdf_shadow_blur_;
    GLint u_sdf_shadow_spread_, u_sdf_is_shadow_pass_, u_sdf_shadow_rect_;

    
    GLint u_text_screen_size_;
    GLint u_text_atlas_, u_text_color_;

    
    GLint u_img_rect_, u_img_uv_, u_img_screen_size_;
    GLint u_img_tex_, u_img_tint_;

    
    GLint u_glass_blur_texture_, u_glass_screen_size_, u_glass_rect_;
    GLint u_glass_radii_, u_glass_tint_, u_glass_saturation_;
    GLint u_glass_opacity_, u_glass_inner_highlight_, u_glass_inner_shadow_;
    GLint u_glass_border_color_, u_glass_border_width_, u_glass_noise_;

    
    GLint u_line_screen_size_, u_line_color_;

    int screen_width_ = 0, screen_height_ = 0;

    
    GLuint text_vao_ = 0;
    GLuint text_vbo_ = 0;
    static constexpr uint32_t MAX_TEXT_BATCH_GLYPHS = 1024;

    GLuint line_vao_ = 0;
    GLuint line_vbo_ = 0;

    void render_rounded_rect(const PaintCmd& cmd, GLuint quad_vao);
    void render_shadow(const PaintCmd& cmd, GLuint quad_vao);
    void render_glass_rect(const PaintCmd& cmd, GLuint quad_vao,
                           GLuint blur_texture);
    void render_text(const PaintCmd& cmd, GLuint quad_vao,
                     GlyphAtlas* atlas,
                     const GlyphRun* runs, uint32_t run_count);
    void render_line(const PaintCmd& cmd);
    void render_icon(const PaintCmd& cmd, GLuint quad_vao);
};

uint32_t canvas_get_theme_icon_texture(const char* icon_name);

} 
