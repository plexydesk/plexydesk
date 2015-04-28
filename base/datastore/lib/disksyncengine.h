#ifndef DISKSYNCENGINE_H
#define DISKSYNCENGINE_H

#include <syncengineinterface.h>
#include <QuetzalDataKit_export.h>

namespace QuetzalKit
{

class QuetzalDataKit_EXPORT DiskSyncEngine : public SyncEngineInterface
{
  Q_OBJECT
public:
  explicit DiskSyncEngine(QObject *a_parent_ptr = 0);
  virtual ~DiskSyncEngine();

  virtual void setEngineName(const QString &name);

  void set_app_name(const std::string &a_app_name);
  void save_request(const SyncObject &a_obj);

  virtual QString data(const QString &fileName);

  virtual void find(const std::string &a_object_name);
  virtual void sync(const QString &datqstoreName, const QString &data);

  virtual bool hasLock();

  void saveDataToDisk(const QString &fileName, const QString &data);

  QString db_home_path();
  QString db_app_path();
private Q_SLOTS:
  void onBytesWritten(qint64 bytes);
  void onDirectoryChanged(const QString &name);

private:
  class PrivateDiskSyncEngine;
  PrivateDiskSyncEngine *const d;
};
}
#endif // DISKSYNCENGINE_H
