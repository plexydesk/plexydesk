#ifndef WIDGETSTYLEINTERFACE_H
#define WIDGETSTYLEINTERFACE_H

#include <style.h>
#include <plexydesk_ui_exports.h>

namespace UI {

class DECL_UI_KIT_EXPORT WidgetStyleInterface {
public:
  WidgetStyleInterface() {}
  virtual QSharedPointer<UI::Style> style() = 0;
};
}

Q_DECLARE_INTERFACE(UI::WidgetStyleInterface,
                    "org.plexydesk.WidgetStyleInterface")

#endif // WIDGETSTYLEINTERFACE_H
