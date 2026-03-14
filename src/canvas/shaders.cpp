#include "shaders.hpp"
#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <sstream>
#include <string>

namespace plexy::ui {



namespace {

static bool is_word_char(char c) {
    return std::isalnum(static_cast<unsigned char>(c)) || c == '_';
}

static void replace_all(std::string& s, const std::string& from, const std::string& to) {
    if (from.empty()) return;
    size_t pos = 0;
    while ((pos = s.find(from, pos)) != std::string::npos) {
        s.replace(pos, from.size(), to);
        pos += to.size();
    }
}

static void replace_word(std::string& s, const std::string& from, const std::string& to) {
    if (from.empty()) return;
    size_t pos = 0;
    while ((pos = s.find(from, pos)) != std::string::npos) {
        const bool left_ok = (pos == 0) || !is_word_char(s[pos - 1]);
        const size_t right_idx = pos + from.size();
        const bool right_ok = (right_idx >= s.size()) || !is_word_char(s[right_idx]);
        if (left_ok && right_ok) {
            s.replace(pos, from.size(), to);
            pos += to.size();
        } else {
            pos += from.size();
        }
    }
}

static std::string downgrade_shader_to_glsl120(GLenum type, const char* src) {
    if (!src) return {};
    std::istringstream input(src);
    std::string out;
    std::string line;
    std::string frag_output_name;

    while (std::getline(input, line)) {
        const size_t first = line.find_first_not_of(" \t");
        const std::string indent = (first == std::string::npos) ? "" : line.substr(0, first);
        std::string code = (first == std::string::npos) ? "" : line.substr(first);

        if (code.rfind("#version", 0) == 0) {
            out += indent + "#version 120\n";
            out += "#define textureLod(s, uv, lod) texture2D(s, uv)\n";
            continue;
        }

        if (code.rfind("layout(", 0) == 0) {
            const size_t close = code.find(')');
            if (close != std::string::npos) {
                std::string tail = code.substr(close + 1);
                const size_t tail_first = tail.find_first_not_of(" \t");
                code = (tail_first == std::string::npos) ? "" : tail.substr(tail_first);
            }
        }

        if (type == GL_VERTEX_SHADER) {
            if (code.rfind("in ", 0) == 0) {
                code = "attribute " + code.substr(3);
            } else if (code.rfind("out ", 0) == 0) {
                code = "varying " + code.substr(4);
            }
        } else if (type == GL_FRAGMENT_SHADER) {
            if (code.rfind("in ", 0) == 0) {
                code = "varying " + code.substr(3);
            } else if (code.rfind("out ", 0) == 0) {
                std::istringstream decl(code);
                std::string kw, var_type, var_name;
                decl >> kw >> var_type >> var_name;
                if (var_type == "vec4" && !var_name.empty()) {
                    while (!var_name.empty() &&
                           (var_name.back() == ';' || var_name.back() == ',')) {
                        var_name.pop_back();
                    }
                    frag_output_name = var_name;
                    code.clear();
                } else {
                    code = "varying " + code.substr(4);
                }
            }
        }

        if (!code.empty()) {
            out += indent + code + "\n";
        } else {
            out += "\n";
        }
    }

    replace_all(out, "texture(", "texture2D(");
    if (!frag_output_name.empty()) {
        replace_word(out, frag_output_name, "gl_FragColor");
    }
    return out;
}

static bool should_try_glsl120_fallback(const char* src) {
    if (!src || std::string(src).find("#version 330") == std::string::npos) return false;
    const GLubyte* glsl = glGetString(GL_SHADING_LANGUAGE_VERSION);
    if (!glsl) return true;
    const std::string glsl_ver(reinterpret_cast<const char*>(glsl));
    return glsl_ver.rfind("1.", 0) == 0 || glsl_ver.find("1.00 ES") != std::string::npos;
}

} 

GLuint canvas_compile_shader(GLenum type, const char* src) {
    GLuint s = glCreateShader(type);

    auto try_compile = [&](const char* shader_src, std::string& log_out) {
        glShaderSource(s, 1, &shader_src, nullptr);
        glCompileShader(s);
        GLint ok = 0;
        glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
        if (ok) return true;
        char log[4096] = {0};
        glGetShaderInfoLog(s, sizeof(log), nullptr, log);
        log_out = log;
        return false;
    };

    std::string primary_log;
    if (try_compile(src, primary_log)) {
        return s;
    }

    if (should_try_glsl120_fallback(src)) {
        const std::string fallback_src = downgrade_shader_to_glsl120(type, src);
        if (!fallback_src.empty() && fallback_src != src) {
            std::string fallback_log;
            const char* fallback_ptr = fallback_src.c_str();
            if (try_compile(fallback_ptr, fallback_log)) {
                static bool logged = false;
                if (!logged) {
                    std::fprintf(stderr, "[Canvas] Using GLSL 120 compatibility fallback\n");
                    logged = true;
                }
                return s;
            }
            std::fprintf(stderr, "[Canvas] Shader compile error:\n%s\n", primary_log.c_str());
            std::fprintf(stderr, "[Canvas] Shader compile fallback error:\n%s\n", fallback_log.c_str());
            glDeleteShader(s);
            return 0;
        }
    }

    std::fprintf(stderr, "[Canvas] Shader compile error:\n%s\n", primary_log.c_str());
    glDeleteShader(s);
    return 0;
}

GLuint canvas_link_program(GLuint vs, GLuint fs) {
    GLuint p = glCreateProgram();
    glAttachShader(p, vs);
    glAttachShader(p, fs);
    glLinkProgram(p);
    GLint ok = 0;
    glGetProgramiv(p, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[512];
        glGetProgramInfoLog(p, sizeof(log), nullptr, log);
        std::fprintf(stderr, "[Canvas] Program link error:\n%s\n", log);
        glDeleteProgram(p);
        glDeleteShader(vs);
        glDeleteShader(fs);
        return 0;
    }
    glDeleteShader(vs);
    glDeleteShader(fs);
    return p;
}

GLuint canvas_create_program(const char* vs_src, const char* fs_src) {
    GLuint vs = canvas_compile_shader(GL_VERTEX_SHADER, vs_src);
    if (!vs) return 0;
    GLuint fs = canvas_compile_shader(GL_FRAGMENT_SHADER, fs_src);
    if (!fs) { glDeleteShader(vs); return 0; }
    return canvas_link_program(vs, fs);
}



const char* kUIKitSdfVS = R"GLSL(
#version 330 core
layout(location=0) in vec2 aPos;
layout(location=1) in vec2 aUv;

out vec2 vUv;
out vec2 vPixelPos;

uniform vec4 u_rect;        
uniform vec2 u_screen_size;  

void main() {
    vUv = aUv;
    vec2 t = aPos * 0.5 + 0.5;
    vec2 px = u_rect.xy + t * u_rect.zw;
    vPixelPos = px;
    vec2 ndc;
    ndc.x = (px.x / u_screen_size.x) * 2.0 - 1.0;
    ndc.y = 1.0 - (px.y / u_screen_size.y) * 2.0;
    gl_Position = vec4(ndc, 0.0, 1.0);
}
)GLSL";



