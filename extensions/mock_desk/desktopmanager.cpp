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

#include <QPushButton>
#include <ck_space.h>
#include <ck_workspace.h>
#include <ck_text_editor.h>
#include <ck_text_view.h>

#if defined (MAC_OS_X_VERSION_MIN_REQUIRED)
#include <carbon/carbon.h>
#endif //defined (MAC_OS_X_VERSION_MIN_REQUIRED)

class DesktopManager::PrivateDesktopManager {
public:
  PrivateDesktopManager() : m_scene(0) {}
  ~PrivateDesktopManager() {
     if (m_scene)
         delete m_scene;
  }

  QGraphicsScene *m_scene;
};

DesktopManager::DesktopManager(QWidget *parent)
    : cherry_kit::workspace(0, parent),
      priv(new PrivateDesktopManager) {

   priv->m_scene = new QGraphicsScene(this);
   setScene(priv->m_scene);
   //setWindowFlags(Qt::Window);
   move_to_screen(0);

#if defined (MAC_OS_X_VERSION_MIN_REQUIRED)
#if MAC_OS_X_VERSION_MIN_REQUIRED <= MAC_OS_X_VERSION_10_4
      HIViewRef _desktopView = reinterpret_cast<HIViewRef>(this->winId());
      HIWindowRef _window = HIViewGetWindow(_desktopView);

      if (_window) {
         ChangeWindowAttributes(_window, kWindowNoShadowAttribute, kWindowNoAttributes);  
         //SetWindowGroupLevel(GetWindowGroup(_window), kCGDesktopIconWindowLevel);
      } 
#endif //MAC_OS_X_VERSION_MIN_REQUIRED <= MAC_OS_X_VERSION_10_4
#endif //defined(MAC_OS_X_VERSION_MIN_REQUIRED)
}

DesktopManager::~DesktopManager() { delete priv; }

void DesktopManager::add_sample_text()
{
  cherry_kit::window *window = new cherry_kit::window();

  scene()->addItem(window);
  window->set_window_title("Note");
  cherry_kit::text_view *edit = new cherry_kit::text_view();
  cherry_kit::widget *widget = new cherry_kit::widget();
  widget->set_geometry(QRectF(0, 0, 640, 480));
  edit->set_geometry(QRectF(0, 0, 640, 480));
  window->set_window_content(edit);
  //window->setScale(1.2);

  window->show();

  /* load data */
  QFile file("/Users/siraj//data.txt");
  if(!file.open(QIODevice::ReadOnly)) {
      return;
  }

  QTextStream in(&file);
  std::string data;

  while(!in.atEnd()) {
      QString line = in.readLine();
      data += line.toStdString() + '\n';
  }

  file.close();

  edit->set_text(data);

  show();
}

void DesktopManager::mouseReleaseEvent(QMouseEvent *event) {
  if (event->button() == Qt::RightButton) {
    if (current_active_space()) {
      desktop_controller_ref dock_controller =
          current_active_space()->controller("dockwidget");
      if (dock_controller) {
        QVariantMap menu_argument;
        menu_argument["menu_pos"] = mapToScene(event->pos());

        dock_controller->task().execute();
      }
    }
  }

  cherry_kit::workspace::mouseReleaseEvent(event);
}
