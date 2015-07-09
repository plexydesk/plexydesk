/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.org>
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
#include <QSharedPointer>
#include <QNetworkReply>
#include <QuetzalSocialKit_export.h>

namespace QuetzalSocialKit {

class ServiceDefinition;

typedef QSharedPointer<ServiceDefinition> ServiceDefinitionPtr;

class QuetzalSocialKit_EXPORT WebService : public QObject {
  Q_OBJECT
public:
  explicit WebService(QObject *a_parent_ptr = 0);

  virtual ~WebService();

  void create(const QString &serviceName);

  QString serviceName() const;

  void queryService(const QString &method, const QVariantMap &arguments,
                    QHttpMultiPart *data = 0,
                    const QByteArray &headerName = QByteArray(),
                    const QByteArray &headerValue = QByteArray());

  QVariantMap inputArgumentForMethod(const QString &str);

  QVariantMap serviceData() const;

  QByteArray rawServiceData() const;

  QString methodName() const;

  QList<QVariantMap> methodData(const QString &methodName) const;

  QStringList availableData() const;

Q_SIGNALS:
  void finished(QuetzalSocialKit::WebService *service);

private
Q_SLOTS:
  void onNetworkRequestFinished(QNetworkReply *reply);

  void onDownloadRequestComplete();

private:
  QString installPrefix() const;

  class PrivateWebService;
  PrivateWebService *const d;
};
}
#endif // WEBSERVICE_H
