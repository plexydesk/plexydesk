#ifndef SERVICEINPUTDEFINITION_H
#define SERVICEINPUTDEFINITION_H

#include <QObject>
#include <QUrl>
#include <QuetzalSocialKit_export.h>

namespace QuetzalSocialKit
{

class QuetzalSocialKit_EXPORT ServiceDefinition : public QObject
{
  Q_OBJECT
public:
  ServiceDefinition(const QString &input, QObject *parent = 0);

  virtual ~ServiceDefinition();

  QStringList knownServices() const;

  QString endpoint(const QString &name) const;

  /*!
  \brief
  retuns the request type GET/POST
  \fn requestType
  \param name method name.
  \return uint
  */
  uint requestType(const QString &name) const;

  QStringList arguments(const QString &name) const;

  QStringList optionalArguments(const QString &name) const;

  QString argumentType(const QString &serviceName,
                       const QString &argument) const;

  QUrl queryURL(const QString &method, const QVariantMap &data) const;

  QMultiMap<QString, QVariantMap> queryResult(const QString &method,
      const QString &data) const;

protected:
  void buildServiceDefs();

private:
  class PrivateServiceDefinition;
  PrivateServiceDefinition *const d;
};
}

#endif // SERVICEINPUTDEFINITION_H
