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

#include "imageplugin.h"
#include <extensionmanager.h>
#include <desktopwidget.h>
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
  _add_dir_action->setProperty("icon_name", "pd_photo_frame_icon.png");

  m_supported_action_list << _add_dir_action;
}

void PhotoFrameController::revokeSession(const QVariantMap &args) {
  QStringList photoList = args["photos"].toString().split(",");

  if (args["photos"].toString().isEmpty())
    return;

  if (mFrameParentitem && !mFrameParentitem->validPhotoFrame()) {
    delete mFrameParentitem;
    mFrameParentitem = 0;
  }

  foreach(const QString & str, photoList) {
    PhotoWidget *photoWidget = new PhotoWidget();
    photoWidget->setWindowFlag(PlexyDesk::UIWidget::kRenderDropShadow, true);
    photoWidget->setWindowFlag(PlexyDesk::UIWidget::kRenderBackground, true);
    photoWidget->setWindowFlag(PlexyDesk::UIWidget::kTopLevelWindow);
    photoWidget->setWindowFlag(PlexyDesk::UIWidget::kConvertToWindowType, true);
    photoWidget->setController(this);
    photoWidget->setLabelName("Photo");
    mPhotoList.append(photoWidget);
    photoWidget->setPhotoURL(str);

    m_current_url_list << str;

    QFileInfo info(str);
    photoWidget->setLabelName(info.baseName());

    QPixmap image(str);

    if (!image.isNull()) {
      photoWidget->setContentImage(image);
    }

    Q_EMIT addWidget(photoWidget);
  }
}

void PhotoFrameController::handleDropEvent(PlexyDesk::Widget *widget,
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
      handler->setLabelName(info.baseName());

      if (!m_current_url_list.contains(droppedFile))
        m_current_url_list << droppedFile;

      if (viewport()) {
        PlexyDesk::Space *view = qobject_cast<PlexyDesk::Space *>(viewport());
        if (view) {
          view->updateSessionValue(controllerName(), "photos",
                                   m_current_url_list.join(","));
        }
      } else
        qDebug() << Q_FUNC_INFO << "Saving session Failed";
    }
  }
}

void PhotoFrameController::setViewRect(const QRectF &rect) {
  if (mFrameParentitem)
    mFrameParentitem->setPos(rect.x(), rect.y());
}

bool PhotoFrameController::removeWidget(PlexyDesk::Widget *widget) {
  if (!widget)
    return 1;

  QStringList rv;

  PhotoWidget *_widget_to_delete = qobject_cast<PhotoWidget *>(widget);

  if (!_widget_to_delete)
    return false;

  m_current_url_list.removeOne(_widget_to_delete->photoURL());

  mPhotoList.removeAll(_widget_to_delete);

  if (viewport()) {
    PlexyDesk::DesktopViewport *view =
        qobject_cast<PlexyDesk::DesktopViewport *>(viewport());
    if (view)
      view->updateSessionValue(controllerName(), "photos",
                               m_current_url_list.join(","));
  }

  if (widget) {
    if (widget->scene()) {
      widget->scene()->removeItem(widget);
      delete widget;
      widget = 0;
    }
  }

  return 1;
}

PlexyDesk::ActionList PhotoFrameController::actions() const {
  return m_supported_action_list;
}

void PhotoFrameController::requestAction(const QString &actionName,
                                         const QVariantMap &args) {
  if (actionName == tr("Photo")) {
    PhotoWidget *photoWidget = new PhotoWidget();
    photoWidget->setWindowFlag(PlexyDesk::UIWidget::kRenderDropShadow, true);
    photoWidget->setWindowFlag(PlexyDesk::UIWidget::kRenderBackground, true);
    photoWidget->setWindowFlag(PlexyDesk::UIWidget::kTopLevelWindow);
    photoWidget->setWindowFlag(PlexyDesk::UIWidget::kConvertToWindowType, true);
    photoWidget->setController(this);
    photoWidget->setLabelName("Photo");
    mPhotoList.append(photoWidget);
    photoWidget->setPhotoURL(QString("ID:%1").arg(mPhotoList.count()));

    Q_EMIT addWidget(photoWidget);
  }
}

QString PhotoFrameController::icon() const {
  return QString("pd_image_icon.png");
}

void PhotoFrameController::prepareRemoval() {
  foreach(PhotoWidget * _widget, mPhotoList) { this->removeWidget(_widget); }
}
