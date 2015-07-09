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
#include "template.h"
#include <desktopwidget.h>
#include <plexyconfig.h>
#include <QTimer>
#include <controllerinterface.h>

class TemplateData::PrivateTemplate {
public:
  PrivateTemplate() {}
  ~PrivateTemplate() {}
  QTimer *mTemplate;
};

TemplateData::TemplateData(QObject *object)
    : PlexyDesk::DataSource(object), d(new PrivateTemplate) {
  startTimer(1000);
}

void TemplateData::init() {}

TemplateData::~TemplateData() { delete d; }

void TemplateData::setArguments(QVariant arg) {}

QVariantMap TemplateData::readAll() {
  QVariant timeVariant;
  QVariantMap dataMap;

  timeVariant.setValue(QTime::currentTime());
  dataMap["currentTime"] = timeVariant;

  return dataMap;
}

void TemplateData::timerEvent(QTimerEvent *event) {
  Q_EMIT sourceUpdated(readAll());
}
