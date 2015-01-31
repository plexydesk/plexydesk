#ifndef ACTIVITY_DESKTOP_MENU_H
#define ACTIVITY_DESKTOP_MENU_H

#include <QObject>
#include <desktopactivity.h>

#include <plexydesk_ui_exports.h>

namespace PlexyDesk {
class Space;

class DECL_UI_KIT_EXPORT DesktopActivityMenu : public QObject {
  Q_OBJECT
public:
  explicit DesktopActivityMenu(QObject *parent = 0);

  virtual ~DesktopActivityMenu();

  virtual void setActivity(DesktopActivityPtr activity);

  virtual void setSpace(Space *space);

  virtual void show();

  virtual void hide();

  virtual void exec(const QPointF &pos);

protected Q_SLOTS:
  virtual void onFocusOutEvent();
  virtual void lateHide();

Q_SIGNALS:
  void deactivated();
  void activated();

private:
  class PrivateActivityPopup;
  PrivateActivityPopup *const d;
};
}

#endif // POPUP_H
