#include "memorysyncengine.h"

namespace QuetzalKit {

MemorySyncEngine::MemorySyncEngine(QObject *parent) {}

QString MemorySyncEngine::data(const QString &storeName) { return mData; }

void MemorySyncEngine::sync(const QString &datastoreName, const QString &data) {
  mData = data;
}

bool MemorySyncEngine::hasLock() {
  // TODO!
  return false;
}
}
