#ifndef CONTROLLERPLUGININTERFACE_H
#define CONTROLLERPLUGININTERFACE_H

#include <plexy.h>

#include <QtCore/QSharedPointer>
#include <QtCore/QObject>

#include <view_controller.h>
#include <plexy_core_exports.h>

namespace PlexyDesk {

class CORE_DECL_EXPORT ControllerPluginInterface {
public:
  ControllerPluginInterface() {}
  virtual ~ControllerPluginInterface() {};

  virtual QSharedPointer<ViewController> controller() = 0;
};
}

Q_DECLARE_INTERFACE(PlexyDesk::ControllerPluginInterface,
                    "org.plexydesk.ControllerPluginInterface")
#endif // CONTROLLERPLUGININTERFACE_H
