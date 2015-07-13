#ifndef SNAPFRAME_H
#define SNAPFRAME_H

#include <ck_widget.h>

class SnapFrame : public cherry_kit::widget {
  Q_OBJECT
public:
  SnapFrame(widget *a_parent_ptr = 0);
  virtual ~SnapFrame();

private:
  class PrivateSnapFrame;
  PrivateSnapFrame *const d;
};

#endif // SNAPFRAME_H
