#include "plexydayinterface.h"
#include "plexyday.h"
#include <ck_extension_manager.h>

#include <QtPlugin>

PlexyDayInterface::PlexyDayInterface(QObject *object) {}

QSharedPointer<PlexyDesk::ControllerInterface> PlexyDayInterface::controller() {
  QSharedPointer<PlexyDesk::ControllerInterface> obj =
      QSharedPointer<PlexyDesk::ControllerInterface>(
          new PlexyDayController(this), &QObject::deleteLater);

  return obj;
}

// Q_EXPORT_PLUGIN2(plexyday, PlexyDayInterface)
