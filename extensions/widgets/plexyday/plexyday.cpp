/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.com>
*  Authored By  : Lahiru Lakmal <llahiru@gmail.com>
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
#include "plexyday.h"
#include "plexydaywidget.h"

PlexyDayController::PlexyDayController(QObject *object) {
#ifdef Q_WS_QPA
  mWidget = new PlexyDesk::PlexyDayWidget(QRectF(0, 0, 768, 1280));
#else
  mWidget = new PlexyDesk::PlexyDayWidget(QRectF(0, 0, 480, 680));
#endif
  mWidget->setController(this);
  mWidget->setLabelName("PlexyDay");
  mWidget->setWidgetFlag(PlexyDesk::Widget::kConvertToWindowType);
  mWidget->setWidgetFlag(PlexyDesk::Widget::kTopLevelWindow);
}

PlexyDayController::~PlexyDayController() {
  if (mWidget) {
    delete mWidget;
  }
}

void PlexyDayController::init() {}

void PlexyDayController::revokeSession(const QVariantMap &args) {}

void PlexyDayController::setViewRect(const QRectF &rect) {
  if (mWidget) {
    mWidget->setPos(rect.x(), rect.y());
  }
}

QString PlexyDayController::icon() const { return QString("pd_day_icon.png"); }
