#ifndef PRIVATE_SCREEN_H
#define PRIVATE_SCREEN_H

#include <ck_screen.h>

namespace cherry_kit {
class screen::platform_screen {
public:
    platform_screen();
    virtual ~platform_screen();

    int screen_count() const;

    float scale_factor(int a_id) const;

    float desktop_width() const;
    float desktop_height() const;

    float display_width(int a_id) const;
    float display_height(int a_id) const;
};
}

#endif // PRIVATE_SCREEN_H
