#include "modelview.h"

#include <QGraphicsLinearLayout>
#include <QDebug>
#include <QGraphicsWidget>

#include <QScroller>

namespace PlexyDesk {

class ModelView::PrivateModelView {
public:
    PrivateModelView(){}
    ~PrivateModelView(){}

    QGraphicsWidget *m_scroll_frame;

    QGraphicsLinearLayout *m_list_layout;
    QRectF m_viewport_geometry;
};

ModelView::ModelView(QGraphicsObject *parent) :
    UIWidget(parent),
    d(new PrivateModelView) {

    d->m_scroll_frame = new QGraphicsWidget(this);

    d->m_list_layout = new QGraphicsLinearLayout(d->m_scroll_frame);
    d->m_list_layout->setOrientation(Qt::Vertical);
    d->m_list_layout->setContentsMargins(0, 0, 0, 0);

    d->m_viewport_geometry = QRectF();
    d->m_scroll_frame->setGeometry(QRectF());

    setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton);
    setFlag(QGraphicsItem::ItemIsMovable, false);
    setFlag(QGraphicsItem::ItemIsFocusable, true);
    setFlag(QGraphicsItem::ItemClipsChildrenToShape, true);

    QScroller::grabGesture(this, QScroller::LeftMouseButtonGesture);
}

ModelView::~ModelView() {
    QScroller::ungrabGesture(this);
    delete d;
}

void ModelView::insert(Widget *widget) {
    widget->setParentItem(d->m_scroll_frame);
    widget->setParent(d->m_scroll_frame);
    //widget->setFlag(QGraphicsItem::ItemIsMovable, false);

    d->m_list_layout->addItem(widget);
    d->m_list_layout->updateGeometry();
    d->m_list_layout->activate();

    d->m_scroll_frame->setGeometry(d->m_list_layout->geometry());

    update();
}

void ModelView::remove(UIWidget *widget) {}

void ModelView::clear() {
    if (d->m_list_layout->count() <= 0)
        return;

    while (d->m_list_layout->count() > 0) {
        int index = d->m_list_layout->count() - 1;
        UIWidget *item = (UIWidget*)(d->m_list_layout->itemAt(index));
        d->m_list_layout->removeAt(index);

        if (item) {
            delete item;
        }
    }

    d->m_list_layout->invalidate();
    d->m_list_layout->updateGeometry();
}

void ModelView::setViewGeometry(const QRectF &rect)
{
    d->m_viewport_geometry = rect;
}

QRectF ModelView::boundingRect() const {
    return d->m_viewport_geometry;
}

void ModelView::setGeometry(const QRectF &rect) {
    setPos(rect.topLeft());
}

QSizeF ModelView::sizeHint(Qt::SizeHint which,
                           const QSizeF &constraint) const {
    return d->m_list_layout->contentsRect().size();
}

bool ModelView::event(QEvent *e) {
    switch (e->type()) {
    case QEvent::GraphicsSceneMouseDoubleClick: {
        return QGraphicsObject::event(e);
    }
    case QScrollPrepareEvent::ScrollPrepare: {
        QScrollPrepareEvent *se = static_cast<QScrollPrepareEvent *>(e);
        se->setViewportSize(d->m_viewport_geometry.size());
        QRectF br = d->m_scroll_frame->geometry();

        qreal _max_width =
                qMax(qreal(0), br.width() - boundingRect().size().width());
        qreal _max_height =
                qMax(qreal(0), br.height() - boundingRect().size().height());

        se->setContentPosRange(QRectF(0, 0, _max_width, _max_height));
        se->setContentPos(-d->m_scroll_frame->pos());

        se->accept();
        return QGraphicsObject::event(e);
    }
    case QScrollEvent::Scroll: {
        QScrollEvent *se = static_cast<QScrollEvent *>(e);
        d->m_scroll_frame->setPos(
                    -se->contentPos() - se->overshootDistance());
        return true;
    }

    default:
        break;
    }
    return QGraphicsObject::event(e);
}

bool ModelView::sceneEvent(QEvent *e) {
  switch (e->type()) {
    case QEvent::TouchBegin: {
      // We need to return true for the TouchBegin here in the
      // top-most graphics object - otherwise gestures in our parent
      // objects will NOT work at all (the accept() flag is already
      // set due to our setAcceptTouchEvents(true) call in the c'tor
      return true;
    }
    case QEvent::GraphicsSceneMousePress: {
      // We need to return true for the MousePress here in the
      // top-most graphics object - otherwise gestures in our parent
      // objects will NOT work at all (the accept() flag is already
      // set to true by Qt)
      return true;
    }
    default:
      break;
  }
  return QGraphicsObject::sceneEvent(e);
}

}

/*
  example:

  UiKit::ModelView model;

  model.setSource(ModelSource("rss"));
  model.setRenderer(ModelRender("rss"));

  model.onModelUpdate([](void) {});
*/
