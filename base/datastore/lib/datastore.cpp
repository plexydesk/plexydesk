#include "datastore.h"
#include <syncobject.h>
#include <syncengineinterface.h>
#include <QStringList>
#include <QDomDocument>
#include <QDomElement>
#include <QDebug>

namespace QuetzalKit {

class DataStore::PrivateDataStore {
public:
  PrivateDataStore() {}

  ~PrivateDataStore() {
    if (mDomDocument)
      delete mDomDocument;
  }

  QDomElement addChildObject(const SyncObject *object, QDomElement *parent);

  QDomElement objectToElement(const SyncObject *object);

  SyncObject *objectFromNode(const QDomNode &node);

  SyncObject *toSyncObject(const QString &data);

  SyncObject *findObject(const SyncObject *src, SyncObject *root);

  SyncObject *findObjectByName(const QString &name, SyncObject *root);

  void addSyncObject(SyncObject *object);

  QDomNode findNodeByName(const QString &name);

  QDomNode findMatchingNode(const QDomDocument &doc, const SyncObject &object);

  void printObject(SyncObject &object);

  uint childCount(const SyncObject &object, uint count);

  void setObjectProperties(const QDomNode &node, SyncObject *object);

  void mergeData(const QString &src_x, const QString &src_y);

  SyncObject *constructObjectTree();

  void deleteChildObjects(QDomDocument &doc, QDomNode &node);

  void inspecObject(SyncObject *object, SyncObject *clone);

  QString mDataStoreName;
  QDomDocument *mDomDocument;
  QDomElement mRootElement;

  SyncEngineInterface *mSyncEngine;
  DataStore *mPriv;

  SyncObject *mRootObject;

