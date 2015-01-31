#ifndef DATASYNC_RUNTIME_H
#define DATASYNC_RUNTIME_H

#include <QObject>

#include <syncobject.h>

class DataSyncRuntime : public QObject {
  Q_OBJECT

public:
  explicit DataSyncRuntime(QObject *parent = 0);

  virtual ~DataSyncRuntime();

  void testCreateDataStore();

  void testDiskEngine();

  void testDataSyncObject();

  void addTestNoteObejct(const QString &title,
                         const QString &content = "This is a test Text");

  void runDiskTest();

  void mergeTest();

  void insertTest();

  void updateTest();

private Q_SLOTS:
  void onStoreUpdated(QuetzalKit::SyncObject *object);

private:
  class PrivateDataSyncRuntime;
  PrivateDataSyncRuntime *const d;
};

#endif // DATASYNC_RUNTIME_H
