#include "desktop_button.h"
#include <ck_resource_manager.h>
#include <ck_screen.h>

desktop_button::desktop_button() {
  m_window = new cherry_kit::window();
  m_button = new cherry_kit::icon_button(m_window);

  float button_size = 38 * cherry_kit::screen::get()->scale_factor(0);
  m_button->set_geometry(QRectF(0, 0, button_size, button_size));
  m_button->setMinimumSize(QSizeF(button_size, button_size));
  m_button->set_size(QSize(button_size, button_size));

  m_window->set_window_type(cherry_kit::window::kPanelWindow);
  m_window->set_window_opacity(0.0);
}

desktop_button::~desktop_button() {
  if (m_window)
    m_window->close();
}

void desktop_button::set_icon(const std::string &a_icon_name) {
  if (!m_button)
    return;

  QPixmap pixmap = cherry_kit::resource_manager::instance()->drawable(
      a_icon_name.c_str(), "mdpi");
  m_button->set_pixmap(pixmap);
}

void desktop_button::set_label(const std::string &a_text_label) {
  if (!m_button)
    return;

 // m_button->set_lable(a_text_label.c_str());
  m_window->set_window_content(m_button);
}

cherry_kit::window *desktop_button::window() { return m_window; }

void desktop_button::set_action(std::function<void()> a_func) {
  m_button->on_click([=]() {
    if (a_func)
      a_func();
  });
}
