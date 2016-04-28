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
#include "webservice.h"

#include <QDebug>
#include <QCoreApplication>
#include <QDir>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QtNetwork>

#ifdef Q_OS_MAC
#include <CoreFoundation/CoreFoundation.h>
#endif

#include "servicedefinition.h"

namespace social_kit {

class WebService::PrivateWebService {

public:
  PrivateWebService() {}
  ~PrivateWebService() {
    // qDebug() << Q_FUNC_INFO;
      if (m_service_def)
          delete m_service_def;
  }

  QString mSocialDefPrefix;
  QString mSocialDefPath;
  QString mServiceName;
  QString mMethodName;

  QByteArray mServiceData;
  QMultiMap<QString, QVariantMap> mProcessedData;
  QMap<QString, QVariantMap> mInputArguments;

  std::map<std::string, service_query_parameters> m_input_argument_map;

  remote_service *m_service_def;
  QNetworkAccessManager *mNetworkManager;
};

WebService::WebService(QObject *parent)
    : QObject(parent), d(new PrivateWebService) {
  d->mNetworkManager = new QNetworkAccessManager(this);
  connect(d->mNetworkManager, SIGNAL(finished(QNetworkReply *)), this,
          SLOT(onNetworkRequestFinished(QNetworkReply *)));

  d->mSocialDefPrefix = QDir::toNativeSeparators(
      QString("%1/%2").arg(installPrefix()).arg("/share/social/"));
}

WebService::~WebService() { delete d; }

void WebService::create(const QString &serviceName) {
  d->mServiceName = serviceName;
  d->mSocialDefPath = QDir::toNativeSeparators(
      QString("%1/%2.xml").arg(d->mSocialDefPrefix).arg(serviceName));
  // qDebug() << Q_FUNC_INFO << d->mSocialDefPath;

  d->m_service_def = new remote_service(d->mSocialDefPath.toStdString());
}

QString WebService::serviceName() const { return d->mServiceName; }

void WebService::queryService(const QString &method,
                              service_query_parameters *a_params,
                              QHttpMultiPart *data,
                              const QByteArray &headerName,
                              const QByteArray &headerValue) {
  if (d->m_service_def) {
    QUrl url = QUrl(d->m_service_def->url(method.toStdString(), a_params).c_str());
    uint requestType = d->m_service_def->method(method.toStdString());

    d->mMethodName = method;
    d->m_input_argument_map[method.toStdString()]= *a_params;

    if (d->mNetworkManager) {
      QNetworkRequest request;
      qDebug() << Q_FUNC_INFO << url;
      request.setRawHeader(headerName, headerValue);

      request.setUrl(url);

      if (requestType == 0) {
        d->mNetworkManager->get(request);
      } else if (requestType == 1 && data != 0) {
        qDebug() << Q_FUNC_INFO << "POST Data Found";
        d->mNetworkManager->post(request, data);
      }
    }
  }
}

service_query_parameters WebService::inputArgumentForMethod(const QString &str) {
  return d->m_input_argument_map[str.toStdString()];
}

QVariantMap WebService::serviceData() const {
  QVariantMap rv;

  return rv;
}

QByteArray WebService::rawServiceData() const { return d->mServiceData; }

QString WebService::methodName() const { return d->mMethodName; }

QList<QVariantMap> WebService::methodData(const QString &methodName) const {
  QList<QVariantMap> rv;
  QMultiMap<QString, QVariantMap>::iterator i =
      d->mProcessedData.find(methodName);
  while (i != d->mProcessedData.end() && i.key() == methodName) {
    rv.append(i.value());
    ++i;
  }

  return rv;
}

QStringList WebService::availableData() const {
  return d->mProcessedData.keys();
}

void WebService::onNetworkRequestFinished(QNetworkReply *reply) {
  /*
  if (reply) {
    d->mServiceData = reply->readAll();
    reply->deleteLater();
    d->mProcessedData =
        d->m_service_def->queryResult(d->mMethodName, d->mServiceData);
  }

  Q_EMIT finished(this);
  */
}

void WebService::onDownloadRequestComplete() {
  QNetworkReply *request = qobject_cast<QNetworkReply *>(sender());

  if (request) {
    d->mServiceData = request->readAll();
    request->deleteLater();
    Q_EMIT finished(this);
  }
}

QString WebService::installPrefix() const {
#ifndef Q_OS_WIN32
  QDir binaryPath(QCoreApplication::applicationDirPath());
  if (binaryPath.cdUp()) {
    return QDir::toNativeSeparators(binaryPath.canonicalPath());
  }
#endif

#ifdef Q_OS_LINUX
  QString basePath(qgetenv("PLEXYDESK_DIR"));
  if (basePath.isEmpty() || basePath.isNull()) {
    return PLEXYPREFIX;
  }

  return basePath;
#endif

#ifdef Q_OS_MAC
  CFURLRef appUrlRef = CFBundleCopyBundleURL(CFBundleGetMainBundle());
  CFStringRef macPath =
      CFURLCopyFileSystemPath(appUrlRef, kCFURLPOSIXPathStyle);
  const char *pathPtr =
      CFStringGetCStringPtr(macPath, CFStringGetSystemEncoding());
  CFRelease(appUrlRef);
  CFRelease(macPath);
  return QLatin1String(pathPtr) + QString("/Contents/");
#endif

  return QString();
}
}
