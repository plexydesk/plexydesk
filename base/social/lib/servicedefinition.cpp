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
#include "ck_url.h"
#include "servicedefinition.h"

#include <sys/stat.h>

#include <algorithm>
#include <iostream>
#include <string>
#include <tinyxml2.h>

namespace social_kit {

typedef enum {
  kStringType,
  kIntType,
  kRealType,
  kBooleanType,
  kUnknown
} service_data_type_t;

class service_input_argument {
public:
  service_input_argument() : m_optional(0) {}
  ~service_input_argument() {}

  bool optional() const;
  void set_optional(bool optional);

  std::string value() const;
  void set_value(const std::string &value);

  service_data_type_t type() const;
  void set_type(const service_data_type_t &type);

  std::string default_value() const;
  void set_default_value(const std::string &default_value);

private:
  bool m_optional;
  std::string m_value;
  std::string m_default_value;
  service_data_type_t m_type;
};

typedef std::vector<service_input_argument *> input_arg_t;

class service_input {
public:
  typedef enum {
    kPOSTRequest,
    kGETRequest,
    kHEADRequest,
    kDELETERequest,
    kCONNECTRequest,
    kPUTRequest,
    kOPTIONSRequest,
    kUndefinedRequest
  } request_type_t;

  service_input() : m_request_type(kUndefinedRequest) {}
  ~service_input() {
    std::for_each(std::begin(m_argument_list), std::end(m_argument_list),
                  [this](service_input_argument *value) {
      if (value)
        delete value;
    });
    m_argument_list.clear();
  }

  std::string url() const;
  void set_url(const std::string &url);

  input_arg_t argument_list() const;
  void insert_argument(service_input_argument *a_arg) {
    m_argument_list.push_back(a_arg);
  }

  request_type_t request_type() const;
  void set_request_type(request_type_t request_type);

private:
  input_arg_t m_argument_list;
  std::string m_url;
  request_type_t m_request_type;
};

class service_result_query_attribute {
public:
  service_result_query_attribute() {}
  ~service_result_query_attribute() {}

  std::string value() const;
  void set_value(const std::string &value);

  service_data_type_t type() const;
  void set_type(const service_data_type_t &type);

private:
  service_data_type_t m_type;
  std::string m_value;
};

class service_result_query {
public:
  typedef std::vector<service_result_query_attribute *> attribute_list_t;

  service_result_query() {}
  ~service_result_query() {}

  std::string name() const { return m_name; }
  void set_name(const std::string &a_name) { m_name = a_name; }

  std::string tag_name() const { return m_tag_name; }
  void set_tag_name(const std::string &a_tag_name) { m_tag_name = a_tag_name; }

  void insert_attribute(service_result_query_attribute *a_attr) {
    m_attribute_list.push_back(a_attr);
  }
  attribute_list_t attribute_list() const { return m_attribute_list; }

private:
  std::string m_name;
  std::string m_tag_name;
  std::vector<service_result_query_attribute *> m_attribute_list;
};

/* result of the service */
class service_result {
public:
  typedef enum {
    kXMLData = 0,
    kJSONData,
    kTextData,
    kBinaryData,
    kUnknown
  } service_result_t;

  service_result() {}
  ~service_result() {}

  void set_result_format(service_result_t a_result) {
    m_result_format = a_result;
  }
  service_result_t result_format() const { return m_result_format; }

  std::vector<service_result_query *> query_list() { return m_query_list; }

  void insert(service_result_query *a_query) {
    m_query_list.push_back(a_query);
  }

private:
  service_result_t m_result_format;
  std::vector<service_result_query *> m_query_list;
};

class service_error {
public:
  service_error() {}

private:
  std::string m_format;
  std::string m_error_message;
  int m_error_code;
};
/* service definition structure contains following three things
         a. input query
         b. result query
         c. error query

         the parsed xml meta data will be stored in this class for easy access.
 */
class service {
public:
  service() : m_input(0) {}
  ~service() {
    if (m_input)
      delete m_input;

    if (m_result)
      delete m_result;
  }

  service_input *input() const;
  void set_input(service_input *input);

  service_result *result() const { return m_result; }
  void set_service_result(service_result *a_result) { m_result = a_result; }

  void set_name(const std::string &a_name) { m_name = a_name; }
  std::string name() const { return m_name; }

private:
  service_input *m_input;
  service_error *m_error;
  service_result *m_result;

