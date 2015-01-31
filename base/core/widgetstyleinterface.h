#ifndef WIDGETSTYLEINTERFACE_H
#define WIDGETSTYLEINTERFACE_H

#include <style.h>
#include <plexy_core_exports.h>

namespace PlexyDesk {

class CORE_DECL_EXPORT WidgetStyleInterface {
public:
  WidgetStyleInterface() {}
  virtual QSharedPointer<PlexyDesk::Style> style() = 0;
};
}

Q_DECLARE_INTERFACE(PlexyDesk::WidgetStyleInterface,
                    "org.plexydesk.WidgetStyleInterface")

#endif // WIDGETSTYLEINTERFACE_H
