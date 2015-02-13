#include "clockinterface.h"
#include "clock.h"

QSharedPointer<UIKit::ViewController> ClockInterface::controller()
{
  QSharedPointer<UIKit::ViewController> obj =
    QSharedPointer<UIKit::ViewController>(new Clock(),
                                       &QObject::deleteLater);

  return obj;
}

// Q_EXPORT_PLUGIN2(plexyclock, ClockInterface)