  bool compareObject(SyncObject *object, SyncObject *clone);
};

DataStore::DataStore(const QString &name, QObject *parent)
    : QObject(parent), d(new PrivateDataStore) {
  d->mDataStoreName = name;
  d->mDomDocument = new QDomDocument(name);
  d->mDomDocument->appendChild(d->mDomDocument->createProcessingInstruction(
      "xml", "version=\"1.0\" encoding=\"utf-8\""));
  d->mRootElement = d->mDomDocument->createElement(name);
  d->mDomDocument->appendChild(d->mRootElement);
  d->mSyncEngine = 0;
  d->mRootObject = 0;
  d->mPriv = this;
}

DataStore::~DataStore() { delete d; }

void DataStore::setSyncEngine(SyncEngineInterface *iface) {
  d->mSyncEngine = iface;
  d->mSyncEngine->setEngineName(d->mDataStoreName);

  connect(d->mSyncEngine, SIGNAL(modified()), this, SLOT(onEngineModified()));

  if (d->mSyncEngine) {
    QString errorMessage;
    int line;
    int column;

    if (!d->mSyncEngine->data(d->mDataStoreName).isEmpty() &&
        !d->mSyncEngine->data(d->mDataStoreName).isNull()) {
      if (!d->mDomDocument->setContent(d->mSyncEngine->data(d->mDataStoreName),
                                       &errorMessage, &line, &column)) {
        qDebug() << Q_FUNC_INFO << errorMessage << "Line: " << line
                 << " Column : " << column;
        return;
      } else {
        d->constructObjectTree();

        if (d->mRootObject)
          d->printObject(*d->mRootObject);
      }
    }
  }
}

QString DataStore::name() const { return d->mDataStoreName; }

SyncObject *DataStore::begin(const QString &name) {
  qDebug() << Q_FUNC_INFO << "Requesting:" << name;

  if (d->mDomDocument->isNull()) {
    qDebug() << Q_FUNC_INFO << "Error :"
             << "Document is Null";
    qFatal("Quit");
    return 0;
  }

  SyncObject *root = this->rootObject();

  if (root) {
    qDebug() << Q_FUNC_INFO << "Current Root Name: " << root->name();
  }

  if (!root) {
    SyncObject *_new_root_object = new SyncObject(this);
    _new_root_object->setName(name);
    _new_root_object->setKey(0);

    QDomElement _root_element = d->objectToElement(_new_root_object);
    d->mDomDocument->documentElement().appendChild(_root_element);

    QDomNode _root_node = d->mDomDocument->documentElement().firstChild();

    d->mRootObject = d->objectFromNode(_root_node);
    d->mRootObject->setDomNode(_root_node);

    root = d->mRootObject;

    if (d->mSyncEngine) {

      d->mSyncEngine->sync(d->mDataStoreName, d->mDomDocument->toString());
    }
  }

  connect(root, SIGNAL(childCreated()), this, SLOT(onChildCreated()));
  return root;
}

void DataStore::addObject(SyncObject *object) { d->addSyncObject(object); }

void DataStore::linkToStore(const QString &name, const QStringList &keyList) {}

SyncObject *DataStore::rootObject() { return d->mRootObject; }

SyncObject *DataStore::PrivateDataStore::findObjectByName(const QString &name,
                                                          SyncObject *root) {
  qDebug() << Q_FUNC_INFO << "Name:" << name;
  if (!root)
    return 0;

  if (root->name() == name) {
    qDebug() << Q_FUNC_INFO << "Return from Here:" << root->key();
    return root;
  }

  if (root->hasChildren()) {
    // return findObjectByName(name, root);
    Q_FOREACH(SyncObject * child, root->childObjects()) {
      qDebug() << Q_FUNC_INFO << "Check Children" << child->key()
               << "child Name:" << child->name();
      if (child->name() == name)
        return child;
      findObjectByName(name, child);
    }
  }

  return 0;
}

void DataStore::PrivateDataStore::addSyncObject(SyncObject *object) {
  SyncObject *parent = 0;

  if (object->parentObject()) {
    parent = object->parentObject();
    QDomNode parentNode = findMatchingNode(*mDomDocument, *parent);
    QDomElement parentElement = parentNode.toElement();

    if (!parentNode.isNull()) {
      parentNode.appendChild(addChildObject(object, &parentElement));
    } else {
      qDebug() << Q_FUNC_INFO << "Error : " << object->parentObject()->name();
      addSyncObject(object->parentObject());
    }
  } else {
    // no parent, so add it to the root Object.
    QDomElement objectElem = objectToElement(object);

    if (object->hasChildren()) {
      addChildObject(object, &objectElem);
    }

    // object->setDomNode(objectElem);
    qDebug() << Q_FUNC_INFO << "JUst Adding the object ";
    mRootElement.appendChild(objectElem);
    QDomNode rootNode = findMatchingNode(*mDomDocument, *object);
    object->setDomNode(rootNode);
  }

  if (mSyncEngine) {
    mSyncEngine->sync(mDataStoreName, mDomDocument->toString());
    // qDebug() << Q_FUNC_INFO << mDomDocument->toString();
    // constructObjectTree();
  }
}

QDomNode DataStore::PrivateDataStore::findNodeByName(const QString &name) {
  qDebug() << Q_FUNC_INFO << "Look for Node : " << name;
  QDomNode n = mDomDocument->documentElement().firstChild();
  while (!n.isNull()) {
    QDomElement e = n.toElement(); // try to convert the node to an element.
    qDebug() << Q_FUNC_INFO << "Loop: "
             << "Check: " << n.nodeName();

    if (!e.isNull()) {
      if (n.nodeName() == name)
        return n;
    }

    n = n.nextSibling();
  }

  qDebug() << Q_FUNC_INFO << "no Valid node for " << name << "Found";
  return QDomNode();
}

QDomNode DataStore::PrivateDataStore::findMatchingNode(
    const QDomDocument &doc, const SyncObject &object) {
  QDomNodeList list = doc.documentElement().elementsByTagName(object.name());

  qDebug() << Q_FUNC_INFO << "Found Object Count of :" << list.count()
           << " type : " << object.name();

  if (list.count() <= 0)
    return QDomNode();

  for (int i = 0; i < list.count(); i++) {
    QDomNode node = list.at(i);

    if (node.isText())
      continue;

    if (node.isAttr())
      continue;

    SyncObject *matchObject = objectFromNode(node);

    qDebug() << Q_FUNC_INFO << matchObject->name() << " vs " << object.name();
    qDebug() << Q_FUNC_INFO << matchObject->key() << " vs " << object.key();

    if (matchObject->key() == object.key() &&
        matchObject->name() == object.name())
      return node;

    /*
    if(node.hasChildNodes() &&  !node.isText() && !node.isAttr()) {
        return findMatchingNode(doc, object);
    }*/

    qDebug() << Q_FUNC_INFO << matchObject->name();
  }

  return QDomNode();
}

void DataStore::PrivateDataStore::printObject(SyncObject &object) {
  if (object.hasChildren()) {
    Q_FOREACH(SyncObject * child, object.childObjects()) {
      printObject(*child);
    }
  }

  qDebug() << Q_FUNC_INFO << object.name();
}

uint DataStore::PrivateDataStore::childCount(const SyncObject &object,
                                             uint count) {
  if (!object.hasChildren()) {
    return count;
  }

  uint _childCount = count + object.childCount();

  Q_FOREACH(SyncObject * child, object.childObjects()) {
    if (child->hasChildren()) {
      _childCount += childCount(*child, count);
    }
  }

  return _childCount;
}

uint DataStore::childCount(const SyncObject &object) const {
  return d->childCount(object, 0);
}

uint DataStore::keyForObject(const SyncObject &object) {
  if (object.parentObject()) {
    SyncObject *parent = object.parentObject();
    uint childCountOfParent = childCount(*parent);
    return childCountOfParent + 1;
  }

  return 0;
}

void DataStore::printObject(SyncObject *object) { d->printObject(*object); }

bool DataStore::beginsWith(const QString &name) const {
  if (!d->mRootObject) {
    return false;
  }

  if (d->mRootObject->name() == name)
    return true;

  return false;
}

void DataStore::onChildObjectAdded() {
  // TODO: Handle Child Object updates?
}

void DataStore::onChildCreated() {
  if (d->mSyncEngine) {
    d->mSyncEngine->sync(d->mDataStoreName, d->mDomDocument->toString());
  }
}

void DataStore::onChildUpdated() { qDebug() << Q_FUNC_INFO << "Not Impl"; }

void DataStore::onEngineModified() {
  QDomDocument doy;

  doy.setContent(d->mSyncEngine->data(d->mDataStoreName));

  // check doxy
  QDomNode rooty = doy.documentElement().firstChild();
  SyncObject *rootObjectY = d->objectFromNode(rooty);

  d->inspecObject(d->mRootObject, rootObjectY);

  Q_EMIT updated(d->mRootObject);
}

QDomElement DataStore::PrivateDataStore::addChildObject(
    const SyncObject *object, QDomElement *parent) {
  QDomElement objectElem =
      objectToElement(object); // mDomDocument->createElement(object->name());

  Q_FOREACH(SyncObject * child, object->childObjects()) {

    if (child) {
      QDomElement childElement = objectToElement(child);

      if (child->hasChildren()) {
        addChildObject(child, &childElement);
      }

      parent->appendChild(childElement);
    }
  }

  return objectElem;
}

QDomElement DataStore::PrivateDataStore::objectToElement(
    const SyncObject *object) {
  QDomElement objectElem = mDomDocument->createElement(object->name());
  objectElem.setAttribute("timestamp", object->timeStamp());
  objectElem.setAttribute("key", object->key());

  Q_FOREACH(const QString & key, object->attributes()) {
    objectElem.setAttribute(key, object->attributeValue(key).toString());
  }

  return objectElem;
}

SyncObject *DataStore::PrivateDataStore::objectFromNode(const QDomNode &node) {
  SyncObject *rootObject = new SyncObject(mPriv);
  rootObject->setName(node.toElement().tagName());
  rootObject->setDomNode(node);
  setObjectProperties(node, rootObject);

  if (node.hasChildNodes()) {
    QDomNodeList list = node.childNodes();

    for (int i = 0; i < list.count(); i++) {
      QDomNode childNode = list.at(i);
      if (childNode.isText()) {
        rootObject->setTextData(childNode.toText().nodeValue());
        continue;
      }
      rootObject->addChildObject(objectFromNode(childNode));
    }
  }

  return rootObject;
}

SyncObject *DataStore::PrivateDataStore::toSyncObject(const QString &data) {
  qDebug() << Q_FUNC_INFO << "Load Data From Engine:";
  if (data.isEmpty())
    return 0;

  QString errorMessage;
  int line;
  int column;

  if (!mDomDocument->setContent(data, &errorMessage, &line, &column)) {
    qDebug() << Q_FUNC_INFO << errorMessage << "Line: " << line
             << " Column : " << column;
    return 0;
  }

  QDomElement docElem = mDomDocument->documentElement();
  SyncObject *root = new SyncObject(mPriv);

  root->setName(docElem.tagName());

  QDomNode n = docElem.firstChild();

  setObjectProperties(n, root);

  while (!n.isNull()) {

    QDomElement e = n.toElement(); // try to convert the node to an element.
    if (!e.isNull()) {
      SyncObject *subRoot = objectFromNode(n);
      setObjectProperties(n, subRoot);
      root->addChildObject(subRoot);
    }

    n = n.nextSibling();
  }

  return root;
}

SyncObject *DataStore::PrivateDataStore::findObject(const SyncObject *src,
                                                    SyncObject *root) {
  if (!root)
    return 0;

  if (root->hasChildren()) {
    Q_FOREACH(SyncObject * child, root->childObjects()) {
      return findObject(src, child);
    }
  }

  qDebug() << Q_FUNC_INFO << "Check :" << src->name() << "Key (" << src->key()
           << ") "
           << " In: " << root->name() << "(" << root->key() << ") "
           << " Tree Depth:" << root->childCount();

  if (root->name() == src->name() && root->key() == src->key()) {
    return root;
  }

  return 0;
}

void DataStore::PrivateDataStore::setObjectProperties(const QDomNode &node,
                                                      SyncObject *object) {
  QDomNamedNodeMap attrMap = node.attributes();

  for (int i = 0; i < attrMap.count(); i++) {
    QDomNode attrNode = attrMap.item(i);
    if (attrNode.isAttr()) {
      QDomAttr attr = attrNode.toAttr();

      if (!attr.isNull()) {
        object->setObjectAttribute(attr.name(), attr.value());
        if (attr.name() == "key") {
          object->setKey(attr.value().toUInt());
        }
      }
    }
  }
}

void DataStore::update() {
  if (d->mSyncEngine) {
    d->mergeData(d->mSyncEngine->data(d->mDataStoreName),
                 d->mDomDocument->toString());
  }
}

void DataStore::PrivateDataStore::mergeData(const QString &src_x,
                                            const QString &src_y) {
  QDomDocument dox;
  QDomDocument doy;

  dox.setContent(src_x);
  doy.setContent(src_y);

  // check dox
  QDomNode rootx = dox.documentElement().firstChild();
  SyncObject *rootObjectX = objectFromNode(rootx);

  // check doxy
  QDomNode rooty = doy.documentElement().firstChild();
  SyncObject *rootObjectY = objectFromNode(rooty);

  inspecObject(rootObjectX, rootObjectY);

  if (mSyncEngine) {
    mSyncEngine->sync(mDataStoreName, dox.toString());
    mDomDocument->setContent(dox.toString());
    qDebug() << Q_FUNC_INFO << "Done Updating Engine";
    // update current root tree
    QDomNode newroot = dox.documentElement().firstChild();
    SyncObject *newRootObject = objectFromNode(newroot);
    inspecObject(mRootObject, newRootObject);
    qDebug() << Q_FUNC_INFO << "####### Root ########";
    qDebug() << Q_FUNC_INFO
             << "##### Child Count: " << mRootObject->childCount();
    printObject(*mRootObject);
    qDebug() << Q_FUNC_INFO << "####### Root ########";
  }
}

SyncObject *DataStore::PrivateDataStore::constructObjectTree() {
  if (!mSyncEngine->data(mDataStoreName).isEmpty() &&
      !mSyncEngine->data(mDataStoreName).isNull()) {
    QString errorMessage;
    int line, column;
    if (!mDomDocument->setContent(mSyncEngine->data(mDataStoreName),
                                  &errorMessage, &line, &column)) {
      qDebug() << Q_FUNC_INFO << "No Root Object";
      return 0;
    }
  }

  if (mDomDocument && !mDomDocument->toString().isEmpty() &&
      !mDomDocument->toString().isNull() &&
      mDomDocument->documentElement().hasChildNodes() &&
      !mDomDocument->documentElement().isNull()) {

    QDomNode rootNode = mDomDocument->documentElement().firstChild();

    if (!mRootObject)
      mRootObject = objectFromNode(rootNode);

    mRootObject->setDomNode(rootNode);

    if (mRootObject->name().isEmpty() || mRootObject->name().isNull())
      return 0;

    return mRootObject;
  }

  return 0;
}

void DataStore::PrivateDataStore::deleteChildObjects(QDomDocument &doc,
                                                     QDomNode &node) {
  if (node.hasChildNodes()) {
    QDomNode n = node.firstChild();
    while (!n.isNull()) {
      if (n.hasChildNodes()) {
        deleteChildObjects(doc, n);
      } else {
        node.removeChild(n);
      }
      n = n.nextSibling();
    }
  }
}

bool DataStore::PrivateDataStore::compareObject(SyncObject *object,
                                                SyncObject *clone) {
  if (object->name() != clone->name())
    return false;

  // check properties
  for (int p = 0; p < object->attributes().count(); p++) {
    QString prop = object->attributes().at(p);

    if (prop == "timestamp" || prop == "key")
      continue;

    if (!clone->attributes().contains(prop))
      return false;

    if (clone->attributeValue(prop) != object->attributeValue(prop))
      return false;
    qDebug() << Q_FUNC_INFO << "Check : " << prop << " = "
             << object->attributeValue(prop);
  }

  return true;
}

void DataStore::PrivateDataStore::inspecObject(SyncObject *object,
                                               SyncObject *clone) {
  if (object->hasChildren()) {
    Q_FOREACH(SyncObject * child, object->childObjects()) {
      inspecObject(child, clone);
    }
  }

  if (clone->hasChildren()) {
    Q_FOREACH(SyncObject * child, clone->childObjects()) {
      inspecObject(child, object);
    }
  }

  if (object->name() == clone->name()) {
    if (object->childCount() == clone->childCount()) {
      Q_FOREACH(SyncObject * childObject, clone->childObjects()) {

        if (object->contains(childObject)) {
          if (object->timeStamp() < childObject->timeStamp()) {
            object->replace(childObject);
          }
          continue;
        }
        qDebug() << Q_FUNC_INFO << "Missing Object:" << childObject->name();
        object->clone(childObject);
      }
    } else if (object->childCount() < clone->childCount()) {

      Q_FOREACH(SyncObject * child, clone->childObjects()) {
        if (object->contains(child))
          continue;
        printObject(*child);
        object->clone(child);
      }
    }
  }
}

// last try

void DataStore::insert(SyncObject *object) {
  if (!d->mSyncEngine)
    return;

  // get current key;
  QDomDocument doc;
  doc.setContent(d->mSyncEngine->data(d->mDataStoreName));
  QDomNode rootNode = doc.documentElement().firstChild();
  QDomNodeList list = doc.documentElement().elementsByTagName(object->name());

  object->setKey(list.count());
  QDomNode objectNode = d->objectToElement(object);
  uint index = list.count();

  qDebug() << Q_FUNC_INFO << "Inserting object " << object->name()
           << "With ID:" << index << "Into :" << rootNode.nodeName();
  rootNode.appendChild(objectNode);
  qDebug() << Q_FUNC_INFO << doc.toString();

  d->mSyncEngine->sync(d->mDataStoreName, doc.toString());
}

void DataStore::updateNode(SyncObject *object) {
  if (!d->mSyncEngine)
    return;

  QDomDocument doc;
  doc.setContent(d->mSyncEngine->data(d->mDataStoreName));

  QDomNode updatedNode = d->findMatchingNode(doc, *object);

  if (updatedNode.isNull()) {
    qDebug() << Q_FUNC_INFO << "Matching Node Not Found";
  } else {
    qDebug() << Q_FUNC_INFO << "Found Matching Node :" << updatedNode.nodeName()
             << "key: " << object->key();
    object->setDomNode(updatedNode);
    QDomElement updateElem = updatedNode.toElement();

    // update properties
    Q_FOREACH(const QString & prop, object->attributes()) {
      if (prop == "key" || prop == "timestamp") {
        continue;
      }
      updateElem.setAttribute(prop, object->attributeValue(prop).toString());
    }

    // update text node;
    object->attachTextNode(object->textData());

    /// update time stamp
    // object->updateTimeStamp();
    // updateElem.setAttribute("timestamp", object->timeStamp());

    qDebug() << Q_FUNC_INFO << doc.toString();
    d->mSyncEngine->sync(d->mDataStoreName, doc.toString());
  }
}

void DataStore::deleteObject(SyncObject *object) {
  if (!d->mSyncEngine)
    return;

  QDomDocument doc;
  doc.setContent(d->mSyncEngine->data(d->mDataStoreName));

  QDomNode oldNode = d->findMatchingNode(doc, *object);
  QDomNode parentNode = oldNode.parentNode();

  d->deleteChildObjects(doc, oldNode);
  parentNode.removeChild(oldNode);

  qDebug() << Q_FUNC_INFO << doc.toString();

  d->mSyncEngine->sync(d->mDataStoreName, doc.toString());
}
}
