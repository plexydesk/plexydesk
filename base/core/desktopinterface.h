#ifndef DESKTOPVIEWPLUGININTERFACE_H
#define DESKTOPVIEWPLUGININTERFACE_H

#include <plexy.h>

#include <QSharedPointer>
#include <QObject>
#include <abstractdesktopview.h>
#include <plexy_core_exports.h>

namespace PlexyDesk {

class PlexyDeskCore_EXPORT DesktopInterface {
public:
  DesktopInterface() {}
  virtual ~DesktopInterface() {}

  virtual void setNativeWindowProperties(WorkSpace *workspace) = 0;
};
}

Q_DECLARE_INTERFACE(PlexyDesk::DesktopInterface,
                    "org.plexydesk.DesktopInterface")
#endif // DESKTOPVIEWPLUGININTERFACE_H
