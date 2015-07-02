#ifndef SNAPFRAME_H
#define SNAPFRAME_H

#include <plexy.h>
#include <widget.h>

class SnapFrame : public CherryKit::Widget {
  Q_OBJECT
public:
  SnapFrame(Widget *a_parent_ptr = 0);
  virtual ~SnapFrame();

private:
  class PrivateSnapFrame;
  PrivateSnapFrame *const d;
};

#endif // SNAPFRAME_H
