#ifndef CONTROLLERPLUGININTERFACE_H
#define CONTROLLERPLUGININTERFACE_H

#include <QtCore/QSharedPointer>
#include <QtCore/QObject>

#include <ck_desktop_controller_interface.h>
#include <plexydesk_ui_exports.h>

namespace cherry_kit {

class DECL_UI_KIT_EXPORT desktop_plugin_interface {
public:
  desktop_plugin_interface() {}
  virtual ~desktop_plugin_interface() {};

  virtual QSharedPointer<desktop_controller_interface> controller() = 0;
};
}

Q_DECLARE_INTERFACE(cherry_kit::desktop_plugin_interface,
                    "org.plexydesk.ControllerPluginInterface")
#endif // CONTROLLERPLUGININTERFACE_H
