#include "desktopactivity.h"
#include <QDebug>
#include <QPropertyAnimation>
#include <QGraphicsScene>

#include "view_controller.h"
#include "desktop_viewport.h"

namespace PlexyDesk {

class DesktopActivity::PrivateDesktopActivity {
public:
  PrivateDesktopActivity() {}

  ~PrivateDesktopActivity() {
    if (m_controller_ptr)
      m_controller_ptr.clear();
  }

public:
  QRectF m_geometry;
  QVariantMap m_arguments;
  ControllerPtr m_controller_ptr;
  DesktopViewport *m_current_viewport;
};

DesktopActivity::DesktopActivity(QGraphicsObject *parent)
    : QObject(parent), d(new PrivateDesktopActivity) {
  d->m_current_viewport = 0;
}

DesktopActivity::~DesktopActivity() { delete d; }

void DesktopActivity::setActivityAttribute(const QString &name,
                                           const QVariant &data) {
  d->m_arguments[name] = data;
}

QVariantMap DesktopActivity::attributes() const { return d->m_arguments; }

void DesktopActivity::updateAttribute(const QString &name,
                                      const QVariant &data) {
  setActivityAttribute(name, data);
  Q_EMIT attributeChanged();
}

void DesktopActivity::setResult(DesktopActivity::ResultType type,
                                const QVariantMap &data) {}

QString DesktopActivity::getErrorMessage() const { return QString(); }

void DesktopActivity::setGeometry(const QRectF &geometry) {
  d->m_geometry = geometry;

  if (window()) {
    window()->setGeometry(geometry);
  }
}

QRectF DesktopActivity::geometry() const { return d->m_geometry; }

bool DesktopActivity::hasAttribute(const QString &arg) {
  return d->m_arguments.keys().contains(arg);
}

void DesktopActivity::exec(const QPointF &pos) {
  if (window()) {
    window()->setPos(pos);
  }
}

void DesktopActivity::showActivity() {
  if (window())
    window()->show();
}

void DesktopActivity::discardActivity() {
  hide();

  if (window()) {
    QGraphicsScene *_scene = window()->scene();

    if (_scene) {
      _scene->removeItem(window());
      cleanup();
    }
  }

  Q_EMIT discarded();
}

void DesktopActivity::hide() {
  if (window()) {
    window()->hide();
  }
}

void DesktopActivity::setController(const ControllerPtr &controller) {
  d->m_controller_ptr = controller;
}

ControllerPtr DesktopActivity::controller() const {
  return d->m_controller_ptr;
}

void DesktopActivity::setViewport(DesktopViewport *viewport) {
  if (d->m_current_viewport)
    return;

  d->m_current_viewport = viewport;
}

DesktopViewport *DesktopActivity::viewport() const {
  return d->m_current_viewport;
}

void DesktopActivity::updateAction() {
  if (!d->m_controller_ptr.data()) {
    qWarning() << Q_FUNC_INFO << "Error: Controller Not Set";
    return;
  }

  d->m_controller_ptr->requestAction(result()["action"].toString(), result());
}

// todo: remove this
void DesktopActivity::updateContentGeometry(Widget *widget) {
  if (!widget)
    return;

  widget->setGeometry(geometry());
}
}
