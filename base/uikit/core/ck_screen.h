#ifndef SCREEN_H
#define SCREEN_H

#include <functional>
#include <vector>
#include <plexydesk_ui_exports.h>

namespace cherry_kit {
typedef enum {
  kScreenCountChange,
  kScreenGeomentyChange,
  kScreenStructureChange,
  kScreenErrorChange
} display_change_notify_t;

typedef std::function<void(display_change_notify_t)>
    display_change_notify_callback_t;

class DECL_UI_KIT_EXPORT screen {
public:
  screen();
  virtual ~screen();

  int screen_count() const;

  float scale_factor(int a_id) const;

  int x_resolution(int a_id) const;
  int y_resolution(int a_id) const;

  void change_notifications(display_change_notify_callback_t a_callback);

protected:
  virtual float virtual_desktop_width();
  virtual float virtual_desktop_height();

private:
  class platform_screen;
  platform_screen* const priv;

  std::vector<display_change_notify_callback_t> m_notify_chain;
};
}

#endif // SCREEN_H
