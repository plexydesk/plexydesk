#ifndef SYNCENGINEINTERFACE_H
#define SYNCENGINEINTERFACE_H

#include <QObject>
#include <QStringList>
#include <QuetzalDataKit_export.h>

namespace QuetzalKit
{

class QuetzalDataKit_EXPORT SyncEngineInterface : public QObject
{
  Q_OBJECT
public:
  explicit SyncEngineInterface(QObject *a_parent_ptr = 0);

  virtual QString data(const QString &storeName) = 0;

  virtual void setEngineName(const QString &name);

  virtual void sync(const QString &storeName, const QString &data) = 0;

  virtual bool hasLock() = 0;

// virtual QStringList storeList() = 0;

Q_SIGNALS:
  void syncDone();
  void dataReady();
  void modified();
};
}

#endif // SYNCENGINEINTERFACE_H
