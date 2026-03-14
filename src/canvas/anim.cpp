#include "anim.hpp"

namespace plexy::ui {

void AnimationEngine::animate(float* target, float to, float duration,
                               float (*easing)(float), uint32_t widget_id) {
    for (int i = 0; i < MAX_ANIMATIONS; i++) {
        if (pool_[i].active && pool_[i].target == target) {
            pool_[i].from = *target;
            pool_[i].to = to;
            pool_[i].duration = duration;
            pool_[i].elapsed = 0;
            pool_[i].easing = easing ? easing : ease_out_cubic;
            pool_[i].widget_id = widget_id;
            return;
        }
    }

    for (int i = 0; i < MAX_ANIMATIONS; i++) {
        if (!pool_[i].active) {
            pool_[i].target = target;
            pool_[i].from = *target;
            pool_[i].to = to;
            pool_[i].duration = duration;
            pool_[i].elapsed = 0;
            pool_[i].easing = easing ? easing : ease_out_cubic;
            pool_[i].active = true;
            pool_[i].widget_id = widget_id;
            return;
        }
    }
    *target = to;
}

void AnimationEngine::cancel(float* target) {
    for (int i = 0; i < MAX_ANIMATIONS; i++) {
        if (pool_[i].active && pool_[i].target == target) {
            pool_[i].active = false;
        }
    }
}

bool AnimationEngine::tick(float dt) {
    bool any_active = false;
    dirty_count_ = 0;

    for (int i = 0; i < MAX_ANIMATIONS; i++) {
        Animation& a = pool_[i];
        if (!a.active) continue;

        a.elapsed += dt;
        float t = a.elapsed / a.duration;

        if (t >= 1.0f) {
            *a.target = a.to;
            a.active = false;
        } else {
            float eased = a.easing ? a.easing(t) : t;
            *a.target = a.from + (a.to - a.from) * eased;
            any_active = true;
        }

        if (a.widget_id != 0 && dirty_count_ < MAX_DIRTY) {
            bool found = false;
            for (uint32_t d = 0; d < dirty_count_; d++) {
                if (dirty_widgets_[d] == a.widget_id) { found = true; break; }
            }
            if (!found) dirty_widgets_[dirty_count_++] = a.widget_id;
        }
    }

    return any_active;
}

bool AnimationEngine::has_active() const {
    for (int i = 0; i < MAX_ANIMATIONS; i++) {
        if (pool_[i].active) return true;
    }
    return false;
}

} 
