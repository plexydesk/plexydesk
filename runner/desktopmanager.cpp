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

void DesktopManager::mouseMoveEvent(QMouseEvent *event) {
  /*
  Space *_current_space = currentVisibleSpace();

  if (_current_space) {
      QPointF _mouse_pos = event->pos();

      QRectF _left_hotspot_region(0.0, 0.0, 64.0, geometry().height());
      QRectF _right_hotspot_region((geometry().width() - 256.0), 0.0, 256.0,
  geometry().height());
      QRectF _top_hotspot_region(0.0, 0.0, geometry().width(), 64.0);
      QRectF _bottom_hotspot_region(0.0, geometry().height() - 64.0,
  geometry().width(), 64.0);

      if (_left_hotspot_region.contains(_mouse_pos)) {
          if (getController()) {
              getController()->requestAction("show-dock");
          }
      } else if (_right_hotspot_region.contains(_mouse_pos)) {
          qDebug() << Q_FUNC_INFO << "Right Corner";
          if (getController()) {
              getController()->requestAction("show-expose");
          }
      } else if (_top_hotspot_region.contains(_mouse_pos)) {
          qDebug() << Q_FUNC_INFO << "Top Corner";
      } else if (_bottom_hotspot_region.contains(_mouse_pos)) {
          qDebug() << Q_FUNC_INFO << "Bottom Corner";
      } else {
           if (getController()) {
              getController()->requestAction("hide-dock");
              getController()->requestAction("hide-expose");
          }
      }
  }
  */

  PlexyDesk::WorkSpace::mouseMoveEvent(event);
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
