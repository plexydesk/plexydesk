#ifndef MEMORYSYNCENGINE_H
#define MEMORYSYNCENGINE_H

#include <syncengineinterface.h>
#include <QuetzalDataKit_export.h>

namespace ck {

class QuetzalDataKit_EXPORT MemorySyncEngine : public SyncEngineInterface {
  Q_OBJECT

public:
  explicit MemorySyncEngine(QObject *a_parent_ptr = 0);

  virtual QString data(const QString &storeName);

  virtual void sync(const QString &datastoreName, const QString &data);

  virtual bool hasLock();

private:
  QString mData;
};
}
#endif // MEMORYSYNCENGINE_H
