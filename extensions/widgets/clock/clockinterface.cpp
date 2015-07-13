#include "clockinterface.h"
#include "clock.h"

QSharedPointer<cherry_kit::desktop_controller_interface>
time_controller_impl::controller() {
  QSharedPointer<cherry_kit::desktop_controller_interface> obj =
      QSharedPointer<cherry_kit::desktop_controller_interface>(
          new time_controller(), &QObject::deleteLater);

  return obj;
}

// Q_EXPORT_PLUGIN2(plexyclock, ClockInterface)
