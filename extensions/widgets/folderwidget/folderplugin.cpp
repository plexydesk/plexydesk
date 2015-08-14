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

#include "folderplugin.h"
#include <ck_extension_manager.h>
#include <ck_widget.h>
#include <QDir>
#include <QDropEvent>
#include <QMimeData>

#define ADD_DIR "addDirectory"
#define CREATE_DIR "createDirectory"

#include <functional>

DirectoryController::DirectoryController(QObject *object)
    : cherry_kit::ViewController(object) {
  /*
  mThemePack = cherry_kit::Theme::instance();

  cherry_kit::Widget *parent = new cherry_kit::Widget();

  IconWidgetView *view = new IconWidgetView(parent);
  view->setPos(0.0, 64.0);

  view->setDirectoryPath(QDir::homePath());
  view->set_controller(this);
  parent->set_widget_name("Home");

  QFileInfo info(
      QDir::toNativeSeparators(QDir::homePath() + QLatin1String("/Desktop/")));

  // parent->setLabelName(info.completeSuffix());
  mFolderViewList.append(parent);
  */
}

DirectoryController::~DirectoryController() {}

void DirectoryController::init() {}

void DirectoryController::session_data_available(
    const cherry::sync_object &a_session_root) {}

void DirectoryController::submit_session_data(cherry::sync_object *a_obj) {}

void DirectoryController::handle_drop_event(cherry_kit::Widget *widget,
                                            QDropEvent *event) {
  /*
  const QString droppedFile = event->mimeData()->urls().value(0).toLocalFile();
  QFileInfo fileInfo(droppedFile);

  if (fileInfo.isDir()) {
    QVariant path = droppedFile;
    QVariantMap args;
    args["path"] = path;
    IconWidgetView *view = qobject_cast<IconWidgetView *>(widget);
    if (view) {
      view->setDirectoryPath(droppedFile);
    }
  }
  */
}

void DirectoryController::set_view_rect(const QRectF &rect) {
  /*
  Q_FOREACH(cherry_kit::Widget * view, mFolderViewList) {
    if (view) {
      view->setPos(rect.x(), rect.y());
    }
  }
  */
}

QString DirectoryController::icon() const {
  return QString("pd_add_folder_icon.png");
}
