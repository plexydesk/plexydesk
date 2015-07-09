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
#ifndef SERVICEINPUTDEFINITION_H
#define SERVICEINPUTDEFINITION_H

#include <QObject>
#include <QUrl>
#include <QuetzalSocialKit_export.h>

namespace QuetzalSocialKit {

class QuetzalSocialKit_EXPORT ServiceDefinition : public QObject {
  Q_OBJECT
public:
  ServiceDefinition(const QString &input, QObject *a_parent_ptr = 0);

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
