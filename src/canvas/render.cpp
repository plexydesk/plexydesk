#include "render.hpp"
#include "shaders.hpp"
#include "text.hpp"
#include "../../include/stb/stb_image.h"
#include <unordered_map>
#include <string>
#include <vector>
#include <unistd.h>
#include <cstdio>

namespace plexy::ui {

static std::unordered_map<std::string, uint32_t> g_icon_cache;

static uint32_t load_png_texture_local(const std::string& path) {
    int w, h, ch;
    unsigned char* pixels = stbi_load(path.c_str(), &w, &h, &ch, 4);
    if (!pixels) return 0;

    GLuint tex = 0;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    stbi_image_free(pixels);
    return tex;
}

uint32_t canvas_get_theme_icon_texture(const char* icon_name) {
    if (!icon_name || !icon_name[0]) return 0;

    auto it = g_icon_cache.find(icon_name);
    if (it != g_icon_cache.end()) return it->second;

    std::vector<std::string> candidates;
    if (const char* icon_path = std::getenv("PLEXY_ICON_PATH"); icon_path && icon_path[0]) {
        candidates.emplace_back(std::string(icon_path) + "/" + icon_name + ".png");
        candidates.emplace_back(std::string(icon_path) + "/actions/" + icon_name + ".png");
        candidates.emplace_back(std::string(icon_path) + "/apps/" + icon_name + ".png");
        candidates.emplace_back(std::string(icon_path) + "/mimetypes/" + icon_name + ".png");
    }
    if (const char* work_dir = std::getenv("PLEXY_WORK_DIR"); work_dir && work_dir[0]) {
        candidates.emplace_back(std::string(work_dir) + "/icons/actions/" + icon_name + ".png");
        candidates.emplace_back(std::string(work_dir) + "/icons/apps/" + icon_name + ".png");
        candidates.emplace_back(std::string(work_dir) + "/icons/mimetypes/" + icon_name + ".png");
        candidates.emplace_back(std::string(work_dir) + "/share/plexydesk/icons/actions/" + icon_name + ".png");
        candidates.emplace_back(std::string(work_dir) + "/share/plexydesk/icons/apps/" + icon_name + ".png");
        candidates.emplace_back(std::string(work_dir) + "/share/plexydesk/icons/mimetypes/" + icon_name + ".png");
    }

    std::vector<std::string> fallback_candidates = {
        std::string("icons/actions/") + icon_name + ".png",
        std::string("icons/apps/") + icon_name + ".png",
        std::string("icons/mimetypes/") + icon_name + ".png",
        std::string("share/plexydesk/icons/actions/") + icon_name + ".png",
        std::string("share/plexydesk/icons/apps/") + icon_name + ".png",
        std::string("share/plexydesk/icons/mimetypes/") + icon_name + ".png",
        std::string("./icons/actions/") + icon_name + ".png",
        std::string("./icons/apps/") + icon_name + ".png",
        std::string("./icons/mimetypes/") + icon_name + ".png",
        std::string("./share/plexydesk/icons/actions/") + icon_name + ".png",
        std::string("./share/plexydesk/icons/apps/") + icon_name + ".png",
        std::string("./share/plexydesk/icons/mimetypes/") + icon_name + ".png",
        std::string("../icons/actions/") + icon_name + ".png",
        std::string("../icons/apps/") + icon_name + ".png",
        std::string("../icons/mimetypes/") + icon_name + ".png",
        std::string("../share/plexydesk/icons/actions/") + icon_name + ".png",
        std::string("../share/plexydesk/icons/apps/") + icon_name + ".png",
        std::string("../share/plexydesk/icons/mimetypes/") + icon_name + ".png",
        std::string("../../icons/actions/") + icon_name + ".png",
        std::string("../../icons/apps/") + icon_name + ".png",
        std::string("../../icons/mimetypes/") + icon_name + ".png",
        std::string("../../share/plexydesk/icons/actions/") + icon_name + ".png",
        std::string("../../share/plexydesk/icons/apps/") + icon_name + ".png",
        std::string("../../share/plexydesk/icons/mimetypes/") + icon_name + ".png",
        std::string("/usr/share/plexydesk/icons/actions/") + icon_name + ".png",
        std::string("/usr/share/plexydesk/icons/apps/") + icon_name + ".png",
        std::string("/usr/share/plexydesk/icons/mimetypes/") + icon_name + ".png",
        std::string("/opt/plexydesk/current/share/plexydesk/icons/actions/") + icon_name + ".png",
        std::string("/opt/plexydesk/current/share/plexydesk/icons/apps/") + icon_name + ".png",
        std::string("/opt/plexydesk/current/share/plexydesk/icons/mimetypes/") + icon_name + ".png",
    };
    candidates.insert(candidates.end(), fallback_candidates.begin(), fallback_candidates.end());

    uint32_t tex = 0;
    for (const auto& p : candidates) {
        if (access(p.c_str(), R_OK) == 0) {
            tex = load_png_texture_local(p);
            if (tex) break;
        }
    }

    if (!tex && std::string(icon_name) != "application-default-icon") {
        tex = canvas_get_theme_icon_texture("application-default-icon");
    }

    g_icon_cache[icon_name] = tex;
    return tex;
}

void RenderBackend::init(int screen_width, int screen_height) {
    screen_width_ = screen_width;
    screen_height_ = screen_height;

    
    prog_sdf_ = canvas_create_program(kUIKitSdfVS, kUIKitSdfFS);
    prog_text_ = canvas_create_program(kUIKitTextBatchVS, kUIKitTextFS);
    prog_image_ = canvas_create_program(kUIKitTextVS, kUIKitImageFS);
    prog_glass_ = canvas_create_program(kUIKitSdfVS, kUIKitGlassFS);
    prog_line_ = canvas_create_program(kUIKitLineVS, kUIKitLineFS);

    if (!prog_sdf_) fprintf(stderr, "[Canvas] Failed to compile SDF shader\n");
    if (!prog_text_) fprintf(stderr, "[Canvas] Failed to compile text shader\n");
    if (!prog_image_) fprintf(stderr, "[Canvas] Failed to compile image shader\n");
    if (!prog_glass_) fprintf(stderr, "[Canvas] Failed to compile glass shader\n");
    if (!prog_line_) fprintf(stderr, "[Canvas] Failed to compile line shader\n");

    
    if (prog_sdf_) {
        u_sdf_rect_ = glGetUniformLocation(prog_sdf_, "u_rect");
        u_sdf_screen_size_ = glGetUniformLocation(prog_sdf_, "u_screen_size");
        u_sdf_radii_ = glGetUniformLocation(prog_sdf_, "u_radii");
        u_sdf_fill_color_ = glGetUniformLocation(prog_sdf_, "u_fill_color");
        u_sdf_border_color_ = glGetUniformLocation(prog_sdf_, "u_border_color");
        u_sdf_border_width_ = glGetUniformLocation(prog_sdf_, "u_border_width");
        u_sdf_inner_highlight_ = glGetUniformLocation(prog_sdf_, "u_inner_highlight");
        u_sdf_inner_shadow_ = glGetUniformLocation(prog_sdf_, "u_inner_shadow");
        u_sdf_shadow_color_ = glGetUniformLocation(prog_sdf_, "u_shadow_color");
        u_sdf_shadow_offset_ = glGetUniformLocation(prog_sdf_, "u_shadow_offset");
        u_sdf_shadow_blur_ = glGetUniformLocation(prog_sdf_, "u_shadow_blur");
        u_sdf_shadow_spread_ = glGetUniformLocation(prog_sdf_, "u_shadow_spread");
        u_sdf_is_shadow_pass_ = glGetUniformLocation(prog_sdf_, "u_is_shadow_pass");
        u_sdf_shadow_rect_ = glGetUniformLocation(prog_sdf_, "u_shadow_rect");
    }

    if (prog_text_) {
        u_text_screen_size_ = glGetUniformLocation(prog_text_, "u_screen_size");
        u_text_atlas_ = glGetUniformLocation(prog_text_, "u_atlas");
        u_text_color_ = glGetUniformLocation(prog_text_, "u_text_color");
    }

    if (prog_image_) {
        u_img_rect_ = glGetUniformLocation(prog_image_, "u_glyph_rect");
        u_img_uv_ = glGetUniformLocation(prog_image_, "u_glyph_uv");
        u_img_screen_size_ = glGetUniformLocation(prog_image_, "u_screen_size");
        u_img_tex_ = glGetUniformLocation(prog_image_, "u_tex");
        u_img_tint_ = glGetUniformLocation(prog_image_, "u_tint");
    }

    if (prog_glass_) {
        u_glass_blur_texture_ = glGetUniformLocation(prog_glass_, "u_blur_texture");
        u_glass_screen_size_ = glGetUniformLocation(prog_glass_, "u_screen_size");
        u_glass_rect_ = glGetUniformLocation(prog_glass_, "u_rect");
        u_glass_radii_ = glGetUniformLocation(prog_glass_, "u_radii");
        u_glass_tint_ = glGetUniformLocation(prog_glass_, "u_tint");
        u_glass_saturation_ = glGetUniformLocation(prog_glass_, "u_saturation");
        u_glass_opacity_ = glGetUniformLocation(prog_glass_, "u_opacity");
        u_glass_inner_highlight_ = glGetUniformLocation(prog_glass_, "u_inner_highlight");
        u_glass_inner_shadow_ = glGetUniformLocation(prog_glass_, "u_inner_shadow");
        u_glass_border_color_ = glGetUniformLocation(prog_glass_, "u_border_color");
        u_glass_border_width_ = glGetUniformLocation(prog_glass_, "u_border_width");
        u_glass_noise_ = glGetUniformLocation(prog_glass_, "u_noise");
    }

    if (prog_line_) {
        u_line_screen_size_ = glGetUniformLocation(prog_line_, "u_screen_size");
        u_line_color_ = glGetUniformLocation(prog_line_, "u_color");
    }

    printf("[Canvas] Render backend initialized (%dx%d)\n", screen_width, screen_height);

    
    glGenVertexArrays(1, &text_vao_);
    glGenBuffers(1, &text_vbo_);
    glBindVertexArray(text_vao_);
    glBindBuffer(GL_ARRAY_BUFFER, text_vbo_);
    
    
    glBufferData(GL_ARRAY_BUFFER, MAX_TEXT_BATCH_GLYPHS * 6 * 4 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0); 
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1); 
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glBindVertexArray(0);

