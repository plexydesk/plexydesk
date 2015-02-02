#include "clockinterface.h"
#include "clock.h"

QSharedPointer<UI::ViewController> ClockInterface::controller() {
  QSharedPointer<UI::ViewController> obj =
      QSharedPointer<UI::ViewController>(new Clock(),
                                                     &QObject::deleteLater);

  return obj;
}

// Q_EXPORT_PLUGIN2(plexyclock, ClockInterface)
