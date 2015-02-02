#include "clockinterface.h"
#include "clock.h"

QSharedPointer<PlexyDesk::ViewController> ClockInterface::controller() {
  QSharedPointer<PlexyDesk::ViewController> obj =
      QSharedPointer<PlexyDesk::ViewController>(new Clock(),
                                                     &QObject::deleteLater);

  return obj;
}

// Q_EXPORT_PLUGIN2(plexyclock, ClockInterface)
