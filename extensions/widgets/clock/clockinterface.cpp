#include "clockinterface.h"
#include "clock.h"

QSharedPointer<cherry_kit::desktop_controller_interface>
ClockInterface::controller() {
  QSharedPointer<cherry_kit::desktop_controller_interface> obj =
      QSharedPointer<cherry_kit::desktop_controller_interface>(
          new Clock(), &QObject::deleteLater);

  return obj;
}

// Q_EXPORT_PLUGIN2(plexyclock, ClockInterface)
