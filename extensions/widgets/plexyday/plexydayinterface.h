#ifndef PLEXY_DAY_I
#define PLEXY_DAY_I

#include <ck_desktop_plugin_interface.h>

class PlexyDayInterface : public QObject,
                          public PlexyDesk::ControllerPluginInterface {
  Q_OBJECT
  Q_INTERFACES(PlexyDesk::ControllerPluginInterface)
  Q_PLUGIN_METADATA(IID "org.qt-project.plexyday")

public:
  PlexyDayInterface(QObject *object = 0);

  QSharedPointer<PlexyDesk::ControllerInterface> controller();
};

#endif
