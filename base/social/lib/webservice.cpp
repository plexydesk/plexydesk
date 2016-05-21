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

#include "ck_remote_service.h"

#include <iostream>
#include <memory>

namespace social_kit {

class web_service::web_service_context {

public:
  web_service_context() : m_service(0) {}
  ~web_service_context() {
    if (m_service)
      delete m_service;

    if (m_network_request)
      delete m_network_request;

    std::cout << "delete -> " << __FUNCTION__ << std::endl;
  }

  QString mSocialDefPrefix;
  QString mSocialDefPath;
  QString mServiceName;
  QString mMethodName;

  QByteArray mServiceData;
  QMultiMap<QString, QVariantMap> mProcessedData;
  QMap<QString, QVariantMap> mInputArguments;

  std::map<cherry_kit::string, service_query_parameters> m_input_argument_map;

  remote_service *m_service_def;
  QNetworkAccessManager *mNetworkManager;

  /* new */
  remote_service *m_service;
  cherry_kit::string m_method_name;
  url_request *m_network_request;
  std::vector<response_callback_t> m_callback_list;

  void notify_observers(const remote_result &a_result,
                        const web_service *a_service) {
    std::for_each(std::begin(m_callback_list), std::end(m_callback_list),
                  [&](response_callback_t a_callback) {
      if (a_callback)
        a_callback(a_result, a_service);
    });
  }
};

web_service::web_service(QObject *parent)
    : QObject(parent), ctx(new web_service_context) {
  ctx->m_network_request = new url_request();
  ctx->m_network_request->on_response_ready([this](
      const url_response &a_response) {
    remote_result result =
        ctx->m_service->response(ctx->m_method_name, a_response);
    ctx->notify_observers(result, this);
    //delete (this)
    std::unique_ptr<web_service>(this);
  });
}

web_service::~web_service() { delete ctx; }

void web_service::create(const cherry_kit::string &serviceName) {
  if (ctx->m_service)
    delete ctx->m_service;

  ctx->m_service = new remote_service(serviceName);
}

void web_service::submit(const QString &method,
                         service_query_parameters *a_params,
                         QHttpMultiPart *data, const QByteArray &headerName,
                         const QByteArray &headerValue) {
  if (!ctx->m_service)
    return;

  ctx->m_method_name = method.toStdString();
  cherry_kit::string request_data =
      ctx->m_service->url(method.toStdString(), a_params);
  url_request::url_request_type_t request_url_method =
      ctx->m_service->method(method.toStdString());

  ctx->m_input_argument_map[method.toStdString()] = *a_params;
  ctx->m_network_request->send_message(request_url_method, request_data);
}

service_query_parameters
web_service::inputArgumentForMethod(const QString &str) {
  return ctx->m_input_argument_map[str.toStdString()];
}

QString web_service::query() const { return ctx->mMethodName; }

QList<QVariantMap> web_service::methodData(const QString &methodName) const {
  QList<QVariantMap> rv;
  return rv;
}

void web_service::on_response_ready(response_callback_t a_callback) {
  ctx->m_callback_list.push_back(a_callback);
}

/*
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
*/
}