  std::string m_name;
};

typedef std::map<std::string, service *> service_map_t;
typedef service_map_t::iterator service_map_iterator_t;

class remote_service::remote_service_context {
public:
  remote_service_context() : m_current_error(remote_service::kNoError) {}
  ~remote_service_context() {
    for (service_map_iterator_t iterator = m_service_dict.begin();
         iterator != m_service_dict.end(); iterator++) {

      if (iterator->second)
        delete iterator->second;
    }

    m_service_dict.clear();
  }

  definition_error_t m_current_error;
  tinyxml2::XMLDocument m_xml_root_doc;
  service_map_t m_service_dict;
};

/* utility functions till we refactor this class properly */

extern bool ck_file_exisits(const std::string &a_file_name);

bool ck_file_exisits(const std::string &a_file_name) {
  struct stat buffer;
  return (stat(a_file_name.c_str(), &buffer) == 0);
}

remote_service::remote_service(const std::string &input)
    : ctx(new remote_service_context) {
  std::string service_file =  data_prefix() + input;
  std::cout << "def ->" << service_file << std::endl;
  if (ck_file_exisits(service_file.c_str())) {
    tinyxml2::XMLError error = ctx->m_xml_root_doc.LoadFile(service_file.c_str());
    if (error != tinyxml2::XML_NO_ERROR) {
      // std::cout << __LINE__ << " : " << __FUNCTION__ << " Error "
      //         << ctx->m_xml_root_doc.GetErrorStr1() << std::endl;
      ctx->m_current_error = kDefinitionLoadError;
    } else {
      ctx->m_current_error = kNoError;
      load_services();
    }
  } else {
    // qWarning() << Q_FUNC_INFO << "No Input File Found at: " << input.c_str();
    // std::cout << __LINE__ << " : " << __FUNCTION__ << " Error "
    //         << "Service Definistion File Not Found : " << input.toStdString()
    //         << std::endl;
    ctx->m_current_error = kDefinitionLoadError;
  }
}

remote_service::~remote_service() { delete ctx; }

string_list remote_service::service_list() const {
  string_list rv;
  for (service_map_iterator_t iterator = ctx->m_service_dict.begin();
       iterator != ctx->m_service_dict.end(); iterator++) {
    rv.push_back(iterator->first);
  }

  return rv;
}

// fetch the url from the <input> tag
std::string remote_service::endpoint(const std::string &a_name) const {
  std::string rv;

  service *srv = ctx->m_service_dict[a_name];

  if (srv && srv->input()) {
    rv = srv->input()->url();
  }

  return rv;
}

uint remote_service::method(const std::string &name) const {
  uint rv = service_input::kUndefinedRequest;

  service *srv = ctx->m_service_dict[name];

  if (srv && srv->input()) {
    rv = (uint)srv->input()->request_type();
  }

  return rv;
}

string_list remote_service::arguments(const std::string &name) const {
  string_list rv;
  service *srv = ctx->m_service_dict[name];

  if (srv && srv->input()) {
    std::vector<service_input_argument *> argument_list =
        srv->input()->argument_list();
    std::for_each(std::begin(argument_list), std::end(argument_list),
                  [&](service_input_argument *arg) {
      if (arg->optional())
        return;

      rv.push_back(arg->value());
    });
  }

  return rv;
}

string_list remote_service::input_arguments(const std::string &a_name,
                                            bool a_optional) {
  std::vector<std::string> rv;
  service *srv = ctx->m_service_dict[a_name];

  if (srv && srv->input()) {
    std::vector<service_input_argument *> argument_list =
        srv->input()->argument_list();
    std::for_each(std::begin(argument_list), std::end(argument_list),
                  [&](service_input_argument *arg) {
      if ((a_optional == false) && arg->optional()) {
        return;
      }

      if ((a_optional == true) && !arg->optional()) {
        return;
      }

      rv.push_back(arg->value());
    });
  }

  return rv;
}

string_list remote_service::optional_arguments(const std::string &name) const {
  string_list rv;
  service *srv = ctx->m_service_dict[name];

  if (srv && srv->input()) {
    std::vector<service_input_argument *> argument_list =
        srv->input()->argument_list();
    std::for_each(std::begin(argument_list), std::end(argument_list),
                  [&](service_input_argument *arg) {
      if (!arg->optional())
        return;

      rv.push_back(arg->value());
    });
  }

  return rv;
}

std::string remote_service::url(const std::string &a_method,
                                service_query_parameters *a_params) const {
  std::string rv;
  std::vector<std::string> default_query_list;
  bool has_errors = false;

  rv = endpoint(a_method);

  if (a_params) {

    service *srv = ctx->m_service_dict.at(a_method);

    if (srv && srv->input()) {
      service_input *input = srv->input();
      std::vector<service_input_argument *> list = input->argument_list();

      std::for_each(std::begin(list), std::end(list),
                    [&](service_input_argument *arg) {
        if (!arg)
          return;
        std::string arg_value = arg->value();
        std::vector<std::string> input_keys = a_params->keys();

        bool key_found = std::find(input_keys.begin(), input_keys.end(),
                                   arg_value) != input_keys.end();

        // assign default values first.
        if (!arg->default_value().empty()) {
          social_kit::url_encode *encoded_str =
              new social_kit::url_encode(arg->default_value());
          std::string query_item = arg_value + "=" + encoded_str->to_string();
          delete encoded_str;
          default_query_list.push_back(query_item);
        }

        if (key_found) {
          social_kit::url_encode *encoded_str =
              new social_kit::url_encode(a_params->value(arg_value));
          std::string query_item = arg_value + "=" + encoded_str->to_string();
          delete encoded_str;
          default_query_list.push_back(query_item);
        } else {
          if (arg->optional() == 0) {
            has_errors = true;
            return;
          }
        }
      });
    } else {
      // qDebug() << Q_FUNC_INFO << "Something Wrong";
    }
  }

  std::string query_str = "?";

  std::vector<std::string>::iterator it;

  for (it = default_query_list.begin(); it < default_query_list.end(); it++) {
    if (it == (default_query_list.end() - 1))
      query_str += *it;
    else
      query_str += *it + "&";
  }

  rv += query_str;

  if (has_errors)
    return std::string();

  return rv;
}

const char *get_attribute_value(tinyxml2::XMLElement *element,
                                const std::string &a_attribute_name) {
  if (!element || a_attribute_name.empty())
    return NULL;

  const char *value = element->Attribute(a_attribute_name.c_str());

  return value;
}

void lookup_element(tinyxml2::XMLElement *node, service_result_query *query,
                    remote_result *a_result) {
  if (!node)
    return;

  for (tinyxml2::XMLElement *child_element = node->FirstChildElement();
       child_element != NULL;
       child_element = child_element->NextSiblingElement()) {
    lookup_element(child_element, query, a_result);
  }

  const char *keyword = query->tag_name().c_str();

  if (keyword && strcmp(node->Name(), keyword) == 0) {
    remote_result_data result;
    result.set_name(node->Name());
    service_result_query::attribute_list_t list = query->attribute_list();

    std::for_each(std::begin(list), std::end(list),
                  [&](service_result_query_attribute *attrib) {
      const char *attribute_value = get_attribute_value(node, attrib->value());
      if (attribute_value) {
        remote_data_attribute attribute;
        attribute.set_key(attrib->value());
        attribute.set_value(attribute_value);
        result.insert(attribute);
      }
    });
    a_result->insert(result);
  }
}

remote_result remote_service::response(const std::string &a_method_name,
                                       const url_response &a_response) const {
  remote_result rv;
  service *srv = ctx->m_service_dict[a_method_name];
  service_result *srv_result = 0;

  if (!srv) {
    std::cout << "Error method name" << std::endl;
    return rv;
  }

  srv_result = srv->result();

  if (!srv_result) {
    std::cout << "No Result Object" << std::endl;
    return rv;
  }

  if (srv_result->result_format() == service_result::kXMLData) {
    // xml data;
    std::vector<service_result_query *> list = srv_result->query_list();
    tinyxml2::XMLDocument doc;
    if (doc.Parse(a_response.data_buffer(), a_response.data_buffer_size()) !=
        tinyxml2::XML_NO_ERROR) {
      return rv;
    }

    std::cout << "result query size : " << list.size() << std::endl;
    std::for_each(std::begin(list), std::end(list),
                  [&](service_result_query *query) {
      if (query) {
        tinyxml2::XMLElement *element = doc.FirstChildElement();
        lookup_element(element, query, &rv);
      }
    });

  } else if (srv_result->result_format() == service_result::kJSONData) {
  } else {
    // unknown data ?
  }

  return rv;
}

/*
QMultiMap<QString, QVariantMap>
remote_service::queryResult(const QString &method, const QString &data) const {
  QHash<QString, PrivateResultQuery> result = ctx->queryForMethod(method);
  QStringList queries = result.keys();

  QMultiMap<QString, QVariantMap> tagData;

  uint docType = ctx->documentType(method);

  if (docType == 0) {
    QDomDocument dataRoot;
    if (dataRoot.setContent(data)) {
      Q_FOREACH(const QString & keyString, queries) {
        QDomNodeList filteredNodeList =
            dataRoot.elementsByTagName(result[keyString].tagName);

        for (int i = 0; i < filteredNodeList.count(); i++) {
          QDomNode dataNode = filteredNodeList.at(i);
          QDomNamedNodeMap dataNodeAttributes = dataNode.attributes();
          QVariantMap attributeData;

          Q_FOREACH(const QString & attrString, result[keyString].attributes) {
            attributeData[attrString] =
                dataNodeAttributes.namedItem(attrString).nodeValue();
          }
          if (dataNode.isText()) {
            QVariant textValue = ctx->getTextValueFromNode(dataNode);
            attributeData["NodeValue"] = textValue;
          } else {
            attributeData["NodeValue"] = QVariant();
          }

          tagData.insert(result[keyString].identifier, attributeData);
        }
      }
    }
  }

  if (docType == 1) {
    // qDebug() << Q_FUNC_INFO << "JSON Data" << data;
    QJsonParseError error;
    QJsonDocument jsonDoc =
        QJsonDocument::fromJson(QByteArray(data.toLatin1()), &error);

    if (error.error == QJsonParseError::NoError) {
      qDebug() << Q_FUNC_INFO << "No Error";
      QJsonObject rootObject = jsonDoc.object();

      Q_FOREACH(const QString & keyString, queries) {
        QJsonValue v = ctx->findJsonObject(rootObject, keyString);
        QVariantMap attributeData;
        qDebug() << Q_FUNC_INFO << v.type();
        if (v.isArray()) {
          // tagData.insert(result[keyString].identifier,
          // _object.toVariantMap());
          for (int i = 0; i < v.toArray().count(); i++) {
            QJsonValue o = v.toArray().at(i);

            Q_FOREACH(const QString & attrKey, o.toObject().keys()) {
              attributeData[attrKey] =
                  ctx->JsonValueToVariant(o.toObject()[attrKey]);
            }

            tagData.insert(result[keyString].identifier, attributeData);
          }

        } else if (v.type() == QJsonValue::String) {
          qDebug() << Q_FUNC_INFO << "String Type Found";
          attributeData[keyString] = v.toString();
          tagData.insert(result[keyString].identifier, attributeData);
        } else if (v.type() == QJsonValue::Double) {
          qDebug() << Q_FUNC_INFO << keyString
                   << " --> Object Type: " << v.type();
          qDebug() << Q_FUNC_INFO << keyString
                   << " --> Object Value: " << v.toDouble();
        } else {
          qDebug() << Q_FUNC_INFO << keyString
                   << " --> Object Type: " << v.type();
          Q_FOREACH(const QString & attrKey, v.toObject().keys()) {
            attributeData[attrKey] = ctx->JsonValueToVariant(
                v.toObject()[attrKey]); // v.toObject()[attrKey].toString();
          }

          qDebug() << Q_FUNC_INFO << "Data: -> " << attributeData;
          tagData.insert(result[keyString].identifier, attributeData);
        }
      }
    } else {
      qDebug() << Q_FUNC_INFO << "Error:" << error.errorString();
    }
  }

  result.clear();
  queries.clear();

  return tagData;
}
*/

remote_service::definition_error_t remote_service::error() const {
  return ctx->m_current_error;
}

service_input_argument *get_input_argument(tinyxml2::XMLElement *a_element) {
  service_input_argument *rv = new service_input_argument();

  const char *type_value = a_element->Attribute("type");
  const char *optional_value = a_element->Attribute("optional");
  const char *default_value = a_element->Attribute("default");
  const char *txt_node_value = a_element->GetText();

  if (!txt_node_value)
    return rv;

  // todo : is this needed ?
  rv->set_type(kStringType);
  rv->set_value(txt_node_value);

  if (optional_value) {
    if (strcmp(optional_value, "true") == 0) {
      rv->set_optional(1);
    } else {
      rv->set_optional(0);
    }
  } else {
    rv->set_optional(0);
  }

  if (default_value) {
    rv->set_default_value(default_value);
  }

  /*
  std::cout << __FUNCTION__ << " ARG :  ....... [" << txt_node_value << "]"
            << std::endl;
  */
  return rv;
}

service_input *get_service_input(tinyxml2::XMLElement *a_element) {
  service_input *rv = new service_input();

  tinyxml2::XMLElement *element = a_element->FirstChildElement("input");

  if (element) {
    // get the <input url=xxx request=GET|POST>"
    const char *url_value = element->Attribute("url");
    const char *request_value = element->Attribute("type");

    /* load url */
    if (url_value) {
      rv->set_url(url_value);
      /*
      std::cout << __FUNCTION__ << " URL :  ....... [" << rv->url() << "]"
                << std::endl;
      */
    }
    /* load type */
    if (request_value) {
      if (strcmp(request_value, "GET") == 0) {
        rv->set_request_type(service_input::kGETRequest);
      } else if (strcmp(request_value, "POST") == 0) {
        rv->set_request_type(service_input::kPOSTRequest);
      }
      /*
      std::cout << __FUNCTION__ << " Type :  ....... [" << rv->request_type()
                << "]" << std::endl;
      */
    }
    /* load input query*/
    tinyxml2::XMLElement *args = element->FirstChildElement("arg");

    while (args != 0) {
      service_input_argument *arg = get_input_argument(args);
      rv->insert_argument(arg);
      args = args->NextSiblingElement("arg");
    }
  }

  return rv;
}

service_result_query_attribute *
get_service_result_query_attribute(tinyxml2::XMLElement *a_element) {
  service_result_query_attribute *rv = new service_result_query_attribute();
  const char *type_value = a_element->Attribute("type");
  const char *txt_value = a_element->GetText();

  if (type_value) {
    if (strcmp(type_value, "string")) {
      rv->set_type(kStringType);
    } else if (strcmp(type_value, "bool")) {
      rv->set_type(kBooleanType);
    } else if (strcmp(type_value, "real")) {
      rv->set_type(kRealType);
    } else if (strcmp(type_value, "integer")) {
      rv->set_type(kIntType);
    } else {
      rv->set_type(kUnknown);
    }
  }

  if (txt_value) {
    rv->set_value(txt_value);
  } else {
    rv->set_value(std::string());
  }

  return rv;
}

void get_service_result_query(tinyxml2::XMLElement *a_element,
                              service_result *a_result) {
  tinyxml2::XMLElement *element = a_element->FirstChildElement("query");

  while (element != 0) {
    const char *name_value = element->Attribute("name");
    const char *tag_value = element->Attribute("tag");
    service_result_query *rv = new service_result_query();

    if (name_value) {
      rv->set_name(name_value);
    }

    if (tag_value) {
      rv->set_tag_name(tag_value);
    }

    tinyxml2::XMLElement *attr_element = element->FirstChildElement("attr");

    while (attr_element != 0) {
      service_result_query_attribute *attribute =
          get_service_result_query_attribute(attr_element);

      if (attribute)
        rv->insert_attribute(attribute);

      attr_element = attr_element->NextSiblingElement("attr");
    }

    if (a_result) {
      a_result->insert(rv);
    }
    element = element->NextSiblingElement("query");
  }
}

service_result *get_service_result(tinyxml2::XMLElement *a_element) {
  service_result *rv = new service_result();

  tinyxml2::XMLElement *element = a_element->FirstChildElement("result");

  if (element) {
    const char *format_value = element->Attribute("format");

    if (format_value) {
      if (strcmp(format_value, "xml") == 0) {
        rv->set_result_format(service_result::kXMLData);
      } else if (strcmp(format_value, "json") == 0) {
        rv->set_result_format(service_result::kJSONData);
      } else if (strcmp(format_value, "binary") == 0) {
        rv->set_result_format(service_result::kBinaryData);
      } else {
        rv->set_result_format(service_result::kUnknown);
      }
    }

    get_service_result_query(element, rv);
  }

  return rv;
}

void remote_service::load_services() {
  if (error() != kNoError)
    return;

  tinyxml2::XMLElement *root =
      ctx->m_xml_root_doc.FirstChildElement("services");

  if (!root) {
    // std::cout << "Error No Root Element" << std::endl;
    ctx->m_current_error = kDefinitionLoadError;
    return;
  }

  tinyxml2::XMLElement *service_element = root->FirstChildElement("service");

  while (service_element != 0) {
    const char *value = service_element->Attribute("name");

    if (value) {
      // std::cout << __FUNCTION__ << " Load Eelement ....... [" << value << "]"
      //          << std::endl;

      service *srv = new service();
      srv->set_name(value);

      /* read service input definition */
      service_input *input = get_service_input(service_element);
      srv->set_input(input);

      /* read service result definition */
      service_result *result = get_service_result(service_element);
      srv->set_service_result(result);

      /* read service error definition */
      // todo

      /* cache parsed results */
      ctx->m_service_dict[std::string(value)] = srv;
    }

    service_element = service_element->NextSiblingElement("service");
  }
}

std::string remote_service::data_prefix() const
{
#ifdef __WINDOWS__
   return std::string();
#endif

#ifdef __GNU_LINUX_PLATFORM__
   return std::string(std::string(PLEXYPREFIX) + "/share/social/");
#endif
}

service_input *service::input() const { return m_input; }

void service::set_input(service_input *input) { m_input = input; }
std::string service_input::url() const { return m_url; }

void service_input::set_url(const std::string &url) { m_url = url; }
input_arg_t service_input::argument_list() const { return m_argument_list; }

service_input::request_type_t service_input::request_type() const {
  return m_request_type;
}

void
service_input::set_request_type(service_input::request_type_t request_type) {
  m_request_type = request_type;
}

bool service_input_argument::optional() const { return m_optional; }

void service_input_argument::set_optional(bool optional) {
  m_optional = optional;
}
std::string service_input_argument::value() const { return m_value; }

void service_input_argument::set_value(const std::string &value) {
  m_value = value;
}

service_data_type_t service_input_argument::type() const { return m_type; }

void service_input_argument::set_type(const service_data_type_t &type) {
  m_type = type;
}

std::string service_input_argument::default_value() const {
  return m_default_value;
}

void
service_input_argument::set_default_value(const std::string &default_value) {
  m_default_value = default_value;
}

std::string service_result_query_attribute::value() const { return m_value; }

void service_result_query_attribute::set_value(const std::string &value) {
  m_value = value;
}

service_data_type_t service_result_query_attribute::type() const {
  return m_type;
}

void service_result_query_attribute::set_type(const service_data_type_t &type) {
  m_type = type;
}

remote_result::remote_result() {}

remote_result::remote_result(const remote_result &a_copy) {
  m_query_list = a_copy.m_query_list;
}

remote_result::~remote_result() { m_query_list.clear(); }

void remote_result::insert(const remote_result_data &a_data) {
  m_query_list.push_back(a_data);
}

result_list_t remote_result::get(const std::string &a_name) const {
  result_list_t rv;
  std::for_each(std::begin(m_query_list), std::end(m_query_list),
                [&](remote_result_data query) {
    if (a_name == query.name())
      rv.push_back(query);
  });

  return rv;
}

remote_result_data::remote_result_data() {}

remote_result_data::~remote_result_data() {}

remote_data_attribute
remote_result_data::get(const std::string &a_attribute_name) {
  remote_data_attribute rv;
  std::for_each(std::begin(m_propery_list), std::end(m_propery_list),
                [&](remote_data_attribute attribute) {
    if (a_attribute_name == attribute.key()) {
      rv = attribute;
    }
  });

  return rv;
}

remote_data_attribute::remote_data_attribute() {}

remote_data_attribute::~remote_data_attribute() {}

remote_data_attribute::property_type_t remote_data_attribute::type() const {
  return m_value_type;
}

void
remote_data_attribute::set_type(remote_data_attribute::property_type_t a_type) {
  m_value_type = a_type;
}

std::string remote_data_attribute::key() const { return m_key; }

void remote_data_attribute::set_key(const std::string &a_key) { m_key = a_key; }

std::string remote_data_attribute::value() const { return m_value; }

void remote_data_attribute::set_value(const std::string &a_value) {
  m_value = a_value;
}
}
