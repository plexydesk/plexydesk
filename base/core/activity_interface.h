#ifndef ACTIVITY_INTERFACE_H
#define ACTIVITY_INTERFACE_H

#include <plexy_core_exports.h>

namespace PlexyDesk {

class DesktopActivity;

class CORE_DECL_EXPORT ActivityInterface {
public:
  explicit ActivityInterface() {}

  virtual QSharedPointer<PlexyDesk::DesktopActivity> activity() = 0;
};
} // namespace PlexyDesk

Q_DECLARE_INTERFACE(PlexyDesk::ActivityInterface,
                    "org.plexydesk.ActivityInterface")
#endif // ACTIVITY_INTERFACE_H
