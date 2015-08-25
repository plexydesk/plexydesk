#include "ck_screen.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QRectF>

namespace cherry_kit {
class screen::private_screen {
public:
    private_screen() {}
    ~private_screen() {}
};

screen::screen() : priv(new private_screen)
{
}

screen::~screen(){
    delete priv;
}

int screen::screen_count() {
    return qApp->desktop()->screenCount();
}

float screen::virtual_desktop_width() {
  return 1920.0f;
}

float screen::virtual_desktop_height() {
    return 1080.0f;
}

}
