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
