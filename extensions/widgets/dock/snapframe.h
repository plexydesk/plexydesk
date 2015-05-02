#ifndef SNAPFRAME_H
#define SNAPFRAME_H

#include <plexy.h>
#include <widget.h>

class SnapFrame : public UIKit::Widget {
  Q_OBJECT
public:
  SnapFrame(QGraphicsObject *a_parent_ptr = 0);
  virtual ~SnapFrame();

private:
  class PrivateSnapFrame;
  PrivateSnapFrame *const d;
};

#endif // SNAPFRAME_H