    glGenVertexArrays(1, &line_vao_);
    glGenBuffers(1, &line_vbo_);
    glBindVertexArray(line_vao_);
    glBindBuffer(GL_ARRAY_BUFFER, line_vbo_);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glBindVertexArray(0);
}

void RenderBackend::shutdown() {
    if (prog_sdf_) { glDeleteProgram(prog_sdf_); prog_sdf_ = 0; }
    if (prog_text_) { glDeleteProgram(prog_text_); prog_text_ = 0; }
    if (prog_image_) { glDeleteProgram(prog_image_); prog_image_ = 0; }
    if (prog_glass_) { glDeleteProgram(prog_glass_); prog_glass_ = 0; }
    if (prog_line_) { glDeleteProgram(prog_line_); prog_line_ = 0; }
    if (text_vbo_) { glDeleteBuffers(1, &text_vbo_); text_vbo_ = 0; }
    if (text_vao_) { glDeleteVertexArrays(1, &text_vao_); text_vao_ = 0; }
    if (line_vbo_) { glDeleteBuffers(1, &line_vbo_); line_vbo_ = 0; }
    if (line_vao_) { glDeleteVertexArrays(1, &line_vao_); line_vao_ = 0; }

    for (auto& kv : g_icon_cache) {
        if (kv.second) {
            GLuint tex = kv.second;
            glDeleteTextures(1, &tex);
        }
    }
    g_icon_cache.clear();
}

