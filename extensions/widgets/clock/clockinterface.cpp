#include "clockinterface.h"
#include "clock.h"

QSharedPointer<CherryKit::ViewController> ClockInterface::controller() {
  QSharedPointer<CherryKit::ViewController> obj =
      QSharedPointer<CherryKit::ViewController>(new Clock(),
                                                &QObject::deleteLater);

  return obj;
}

// Q_EXPORT_PLUGIN2(plexyclock, ClockInterface)
