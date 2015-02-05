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

#include "folderplugin.h"
#include <extensionmanager.h>
#include <widget.h>
#include <QDir>
#include <QDropEvent>
#include <QMimeData>
#include <QAction>

#define ADD_DIR "addDirectory"
#define CREATE_DIR "createDirectory"

DirectoryController::DirectoryController(QObject *object)
  : UI::ViewController(object)
{
  mThemePack = UI::Theme::instance();

  UI::Widget *parent = new UI::Widget();

  parent->setWindowFlag(UI::Widget::kRenderBackground);
  parent->setWindowFlag(UI::Widget::kConvertToWindowType);
  parent->setWindowFlag(UI::Widget::kRenderDropShadow);

  IconWidgetView *view = new IconWidgetView(parent);
  view->setPos(0.0, 64.0);

  view->setDirectoryPath(QDir::homePath());
  view->setController(this);
  parent->setLabelName("Home");

  QFileInfo info(
    QDir::toNativeSeparators(QDir::homePath() + QLatin1String("/Desktop/")));

  // parent->setLabelName(info.completeSuffix());
  mFolderViewList.append(parent);
}

DirectoryController::~DirectoryController() {}

void DirectoryController::init()
{
  QAction *_add_dir_action = new QAction(this);
  _add_dir_action->setText(tr("Folder"));

  _add_dir_action->setProperty("id", QVariant(1));
  _add_dir_action->setProperty("icon_name", "pd_add_folder_frame_icon.png");

  m_supported_action_list << _add_dir_action;
}

void DirectoryController::revokeSession(const QVariantMap &args) {}

UI::ActionList DirectoryController::actions() const
{
  return m_supported_action_list;
}

void DirectoryController::requestAction(const QString &actionName,
                                        const QVariantMap &args)
{
  if (actionName == CREATE_DIR) {
    qDebug() << "Not supported yet";
  } else if (actionName == tr("Folder")) {
    qDebug() << Q_FUNC_INFO << "Request Add DIR";
    UI::Widget *parent = new UI::Widget();

    parent->setWindowFlag(UI::Widget::kRenderBackground);
    parent->setWindowFlag(UI::Widget::kConvertToWindowType);
    parent->setWindowFlag(UI::Widget::kRenderDropShadow);

    IconWidgetView *view = new IconWidgetView(parent);
    view->setPos(0.0, 64.0);

    view->setDirectoryPath(args["path"].toString());
    view->setController(this);

    QFileInfo info(args["path"].toString());

    parent->setLabelName(info.baseName());

    mFolderViewList.append(parent);

    insert(parent);
  }
}

void DirectoryController::handleDropEvent(UI::Widget *widget,
    QDropEvent *event)
{
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
}

void DirectoryController::setViewRect(const QRectF &rect)
{
  Q_FOREACH(UI::Widget * view, mFolderViewList) {
    if (view) {
      view->setPos(rect.x(), rect.y());
    }
  }
}

QString DirectoryController::icon() const
{
  return QString("pd_add_folder_icon.png");
}
