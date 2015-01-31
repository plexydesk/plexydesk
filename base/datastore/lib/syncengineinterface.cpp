#include "syncengineinterface.h"

namespace QuetzalKit {

SyncEngineInterface::SyncEngineInterface(QObject *parent) : QObject(parent) {}

void SyncEngineInterface::setEngineName(const QString &name) {
  // engine name;
}
}
