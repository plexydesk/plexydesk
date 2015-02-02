/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.org>
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
#include <desktopwidget.h>
#include <plexyconfig.h>
#include <QTimer>
#include <view_controller.h>
#include <QNetworkAccessManager>
#include <webservice.h>

class FlickrPhotoSearchData::PrivateFlickrPhotoSearch {
public:
  PrivateFlickrPhotoSearch() {}
  ~PrivateFlickrPhotoSearch() {}

  QuetzalSocialKit::WebService *mWebService;
};

FlickrPhotoSearchData::FlickrPhotoSearchData(QObject *object)
    : UI::DataSource(object), d(new PrivateFlickrPhotoSearch) {}

void FlickrPhotoSearchData::init() {}

FlickrPhotoSearchData::~FlickrPhotoSearchData() { delete d; }

void FlickrPhotoSearchData::setArguments(QVariant arg) {
  QVariantMap data = arg.toMap();
  QuetzalSocialKit::WebService *service =
      new QuetzalSocialKit::WebService(this);

  service->create("com.flikr.api");
  service->queryService("flickr.photos.search", data);

  connect(service, SIGNAL(finished(QuetzalSocialKit::WebService *)), this,
          SLOT(onServiceComplete(QuetzalSocialKit::WebService *)));
}

void FlickrPhotoSearchData::onServiceComplete(
    QuetzalSocialKit::WebService *service) {
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
