#include "ck_desktop_dialog.h"
#include <QDebug>
#include <QPropertyAnimation>
#include <QGraphicsScene>

#include "ck_desktop_controller_interface.h"
#include <ck_space.h>

#include <future>

namespace cherry_kit {

class desktop_dialog::PrivateDesktopActivity {
public:
  PrivateDesktopActivity() {}

  ~PrivateDesktopActivity() {
    if (m_controller_ptr) {
      m_controller_ptr.reset();
    }
  }

public:
  QRectF m_geometry;
  QVariantMap m_arguments;
  desktop_controller_ref m_controller_ptr;
  space *m_current_viewport;

  QList<std::function<void()> > m_arg_handler_list;
  QList<std::function<void(const QVariantMap &a_result)> >
  m_action_completed_list;
  std::vector<std::function<void(desktop_dialog *)> > m_discard_handler_list;
  std::future<void> m_async_notification_result;

  std::vector<dialog_message_t> m_notify_chain;
};

desktop_dialog::desktop_dialog(QObject *parent)
    : QObject(parent), priv(new PrivateDesktopActivity) {
  priv->m_current_viewport = 0;
}

desktop_dialog::~desktop_dialog() { delete priv; }

void desktop_dialog::show_activity() {
  if (dialog_window()) {
    dialog_window()->show();
  }
}

void desktop_dialog::notify_exit() {
  std::for_each(std::begin(priv->m_discard_handler_list),
                std::end(priv->m_discard_handler_list),
                [this](std::function<void(desktop_dialog *)> a_func) {

    if (a_func) {
      a_func(this);
    }
  });
}

void desktop_dialog::discard_activity() {
  hide();

  if (dialog_window()) {
    if (!purge()) {
      show_activity();
      return;
    }
  }

  notify_exit();
}

void desktop_dialog::hide() {
  if (dialog_window()) {
    dialog_window()->hide();
  }
}

void
desktop_dialog::set_controller(const desktop_controller_ref &a_controller) {
  priv->m_controller_ptr = a_controller;
}

desktop_controller_ref desktop_dialog::controller() const {
  return priv->m_controller_ptr;
}

void desktop_dialog::set_viewport(space *a_viewport_ptr) {
  if (priv->m_current_viewport) {
    return;
  }

  priv->m_current_viewport = a_viewport_ptr;
}

space *desktop_dialog::viewport() const { return priv->m_current_viewport; }

bool desktop_dialog::busy() { return false; }

void desktop_dialog::on_arguments_updated(std::function<void()> a_handler) {
  priv->m_arg_handler_list.append(a_handler);
}

void desktop_dialog::on_action_completed(
    std::function<void(const QVariantMap &)> a_handler) {
  priv->m_action_completed_list.append(a_handler);
}

void
desktop_dialog::on_discarded(std::function<void(desktop_dialog *)> a_handler) {
  priv->m_discard_handler_list.push_back(a_handler);
}

void desktop_dialog::on_notify(dialog_message_t callback) {
  priv->m_notify_chain.push_back(callback);
}

void desktop_dialog::notify_done() {}

void desktop_dialog::notify_message(const std::string &a_key,
                                    const std::string &a_value) {
  std::for_each(std::begin(priv->m_notify_chain),
                std::end(priv->m_notify_chain), [&](dialog_message_t a_func) {

    if (a_func)
      a_func(a_key, a_value);
  });
}
}
