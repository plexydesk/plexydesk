/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.com>
*  Authored By  :
*
*  PlexyDesk is free software: you can redistribute it and/or modify
*  it under the terms of the GNU Lesser General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  PlexyDesk is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU Lesser General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with PlexyDesk. If not, see <http://www.gnu.org/licenses/lgpl.html>
*******************************************************************************/

#include "desktopmanager.h"

#include <QDebug>

#include <ck_space.h>
#include <ck_workspace.h>

class DesktopManager::PrivateDesktopManager {
public:
  PrivateDesktopManager() : m_scene(0) {}
  ~PrivateDesktopManager() {}

  QGraphicsScene *m_scene;
};

DesktopManager::DesktopManager(QWidget *parent)
    : cherry_kit::workspace(0, parent),
      priv(new PrivateDesktopManager) {
    priv->m_scene = new QGraphicsScene(this);
    priv->m_scene->setItemIndexMethod(QGraphicsScene::NoIndex);

    setScene(priv->m_scene);
}

DesktopManager::~DesktopManager() { delete priv; }

void DesktopManager::mouseReleaseEvent(QMouseEvent *event) {
  if (event->button() == Qt::RightButton) {
    if (current_active_space()) {
      desktop_controller_ref dock_controller =
          current_active_space()->controller("dockwidget");
      if (dock_controller) {
        QPointF pos = mapToScene(event->pos());

        cherry_kit::ui_task_data_t data;
        data["x"] = std::to_string(pos.x());
        data["y"] = std::to_string(pos.y());

        dock_controller->task().execute("menu", data);
      }
    }
  }

  cherry_kit::workspace::mouseReleaseEvent(event);
}