void RenderBackend::resize(int screen_width, int screen_height) {
    screen_width_ = screen_width;
    screen_height_ = screen_height;
}

void RenderBackend::render(const PaintList& cmds,
                           GLuint scene_texture,
                           GLuint blur_quarter_texture,
                           GLuint quad_vao,
                           bool dark_mode,
                           float glass_alpha,
                           GlyphAtlas* atlas,
                           const GlyphRun* glyph_runs,
                           uint32_t glyph_run_count) {
    if (cmds.count == 0) return;

    glEnable(GL_BLEND);
    
    
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
                        GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glBindVertexArray(quad_vao);

    for (uint32_t i = 0; i < cmds.count; i++) {
        const PaintCmd& cmd = cmds.cmds[i];
        switch (cmd.op) {
            case PaintOp::Shadow:
                render_shadow(cmd, quad_vao);
                break;
            case PaintOp::RoundedRect:
                if (cmd.material.type == Material::Type::Glass ||
                    cmd.material.type == Material::Type::Vibrancy) {
                    render_glass_rect(cmd, quad_vao, blur_quarter_texture);
                } else {
                    render_rounded_rect(cmd, quad_vao);
                }
                break;
            case PaintOp::Text:
                render_text(cmd, quad_vao, atlas, glyph_runs, glyph_run_count);
                break;
            case PaintOp::Line:
                render_line(cmd);
                break;
            case PaintOp::Icon:
                render_icon(cmd, quad_vao);
                break;
            default:
                break;
        }
    }

    glBindVertexArray(0);
    glUseProgram(0);
}

