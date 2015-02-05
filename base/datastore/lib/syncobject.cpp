#include "syncobject.h"

#include <QDateTime>
#include <QMap>
#include <QVariant>
#include <QStringList>
#include <QDebug>

namespace QuetzalKit
{

class SyncObject::PrivateSyncObject
{
public:
  PrivateSyncObject() {}
  ~PrivateSyncObject()
  {
    // qDeleteAll(mChildList);
    // todo : delete later;
  }

  void addChildNode(SyncObject *object);

  uint mTimeStamp;
  QMap<QString, QVariant> mPropertyMap;
  /// QList<SyncObject*> mChildList;
  QMap<uint, SyncObject *> mChildMap;
  SyncObject *mParent;
  uint mCount;

  QDomNode mNode;
  QString mName;
  QString mTextData;
  uint mKey;

  DataStore *mDataStore;
};

void SyncObject::updateTimeStamp()
{
  d->mTimeStamp = QDateTime::currentDateTime().currentMSecsSinceEpoch();
}

void SyncObject::removeObject(uint key)
{
  // todo;
}

QDomNode SyncObject::node() { return d->mNode; }

void SyncObject::setDomNode(const QDomNode &node) { d->mNode = node; }

SyncObject::SyncObject(QObject *parent)
  : QObject(parent), d(new PrivateSyncObject)
{
  updateTimeStamp();
  d->mKey = -1;
  d->mCount = -1;
  d->mParent = 0;
  d->mDataStore = 0;
}

SyncObject::~SyncObject()
{
  // qDebug() << Q_FUNC_INFO << name();
  delete d;
}

uint SyncObject::timeStamp() const { return d->mTimeStamp; }

void SyncObject::setTimeStamp(uint timestamp) { d->mTimeStamp = timestamp; }

uint SyncObject::updatedTimeStamp() const
{
  return QDateTime::currentDateTime().currentMSecsSinceEpoch();
}

void SyncObject::setName(const QString &name) { d->mName = name; }

QString SyncObject::name() const { return d->mName; }

void SyncObject::setKey(uint key) { d->mKey = key; }

uint SyncObject::key() const { return d->mKey; }

SyncObject *SyncObject::parentObject() const { return d->mParent; }

void SyncObject::setParentObject(SyncObject *parent) { d->mParent = parent; }

void SyncObject::setObjectAttribute(const QString &name,
                                    const QVariant &value)
{
  d->mPropertyMap[name] = value;

  if (!d->mNode.isNull()) {
    QDomElement elem = d->mNode.toElement();
    elem.setAttribute(name, value.toString());
  } else {
    // qFatal("Failed to convert the node to an element");
  }
  updateTimeStamp();
}

void SyncObject::attachTextNode(const QString &data)
{
  d->mTextData = data;

  if (!d->mNode.isNull()) {
    if (d->mNode.hasChildNodes()) {
      for (int i = 0; i < d->mNode.childNodes().count(); i++) {
        QDomNode node = d->mNode.childNodes().at(i);
        if (node.isText()) {
          QDomText textNode = node.toText();
          textNode.setNodeValue(data);

          if (d->mDataStore) {
            d->mDataStore->updateNode(this);
          }

          return;
        }
      }
    }

    QDomDocument doc = d->mNode.ownerDocument();
    QDomText textNode = doc.createTextNode(data);

    d->mNode.appendChild(textNode);
  } else {
    qFatal("Failed to convert the node to an element");
  }

  Q_EMIT updated();

  if (d->mDataStore) {
    d->mDataStore->updateNode(this);
  }

  updateTimeStamp();
}

void SyncObject::setTextData(const QString &data) { d->mTextData = data; }

QString SyncObject::textData() const { return d->mTextData; }

SyncObject *SyncObject::clone(SyncObject *object)
{
  if (!object) {
    return 0;
  }

  SyncObject *rv = createNewObject(object->name());

  Q_FOREACH(const QString & key, object->attributes()) {
    if (key == "key") {
      continue;
    }
    rv->setObjectAttribute(key, object->attributeValue(key));
  }

  // attach any text nodes
  rv->attachTextNode(object->textData());

  return rv;
}

void SyncObject::setDataStore(DataStore *store) { d->mDataStore = store; }

DataStore *SyncObject::store() { return d->mDataStore; }

QStringList SyncObject::attributes() const { return d->mPropertyMap.keys(); }

QVariant SyncObject::attributeValue(const QString &name) const
{
  return d->mPropertyMap[name];
}

QList<SyncObject *> SyncObject::childObjects() const
{
  return d->mChildMap.values();
}

SyncObject *SyncObject::childObject(uint key)
{
  if (d->mChildMap.keys().contains(key)) {
    return d->mChildMap[key];
  }

  return 0;
}

SyncObject *SyncObject::createNewObject(const QString &name)
{
  SyncObject *rv = new SyncObject(this);
  rv->setName(name);
  d->mCount = d->mCount + 1;
  rv->setKey(d->mCount);

  if (!d->mNode.isNull()) {
    // qDebug() << Q_FUNC_INFO << "Create Element";
    QDomElement childNode = d->mNode.ownerDocument().createElement(name);

    childNode.setAttribute("key", d->mCount);
    childNode.setAttribute("timestamp", rv->timeStamp());
    d->mNode.appendChild(childNode);

    rv->setDomNode(childNode);

    updateTimeStamp();
  } else {
    qDebug() << Q_FUNC_INFO << this->name();
    qFatal("No Node Found");
  }

  rv->setParent(this);
  rv->setParentObject(this);
  d->mChildMap[d->mCount] = rv;

  rv->setDataStore(d->mDataStore);
  return rv;
}

QString SyncObject::dumpContent() const
{
  qDebug() << Q_FUNC_INFO << d->mNode.ownerDocument().toString();
  return d->mNode.ownerDocument().toString();
}

bool SyncObject::contains(SyncObject *object)
{
  if (!this->hasChildren()) {
    qDebug() << Q_FUNC_INFO
             << "Error: No Children for Object : " << object->name();
    return false;
  }

  Q_FOREACH(SyncObject * child, d->mChildMap.values()) {
    if (!child) {
      continue;
    }

    if (child->similarObject(object)) {
      return true;
    }
  }

  qDebug() << Q_FUNC_INFO << "No Object found for Object:" << object->name();
  return false;
}

bool SyncObject::similarObject(SyncObject *object)
{
  if (object->name() != this->name()) {
    return false;
  }

  /*j
  if (object->key() != this->key()) {
      return false;
  }
  */

  // check properties
  for (int p = 0; p < object->attributes().count(); p++) {
    QString prop = object->attributes().at(p);

    if (prop == "timestamp" || prop == "key") {
      continue;
    }

    if (!this->attributes().contains(prop)) {
      // qDebug() << Q_FUNC_INFO << "Missing Property:" << prop;
      return false;
    }

    if (this->attributeValue(prop) != object->attributeValue(prop)) {
      // qDebug() << Q_FUNC_INFO << "Property Missmatch:" << prop;
      return false;
    }
    // qDebug() << Q_FUNC_INFO << "Check : " << prop << " = " <<
    // object->value(prop);
  }

  return true;
}

void SyncObject::replace(SyncObject *object)
{
  if (!object) {
    return;
  }
  QDomNode node = object->node();

  if (!node.isNull()) {
    Q_FOREACH(const QString & prop, object->attributes()) {
      // qDebug() << Q_FUNC_INFO << "Replace Property: " << prop;
      if (prop == "key") {
        continue;
      }
      QDomElement elm = node.toElement();
      elm.setAttribute(prop, object->attributeValue(prop).toString());
    }
  }
}

SyncObject *SyncObject::childObject(const QString &name)
{
  if (!this->hasChildren()) {
    SyncObject *rv = createNewObject(name);
    return rv;
  }

  Q_FOREACH(SyncObject * object, d->mChildMap.values()) {
    if (!object) {
      continue;
    }
    if (object->name() == name) {
      return object;
    }
  }

  return 0;
}

void SyncObject::linksToObject(const QString &dataStoreName,
                               const QString &objectName) {}

/*
SyncObject SyncObject::linkedObject() const
{
    SyncObject object;
    return object;
}
*/

bool SyncObject::hasChildren() const
{
  if (d->mChildMap.keys().count() > 0) {
    return true;
  }
  return false;
}

uint SyncObject::childCount() const { return d->mChildMap.keys().count(); }

void SyncObject::addChildObject(SyncObject *object)
{
  d->mCount += 1;
  uint currentKey = d->mCount;

  if (!d->mChildMap.keys().contains(object->key())) {
    d->mChildMap[currentKey] = object;
    // object->setKey(currentKey);
    object->setParentObject(this);
    object->setParent(this);
  } else {
    SyncObject *tmpObject = d->mChildMap[object->key()];
    d->mChildMap.remove(object->key());

    if (tmpObject) {
      delete tmpObject;
    }

    d->mChildMap[object->key()] = object;
    object->setParentObject(this);
    object->setParent(this);
  }
  updateTimeStamp();
}

void SyncObject::PrivateSyncObject::addChildNode(SyncObject *object)
{
  if (!object) {
    return;
  }

  if (!mNode.isNull()) {
    QDomDocument doc = mNode.ownerDocument();

    QDomNode node = doc.createElement(object->name());
    mNode.appendChild(node);
  }
}
}
