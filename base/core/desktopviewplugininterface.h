#ifndef DESKTOPVIEWPLUGININTERFACE_H
#define DESKTOPVIEWPLUGININTERFACE_H

#include <plexy.h>

#include <QtCore/QSharedPointer>
#include <QtCore/QObject>

#include <desktopviewplugin.h>
#include <plexy_core_exports.h>

namespace PlexyDesk {

class CORE_DECL_EXPORT DesktopViewPluginInterface {
public:
  DesktopViewPluginInterface() {}
  virtual ~DesktopViewPluginInterface() {}

  virtual QSharedPointer<DesktopViewPlugin> view() = 0;
};
}

Q_DECLARE_INTERFACE(PlexyDesk::DesktopViewPluginInterface,
                    "org.plexydesk.DesktopViewPluginInterface")
#endif // DESKTOPVIEWPLUGININTERFACE_H
