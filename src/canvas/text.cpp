#include "text.hpp"
#include <cstdio>
#include <cstring>
#include <cmath>

namespace plexy::ui {


static uint32_t utf8_decode(const char** ptr) {
    const uint8_t* s = (const uint8_t*)*ptr;
    uint32_t cp;
    int len;

    if (s[0] < 0x80) {
        cp = s[0]; len = 1;
    } else if ((s[0] & 0xE0) == 0xC0) {
        cp = s[0] & 0x1F; len = 2;
    } else if ((s[0] & 0xF0) == 0xE0) {
        cp = s[0] & 0x0F; len = 3;
    } else if ((s[0] & 0xF8) == 0xF0) {
        cp = s[0] & 0x07; len = 4;
    } else {
        *ptr += 1;
        return 0xFFFD;
    }

    for (int i = 1; i < len; i++) {
        if ((s[i] & 0xC0) != 0x80) {
            *ptr += 1;
            return 0xFFFD;
        }
        cp = (cp << 6) | (s[i] & 0x3F);
    }

    *ptr += len;
    return cp;
}



void GlyphAtlas::init(const char* font_path, int default_size) {
    
    if (FT_Init_FreeType(&ft_lib_)) {
        fprintf(stderr, "[Canvas] Failed to init FreeType\n");
        return;
    }

    if (FT_New_Face(ft_lib_, font_path, 0, &ft_face_)) {
        fprintf(stderr, "[Canvas] Failed to load font: %s\n", font_path);
        FT_Done_FreeType(ft_lib_);
        ft_lib_ = nullptr;
        return;
    }

    current_size_ = (uint16_t)default_size;
    FT_Set_Pixel_Sizes(ft_face_, 0, current_size_);

    glGenTextures(1, &texture_);
    glBindTexture(GL_TEXTURE_2D, texture_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, ATLAS_SIZE, ATLAS_SIZE,
                 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    uint8_t* zeros = new uint8_t[ATLAS_SIZE * ATLAS_SIZE]();
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, ATLAS_SIZE, ATLAS_SIZE,
                    GL_RED, GL_UNSIGNED_BYTE, zeros);
    delete[] zeros;

    glBindTexture(GL_TEXTURE_2D, 0);

}

void GlyphAtlas::shutdown() {
    if (texture_) { glDeleteTextures(1, &texture_); texture_ = 0; }
    if (ft_face_) { FT_Done_Face(ft_face_); ft_face_ = nullptr; }
    if (ft_lib_) { FT_Done_FreeType(ft_lib_); ft_lib_ = nullptr; }
}

void GlyphAtlas::set_font_size(uint16_t size) {
    if (size == current_size_) return;
    current_size_ = size;
    FT_Set_Pixel_Sizes(ft_face_, 0, size);
}

GlyphEntry* GlyphAtlas::cache_find(uint64_t key) {
    uint32_t idx = hash_key(key);
    for (uint32_t i = 0; i < CACHE_SIZE; i++) {
        uint32_t slot = (idx + i) & (CACHE_SIZE - 1);
        if (!cache_[slot].occupied) return nullptr;
        if (cache_[slot].key == key) return &cache_[slot].glyph;
    }
    return nullptr;
}

GlyphEntry* GlyphAtlas::cache_insert(uint64_t key, const GlyphEntry& entry) {
    uint32_t idx = hash_key(key);
    for (uint32_t i = 0; i < CACHE_SIZE; i++) {
        uint32_t slot = (idx + i) & (CACHE_SIZE - 1);
        if (!cache_[slot].occupied) {
            cache_[slot].key = key;
            cache_[slot].glyph = entry;
            cache_[slot].occupied = true;
            return &cache_[slot].glyph;
        }
    }
    cache_[idx].key = key;
    cache_[idx].glyph = entry;
    return &cache_[idx].glyph;
}

GlyphEntry* GlyphAtlas::get_or_rasterize(uint32_t codepoint, uint16_t font_size) {
    uint64_t key = ((uint64_t)font_size << 32) | codepoint;

    GlyphEntry* cached = cache_find(key);
    if (cached) return cached;

    if (!ft_face_) return nullptr;

    set_font_size(font_size);

    FT_UInt glyph_index = FT_Get_Char_Index(ft_face_, codepoint);
    if (glyph_index == 0 && codepoint != 0) {
        if (codepoint == ' ' || codepoint == '\t') {
            GlyphEntry space_entry = {};
            space_entry.valid = true;
            space_entry.width = 0;
            space_entry.height = 0;
            space_entry.advance_x = (int16_t)(ft_face_->size->metrics.max_advance >> 6);
            if (codepoint == '\t') space_entry.advance_x *= 4;
            return cache_insert(key, space_entry);
        }
    }

    if (FT_Load_Glyph(ft_face_, glyph_index, FT_LOAD_RENDER)) {
        GlyphEntry invalid = {};
        invalid.valid = false;
        return cache_insert(key, invalid);
    }

    FT_GlyphSlot g = ft_face_->glyph;
    uint32_t gw = g->bitmap.width;
    uint32_t gh = g->bitmap.rows;

    GlyphEntry entry = {};
    entry.bearing_x = (int16_t)g->bitmap_left;
    entry.bearing_y = (int16_t)g->bitmap_top;
    entry.advance_x = (int16_t)(g->advance.x >> 6);
    entry.width = (uint16_t)gw;
    entry.height = (uint16_t)gh;
    entry.valid = true;

    if (gw == 0 || gh == 0) {
        entry.u0 = entry.v0 = entry.u1 = entry.v1 = 0.0f;
        return cache_insert(key, entry);
    }

    if (atlas_x_ + gw + 1 >= (uint32_t)ATLAS_SIZE) {
        atlas_x_ = 1;
        atlas_y_ += row_height_ + 1;
        row_height_ = 0;
    }

    if (atlas_y_ + gh + 1 >= (uint32_t)ATLAS_SIZE) {
        fprintf(stderr, "[Canvas] Glyph atlas full! Cannot fit codepoint U+%04X\n", codepoint);
        entry.valid = false;
        return cache_insert(key, entry);
    }

    glBindTexture(GL_TEXTURE_2D, texture_);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexSubImage2D(GL_TEXTURE_2D, 0,
                    (GLint)atlas_x_, (GLint)atlas_y_,
                    (GLsizei)gw, (GLsizei)gh,
                    GL_RED, GL_UNSIGNED_BYTE,
                    g->bitmap.buffer);
    glBindTexture(GL_TEXTURE_2D, 0);

    float inv = 1.0f / (float)ATLAS_SIZE;
    entry.u0 = (float)atlas_x_ * inv;
    entry.v0 = (float)atlas_y_ * inv;
    entry.u1 = (float)(atlas_x_ + gw) * inv;
    entry.v1 = (float)(atlas_y_ + gh) * inv;

    atlas_x_ += gw + 1;
    if (gh > row_height_) row_height_ = gh;

    return cache_insert(key, entry);
}

GlyphRun* GlyphAtlas::layout_text(FrameArena* arena, const char* text,
                                   uint16_t font_size, float x, float y,
                                   uint32_t run_id) {
    if (!text || !arena) return nullptr;

    uint32_t cp_count = 0;
    {
        const char* p = text;
        while (*p) { utf8_decode(&p); cp_count++; }
    }

    if (cp_count == 0) return nullptr;

    GlyphRun* run = arena->alloc_obj<GlyphRun>();
    run->id = run_id;
    run->glyph_count = 0;
    run->glyphs = arena->alloc_array<GlyphRunInstance>(cp_count);

    set_font_size(font_size);

    float pen_x = x;
    float pen_y = y;
    const char* p = text;

    while (*p) {
        uint32_t cp = utf8_decode(&p);
        GlyphEntry* ge = get_or_rasterize(cp, font_size);
        if (!ge || !ge->valid) continue;

        if (ge->width > 0 && ge->height > 0) {
            GlyphRunInstance& gi = run->glyphs[run->glyph_count];
            gi.x = pen_x + (float)ge->bearing_x;
            gi.y = pen_y - (float)ge->bearing_y;
            gi.w = (float)ge->width;
            gi.h = (float)ge->height;
            gi.u0 = ge->u0;
            gi.v0 = ge->v0;
            gi.u1 = ge->u1;
            gi.v1 = ge->v1;
            run->glyph_count++;
        }

        pen_x += (float)ge->advance_x;
    }

    return run;
}

void GlyphAtlas::measure(const char* text, uint16_t font_size,
                          float* out_width, float* out_height) {
    if (!text || !ft_face_) {
        if (out_width) *out_width = 0;
        if (out_height) *out_height = 0;
        return;
    }

    set_font_size(font_size);

    float width = 0;
    float max_ascent = 0;
    float max_descent = 0;
    const char* p = text;

    while (*p) {
        uint32_t cp = utf8_decode(&p);
        GlyphEntry* ge = get_or_rasterize(cp, font_size);
        if (!ge || !ge->valid) continue;

        width += (float)ge->advance_x;
        float ascent = (float)ge->bearing_y;
        float descent = (float)ge->height - (float)ge->bearing_y;
        if (ascent > max_ascent) max_ascent = ascent;
        if (descent > max_descent) max_descent = descent;
    }

    if (out_width) *out_width = width;
    if (out_height) *out_height = max_ascent + max_descent;
}

} 
