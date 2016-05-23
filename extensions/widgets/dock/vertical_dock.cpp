#include "vertical_dock.h"

vertical_dock::vertical_dock() {

  m_window = new cherry_kit::window();
  cherry_kit::fixed_layout *panel_ui = new cherry_kit::fixed_layout(m_window);
  float icon_size = 48.0f;

  m_window->set_window_type(cherry_kit::window::kPanelWindow);

  panel_ui->set_content_margin(0, 0, 5, 5);
  panel_ui->set_verticle_spacing(10.0f);
  panel_ui->set_geometry(0, 0, icon_size + 8, icon_size * 8);
  panel_ui->add_rows(7);

  m_window->on_window_discarded([=](cherry_kit::window *aWindow) {
    if (panel_ui)
      delete panel_ui;
    delete aWindow;
  });

  std::string default_height =
      std::to_string((icon_size / (icon_size * 7)) * 100) + "%";

  for (int i = 0; i < 7; i++) {
    panel_ui->add_segments(i, 1);
    panel_ui->set_row_height(i, default_height);
  }

  m_window->set_window_content(panel_ui->viewport());
  m_fixed_layout = panel_ui;
}

vertical_dock::~vertical_dock() {
  if (m_window)
    m_window->close();
}

cherry_kit::window *vertical_dock::window() { return m_window; }

void vertical_dock::create_dock_action(
    int row, int column, const std::string &icon,
    std::function<void()> a_button_action_func) {
  cherry_kit::widget_properties_t prop;
  cherry_kit::widget *ck_widget;
  prop["label"] = "";
  prop["icon"] = icon;
  ck_widget =
      m_fixed_layout->add_widget(row, column, "image_button", prop, [=]() {});

  ck_widget->on_click([=]() {
    if (a_button_action_func)
      a_button_action_func();
  });
}
