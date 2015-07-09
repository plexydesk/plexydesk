#ifndef SYNCENGINEINTERFACE_H
#define SYNCENGINEINTERFACE_H

#include <functional>

#include <QObject>
#include <syncobject.h>
#include <QStringList>
#include <QuetzalDataKit_export.h>

namespace ck {

class QuetzalDataKit_EXPORT SyncEngineInterface : public QObject {
  Q_OBJECT
public:
  explicit SyncEngineInterface(QObject *a_parent_ptr = 0);
  virtual ~SyncEngineInterface();

  virtual QString data(const QString &storeName) = 0;

  virtual void setEngineName(const QString &name);

  virtual void sync(const QString &storeName, const QString &data) = 0;

  virtual bool hasLock() = 0;

  virtual void set_app_name(const std::string &a_app_name) {}

  virtual void insert_request(const sync_object &a_object) {}
  virtual void update_request(const sync_object &a_object) {}
  virtual void delete_request(const std::string &a_object_name,
                              const std::string &a_key,
                              const std::string &value) {}

  virtual void find(const std::string &a_object_name,
                    const std::string &a_attrib, const std::string &a_value) {}

  virtual void on_search_complete(std::function<void(
      sync_object &a_object, const std::string &a_app_name, bool)> a_handler);

  virtual void
  on_insert_complete(std::function<void(const sync_object &)> a_handler);

protected:
  virtual void search_request_complete(sync_object &a_object,
                                       std::string &a_app_name, bool a_found);
  virtual void insert_request_complete(const sync_object &a_object);

private:
  class PrivateSyncEngineIface;
  PrivateSyncEngineIface *const d;
// virtual QStringList storeList() = 0;

Q_SIGNALS:
  void syncDone();
  void dataReady();
  void modified();
};
}

#endif // SYNCENGINEINTERFACE_H