const char* kUIKitSdfFS = R"GLSL(
#version 330 core
in vec2 vUv;
in vec2 vPixelPos;
out vec4 fragColor;

uniform vec4 u_rect;
uniform vec4 u_radii;
uniform vec4 u_fill_color;
uniform vec4 u_border_color;
uniform float u_border_width;
uniform float u_inner_highlight;
uniform float u_inner_shadow;

uniform vec4 u_shadow_color;
uniform vec2 u_shadow_offset;
uniform float u_shadow_blur;
uniform float u_shadow_spread;
uniform int u_is_shadow_pass;
uniform vec4 u_shadow_rect;  

float sdRoundedRect(vec2 p, vec2 halfSize, vec4 radii) {
    float r;
    if (p.x > 0.0) {
        r = (p.y < 0.0) ? radii.y : radii.z;
    } else {
        r = (p.y < 0.0) ? radii.x : radii.w;
    }
    vec2 q = abs(p) - halfSize + r;
    return min(max(q.x, q.y), 0.0) + length(max(q, 0.0)) - r;
}

void main() {
    vec2 center = u_rect.xy + u_rect.zw * 0.5;
    vec2 halfSize = u_rect.zw * 0.5;
    vec2 p = vPixelPos - center;

    if (u_is_shadow_pass == 1) {
        
        vec2 origCenter = u_shadow_rect.xy + u_shadow_rect.zw * 0.5;
        vec2 origHalf = u_shadow_rect.zw * 0.5;
        vec2 shadowP = vPixelPos - origCenter - u_shadow_offset;
        vec2 expandedHalf = origHalf + u_shadow_spread;
        float d = sdRoundedRect(shadowP, expandedHalf, u_radii);
        float blur = max(u_shadow_blur, 0.5);
        float alpha = 1.0 - smoothstep(-blur, blur, d);
        alpha *= u_shadow_color.a;
        fragColor = vec4(u_shadow_color.rgb, alpha);
        return;
    }

    float d = sdRoundedRect(p, halfSize, u_radii);
    float aa = fwidth(d) * 1.0;
    float fillAlpha = 1.0 - smoothstep(-aa, aa, d);
    if (fillAlpha <= 0.0) discard;

    vec4 color = u_fill_color;

    if (u_border_width > 0.0) {
        float borderDist = abs(d + u_border_width * 0.5) - u_border_width * 0.5;
        float borderAlpha = 1.0 - smoothstep(-aa, aa, borderDist);
        color = mix(color, u_border_color, borderAlpha);
    }

    if (u_inner_highlight > 0.0) {
        float topT = 1.0 - clamp((p.y + halfSize.y) / (halfSize.y * 0.4), 0.0, 1.0);
        float highlightMask = topT * (1.0 - smoothstep(-2.0, 0.0, d));
        color.rgb += vec3(1.0) * highlightMask * u_inner_highlight;
    }

    if (u_inner_shadow > 0.0) {
        float botT = clamp((p.y - halfSize.y * 0.3) / (halfSize.y * 0.7), 0.0, 1.0);
        float shadowMask = botT * (1.0 - smoothstep(-2.0, 0.0, d));
        color.rgb -= vec3(1.0) * shadowMask * u_inner_shadow;
    }

    color.a *= fillAlpha;
    fragColor = color;
}
)GLSL";



