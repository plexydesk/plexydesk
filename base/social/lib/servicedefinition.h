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
#ifndef SERVICEINPUTDEFINITION_H
#define SERVICEINPUTDEFINITION_H

#include <QObject>
#include <QUrl>
#include <QuetzalSocialKitQt_export.h>

namespace social_kit {

typedef std::map<std::string, std::string> query_parameter_map_t;

class QuetzalSocialKitQt_EXPORT service_query_parameters {
public:
  service_query_parameters() {}
  ~service_query_parameters() { m_parameter_map.clear(); }

  void insert(const std::string &a_key, const std::string &a_value) {
    m_parameter_map[a_key] = a_value;
  }

  std::vector<std::string> keys() {
    std::vector<std::string> rv;

    for (std::map<std::string, std::string>::iterator it =
             m_parameter_map.begin();
         it != m_parameter_map.end(); ++it) {
      rv.push_back(it->first);
    }

    return rv;
  }

  std::string value(const std::string &a_key) { return m_parameter_map[a_key]; }

  query_parameter_map_t data() const { return m_parameter_map; }

private:
  std::map<std::string, std::string> m_parameter_map;
};

class QuetzalSocialKitQt_EXPORT ServiceDefinition : public QObject {
  Q_OBJECT
public:
  typedef enum {
    kDefinitionLoadError,
    kNoError
  } definition_error_t;

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
  std::vector<std::string> input_arguments(const std::string &a_name,
                                           bool a_optional = false);

  QStringList optionalArguments(const QString &name) const;

  QString argumentType(const QString &serviceName,
                       const QString &argument) const;

  QUrl queryURL(const QString &method, const QVariantMap &data) const;

  std::string service_url(const std::string &a_method,
                          service_query_parameters *a_params) const;

  QMultiMap<QString, QVariantMap> queryResult(const QString &method,
                                              const QString &data) const;

  definition_error_t error() const;

  void load_services();

protected:
  void buildServiceDefs();

private:
  class PrivateServiceDefinition;
  PrivateServiceDefinition *const d;
};
}

#endif // SERVICEINPUTDEFINITION_H
