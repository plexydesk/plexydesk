#ifndef PLEXY_DAY_CONTROLLER_H
#define PLEXY_DAY_CONTROLLER_H

#include <controllerinterface.h>
#include "plexydaywidget.h"

class PlexyDayController : public PlexyDesk::ControllerInterface {
  Q_OBJECT

public:
  PlexyDayController(QObject *object = 0);
  virtual ~PlexyDayController();

  void init();

  void revokeSession(const QVariantMap &args);

  void setViewRect(const QRectF &rect);

  QString icon() const;

private:
  PlexyDesk::PlexyDayWidget *mWidget;
};

#endif
