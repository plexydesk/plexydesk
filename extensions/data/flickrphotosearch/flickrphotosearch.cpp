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
#include "flickrphotosearch.h"
#include <ck_widget.h>
#include <ck_config.h>
#include <QTimer>
#include <ck_desktop_controller_interface.h>
#include <QNetworkAccessManager>
#include <webservice.h>

class FlickrPhotoSearchData::PrivateFlickrPhotoSearch {
public:
  PrivateFlickrPhotoSearch() {}
  ~PrivateFlickrPhotoSearch() {}

  social_kit::WebService *mWebService;
};

FlickrPhotoSearchData::FlickrPhotoSearchData(QObject *object)
    : cherry_kit::data_source(object),
      o_data_soure(new PrivateFlickrPhotoSearch) {}

void FlickrPhotoSearchData::init() {}

FlickrPhotoSearchData::~FlickrPhotoSearchData() { delete o_data_soure; }

void FlickrPhotoSearchData::set_arguments(QVariant arg) {
  QVariantMap data = arg.toMap();
  social_kit::WebService *service =
      new social_kit::WebService(this);

  social_kit::service_query_parameters input_data;

  Q_FOREACH(const QString &key, data.keys()) {
     input_data.insert(key.toStdString(), data[key].toString().toStdString());
  }

  service->create("com.flikr.api");
  service->queryService("flickr.photos.search", &input_data);

  connect(service, SIGNAL(finished(social_kit::WebService *)), this,
          SLOT(onServiceComplete(social_kit::WebService *)));
}

void FlickrPhotoSearchData::onServiceComplete(
    social_kit::WebService *service) {
  qDebug() << Q_FUNC_INFO
           << "Service Complete :" << service->methodData("photo").count();
  qDebug() << Q_FUNC_INFO
           << "Service Complete :" << service->methodData("status").count();
  qDebug() << Q_FUNC_INFO
           << "Service Complete :" << service->methodData("photos").count();

  delete service;
}

QVariantMap FlickrPhotoSearchData::readAll() {
  QVariant timeVariant;
  QVariantMap dataMap;

  timeVariant.setValue(QTime::currentTime());
  dataMap["currentTime"] = timeVariant;

  return dataMap;
}

void FlickrPhotoSearchData::timerEvent(QTimerEvent *event) {
  // Q_EMIT sourceUpdated(readAll());
}
