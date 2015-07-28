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
      m_controller_ptr.clear();
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
  QList<std::function<void(const desktop_dialog *)> > m_discard_handler_list;
  std::future<void> m_async_notification_result;

  std::vector<dialog_message_t> m_notify_chain;
};

desktop_dialog::desktop_dialog(QGraphicsObject *parent)
    : QObject(parent), priv(new PrivateDesktopActivity) {
  priv->m_current_viewport = 0;
}

desktop_dialog::~desktop_dialog() { delete priv; }

void desktop_dialog::set_activity_attribute(const QString &a_name,
                                            const QVariant &a_data) {
  priv->m_arguments[a_name] = a_data;
}

QVariantMap desktop_dialog::attributes() const { return priv->m_arguments; }

void desktop_dialog::update_attribute(const QString &a_name,
                                      const QVariant &a_data) {
  set_activity_attribute(a_name, a_data);
  Q_EMIT attribute_changed();

  foreach(std::function<void()> l_handler, priv->m_arg_handler_list) {
    if (l_handler) {
      l_handler();
    }
  }
}

QString desktop_dialog::error_message() const { return QString(); }

void desktop_dialog::set_geometry(const QRectF &a_geometry) {
  priv->m_geometry = a_geometry;

  if (dialog_window()) {
    dialog_window()->setGeometry(a_geometry);
  }
}

QRectF desktop_dialog::geometry() const { return priv->m_geometry; }

bool desktop_dialog::has_attribute(const QString &a_arg) {
  return priv->m_arguments.keys().contains(a_arg);
}

void desktop_dialog::exec(const QPointF &a_pos) {
  if (dialog_window()) {
    dialog_window()->setPos(a_pos);
  }
}

void desktop_dialog::show_activity() {
  if (dialog_window()) {
    dialog_window()->show();
  }
}

void desktop_dialog::discard_activity() {
  foreach(std::function<void(const desktop_dialog *)> func,
          priv->m_discard_handler_list) {
    if (func) {
      func(this);
    }
  }

  hide();

  if (dialog_window()) {
    cleanup();
  }
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

void desktop_dialog::on_arguments_updated(std::function<void()> a_handler) {
  priv->m_arg_handler_list.append(a_handler);
}

void desktop_dialog::on_action_completed(
    std::function<void(const QVariantMap &)> a_handler) {
  priv->m_action_completed_list.append(a_handler);
}

void desktop_dialog::on_discarded(
    std::function<void(const desktop_dialog *)> a_handler) {
  priv->m_discard_handler_list.append(a_handler);
}

void desktop_dialog::on_notify(dialog_message_t callback) {
  priv->m_notify_chain.push_back(callback);
}

void desktop_dialog::update_action() {
  if (!priv->m_controller_ptr.data()) {
    qWarning() << Q_FUNC_INFO << "Error: Controller Not Set";
    return;
  }

  priv->m_controller_ptr->request_action(result()["action"].toString(),
                                         result());
}

// todo: remove this
void desktop_dialog::update_content_geometry(widget *a_widget_ptr) {
  if (!a_widget_ptr) {
    return;
  }

  a_widget_ptr->setGeometry(geometry());
}

void desktop_dialog::notify_done() {
  foreach(std::function<void(const QVariantMap & l_data)> l_func,
          priv->m_action_completed_list) {
    if (l_func) {
      l_func(result());
    }
  }

  priv->m_async_notification_result = std::async(std::launch::async, [this]() {
    if (dialog_window())
      dialog_window()->close();
  });
}

void desktop_dialog::notify_message(const std::string &a_key,
                                    const std::string &a_value) const{
  std::for_each(std::begin(priv->m_notify_chain),
                std::end(priv->m_notify_chain), [&](dialog_message_t a_func) {

    if (a_func)
      a_func(a_key, a_value);
  });
}
}
