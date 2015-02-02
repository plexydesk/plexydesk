#ifndef CONTROLLERPLUGININTERFACE_H
#define CONTROLLERPLUGININTERFACE_H

#include <plexy.h>

#include <QtCore/QSharedPointer>
#include <QtCore/QObject>

#include <view_controller.h>
#include <plexydesk_ui_exports.h>

namespace PlexyDesk {

class DECL_UI_KIT_EXPORT ControllerPluginInterface {
public:
  ControllerPluginInterface() {}
  virtual ~ControllerPluginInterface() {};

  virtual QSharedPointer<ViewController> controller() = 0;
};
}

Q_DECLARE_INTERFACE(PlexyDesk::ControllerPluginInterface,
                    "org.plexydesk.ControllerPluginInterface")
#endif // CONTROLLERPLUGININTERFACE_H
