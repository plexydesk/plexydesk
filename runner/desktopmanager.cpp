/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.org>
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

#include <space.h>
#include <workspace.h>

class DesktopManager::PrivateDesktopManager
{
public:
  PrivateDesktopManager() {}
  ~PrivateDesktopManager() {}
};

DesktopManager::DesktopManager(QWidget *parent)
  : UI::WorkSpace(new QGraphicsScene, parent),
    d(new PrivateDesktopManager) {}

DesktopManager::~DesktopManager() { delete d; }

void DesktopManager::mouseReleaseEvent(QMouseEvent *event)
{
  if (event->button() == Qt::RightButton) {
    if (currentVisibleSpace()) {
      ViewControllerPtr dock_controller =
        currentVisibleSpace()->controller("dockwidget");

      if (dock_controller) {
        QVariantMap menu_argument;
        menu_argument["menu_pos"] = mapToScene(event->pos());

        dock_controller->requestAction(tr("Menu"), menu_argument);
      }
    }
  }

  UI::WorkSpace::mouseReleaseEvent(event);
}
