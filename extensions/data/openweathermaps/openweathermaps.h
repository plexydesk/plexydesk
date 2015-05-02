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
#ifndef OPENWEATHERMAPS_DATA_H
#define OPENWEATHERMAPS_DATA_H

#include <QtCore>
#include <plexy.h>
#include <abstractplugininterface.h>
#include <datasource.h>
#include <QtNetwork>

class openweathermapsData : public PlexyDesk::DataSource {
  Q_OBJECT

public:
  openweathermapsData(QObject *object = 0);
  virtual ~openweathermapsData();
  void init();
  QVariantMap readAll();

  void getCurrentWeather(QVariantMap param, const QString &apiKey,
                         const QString &request);
public
Q_SLOTS:
  void setArguments(QVariant args);
  void onDataReadyForCityName();

private:
  class Privateopenweathermaps;
  Privateopenweathermaps *const d;
};

#endif
