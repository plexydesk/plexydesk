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
#ifndef DESKTOPACTIVITYMONITOR_DATA_H
#define DESKTOPACTIVITYMONITOR_DATA_H

#include <QtCore>

#include <abstractplugininterface.h>
#include <datasource.h>
#include <QtNetwork>

#include <datastore.h>

class DesktopActivityMonitorData : public PlexyDesk::DataSource {
  Q_OBJECT

public:
  DesktopActivityMonitorData(QObject *object = 0);
  virtual ~DesktopActivityMonitorData();
  void init();
  QVariantMap readAll();

  void timerEvent(QTimerEvent *event);

public
Q_SLOTS:
  void setArguments(QVariant sourceUpdated);
  void onTimeout();

private:
  class PrivateDesktopActivityMonitor;
  PrivateDesktopActivityMonitor *const d;
};

#endif
