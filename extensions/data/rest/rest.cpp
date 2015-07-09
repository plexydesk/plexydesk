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
#include "rest.h"
#include <widget.h>
#include <plexyconfig.h>
#include <QNetworkAccessManager>

#define POST 0
#define GET 1

class RestData::Private {
public:
  Private() {}
  ~Private() {}
  QNetworkAccessManager *manager;
  QString user;
  QString pass;
  QVariantMap data;
};

RestData::RestData(QObject * /*object*/) : o_data_soure(new Private) {
  o_data_soure->manager = new QNetworkAccessManager(this);
  connect(o_data_soure->manager, SIGNAL(finished(QNetworkReply *)), this,
          SLOT(replyFinished(QNetworkReply *)));
  connect(o_data_soure->manager,
          SIGNAL(authenticationRequired(QNetworkReply *, QAuthenticator *)),
          this, SLOT(handleAuth(QNetworkReply *, QAuthenticator *)));
}

void RestData::init() { qDebug() << Q_FUNC_INFO; }

RestData::~RestData() { delete o_data_soure; }

void RestData::set_arguments(QVariant arg) {
  qDebug() << Q_FUNC_INFO << arg;
  QVariantMap param = arg.toMap();

  QUrl url = param["url"].toUrl();
  int type = param["type"].toInt();
  QString par = param["params"].toString();
  o_data_soure->user = param["user"].toString();
  o_data_soure->pass = param["pass"].toString();

  if (type == GET) {
    o_data_soure->manager->get(QNetworkRequest(url));
  } else if (type == POST) {
    o_data_soure->manager->post(QNetworkRequest(url), par.toLatin1());
  }
}

void RestData::replyFinished(QNetworkReply *reply) {
  QVariantMap response;
  response["data"] = QVariant(reply->readAll());
  o_data_soure->data = response;
  Q_EMIT ready();
}

void RestData::handleAuth(QNetworkReply * /*r*/, QAuthenticator *auth) {
  auth->setUser(o_data_soure->user);
  auth->setPassword(o_data_soure->pass);
}

QVariantMap RestData::readAll() { return o_data_soure->data; }
