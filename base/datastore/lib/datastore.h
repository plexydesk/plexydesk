#ifndef DATASTORE_H
#define DATASTORE_H

#include <QObject>
#include <QuetzalDataKit_export.h>

namespace QuetzalKit
{

class SyncObject;
class SyncEngineInterface;

class QuetzalDataKit_EXPORT DataStore : public QObject
{
  Q_OBJECT
public:
  explicit DataStore(const QString &name, QObject *parent = 0);

  virtual ~DataStore();

  /**
  * @brief
  *
  * @param iface
  */
  virtual void setSyncEngine(SyncEngineInterface *iface);

  /**
  * @brief Name of the Data Store
  *
  * @return QString
  */
  virtual QString name() const;

  /**
  * @brief Adds a Sync Object to the DataStore
  *
  * This method is used to add a sync Method to the root node of the
  * DataStore. once this object is added to the root it's not allowed to
  * reparent at this moment.
  *
  * @param object The DataSync Object which we need to save to the Data Store
  */
  virtual void addObject(SyncObject *begin);

  /**
  * @brief Update an exising SyncObject
  *
  * This method looks for the matching object in the current list of objects
  * in the dataStore, once the object is located it will update the value.
  * if the object is found this method will return true, and if not found this
  *method
  * will simply return false.
  *
  * @return bool if the object was updated in the store returns true, false
  *otherwise.
  */
  virtual void update();

  /**
  * @brief
  *
  * @param name
  * @param keyList
  */
  virtual void linkToStore(const QString &name, const QStringList &keyList);

  /**
  * @brief The root Object of the storage
  *
  * The data objects are stored as a tree of objects. this mehtod
  * will return the root object of the stoage objects. This method will
  * task to the syncengine associated with the datastore to get the root object.
  * if the syncengine is not set then. it will return a null object since it
  *can't
  * locate any objects.
  *
  * \sa SyncEngineInterface
  * \sa setSyncEngine();
  *
  * @return SyncObject the root object or null if the data store is empty or
  *invalid.
  */
  virtual SyncObject *rootObject();

  virtual void insert(SyncObject *begin);

  virtual void deleteObject(SyncObject *object);

  virtual SyncObject *begin(const QString &name);

  virtual uint childCount(const SyncObject &begin) const;

  virtual uint keyForObject(const SyncObject &begin);

  virtual void updateNode(QuetzalKit::SyncObject *begin);

  virtual void printObject(SyncObject *begin);

  virtual bool beginsWith(const QString &name) const;

Q_SIGNALS:
  void updated(QuetzalKit::SyncObject *rootObject);

private Q_SLOTS:
  void onChildObjectAdded();

  void onChildCreated();

  void onChildUpdated();

  void onEngineModified();

private:
  class PrivateDataStore;
  PrivateDataStore *const d;
};
}
#endif // DATASTORE_H
