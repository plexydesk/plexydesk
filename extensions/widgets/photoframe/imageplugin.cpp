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

#include "imageplugin.h"
#include <extensionmanager.h>
#include <widget.h>
#include <space.h>

// qt
#include <QGraphicsScene>
#include <QDropEvent>
#include <QMimeData>
#include <QFileInfo>
#include <QAction>

PhotoFrameController::PhotoFrameController(QObject * /*object*/)
    : mFrameParentitem(0) {}

PhotoFrameController::~PhotoFrameController() {
  mPhotoList.clear();
  qDeleteAll(mPhotoList);
}

void PhotoFrameController::init() {
  QAction *_add_dir_action = new QAction(this);
  _add_dir_action->setText(tr("Photo"));

  _add_dir_action->setProperty("id", QVariant(1));
  _add_dir_action->setProperty("icon_name", "pd_photo_icon.png");

  m_supported_action_list << _add_dir_action;
}

void PhotoFrameController::revoke_session(const QVariantMap &args) {
  QStringList photoList = args["photos"].toString().split(",");

  if (args["photos"].toString().isEmpty()) {
    return;
  }

  if (mFrameParentitem && !mFrameParentitem->validPhotoFrame()) {
    delete mFrameParentitem;
    mFrameParentitem = 0;
  }

  foreach(const QString & str, photoList) {
    CherryKit::Window *window = new CherryKit::Window();
    PhotoWidget *photoWidget = new PhotoWidget();
    window->set_window_content(photoWidget);

    photoWidget->set_controller(this);
    photoWidget->set_widget_name("Photo");
    mPhotoList.append(photoWidget);
    photoWidget->setPhotoURL(str);

    m_current_url_list << str;

    QFileInfo info(str);
    photoWidget->set_widget_name(info.baseName());

    QPixmap image(str);

    if (!image.isNull()) {
      photoWidget->setContentImage(image);
    }

    insert(window);
  }
}

void PhotoFrameController::session_data_available(
    const cherry::sync_object &a_session_root) {}

void PhotoFrameController::submit_session_data(cherry::sync_object *a_obj) {}

void PhotoFrameController::handle_drop_event(CherryKit::Widget *widget,
                                             QDropEvent *event) {
  if (event->mimeData()->urls().count() >= 0) {
    const QString droppedFile =
        event->mimeData()->urls().value(0).toLocalFile();

    QFileInfo info(droppedFile);
    QPixmap droppedPixmap(droppedFile);
    PhotoWidget *handler = qobject_cast<PhotoWidget *>(widget);

    if (!info.isDir() && !droppedPixmap.isNull() && handler) {
      handler->setContentImage(droppedPixmap);
      handler->setPhotoURL(droppedFile);
      handler->set_widget_name(info.baseName());

      if (!m_current_url_list.contains(droppedFile)) {
        m_current_url_list << droppedFile;
      }

      if (viewport()) {
        CherryKit::Space *view = viewport();
        if (view) {
          view->update_session_value(controller_name(), "photos",
                                     m_current_url_list.join(","));
        }
      } else {
        qDebug() << Q_FUNC_INFO << "Saving session Failed";
      }
    }
  }
}

void PhotoFrameController::set_view_rect(const QRectF &rect) {
  if (mFrameParentitem) {
    mFrameParentitem->setPos(rect.x(), rect.y());
  }
}

bool PhotoFrameController::remove_widget(CherryKit::Widget *widget) {
  if (!widget) {
    return 1;
  }

  QStringList rv;

  PhotoWidget *_widget_to_delete = qobject_cast<PhotoWidget *>(widget);

  if (!_widget_to_delete) {
    return false;
  }

  m_current_url_list.removeOne(_widget_to_delete->photoURL());

  mPhotoList.removeAll(_widget_to_delete);

  if (viewport())
    viewport()->update_session_value(controller_name(), "photos",
                                     m_current_url_list.join(","));
  if (widget) {
    if (widget->scene()) {
      widget->scene()->removeItem(widget);
      delete widget;
      widget = 0;
    }
  }

  return 1;
}

CherryKit::ActionList PhotoFrameController::actions() const {
  return m_supported_action_list;
}

void PhotoFrameController::request_action(const QString &actionName,
                                          const QVariantMap &args) {
  if (actionName == tr("Photo")) {
    CherryKit::Window *window = new CherryKit::Window();
    PhotoWidget *photoWidget = new PhotoWidget();

    window->set_window_content(photoWidget);

    photoWidget->set_controller(this);
    photoWidget->set_widget_name("Photo");
    mPhotoList.append(photoWidget);
    photoWidget->setPhotoURL(QString("ID:%1").arg(mPhotoList.count()));

    insert(window);
  }
}

QString PhotoFrameController::icon() const {
  return QString("pd_image_icon.png");
}

void PhotoFrameController::prepare_removal() {
  foreach(PhotoWidget * _widget, mPhotoList) { this->remove_widget(_widget); }
}
