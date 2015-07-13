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
#include "httpserver.h"
#include <ck_widget.h>
#include <ck_config.h>
#include <QTimer>
#include <ck_desktop_controller_interface.h>

class HttpServerData::PrivateHttpServer {
public:
  PrivateHttpServer() {}
  ~PrivateHttpServer() {}
  QTimer *mHttpServer;
};

HttpServerData::HttpServerData(QObject *object)
    : cherry_kit::data_source(object), o_data_soure(new PrivateHttpServer) {
  startTimer(1000);
}

void HttpServerData::init() {}

HttpServerData::~HttpServerData() { delete o_data_soure; }

void HttpServerData::set_arguments(QVariant arg) {}

QVariantMap HttpServerData::readAll() {
  QVariant timeVariant;
  QVariantMap dataMap;

  timeVariant.setValue(QTime::currentTime());
  dataMap["currentTime"] = timeVariant;

  return dataMap;
}

void HttpServerData::timerEvent(QTimerEvent *event) {
  Q_EMIT source_updated(readAll());
}
