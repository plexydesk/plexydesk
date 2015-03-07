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

  virtual QString data(const QString &fileName);

  virtual void sync(const QString &datqstoreName, const QString &data);

  virtual bool hasLock();

  void saveDataToDisk(const QString &fileName, const QString &data);

private Q_SLOTS:
  void onBytesWritten(qint64 bytes);
  void onDirectoryChanged(const QString &name);

private:
  class PrivateDiskSyncEngine;
  PrivateDiskSyncEngine *const d;
};
}
#endif // DISKSYNCENGINE_H
