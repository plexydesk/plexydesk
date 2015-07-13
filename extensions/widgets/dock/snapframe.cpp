#include "snapframe.h"

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <toolbar.h>

class SnapFrame::PrivateSnapFrame {
public:
  PrivateSnapFrame() {}
  ~PrivateSnapFrame() {}
};

SnapFrame::SnapFrame(widget *parent)
    : cherry_kit::widget(parent), d(new PrivateSnapFrame) {
  setFlag(QGraphicsItem::ItemIsMovable, false);
}

SnapFrame::~SnapFrame() { delete d; }
