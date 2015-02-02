#include "desktopmanager.h"

#include <QDebug>

#include <desktop_viewport.h>
#include <space.h>
#include <workspace.h>

class DesktopManager::PrivateDesktopManager {
public:
  PrivateDesktopManager() {}
  ~PrivateDesktopManager() {}
};

DesktopManager::DesktopManager(QWidget *parent)
    : PlexyDesk::WorkSpace(new QGraphicsScene, parent),
      d(new PrivateDesktopManager) {}

DesktopManager::~DesktopManager() { delete d; }

void DesktopManager::mouseReleaseEvent(QMouseEvent *event) {
  if (event->button() == Qt::RightButton) {
    if (currentVisibleSpace()) {
      ControllerPtr dock_controller =
              currentVisibleSpace()->controller("dockwidget");

      if (dock_controller) {
        QVariantMap menu_argument;
        menu_argument["menu_pos"] = mapToScene(event->pos());

        dock_controller->requestAction(tr("Menu"), menu_argument);
      }
    }
  }

  PlexyDesk::WorkSpace::mouseReleaseEvent(event);
}
