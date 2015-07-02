#ifndef ACTIVITY_INTERFACE_H
#define ACTIVITY_INTERFACE_H

#include <plexydesk_ui_exports.h>

namespace CherryKit {

class DesktopActivity;

class DECL_UI_KIT_EXPORT ActivityInterface {
public:
  explicit ActivityInterface() {}

  virtual QSharedPointer<CherryKit::DesktopActivity> activity() = 0;
};
} // namespace PlexyDesk

Q_DECLARE_INTERFACE(CherryKit::ActivityInterface,
                    "org.plexydesk.ActivityInterface")
#endif // ACTIVITY_INTERFACE_H