void RenderBackend::render_rounded_rect(const PaintCmd& cmd, GLuint quad_vao) {
    if (!prog_sdf_) return;

    glUseProgram(prog_sdf_);
    glUniform4f(u_sdf_rect_, cmd.x, cmd.y, cmd.w, cmd.h);
    glUniform2f(u_sdf_screen_size_, (float)screen_width_, (float)screen_height_);
    glUniform4f(u_sdf_radii_, cmd.radii.tl, cmd.radii.tr, cmd.radii.br, cmd.radii.bl);

    const Color& fc = cmd.material.color;
    glUniform4f(u_sdf_fill_color_, fc.r, fc.g, fc.b, fc.a);

    const Color& bc = cmd.material.border_color;
    glUniform4f(u_sdf_border_color_, bc.r, bc.g, bc.b, bc.a);
    glUniform1f(u_sdf_border_width_, cmd.material.border_width);
    glUniform1f(u_sdf_inner_highlight_, cmd.material.inner_highlight);
    glUniform1f(u_sdf_inner_shadow_, cmd.material.inner_shadow);

    glUniform4f(u_sdf_shadow_color_, 0, 0, 0, 0);
    glUniform2f(u_sdf_shadow_offset_, 0, 0);
    glUniform1f(u_sdf_shadow_blur_, 0);
    glUniform1f(u_sdf_shadow_spread_, 0);
    glUniform1i(u_sdf_is_shadow_pass_, 0);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void RenderBackend::render_shadow(const PaintCmd& cmd, GLuint quad_vao) {
    if (!prog_sdf_) return;

    float expand = cmd.shadow.sigma * 3.0f + cmd.shadow.spread + 4.0f;

    glUseProgram(prog_sdf_);

    float orig_cx = cmd.x + cmd.w * 0.5f;
    float orig_cy = cmd.y + cmd.h * 0.5f;
    float exp_w = cmd.w + expand * 2;
    float exp_h = cmd.h + expand * 2;
    glUniform4f(u_sdf_rect_, orig_cx - exp_w * 0.5f, orig_cy - exp_h * 0.5f, exp_w, exp_h);
    glUniform2f(u_sdf_screen_size_, (float)screen_width_, (float)screen_height_);
    glUniform4f(u_sdf_radii_, cmd.shadow.radius, cmd.shadow.radius,
                cmd.shadow.radius, cmd.shadow.radius);

    glUniform4f(u_sdf_fill_color_, 0, 0, 0, 0);
    glUniform4f(u_sdf_border_color_, 0, 0, 0, 0);
    glUniform1f(u_sdf_border_width_, 0);
    glUniform1f(u_sdf_inner_highlight_, 0);
    glUniform1f(u_sdf_inner_shadow_, 0);

    const Color& sc = cmd.shadow.color;
    glUniform4f(u_sdf_shadow_color_, sc.r, sc.g, sc.b, sc.a);
    glUniform2f(u_sdf_shadow_offset_, cmd.shadow.ox, cmd.shadow.oy);
    glUniform1f(u_sdf_shadow_blur_, cmd.shadow.sigma);
    glUniform1f(u_sdf_shadow_spread_, cmd.shadow.spread);
    glUniform1i(u_sdf_is_shadow_pass_, 1);
    
    glUniform4f(u_sdf_shadow_rect_, cmd.x, cmd.y, cmd.w, cmd.h);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void RenderBackend::render_glass_rect(const PaintCmd& cmd, GLuint quad_vao,
                                       GLuint blur_texture) {
    if (!prog_glass_) return;

    glUseProgram(prog_glass_);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, blur_texture);
    glUniform1i(u_glass_blur_texture_, 0);

    glUniform2f(u_glass_screen_size_, (float)screen_width_, (float)screen_height_);
    glUniform4f(u_glass_rect_, cmd.x, cmd.y, cmd.w, cmd.h);
    glUniform4f(u_glass_radii_, cmd.radii.tl, cmd.radii.tr, cmd.radii.br, cmd.radii.bl);

    const Color& t = cmd.material.tint;
    glUniform4f(u_glass_tint_, t.r, t.g, t.b, t.a);
    glUniform1f(u_glass_saturation_, cmd.material.saturation);
    glUniform1f(u_glass_opacity_, cmd.material.opacity);
    glUniform1f(u_glass_inner_highlight_, cmd.material.inner_highlight);
    glUniform1f(u_glass_inner_shadow_, cmd.material.inner_shadow);

    const Color& bc = cmd.material.border_color;
    glUniform4f(u_glass_border_color_, bc.r, bc.g, bc.b, bc.a);
    glUniform1f(u_glass_border_width_, cmd.material.border_width);
    glUniform1f(u_glass_noise_, cmd.material.noise);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void RenderBackend::render_text(const PaintCmd& cmd, GLuint quad_vao,
                                GlyphAtlas* atlas,
                                const GlyphRun* runs, uint32_t run_count) {
    if (!prog_text_ || !atlas || !runs) return;

    const GlyphRun* run = nullptr;
    
    uint32_t rid = cmd.text.glyph_run_id;
    if (rid >= 1 && rid <= run_count && runs[rid - 1].id == rid) {
        run = &runs[rid - 1];
    } else {
        for (uint32_t i = 0; i < run_count; i++) {
            if (runs[i].id == rid) {
                run = &runs[i];
                break;
            }
        }
    }
    if (!run || run->glyph_count == 0) return;

    glUseProgram(prog_text_);
    glUniform2f(u_text_screen_size_, (float)screen_width_, (float)screen_height_);

    const Color& tc = cmd.text.color;
    glUniform4f(u_text_color_, tc.r, tc.g, tc.b, tc.a);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, atlas->texture());
    glUniform1i(u_text_atlas_, 0);

    
    uint32_t batch_count = run->glyph_count;
    if (batch_count > MAX_TEXT_BATCH_GLYPHS) batch_count = MAX_TEXT_BATCH_GLYPHS;

    
    float* verts = (float*)alloca(batch_count * 6 * 4 * sizeof(float));
    uint32_t vi = 0;

    for (uint32_t g = 0; g < batch_count; g++) {
        const GlyphRunInstance& gi = run->glyphs[g];
        float x0 = gi.x, y0 = gi.y;
        float x1 = gi.x + gi.w, y1 = gi.y + gi.h;

        
        verts[vi++] = x0; verts[vi++] = y0; verts[vi++] = gi.u0; verts[vi++] = gi.v0;
        verts[vi++] = x1; verts[vi++] = y0; verts[vi++] = gi.u1; verts[vi++] = gi.v0;
        verts[vi++] = x1; verts[vi++] = y1; verts[vi++] = gi.u1; verts[vi++] = gi.v1;
        
        verts[vi++] = x0; verts[vi++] = y0; verts[vi++] = gi.u0; verts[vi++] = gi.v0;
        verts[vi++] = x1; verts[vi++] = y1; verts[vi++] = gi.u1; verts[vi++] = gi.v1;
        verts[vi++] = x0; verts[vi++] = y1; verts[vi++] = gi.u0; verts[vi++] = gi.v1;
    }

    glBindVertexArray(text_vao_);
    glBindBuffer(GL_ARRAY_BUFFER, text_vbo_);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vi * sizeof(float), verts);
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(batch_count * 6));
    glBindVertexArray(quad_vao); 
}

