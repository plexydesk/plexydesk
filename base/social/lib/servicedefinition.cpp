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
#include "servicedefinition.h"

#include <QBuffer>
#include <QDomAttr>
#include <QDomDocument>
#include <QDomNode>
#include <QDomNodeList>
#include <QFile>
#include <QIODevice>
#include <QJsonDocument>
#include <QUrlQuery>
#include <QtGui>

#include <QDebug>

#include <sys/stat.h>

#include <algorithm>
#include <iostream>
#include <string>
#include <tinyxml2.h>

namespace social_kit {
typedef QMap<QString, QDomNode> DefinitionMapType;

class PrivateResultQuery {
public:
  PrivateResultQuery() {}
  ~PrivateResultQuery() {
    // qDebug() << Q_FUNC_INFO;
  }

  QString format;
  QDomNode mDataNode;
  QString filter;
  QStringList attributes;
  QStringList values;
  QString mNodeName;
  QString tagName;
  QString identifier;
};

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
  ~service_input_argument() { qDebug() << Q_FUNC_INFO; }

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
    qDebug() << Q_FUNC_INFO;
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
    kXMLData,
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
    qDebug() << Q_FUNC_INFO;
    if (m_input)
      delete m_input;

    if (m_result)
      delete m_result;
  }

  service_input *input() const;
  void set_input(service_input *input);

  service_result *result() const;
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

class ServiceDefinition::PrivateServiceDefinition {
public:
  PrivateServiceDefinition() : m_current_error(ServiceDefinition::kNoError) {}
  ~PrivateServiceDefinition() {
    for (service_map_iterator_t iterator = m_service_dict.begin();
         iterator != m_service_dict.end(); iterator++) {

      if (iterator->second)
        delete iterator->second;
    }

    m_service_dict.clear();
  }

  DefinitionMapType definitionsOfServiceName(const QString &name);

  QDomAttr getAttributeFromNode(const QDomNode &node, const QString &key) const;
  QString getTextValueFromNode(const QDomNode &node) const;

  void buildServiceInputDefs(const QString &name);

  bool hasDefaultValue(const QString &service, const QString &key) const;

  QString defaultValue(const QString &service, const QString &key) const;

  int nativeType(const QString &type);

  void buildArgTypes();

  uint documentType(const QString &method);

  QVariant JsonValueToVariant(const QJsonValue &object);

  QJsonValue findJsonObject(const QJsonObject &root, const QString &key);

  QHash<QString, PrivateResultQuery> queryForMethod(const QString &method);

  QDomDocument mRootDoc;
  QHash<QString, QDomNode> mSeriviceMap;

  QHash<QString, DefinitionMapType> mDefMap;
  QHash<QString, DefinitionMapType> mInputArguments;
  QHash<QString, DefinitionMapType> mOptionalInputArgument;
  QHash<QString, DefinitionMapType> mDefaultInputArgument;

