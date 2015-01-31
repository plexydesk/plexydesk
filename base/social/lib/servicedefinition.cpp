#include "servicedefinition.h"

#include <QDomDocument>
#include <QDomAttr>
#include <QDomNode>
#include <QDomNodeList>
#include <QFile>
#include <QUrlQuery>
#include <QXmlQuery>
#include <QXmlResultItems>
#include <QXmlSerializer>
#include <QBuffer>
#include <QIODevice>
#include <QJsonDocument>
#include <QtGui>

#include <QDebug>

namespace QuetzalSocialKit {
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

class ServiceDefinition::PrivateServiceDefinition {
public:
  PrivateServiceDefinition() {}
  ~PrivateServiceDefinition() {
    // qDebug() << Q_FUNC_INFO;
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
};

ServiceDefinition::ServiceDefinition(const QString &input, QObject *parent)
    : QObject(parent), d(new PrivateServiceDefinition) {
  QFile inputFile(input);

  if (inputFile.exists() && inputFile.open(QIODevice::ReadOnly)) {
    QString errorMessage;
    int errorLine, errorColumn;
    if (d->mRootDoc.setContent(&inputFile, &errorMessage, &errorLine,
                               &errorColumn)) {
      this->buildServiceDefs();
    } else {
      qWarning() << Q_FUNC_INFO << "Failed to parse file:" << errorMessage
                 << " Line: " << errorLine << "Column: " << errorColumn;
    }
  } else {
    qWarning() << Q_FUNC_INFO << "No Input File Found at: " << input;
  }

  inputFile.close();
}

ServiceDefinition::~ServiceDefinition() { delete d; }

QStringList ServiceDefinition::knownServices() const {
  return d->mSeriviceMap.keys();
}

// fetch the url from the <input> tag
QString ServiceDefinition::endpoint(const QString &name) const {
  QDomNode node = d->definitionsOfServiceName(name)["input"];
  QDomAttr attr = d->getAttributeFromNode(node, "url");

  return attr.nodeValue();
}

uint ServiceDefinition::requestType(const QString &name) const {
  QDomNode node = d->definitionsOfServiceName(name)["input"];
  QDomAttr attr = d->getAttributeFromNode(node, "type");

  QString value = attr.nodeValue();

  if (value.toLower() == "get")
    return 0;

  if (value.toLower() == "post")
    return 1;

  return -1;
}

QStringList ServiceDefinition::arguments(const QString &name) const {
  return d->mInputArguments[name].keys();
}

QStringList ServiceDefinition::optionalArguments(const QString &name) const {
  return d->mOptionalInputArgument[name].keys();
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

QMultiMap<QString, QVariantMap> ServiceDefinition::queryResult(
    const QString &method, const QString &data) const {
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

  if (root.keys().contains(key))
    return root[key];

  Q_FOREACH(const QString & subKey, root.keys()) {
    if (root[subKey].isObject()) {
      QJsonObject v = root[subKey].toObject();
      return findJsonObject(v, key);
    }
  }

  return rv;
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

int ServiceDefinition::PrivateServiceDefinition::nativeType(
    const QString &type) {
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
    if (resultType == "json")
      return 1;
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
}
