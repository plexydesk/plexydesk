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
};

desktop_dialog::desktop_dialog(QGraphicsObject *parent)
    : QObject(parent), o_desktop_dialog(new PrivateDesktopActivity) {
  o_desktop_dialog->m_current_viewport = 0;
}

desktop_dialog::~desktop_dialog() { delete o_desktop_dialog; }

void desktop_dialog::set_activity_attribute(const QString &a_name,
                                            const QVariant &a_data) {
  o_desktop_dialog->m_arguments[a_name] = a_data;
}

QVariantMap desktop_dialog::attributes() const {
  return o_desktop_dialog->m_arguments;
}

void desktop_dialog::update_attribute(const QString &a_name,
                                      const QVariant &a_data) {
  set_activity_attribute(a_name, a_data);
  Q_EMIT attribute_changed();

  foreach(std::function<void()> l_handler,
          o_desktop_dialog->m_arg_handler_list) {
    if (l_handler) {
      l_handler();
    }
  }
}

QString desktop_dialog::error_message() const { return QString(); }

void desktop_dialog::set_geometry(const QRectF &a_geometry) {
  o_desktop_dialog->m_geometry = a_geometry;

  if (activity_window()) {
    activity_window()->setGeometry(a_geometry);
  }
}

QRectF desktop_dialog::geometry() const { return o_desktop_dialog->m_geometry; }

bool desktop_dialog::has_attribute(const QString &a_arg) {
  return o_desktop_dialog->m_arguments.keys().contains(a_arg);
}

void desktop_dialog::exec(const QPointF &a_pos) {
  if (activity_window()) {
    activity_window()->setPos(a_pos);
  }
}

void desktop_dialog::show_activity() {
  if (activity_window()) {
    activity_window()->show();
  }
}

void desktop_dialog::discard_activity() {
  foreach(std::function<void(const desktop_dialog *)> func,
          o_desktop_dialog->m_discard_handler_list) {
    if (func) {
      func(this);
    }
  }

  hide();

  if (activity_window()) {
    cleanup();
  }
}

void desktop_dialog::hide() {
  if (activity_window()) {
    activity_window()->hide();
  }
}

void
desktop_dialog::set_controller(const desktop_controller_ref &a_controller) {
  o_desktop_dialog->m_controller_ptr = a_controller;
}

desktop_controller_ref desktop_dialog::controller() const {
  return o_desktop_dialog->m_controller_ptr;
}

void desktop_dialog::set_viewport(space *a_viewport_ptr) {
  if (o_desktop_dialog->m_current_viewport) {
    return;
  }

  o_desktop_dialog->m_current_viewport = a_viewport_ptr;
}

space *desktop_dialog::viewport() const {
  return o_desktop_dialog->m_current_viewport;
}

void desktop_dialog::on_arguments_updated(std::function<void()> a_handler) {
  o_desktop_dialog->m_arg_handler_list.append(a_handler);
}

void desktop_dialog::on_action_completed(
    std::function<void(const QVariantMap &)> a_handler) {
  o_desktop_dialog->m_action_completed_list.append(a_handler);
}

void desktop_dialog::on_discarded(
    std::function<void(const desktop_dialog *)> a_handler) {
  o_desktop_dialog->m_discard_handler_list.append(a_handler);
}

void desktop_dialog::update_action() {
  if (!o_desktop_dialog->m_controller_ptr.data()) {
    qWarning() << Q_FUNC_INFO << "Error: Controller Not Set";
    return;
  }

  o_desktop_dialog->m_controller_ptr->request_action(
      result()["action"].toString(), result());
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
          o_desktop_dialog->m_action_completed_list) {
    if (l_func) {
      l_func(result());
    }
  }

  o_desktop_dialog->m_async_notification_result =
      std::async(std::launch::async, [this]() {
        if (activity_window())
          activity_window()->close();
      });
}
}
