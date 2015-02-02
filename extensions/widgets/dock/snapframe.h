#ifndef SNAPFRAME_H
#define SNAPFRAME_H

#include <plexy.h>
#include <desktopwidget.h>

class SnapFrame : public UI::UIWidget {
  Q_OBJECT
public:
  SnapFrame(QGraphicsObject *parent = 0);
  virtual ~SnapFrame();

private:
  class PrivateSnapFrame;
  PrivateSnapFrame *const d;
};

#endif // SNAPFRAME_H
