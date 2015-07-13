/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.com>
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
#include "idletimeengine.h"
#include <desktopck_widget.h>
#include <ck_config.h>
#include <QTimer>
#include <controllerinterface.h>

#include "idletimedetector.h"

class IdleTimeEngineData::PrivateIdleTimeEngine {
public:
  PrivateIdleTimeEngine() {}
  ~PrivateIdleTimeEngine() {}
  QTimer *mIdleTimeEngine;
  IdleTimeDetector *mIdleEngine;
};

IdleTimeEngineData::IdleTimeEngineData(QObject *object)
    : PlexyDesk::DataSource(object), d(new PrivateIdleTimeEngine) {
  d->mIdleEngine = new IdleTimeDetector(this);
  startTimer(1000);

  connect(d->mIdleEngine, SIGNAL(awake()), this, SLOT(onAwakEvent()));
}

void IdleTimeEngineData::init() {}

IdleTimeEngineData::~IdleTimeEngineData() { delete d; }

void IdleTimeEngineData::setArguments(QVariant arg) {}

void IdleTimeEngineData::onAwakeEvent() { Q_EMIT sourceUpdated(readAll()); }

QVariantMap IdleTimeEngineData::readAll() {
  QVariant timeVariant;
  QVariantMap dataMap;

  timeVariant.setValue(QTime::currentTime());
  dataMap["currentTime"] = timeVariant;
  dataMap["SystemIdleTime"] = QVariant(d->mIdleEngine->duration());
  dataMap["SystemSleepTime"] = QVariant(d->mIdleEngine->sleepTime());

  return dataMap;
}

void IdleTimeEngineData::timerEvent(QTimerEvent *event) {
  if (d->mIdleEngine && d->mIdleEngine->duration() <= 0) {
    return;
  }

  Q_EMIT sourceUpdated(readAll());
}
