#pragma once

#include <GL/glew.h>
#include "types.hpp"
#include "arena.hpp"
#include "render.hpp"
#include <ft2build.h>
#include FT_FREETYPE_H

namespace plexy::ui {

struct GlyphEntry {
    float u0, v0, u1, v1;   
    int16_t bearing_x, bearing_y;
    int16_t advance_x;
    uint16_t width, height;
    bool valid;
};

class GlyphAtlas {
public:
    void init(const char* font_path, int default_size);
    void shutdown();

    GlyphEntry* get_or_rasterize(uint32_t codepoint, uint16_t font_size);

    GlyphRun* layout_text(FrameArena* arena, const char* text,
                          uint16_t font_size, float x, float y,
                          uint32_t run_id);

    void measure(const char* text, uint16_t font_size,
                 float* out_width, float* out_height);

    GLuint texture() const { return texture_; }

private:
    GLuint texture_ = 0;
    static constexpr int ATLAS_SIZE = 1024;
    uint32_t atlas_x_ = 1;
    uint32_t atlas_y_ = 1;
    uint32_t row_height_ = 0;

    FT_Library ft_lib_ = nullptr;
    FT_Face ft_face_ = nullptr;

    static constexpr uint32_t CACHE_SIZE = 4096;
    struct CacheSlot {
        uint64_t key;
        GlyphEntry glyph;
        bool occupied;
    };
    CacheSlot cache_[CACHE_SIZE] = {};

    uint32_t hash_key(uint64_t key) const {
        key ^= key >> 33;
        key *= 0xff51afd7ed558ccdULL;
        key ^= key >> 33;
        return (uint32_t)(key & (CACHE_SIZE - 1));
    }

    GlyphEntry* cache_find(uint64_t key);
    GlyphEntry* cache_insert(uint64_t key, const GlyphEntry& entry);

    void set_font_size(uint16_t size);
    uint16_t current_size_ = 0;
};

} 
