#ifndef DESKTOP_BUTTON_H
#define DESKTOP_BUTTON_H

#include <ck_window.h>
#include <ck_widget.h>
#include <ck_icon_button.h>

class desktop_button
{
public:
    desktop_button();
    ~desktop_button();

    void set_icon(const std::string &a_icon_name);

    void set_label(const std::string &a_text_label);

    cherry_kit::window *window();

    void set_action(std::function<void()> a_func);
private:
   cherry_kit::window *m_window;
   cherry_kit::icon_button *m_button;
};

#endif // DESKTOP_BUTTON_H
