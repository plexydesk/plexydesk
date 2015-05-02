#include "desktopactivity.h"
#include <QDebug>
#include <QPropertyAnimation>
#include <QGraphicsScene>

#include "view_controller.h"
#include <space.h>

namespace UIKit {

class DesktopActivity::PrivateDesktopActivity {
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
  ViewControllerPtr m_controller_ptr;
  Space *m_current_viewport;

  QList<std::function<void()> > m_arg_handler_list;
  QList<std::function<void(const QVariantMap &a_result)> >
  m_action_completed_list;
  QList<std::function<void(const DesktopActivity *)> > m_discard_handler_list;
};

DesktopActivity::DesktopActivity(QGraphicsObject *parent)
    : QObject(parent), d(new PrivateDesktopActivity) {
  d->m_current_viewport = 0;
}

DesktopActivity::~DesktopActivity() { delete d; }

void DesktopActivity::set_activity_attribute(const QString &a_name,
                                             const QVariant &a_data) {
  d->m_arguments[a_name] = a_data;
}

QVariantMap DesktopActivity::attributes() const { return d->m_arguments; }

void DesktopActivity::update_attribute(const QString &a_name,
                                       const QVariant &a_data) {
  set_activity_attribute(a_name, a_data);
  Q_EMIT attribute_changed();

  foreach(std::function<void()> l_handler, d->m_arg_handler_list) {
    if (l_handler) {
      l_handler();
    }
  }
}

QString DesktopActivity::error_message() const { return QString(); }

void DesktopActivity::set_geometry(const QRectF &a_geometry) {
  d->m_geometry = a_geometry;

  if (window()) {
    window()->setGeometry(a_geometry);
  }
}

QRectF DesktopActivity::geometry() const { return d->m_geometry; }

bool DesktopActivity::has_attribute(const QString &a_arg) {
  return d->m_arguments.keys().contains(a_arg);
}

void DesktopActivity::exec(const QPointF &a_pos) {
  if (window()) {
    window()->setPos(a_pos);
  }
}

void DesktopActivity::show_activity() {
  if (window()) {
    window()->show();
  }
}

void DesktopActivity::discard_activity() {
  foreach(std::function<void(const DesktopActivity *)> func,
          d->m_discard_handler_list) {
    if (func) {
      func(this);
    }
  }

  hide();
  if (window()) {
    cleanup();
  }
}

void DesktopActivity::hide() {
  if (window()) {
    window()->hide();
  }
}

void DesktopActivity::set_controller(const ViewControllerPtr &a_controller) {
  d->m_controller_ptr = a_controller;
}

ViewControllerPtr DesktopActivity::controller() const {
  return d->m_controller_ptr;
}

void DesktopActivity::set_viewport(Space *a_viewport_ptr) {
  if (d->m_current_viewport) {
    return;
  }

  d->m_current_viewport = a_viewport_ptr;
}

Space *DesktopActivity::viewport() const { return d->m_current_viewport; }

void DesktopActivity::on_arguments_updated(std::function<void()> a_handler) {
  d->m_arg_handler_list.append(a_handler);
}

void DesktopActivity::on_action_completed(
    std::function<void(const QVariantMap &)> a_handler) {
  d->m_action_completed_list.append(a_handler);
}

void DesktopActivity::on_discarded(
    std::function<void(const DesktopActivity *)> a_handler) {
  d->m_discard_handler_list.append(a_handler);
}

void DesktopActivity::update_action() {
  if (!d->m_controller_ptr.data()) {
    qWarning() << Q_FUNC_INFO << "Error: Controller Not Set";
    return;
  }

  d->m_controller_ptr->request_action(result()["action"].toString(), result());
}

// todo: remove this
void DesktopActivity::update_content_geometry(Widget *a_widget_ptr) {
  if (!a_widget_ptr) {
    return;
  }

  a_widget_ptr->setGeometry(geometry());
}

void DesktopActivity::activate_response() {
  foreach(std::function<void(const QVariantMap & l_data)> l_func,
          d->m_action_completed_list) {
    if (l_func) {
      l_func(result());
    }
  }
}
}