void RenderBackend::render_line(const PaintCmd& cmd) {
    if (!prog_line_ || !line_vao_ || !line_vbo_) return;

    const float verts[4] = {
        cmd.line.x1, cmd.line.y1,
        cmd.line.x2, cmd.line.y2
    };

    glUseProgram(prog_line_);
    glUniform2f(u_line_screen_size_, (float)screen_width_, (float)screen_height_);
    glUniform4f(u_line_color_,
                cmd.line.color.r, cmd.line.color.g,
                cmd.line.color.b, cmd.line.color.a);

    glBindVertexArray(line_vao_);
    glBindBuffer(GL_ARRAY_BUFFER, line_vbo_);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);

    glLineWidth(cmd.line.thickness > 1.0f ? cmd.line.thickness : 1.0f);
    glDrawArrays(GL_LINES, 0, 2);
    glLineWidth(1.0f);
    glBindVertexArray(0);
}

void RenderBackend::render_icon(const PaintCmd& cmd, GLuint quad_vao) {
    if (!prog_image_ || cmd.icon.texture_id == 0) return;

    glUseProgram(prog_image_);
    glUniform2f(u_img_screen_size_, (float)screen_width_, (float)screen_height_);
    glUniform4f(u_img_rect_, cmd.x, cmd.y, cmd.w, cmd.h);
    glUniform4f(u_img_uv_, cmd.icon.u0, cmd.icon.v0, cmd.icon.u1, cmd.icon.v1);
    glUniform4f(u_img_tint_, cmd.icon.tint.r, cmd.icon.tint.g, cmd.icon.tint.b, cmd.icon.tint.a);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, cmd.icon.texture_id);
    glUniform1i(u_img_tex_, 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

} 