const char* kUIKitTextVS = R"GLSL(
#version 330 core
layout(location=0) in vec2 aPos;
layout(location=1) in vec2 aUv;

out vec2 vUv;

uniform vec4 u_glyph_rect;
uniform vec4 u_glyph_uv;
uniform vec2 u_screen_size;

void main() {
    vec2 t = aPos * 0.5 + 0.5;
    vUv = mix(u_glyph_uv.xy, u_glyph_uv.zw, t);
    vec2 px = u_glyph_rect.xy + t * u_glyph_rect.zw;
    vec2 ndc;
    ndc.x = (px.x / u_screen_size.x) * 2.0 - 1.0;
    ndc.y = 1.0 - (px.y / u_screen_size.y) * 2.0;
    gl_Position = vec4(ndc, 0.0, 1.0);
}
)GLSL";




const char* kUIKitTextBatchVS = R"GLSL(
#version 330 core
layout(location=0) in vec2 aPos;
layout(location=1) in vec2 aUv;

out vec2 vUv;

uniform vec2 u_screen_size;

void main() {
    vUv = aUv;
    vec2 ndc;
    ndc.x = (aPos.x / u_screen_size.x) * 2.0 - 1.0;
    ndc.y = 1.0 - (aPos.y / u_screen_size.y) * 2.0;
    gl_Position = vec4(ndc, 0.0, 1.0);
}
)GLSL";



const char* kUIKitTextFS = R"GLSL(
#version 330 core
in vec2 vUv;
out vec4 fragColor;

uniform sampler2D u_atlas;
uniform vec4 u_text_color;

void main() {
    float alpha = texture(u_atlas, vUv).r;
    if (alpha < 0.01) discard;
    fragColor = vec4(u_text_color.rgb, u_text_color.a * alpha);
}
)GLSL";



const char* kUIKitImageFS = R"GLSL(
#version 330 core
in vec2 vUv;
out vec4 fragColor;

uniform sampler2D u_tex;
uniform vec4 u_tint;

