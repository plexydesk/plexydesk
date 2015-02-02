#include "desktopmanager.h"

#include <QDebug>
#include <default_table_model.h>
#include <desktopactivitymenu.h>
#include <space.h>
#include <tableview.h>
#include <toolbar.h>

class DesktopManager::PrivateDesktopManager {
public:
  PrivateDesktopManager() {}

  ~PrivateDesktopManager() {}

  QMap<QString, QRectF> m_hotspot_map;
};

DesktopManager::DesktopManager(QWidget *parent)
    : PlexyDesk::WorkSpace(new QGraphicsScene, parent),
      d(new PrivateDesktopManager) {}

DesktopManager::~DesktopManager() { delete d; }

void DesktopManager::mouseReleaseEvent(QMouseEvent *event) {
  if (d->m_hotspot_map.keys().count() <= 0) {
  }

  if (event->button() == Qt::RightButton) {

    Space *_current_space = currentVisibleSpace();
    if (_current_space) {
      ControllerPtr _dock_controller = _current_space->controller("dockwidget");
      if (_dock_controller) {
        QVariantMap _data;
        QPointF _menu_pos = mapToScene(event->pos());

        _data["menu_pos"] = _menu_pos;
        qDebug() << Q_FUNC_INFO << "request Action:";
        _dock_controller->requestAction("Menu", _data);
      }
    } else {
      qWarning() << Q_FUNC_INFO
                 << "Error: Invalid Space found as Current Space";
    }

    return;
  }

  PlexyDesk::WorkSpace::mouseReleaseEvent(event);
}

PlexyDesk::ControllerPtr DesktopManager::getController() {
  Space *_current_space = currentVisibleSpace();
  if (_current_space) {
    ControllerPtr _dock_controller = _current_space->controller("dockwidget");
    if (_dock_controller) {
      return _dock_controller;
    }
  }

  return PlexyDesk::ControllerPtr();
}
