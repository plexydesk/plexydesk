#ifndef SYNCOBJECT_H
#define SYNCOBJECT_H

#include <QObject>
#include <QDomNode>
#include <QuetzalDataKit_export.h>

namespace QuetzalKit {
class DataSync;
/**
    * @brief
    *
    */
class QuetzalDataKit_EXPORT SyncObject : public QObject {
  Q_OBJECT
public:
  /**
      * @brief
      *
      * @param parent
      */
  explicit SyncObject(QObject *a_parent_ptr = 0);

  /**
      * @brief
      *
      */
  virtual ~SyncObject();

  /**
      * @brief
      *
      * @return uint
      */
  virtual uint timeStamp() const;

  /**
      * @brief
      *
      * @param timestamp
      */
  virtual void setTimeStamp(uint timestamp);

  /**
      * @brief
      *
      * @return uint
      */
  virtual uint updatedTimeStamp() const;

  /**
      * @brief
      *
      * @param name
      */
  void setName(const QString &name);

  /**
      * @brief
      *
      * @return QString
      */
  QString name() const;

  void setKey(uint key);
  uint key() const;

  SyncObject *parentObject() const;
  void setParentObject(SyncObject *a_parent_ptr);

  void setObjectAttribute(const QString &name, const QVariant &attributeValue);
  QStringList attributes() const;
  QVariant attributeValue(const QString &name) const;

  bool hasChildren() const;
  uint childCount() const;
  void addChildObject(SyncObject *object);
  QList<SyncObject *> childObjects() const;
  SyncObject *childObject(uint key);
  SyncObject *childObject(const QString &name);
  void linksToObject(const QString &dataStoreName, const QString &objectName);

  // SyncObject linkedObject() const;

  void updateTimeStamp();

  void removeObject(uint key);

  void attachTextNode(const QString &data);
  void setTextData(const QString &data);
  QString textData() const;

  SyncObject *clone(SyncObject *object);

  void set_data_sync(DataSync *a_sync);
  virtual void sync();

  // protected:
  QDomNode node();
  void setDomNode(const QDomNode &node);
  SyncObject *createNewObject(const QString &name);
  QString dumpContent() const;
  bool contains(SyncObject *object);
  bool similarObject(SyncObject *object);
  void replace(SyncObject *object);

Q_SIGNALS:
  void childAdded();
  void childCreated();
  void updated();

private:
  class PrivateSyncObject;
  PrivateSyncObject *const d; /**< TODO */
};
}
#endif // SYNCOBJECT_H
