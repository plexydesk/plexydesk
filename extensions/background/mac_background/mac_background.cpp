/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@kde.org>
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

#include <QDir>
#include <QPixmapCache>
#include <QDeclarativeContext>
#include <QtDebug>

#include <desktopwidget.h>
#include <plexyconfig.h>

#include "mac_background.h"

MacBackgroundController::MacBackgroundController(QObject *object)
    : PlexyDesk::ControllerInterface(object) {
  mMacDesktopRectItem = new MacBackgroundRender(QRectF(), 0);
}

MacBackgroundController::~MacBackgroundController() {
  if (mMacDesktopRectItem) {
    delete mMacDesktopRectItem;
  }
}

void MacBackgroundController::init() {
  QSizeF desktopSize = mDesktopScreenRect.size();
  mMacDesktopRectItem->setContentRect(
      QRectF(0.0, 0, desktopSize.width(), desktopSize.height()));
  mMacDesktopRectItem->setLayerType(
      PlexyDesk::AbstractDesktopWidget::Background);
  Q_EMIT spawnView(mMacDesktopRectItem);
}

void MacBackgroundController::revokeSession(const QVariantMap &args) {}

QStringList MacBackgroundController::actions() const { return QStringList(); }

void MacBackgroundController::setViewRect(const QRectF &rect) {
  mDesktopScreenRect = rect;
  mMacDesktopRectItem->setContentRect(rect);
}

void MacBackgroundController::handleDropEvent(
    PlexyDesk::AbstractDesktopWidget *widget, QDropEvent *event) {}

void MacBackgroundController::requestAction(const QString &actionName,
                                            const QVariantMap &args) {}
