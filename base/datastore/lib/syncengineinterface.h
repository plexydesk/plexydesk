#ifndef SYNCENGINEINTERFACE_H
#define SYNCENGINEINTERFACE_H

#include <QObject>
#include <syncobject.h>
#include <QStringList>
#include <QuetzalDataKit_export.h>

namespace QuetzalKit
{

class QuetzalDataKit_EXPORT SyncEngineInterface : public QObject
{
  Q_OBJECT
public:
  explicit SyncEngineInterface(QObject *a_parent_ptr = 0);
  virtual ~SyncEngineInterface();

  virtual QString data(const QString &storeName) = 0;

  virtual void setEngineName(const QString &name);

  virtual void sync(const QString &storeName, const QString &data) = 0;

  virtual bool hasLock() = 0;

  virtual void set_app_name(const std::string &a_app_name) {}

  virtual void save_request(const SyncObject &a_object) {}
  virtual void remove_object_request(const SyncObject &a_object) {}

  virtual void find(const std::string &a_object_name) {}

  virtual void on_search_complete(std::function<void (const SyncObject &a_object,
                                const std::string &a_app_name)> a_handler);
protected:
  virtual void search_request_complete(const SyncObject &a_object,
                                       std::string &a_app_name);

private:
  class PrivateSyncEngineIface;
  PrivateSyncEngineIface * const d;

// virtual QStringList storeList() = 0;

Q_SIGNALS:
  void syncDone();
  void dataReady();
  void modified();
};
}

#endif // SYNCENGINEINTERFACE_H
