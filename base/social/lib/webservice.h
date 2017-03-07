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
#ifndef WEBSERVICE_H
#define WEBSERVICE_H

#include <QObject>
#include <QVariantMap>

#include <social_kit_export.h>
#include <ck_remote_service.h>
#include <ck_url.h>

#include <functional>

namespace social_kit {

class remote_service;
class web_service;

typedef std::function<void(const remote_result &, const web_service *)>
response_callback_t;

class DECL_SOCIAL_KIT_EXPORT web_service : public QObject {
  Q_OBJECT
public:
  explicit web_service(QObject *a_parent_ptr = 0);
  virtual ~web_service();

  void create(const std::string &serviceName);

  /*
  void submit(const QString &method, service_query_parameters *a_params,
                    QHttpMultiPart *data = 0,
                    const QByteArray &headerName = QByteArray(),
                    const QByteArray &headerValue = QByteArray());
                    */

  void submit(const std::string &a_method, service_query_parameters *a_params);

  service_query_parameters inputArgumentForMethod(const QString &str);

  QString query() const;

  QList<QVariantMap> methodData(const QString &query) const;

  void on_response_ready(response_callback_t a_callback);

private:
  class web_service_context;
  web_service_context *const ctx;
};
}
#endif // WEBSERVICE_H
