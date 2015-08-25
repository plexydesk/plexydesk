#ifndef SCREEN_H
#define SCREEN_H

namespace cherry_kit {
class screen
{
public:
    screen();
    virtual ~screen();

    static int screen_count();

protected:
    virtual float virtual_desktop_width();
    virtual float virtual_desktop_height();

private:
    class private_screen;
    private_screen * const priv;
};

}

#endif // SCREEN_H
