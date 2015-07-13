#ifndef WIDGETSTYLEINTERFACE_H
#define WIDGETSTYLEINTERFACE_H

#include <ck_style.h>
#include <plexydesk_ui_exports.h>

namespace cherry_kit {

class DECL_UI_KIT_EXPORT style_interface {
public:
  style_interface() {}
  virtual QSharedPointer<cherry_kit::style> style() = 0;
};
}

Q_DECLARE_INTERFACE(cherry_kit::style_interface,
                    "org.plexydesk.WidgetStyleInterface")

#endif // WIDGETSTYLEINTERFACE_H