void main() {
    vec4 c = texture(u_tex, vUv);
    if (c.a < 0.01) discard;
    
    
    float is_white = step(0.999, u_tint.r) * step(0.999, u_tint.g) * step(0.999, u_tint.b);
    vec3 rgb = mix(u_tint.rgb, c.rgb, is_white);
    fragColor = vec4(rgb, c.a * u_tint.a);
}
)GLSL";



const char* kUIKitGlassFS = R"GLSL(
#version 330 core
in vec2 vUv;
in vec2 vPixelPos;
out vec4 fragColor;

uniform sampler2D u_blur_texture;
uniform vec2 u_screen_size;
uniform vec4 u_rect;
uniform vec4 u_radii;
uniform vec4 u_tint;
uniform float u_saturation;
uniform float u_opacity;
uniform float u_inner_highlight;
uniform float u_inner_shadow;
uniform vec4 u_border_color;
uniform float u_border_width;
uniform float u_noise;

float sdRoundedRect(vec2 p, vec2 halfSize, vec4 radii) {
    float r;
    if (p.x > 0.0) {
        r = (p.y < 0.0) ? radii.y : radii.z;
    } else {
        r = (p.y < 0.0) ? radii.x : radii.w;
    }
    vec2 q = abs(p) - halfSize + r;
    return min(max(q.x, q.y), 0.0) + length(max(q, 0.0)) - r;
}

float hash(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

void main() {
    vec2 center = u_rect.xy + u_rect.zw * 0.5;
    vec2 halfSize = u_rect.zw * 0.5;
    vec2 p = vPixelPos - center;

    float d = sdRoundedRect(p, halfSize, u_radii);
    float aa = fwidth(d);
    float mask = 1.0 - smoothstep(-aa, aa, d);
    if (mask <= 0.0) discard;

    vec2 screenUv = vec2(vPixelPos.x / u_screen_size.x,
                         1.0 - vPixelPos.y / u_screen_size.y);
    vec3 blurred = texture(u_blur_texture, clamp(screenUv, 0.001, 0.999)).rgb;

    float luma = dot(blurred, vec3(0.299, 0.587, 0.114));
    blurred = mix(vec3(luma), blurred, u_saturation);

    vec3 col = mix(blurred, u_tint.rgb, u_tint.a);

    if (u_inner_highlight > 0.0) {
        float topT = 1.0 - clamp((p.y + halfSize.y) / (halfSize.y * 0.4), 0.0, 1.0);
        float hlMask = topT * (1.0 - smoothstep(-2.0, 0.0, d));
        col += vec3(1.0) * hlMask * u_inner_highlight;
    }

    if (u_inner_shadow > 0.0) {
        float botT = clamp((p.y - halfSize.y * 0.3) / (halfSize.y * 0.7), 0.0, 1.0);
        float shMask = botT * (1.0 - smoothstep(-2.0, 0.0, d));
        col -= vec3(1.0) * shMask * u_inner_shadow;
    }

    if (u_noise > 0.0) {
        float n = hash(vPixelPos * 0.5) * 2.0 - 1.0;
        col += vec3(n) * u_noise;
    }

    if (u_border_width > 0.0) {
        float borderDist = abs(d + u_border_width * 0.5) - u_border_width * 0.5;
        float borderAlpha = 1.0 - smoothstep(-aa, aa, borderDist);
        col = mix(col, u_border_color.rgb, borderAlpha * u_border_color.a);
    }

    float rim = 1.0 - smoothstep(0.0, aa * 3.0, -d);
    col += vec3(1.0) * rim * 0.1;

    fragColor = vec4(col, u_opacity * mask);
}
)GLSL";



const char* kUIKitLineVS = R"GLSL(
#version 330 core
layout(location=0) in vec2 aPos;

uniform vec2 u_screen_size;

void main() {
    vec2 ndc;
    ndc.x = (aPos.x / u_screen_size.x) * 2.0 - 1.0;
    ndc.y = 1.0 - (aPos.y / u_screen_size.y) * 2.0;
    gl_Position = vec4(ndc, 0.0, 1.0);
}
)GLSL";

const char* kUIKitLineFS = R"GLSL(
#version 330 core
out vec4 fragColor;

uniform vec4 u_color;

void main() {
    fragColor = u_color;
}
)GLSL";

} 
