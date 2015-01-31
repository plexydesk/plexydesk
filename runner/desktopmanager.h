#ifndef DESKTOPMANAGER_H
#define DESKTOPMANAGER_H

#include <QObject>
#include <plexy.h>
#include <workspace.h>
#include <controllerinterface.h>

using namespace PlexyDesk;

class DesktopManager : public PlexyDesk::WorkSpace {
  Q_OBJECT
public:
  explicit DesktopManager(QWidget *parent = 0);

  virtual ~DesktopManager();

protected:
  virtual void mouseReleaseEvent(QMouseEvent *event);
  virtual void mouseMoveEvent(QMouseEvent *event);

private:
  class PrivateDesktopManager;
  PrivateDesktopManager *const d;

  PlexyDesk::ControllerPtr getController();
};

#endif // DESKTOPMANAGER_H
