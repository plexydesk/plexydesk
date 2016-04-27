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

#include <QString>
#include <QVariant>
#include <QMap>

#include <algorithm>
#include <vector>
#include <map>
#include <string>

#include "ck_url.h"

#include <social_kit_export.h>

namespace social_kit {

typedef std::map<std::string, std::string> query_parameter_map_t;
typedef std::vector<std::string> string_list;

class DECL_SOCIAL_KIT_EXPORT service_query_parameters {
public:
  service_query_parameters() {}
  service_query_parameters(const service_query_parameters &copy) {
    m_parameter_map = copy.m_parameter_map;
  }

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

class DECL_SOCIAL_KIT_EXPORT remote_service_response {
public:
  typedef std::map<std::string, std::string> data_container_t;

  remote_service_response();
  remote_service_response(const remote_service_response &a_copy);

  ~remote_service_response();

  void insert(const std::string &a_key, const data_container_t &a_data);

  data_container_t data(const std::string &a_key) const;

  string_list query_list() const;

private:
  std::map<std::string, data_container_t> m_data;
  string_list m_query_list;
};

class DECL_SOCIAL_KIT_EXPORT remote_service {
public:
  typedef enum { kDefinitionLoadError, kNoError } definition_error_t;

  remote_service(const QString &input);
  virtual ~remote_service();

  QStringList service_list() const;

  std::string endpoint(const std::string &a_name) const;

  uint method(const QString &name) const;

  string_list arguments(const QString &name) const;

  string_list input_arguments(const std::string &a_name,
                              bool a_optional = false);
  QStringList optional_arguments(const std::string &name) const;

  QString argument_type(const QString &serviceName,
                        const QString &argument) const;

  std::string url(const std::string &a_method,
                  service_query_parameters *a_params) const;

  remote_service_response response(const std::string &a_method_name,
                                   const url_response &a_response);

  QMultiMap<QString, QVariantMap> queryResult(const QString &method,
                                              const QString &data) const;

  definition_error_t error() const;

protected:
  void load_services();

private:
  class remote_service_context;
  remote_service_context *const ctx;
};
}

#endif // SERVICEINPUTDEFINITION_H
