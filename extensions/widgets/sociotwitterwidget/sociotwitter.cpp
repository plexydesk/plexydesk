#include "sociotwitter.h"
#include "sociotwitterck_widget.h"
#include "sociotwitterinterface.h"

SocioTwitter::SocioTwitter(QObject *object) {}

SocioTwitter::~SocioTwitter() {}

QGraphicsItem *SocioTwitter::item() {
  return new PlexyDesk::SocioTwitterwidget(QRectF(0, 0, 400, 400), 0);
}
