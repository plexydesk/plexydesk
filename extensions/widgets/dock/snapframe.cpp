#include "snapframe.h"

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <toolbar.h>

class SnapFrame::PrivateSnapFrame
{
public:
  PrivateSnapFrame() {}
  ~PrivateSnapFrame() {}
};

SnapFrame::SnapFrame(QGraphicsObject *parent)
  : UI::UIWidget(parent), d(new PrivateSnapFrame)
{
  setFlag(QGraphicsItem::ItemIsMovable, false);
}

SnapFrame::~SnapFrame() { delete d; }
