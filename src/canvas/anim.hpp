#pragma once

#include <cstdint>
#include <cmath>

namespace plexy::ui {



inline float ease_linear(float t) { return t; }

inline float ease_out_cubic(float t) {
    float f = t - 1.0f;
    return f * f * f + 1.0f;
}

inline float ease_in_cubic(float t) { return t * t * t; }

inline float ease_in_out_cubic(float t) {
    return t < 0.5f ? 4.0f * t * t * t : 1.0f - 0.5f * (-2.0f * t + 2.0f) * (-2.0f * t + 2.0f) * (-2.0f * t + 2.0f);
}

inline float ease_out_quart(float t) {
    float f = t - 1.0f;
    float f2 = f * f;
    return 1.0f - f2 * f2;
}

inline float ease_spring(float t, float stiffness = 10.0f, float damping = 0.7f) {
    float decay = expf(-damping * t * stiffness);
    return 1.0f - decay * cosf(stiffness * t * (1.0f - damping));
}



struct Animation {
    float* target = nullptr;
    float from = 0;
    float to = 0;
    float duration = 0;
    float elapsed = 0;
    float (*easing)(float t) = ease_out_cubic;
    bool active = false;
    uint32_t widget_id = 0;
};



class AnimationEngine {
public:
    void animate(float* target, float to, float duration,
                 float (*easing)(float) = ease_out_cubic,
                 uint32_t widget_id = 0);
    void cancel(float* target);
    bool tick(float dt);
    bool has_active() const;

    const uint32_t* dirty_widgets() const { return dirty_widgets_; }
    uint32_t dirty_widget_count() const { return dirty_count_; }

private:
    static constexpr int MAX_ANIMATIONS = 128;
    Animation pool_[MAX_ANIMATIONS];
    static constexpr int MAX_DIRTY = 64;
    uint32_t dirty_widgets_[MAX_DIRTY];
    uint32_t dirty_count_ = 0;
};

} 
