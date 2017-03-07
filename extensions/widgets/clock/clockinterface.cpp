#include "clockinterface.h"
#include "clock.h"

cherry_kit::desktop_controller_ref time_controller_impl::controller() {
  cherry_kit::desktop_controller_ref obj =
      cherry_kit::desktop_controller_ref(new time_controller());

  return obj;
}

#ifdef __QT4_TOOLKIT__
Q_EXPORT_PLUGIN2(plexyclock, time_controller_impl)
#endif
