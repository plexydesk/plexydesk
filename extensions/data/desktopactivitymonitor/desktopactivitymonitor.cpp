/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@kde.org>
*  Authored By  : *
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
#include "desktopactivitymonitor.h"
#include <desktopwidget.h>
#include <plexyconfig.h>
#include <QTimer>
#include <controllerinterface.h>

#include "platformactivitymonitor.h"
#include "windowobject.h"

class DesktopActivityMonitorData::PrivateDesktopActivityMonitor {
public:
  PrivateDesktopActivityMonitor() {}

  ~PrivateDesktopActivityMonitor() {}

  PlatformActivityMonitor *mDesktopActivityMonitor;
  QMap<QString, WindowObject> mData;

  WindowObject mLastObject;

  QTimer *mTimer;
};

DesktopActivityMonitorData::DesktopActivityMonitorData(QObject *object)
    : PlexyDesk::DataSource(object), d(new PrivateDesktopActivityMonitor) {
  d->mDesktopActivityMonitor = new PlatformActivityMonitor(this);
  d->mLastObject = d->mDesktopActivityMonitor->updateWindowList();
  startTimer(1000);

  /*
  d->mTimer = new QTimer(this);
  d->mTimer->setTimerType(Qt::PreciseTimer);

  connect(d->mTimer, SIGNAL(timeout()), this,
          SLOT(onTimeout()));
  d->mTimer->start(1000);
  */
}

void DesktopActivityMonitorData::init() {}

DesktopActivityMonitorData::~DesktopActivityMonitorData() { delete d; }

void DesktopActivityMonitorData::setArguments(QVariant arg) {}

void DesktopActivityMonitorData::onTimeout() { qDebug() << Q_FUNC_INFO; }

QVariantMap DesktopActivityMonitorData::readAll() {
  QVariantMap dataMap;

  // check activities.
  WindowObject object = d->mLastObject;
  dataMap["currentApp"] = object.name();
  dataMap["currentAction"] = object.title();
  dataMap["currentTime"] = object.time();

  return dataMap;
}

void DesktopActivityMonitorData::timerEvent(QTimerEvent *event) {
  WindowObject object = d->mDesktopActivityMonitor->updateWindowList();

  if (object.name() != d->mLastObject.name() ||
      (object.name() == d->mLastObject.name() &&
       object.title() != d->mLastObject.title())) {
    // save the last object to the database
    d->mLastObject.updateDuration(1);

    d->mLastObject = object;
    Q_EMIT sourceUpdated(readAll());
  }
}
