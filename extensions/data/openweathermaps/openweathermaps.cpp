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
#include "openweathermaps.h"

#include <QTimer>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include <controllerinterface.h>
#include <desktopwidget.h>
#include <plexyconfig.h>
#include <config.h>

#include <json.h>

class openweathermapsData::Privateopenweathermaps
{
public:
  Privateopenweathermaps() {}
  ~Privateopenweathermaps() {}

  QNetworkAccessManager *mManager;
  QVariantMap mWeatherDataMap;
  QString mBaseUrlStr;
};

openweathermapsData::openweathermapsData(QObject *object)
  : PlexyDesk::DataSource(object), d(new Privateopenweathermaps)
{
  d->mBaseUrlStr = "http://api.openweathermap.org/data/2.5/weather?q=";
  ;
  d->mManager = new QNetworkAccessManager(this);
}

void openweathermapsData::init() {}

openweathermapsData::~openweathermapsData() { delete d; }

void openweathermapsData::getCurrentWeather(QVariantMap param,
    const QString &apiKey,
    const QString &request)
{
  QString requestURL = d->mBaseUrlStr;

  /* Request for Current weather data*/
  if (request == "current") {
    qDebug() << Q_FUNC_INFO << "Request Current Weather";
    /* Type of data to request
     * 1.) by city name = city_name;
     * 2.) by geographic coordinats = geo_coords
     * 3.) by city id = city_id
     */
    QString dataType = param["type"].toString();

    if (dataType == "city_name") {
      QString city_name = param["city_name"].toString();
      // QString country_name = param["country_name"].toString();
      requestURL += city_name;
      requestURL += QString("&") + apiKey;
      qDebug() << Q_FUNC_INFO << requestURL;

      QUrl url(requestURL);
      QNetworkReply *reply = d->mManager->get(QNetworkRequest(url));
      connect(reply, SIGNAL(finished()), this, SLOT(onDataReadyForCityName()));
    }
  }
}

void openweathermapsData::setArguments(QVariant args)
{
  qDebug() << Q_FUNC_INFO << "Start Weather Engine" << args;
  QString apiKey = "APPID=" + QString(OPEN_WEATHER_MAP_API_KEY);
  getCurrentWeather(args.toMap(), apiKey, args.toMap()["request"].toString());
}

void openweathermapsData::onDataReadyForCityName()
{
  if (sender()) {
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());

    if (reply) {
      QString data = reply->readAll();
      Json::Value root;
      Json::Reader jsonReader;
      bool parsingSuccessful = jsonReader.parse(data.toStdString(), root);

      if (parsingSuccessful) {
        qDebug() << Q_FUNC_INFO << "Parsing Success" << data;
        QVariantMap response;
        response["result"] = QString("city_name");

        /* Fetch Temperature data */
        if (!root["main"]["temp"].isNull()) {
          float temp = root["main"]["temp"].asFloat() - 273.15f;
          response["current_temp"] = QString::number(temp, 'f', 0);
        }

        Q_EMIT sourceUpdated(response);
      }
    }
  }
}

QVariantMap openweathermapsData::readAll()
{
  QVariant timeVariant;
  QVariantMap dataMap;

  timeVariant.setValue(QTime::currentTime());
  dataMap["currentTime"] = timeVariant;

  return dataMap;
}