  QMap<QString, int> mTypeMap;

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

ServiceDefinition::ServiceDefinition(const QString &input, QObject *parent)
    : QObject(parent), d(new PrivateServiceDefinition) {
  if (ck_file_exisits(input.toLatin1().data())) {
    tinyxml2::XMLError error =
        d->m_xml_root_doc.LoadFile(input.toStdString().c_str());
    if (error != tinyxml2::XML_NO_ERROR) {
      std::cout << __LINE__ << " : " << __FUNCTION__ << " Error "
                << d->m_xml_root_doc.GetErrorStr1() << std::endl;
      d->m_current_error = kDefinitionLoadError;
    } else {
      d->m_current_error = kNoError;
      load_services();
    }
  } else {
    qWarning() << Q_FUNC_INFO << "No Input File Found at: " << input;
    std::cout << __LINE__ << " : " << __FUNCTION__ << " Error "
              << "Service Definistion File Not Found : " << input.toStdString()
              << std::endl;
    d->m_current_error = kDefinitionLoadError;
  }
}

ServiceDefinition::~ServiceDefinition() { delete d; }

QStringList ServiceDefinition::knownServices() const {
  QStringList rv;
  for (service_map_iterator_t iterator = d->m_service_dict.begin();
       iterator != d->m_service_dict.end(); iterator++) {
    rv << QString::fromStdString(iterator->first);
  }

  return rv;
}

// fetch the url from the <input> tag
QString ServiceDefinition::endpoint(const QString &name) const {
  QString rv;

  service *srv = d->m_service_dict[name.toStdString()];

  if (srv && srv->input()) {
    rv = QString::fromStdString(srv->input()->url());
  }

  return rv;
}

uint ServiceDefinition::requestType(const QString &name) const {
  uint rv = service_input::kUndefinedRequest;

  service *srv = d->m_service_dict[name.toStdString()];

  if (srv && srv->input()) {
    rv = (uint)srv->input()->request_type();
  }

  return rv;
}

QStringList ServiceDefinition::arguments(const QString &name) const {
  QStringList rv;
  service *srv = d->m_service_dict[name.toStdString()];

  if (srv && srv->input()) {
    std::vector<service_input_argument *> argument_list =
        srv->input()->argument_list();
    std::for_each(std::begin(argument_list), std::end(argument_list),
                  [&](service_input_argument *arg) {
      if (arg->optional())
        return;

      rv << QString::fromStdString(arg->value());
    });
  }

  return rv;
}

std::vector<std::string>
ServiceDefinition::input_arguments(const std::string &a_name, bool a_optional) {
  std::vector<std::string> rv;
  service *srv = d->m_service_dict[a_name];

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

QStringList ServiceDefinition::optionalArguments(const QString &name) const {
  QStringList rv;
  service *srv = d->m_service_dict[name.toStdString()];

  if (srv && srv->input()) {
    std::vector<service_input_argument *> argument_list =
        srv->input()->argument_list();
    std::for_each(std::begin(argument_list), std::end(argument_list),
                  [&](service_input_argument *arg) {
      if (!arg->optional())
        return;

      rv << QString::fromStdString(arg->value());
    });
  }

  return rv;
}

QString ServiceDefinition::argumentType(const QString &serviceName,
                                        const QString &argument) const {
  QDomNode node = d->mInputArguments[serviceName][argument];

  if (node.hasAttributes()) {
    QDomAttr attr = d->getAttributeFromNode(node, "type");
    return attr.nodeValue();
  }

  return "";
}

QUrl ServiceDefinition::queryURL(const QString &method,
                                 const QVariantMap &data) const {
  QUrl url;
  QString sUrl = endpoint(method);
  QUrl serviceURL = QUrl(sUrl);

  QUrlQuery queryArg;

  Q_FOREACH(const QString & key, d->mDefaultInputArgument[method].keys()) {
    queryArg.addQueryItem(key, d->defaultValue(method, key));
  }

  Q_FOREACH(const QString & key, d->mInputArguments[method].keys()) {
    // a given argument is invalid if the mandertory arguments are not
    // set. so we return since the required key=value pares are missing in the
    // input data (data).
    if (!data.keys().contains(key)) {
      return url;
    }

    if (d->hasDefaultValue(method, key)) {
      queryArg.addQueryItem(key, d->defaultValue(method, key));
    } else {
      queryArg.addQueryItem(key, data[key].toString());
    }
  }

  // load the optional data provided
  Q_FOREACH(const QString & key, data.keys()) {
    if (!d->mInputArguments[method].keys().contains(key)) {
      queryArg.addQueryItem(key, data[key].toString());
    }
  }

  serviceURL.setQuery(queryArg);
  return serviceURL;
}

std::string
ServiceDefinition::service_url(const std::string &a_method,
                               service_query_parameters *a_params) const {
  std::string rv;

  rv = endpoint(a_method.c_str()).toStdString();

  if (a_params) {
    /*
    std::vector<std::string> input_keys = a_params->keys();

    bool key_found = std::find(input_keys.begin(), input_keys.end(), item) !=
    input_keys.end();

    if (key_found) {

    }
    */

    service *srv = d->m_service_dict.at(a_method);

    if (srv && srv->input()) {
      service_input *input = srv->input();
    }
  }

  return rv;
}

QMultiMap<QString, QVariantMap>
ServiceDefinition::queryResult(const QString &method,
                               const QString &data) const {
  // QVariantMap rv;

  QHash<QString, PrivateResultQuery> result = d->queryForMethod(method);
  QStringList queries = result.keys();

  QMultiMap<QString, QVariantMap> tagData;

  uint docType = d->documentType(method);

  qDebug() << Q_FUNC_INFO << "Doc Type :" << (docType);

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
            QVariant textValue = d->getTextValueFromNode(dataNode);
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
        QJsonValue v = d->findJsonObject(rootObject, keyString);
        QVariantMap attributeData;
        qDebug() << Q_FUNC_INFO << v.type();
        if (v.isArray()) {
          // tagData.insert(result[keyString].identifier,
          // _object.toVariantMap());
          for (int i = 0; i < v.toArray().count(); i++) {
            QJsonValue o = v.toArray().at(i);

            Q_FOREACH(const QString & attrKey, o.toObject().keys()) {
              attributeData[attrKey] =
                  d->JsonValueToVariant(o.toObject()[attrKey]);
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
            attributeData[attrKey] = d->JsonValueToVariant(
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

ServiceDefinition::definition_error_t ServiceDefinition::error() const {
  return d->m_current_error;
}

QVariant ServiceDefinition::PrivateServiceDefinition::JsonValueToVariant(
    const QJsonValue &object) {
  QVariant rv;

  switch (object.type()) {
  case QJsonValue::Double:
    rv = object.toDouble();
    break;
  case QJsonValue::String:
    rv = object.toString();
    break;
  case QJsonValue::Bool:
    rv = object.toBool();
  default:
    qDebug() << Q_FUNC_INFO << "Unknown Type Found";
  }

  qDebug() << Q_FUNC_INFO << rv;
  return rv;
}

QJsonValue ServiceDefinition::PrivateServiceDefinition::findJsonObject(
    const QJsonObject &root, const QString &key) {
  QJsonValue rv;

  if (root.keys().contains(key)) {
    return root[key];
  }

  Q_FOREACH(const QString & subKey, root.keys()) {
    if (root[subKey].isObject()) {
      QJsonObject v = root[subKey].toObject();
      return findJsonObject(v, key);
    }
  }

  return rv;
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

  std::cout << __FUNCTION__ << " ARG :  ....... [" << txt_node_value << "]"
            << std::endl;
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
      std::cout << __FUNCTION__ << " URL :  ....... [" << rv->url() << "]"
                << std::endl;
    }
    /* load type */
    if (request_value) {
      if (strcmp(request_value, "GET") == 0) {
        rv->set_request_type(service_input::kGETRequest);
      } else if (strcmp(request_value, "POST") == 0) {
        rv->set_request_type(service_input::kPOSTRequest);
      }
      std::cout << __FUNCTION__ << " Type :  ....... [" << rv->request_type()
                << "]" << std::endl;
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

service_result_query *
get_service_result_query(tinyxml2::XMLElement *a_element) {
  service_result_query *rv = new service_result_query();

  tinyxml2::XMLElement *element = a_element->FirstChildElement("query");

  while (element != 0) {
    const char *name_value = element->Attribute("name");
    const char *tag_value = element->Attribute("tag");

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

    element = element->NextSiblingElement("query");
  }

  return rv;
}

service_result *get_service_result(tinyxml2::XMLElement *a_element) {
  service_result *rv = new service_result();

  tinyxml2::XMLElement *element = a_element->FirstChildElement("result");

  if (element) {
    const char *format_value = element->Attribute("format");

    if (format_value) {
      if (strcmp(format_value, "xml")) {
        rv->set_result_format(service_result::kXMLData);
      } else if (strcmp(format_value, "json")) {
        rv->set_result_format(service_result::kJSONData);
      } else if (strcmp(format_value, "binary")) {
        rv->set_result_format(service_result::kBinaryData);
      } else {
        rv->set_result_format(service_result::kUnknown);
      }
    }
  }

  return rv;
}

void ServiceDefinition::load_services() {
  if (error() != kNoError)
    return;

  tinyxml2::XMLElement *root = d->m_xml_root_doc.FirstChildElement("services");

  if (!root) {
    std::cout << "Error No Root Element" << std::endl;
    d->m_current_error = kDefinitionLoadError;
    return;
  }

  tinyxml2::XMLElement *service_element = root->FirstChildElement("service");

  while (service_element != 0) {
    const char *value = service_element->Attribute("name");

    if (value) {
      std::cout << __FUNCTION__ << " Load Eelement ....... [" << value << "]"
                << std::endl;
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
      d->m_service_dict[std::string(value)] = srv;
    }

    service_element = service_element->NextSiblingElement("service");
  }
}

void ServiceDefinition::buildServiceDefs() {
  QDomNodeList serviceNodes = d->mRootDoc.elementsByTagName("service");

  for (int i = 0; i < serviceNodes.count(); i++) {
    QDomNode node = serviceNodes.at(i);
    if (node.hasAttributes()) {

      QDomAttr attr = d->getAttributeFromNode(node, "name");
      d->mSeriviceMap[attr.nodeValue()] = node;

      // build input def;
      if (node.hasChildNodes()) {
        DefinitionMapType map;
        QDomNodeList definitionList = node.childNodes();

        for (int j = 0; j < definitionList.count(); j++) {
          QDomNode defNode = definitionList.at(j);
          map[defNode.nodeName()] = defNode;
        }

        d->mDefMap[attr.nodeValue()] = map;
      }
    }
  }

  Q_FOREACH(const QString & serviceName, this->knownServices()) {
    d->buildServiceInputDefs(serviceName);
  }
}

DefinitionMapType
ServiceDefinition::PrivateServiceDefinition::definitionsOfServiceName(
    const QString &name) {
  return mDefMap[name];
}

QDomAttr ServiceDefinition::PrivateServiceDefinition::getAttributeFromNode(
    const QDomNode &node, const QString &key) const {
  QDomNamedNodeMap attrMap = node.attributes();
  return attrMap.namedItem(key).toAttr();
}

QString ServiceDefinition::PrivateServiceDefinition::getTextValueFromNode(
    const QDomNode &node) const {
  QString rv;

  if (node.hasChildNodes()) {
    QDomNode textNode = node.childNodes().at(0);
    if (!textNode.isNull() && textNode.isText()) {
      rv = textNode.nodeValue();
    }
  }

  return rv;
}

void ServiceDefinition::PrivateServiceDefinition::buildServiceInputDefs(
    const QString &name) {
  QDomNode node = definitionsOfServiceName(name)["input"];
  QDomNodeList childNodes = node.childNodes();

  DefinitionMapType map;
  DefinitionMapType OptionalMap;
  DefinitionMapType defaultValueMap;

  for (int i = 0; i < childNodes.count(); i++) {
    QDomNode argNode = childNodes.at(i);
    QDomText text = argNode.childNodes().at(0).toText();
    QDomAttr optionalAttr = getAttributeFromNode(argNode, "optional");
    QDomAttr defaultAttr = getAttributeFromNode(argNode, "default");

    if (!defaultAttr.isNull()) {
      defaultValueMap[text.nodeValue()] = argNode;
      continue;
    }

    if (optionalAttr.nodeValue() == "true") {
      OptionalMap[text.nodeValue()] = argNode;
    } else {
      map[text.nodeValue()] = argNode;
    }
  }

  mInputArguments[name] = map;
  mOptionalInputArgument[name] = OptionalMap;
  mDefaultInputArgument[name] = defaultValueMap;
}

bool ServiceDefinition::PrivateServiceDefinition::hasDefaultValue(
    const QString &service, const QString &key) const {
  return mDefaultInputArgument[service].keys().contains(key);
}

QString ServiceDefinition::PrivateServiceDefinition::defaultValue(
    const QString &service, const QString &key) const {
  if (hasDefaultValue(service, key)) {
    QDomNode node = mDefaultInputArgument[service][key];
    QDomAttr defaultValue = getAttributeFromNode(node, "default");

    if (!defaultValue.isNull()) {
      return defaultValue.nodeValue();
    }
  }
  return "";
}

int
ServiceDefinition::PrivateServiceDefinition::nativeType(const QString &type) {
  return mTypeMap[type];
}

void ServiceDefinition::PrivateServiceDefinition::buildArgTypes() {
  mTypeMap["string"] = 1;
  mTypeMap["int"] = 2;
  mTypeMap["flaot"] = 3;
  mTypeMap["bool"] = 4;
  mTypeMap["binary_base64"] = 5;
}

uint ServiceDefinition::PrivateServiceDefinition::documentType(
    const QString &method) {
  QDomNode resultNode = definitionsOfServiceName(method)["result"];
  QDomNodeList queryNodeList = resultNode.childNodes();

  QDomAttr typeAttr = getAttributeFromNode(resultNode, "format");

  if (!typeAttr.isNull()) {
    QString resultType = typeAttr.value();
    if (resultType == "json") {
      return 1;
    }
  }

  return 0;
}

QHash<QString, PrivateResultQuery>
ServiceDefinition::PrivateServiceDefinition::queryForMethod(
    const QString &method) {
  QHash<QString, PrivateResultQuery> mQueryData;
  QDomNode resultNode = definitionsOfServiceName(method)["result"];
  QDomNodeList queryNodeList = resultNode.childNodes();

  QDomAttr typeAttr = getAttributeFromNode(resultNode, "format");

  if (!typeAttr.isNull()) {
    for (int i = 0; i < queryNodeList.count(); i++) {
      QDomNode queryNode = queryNodeList.at(i);
      QDomAttr queryNodeName = getAttributeFromNode(queryNode, "name");
      QDomAttr queryTagName = getAttributeFromNode(queryNode, "tag");
      PrivateResultQuery result;

      result.tagName = queryTagName.nodeValue();
      result.identifier = queryNodeName.nodeValue();

      // get attributes tags <attr></attr>
      QDomNodeList attributeNodeList = queryNode.childNodes();

      for (int j = 0; j < attributeNodeList.count(); j++) {
        QDomNode attrNode = attributeNodeList.at(j);

        if (attrNode.nodeName() == "attr") {
          result.attributes << getTextValueFromNode(attrNode);
        }

        if (attrNode.nodeName() == "value") {
          result.values << getTextValueFromNode((attrNode));
        }
      }

      mQueryData[queryNodeName.nodeValue()] = result;
    }
  }

  return mQueryData;
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
}
