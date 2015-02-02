#ifndef ACTIVITY_INTERFACE_H
#define ACTIVITY_INTERFACE_H

#include <plexydesk_ui_exports.h>

namespace PlexyDesk {

class DesktopActivity;

class DECL_UI_KIT_EXPORT ActivityInterface {
public:
  explicit ActivityInterface() {}

  virtual QSharedPointer<PlexyDesk::DesktopActivity> activity() = 0;
};
} // namespace PlexyDesk

Q_DECLARE_INTERFACE(PlexyDesk::ActivityInterface,
                    "org.plexydesk.ActivityInterface")
#endif // ACTIVITY_INTERFACE_H
