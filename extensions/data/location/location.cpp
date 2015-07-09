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
#include "location.h"
#include <desktopwidget.h>
#include <plexyconfig.h>
#include <QTimer>
#include <controllerinterface.h>

#ifdef Q_WS_QPA
#include <bps/bps.h>
#include <bps/geolocation.h>
#include <errno.h>
#include <bps/dialog.h>
#endif

class LocatoinData::Private {
public:
  Private() {}
  ~Private() {}

  QVariantMap geoLocationEvent(bps_event_t *event);

  QTimer *mTimer;
};

void LocatoinData::startGeoTracking() {
  if (BPS_SUCCESS != geolocation_request_events(0)) {
    qWarning() << "Error requesting geolocation events:" << strerror(errno);
  } else {

    /*
    * Get geolocation events once a second, which is the most often that they
    * are capable of being reported.
    */
    geolocation_set_provider(GEOLOCATION_PROVIDER_HYBRID);
    geolocation_set_background(1);
    geolocation_set_period(10);
  }
}

LocatoinData::LocatoinData(QObject *object)
    : PlexyDesk::DataSource(object), d(new Private) {
  bps_initialize();

  startGeoTracking();

  startTimer(10000);

  Q_EMIT(ready());
}

void LocatoinData::init() {}

LocatoinData::~LocatoinData() {
  geolocation_stop_events(0);
  bps_shutdown();
  delete d;
}

void LocatoinData::setArguments(QVariant arg) {}

QVariantMap LocatoinData::readAll() {
  QVariant timeVariant;
  QVariantMap dataMap;

  timeVariant.setValue(QTime::currentTime());
  dataMap["currentTime"] = timeVariant;

  return dataMap;
}

void LocatoinData::timerEvent(QTimerEvent *timerevent) {
  bps_event_t *event = NULL;
  bps_get_event(&event, -1);

  if (event) {
    if (bps_event_get_domain(event) == geolocation_get_domain()) {
      Q_EMIT sourceUpdated(d->geoLocationEvent(event));
    } else {
      qDebug() << Q_FUNC_INFO << "Not a Geo Info Event";
      geolocation_stop_events(0);
      this->startGeoTracking();
    }
  } else {
    qDebug() << Q_FUNC_INFO << "Event not ready";
  }
}

QVariantMap LocatoinData::Private::geoLocationEvent(bps_event_t *event) {
  static int count = 0;

  /* Double check that the event is valid */
  if (event == NULL || bps_event_get_code(event) != GEOLOCATION_INFO) {
    qDebug() << Q_FUNC_INFO << "Invalid geo location data";
    return QVariantMap();
  }

  QVariantMap dataMap;

  dataMap["latitude"] = QVariant(geolocation_event_get_latitude(event));
  dataMap["longitude"] = geolocation_event_get_longitude(event);
  dataMap["accuracy"] = geolocation_event_get_accuracy(event);
  dataMap["altitude"] = geolocation_event_get_altitude(event);
  dataMap["altitude_valid"] = geolocation_event_is_altitude_valid(event);
  dataMap["altitude_accuracy"] = geolocation_event_get_altitude_accuracy(event);
  dataMap["altitude_accuracy_valid"] =
      geolocation_event_is_altitude_accuracy_valid(event);
  dataMap["heading"] = geolocation_event_get_heading(event);
  dataMap["heading_valid"] = geolocation_event_is_heading_valid(event);
  dataMap["speed"] = geolocation_event_get_speed(event);
  dataMap["speed_valid"] = geolocation_event_is_speed_valid(event);
  dataMap["num_satellites_used"] =
      geolocation_event_get_num_satellites_used(event);
  dataMap["num_satellites_valid"] =
      geolocation_event_is_num_satellites_valid(event);

  qDebug() << Q_FUNC_INFO << dataMap;

  return dataMap;
}
