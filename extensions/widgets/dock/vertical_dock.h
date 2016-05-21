#ifndef VERTICAL_DOCK_H
#define VERTICAL_DOCK_H

#include <ck_fixed_layout.h>
#include <ck_ui_action.h>
#include <ck_widget.h>
#include <ck_window.h>

class vertical_dock {
public:
  vertical_dock();
  virtual ~vertical_dock();

  cherry_kit::window *window();

  void create_dock_action(int row, int column, const cherry_kit::string &icon,
                          std::function<void()> a_button_action_func);

private:
  cherry_kit::window *m_window;
  cherry_kit::fixed_layout *m_fixed_layout;
};

#endif // VERTICAL_DOCK_H
