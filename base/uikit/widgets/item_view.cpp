#include "item_view.h"

#include <QGraphicsLinearLayout>
#include <QDebug>
#include <QGraphicsWidget>
#include <QGraphicsGridLayout>

#include <QScroller>

namespace UIKit
{

class ItemView::PrivateModelView
{
public:
  PrivateModelView() {}
  ~PrivateModelView() {}

  ModelType m_model_view_type;
  QGraphicsWidget *m_scroll_frame;

  QGraphicsLinearLayout *m_list_layout;
  QGraphicsGridLayout *m_grid_layout;
  QRectF m_viewport_geometry;

  QList<ModelViewItem*> m_model_item_list;

  std::function<void (int index)> m_activation_handler;
};

ItemView::ItemView(QGraphicsObject *parent, ModelType a_model_type) :
  Widget(parent),
  d(new PrivateModelView)
{
  d->m_model_view_type = a_model_type;
  d->m_scroll_frame = new QGraphicsWidget(this);

  if (d->m_model_view_type == kListModel) {
    d->m_list_layout = new QGraphicsLinearLayout(d->m_scroll_frame);
    d->m_list_layout->setOrientation(Qt::Vertical);
    d->m_list_layout->setContentsMargins(0, 0, 0, 0);
  }

  if (d->m_model_view_type == kGridModel) {
    d->m_grid_layout = new QGraphicsGridLayout(d->m_scroll_frame);
    d->m_grid_layout->setContentsMargins(16, 0, 0, 0);
  }

  d->m_viewport_geometry = QRectF();
  d->m_scroll_frame->setGeometry(QRectF());

  setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton);
  setFlag(QGraphicsItem::ItemIsMovable, false);
  setFlag(QGraphicsItem::ItemIsFocusable, true);
  setFlag(QGraphicsItem::ItemClipsChildrenToShape, true);

  QScroller::grabGesture(this, QScroller::LeftMouseButtonGesture);
}

ItemView::~ItemView()
{
  QScroller::ungrabGesture(this);
  delete d;
}

void ItemView::insert_to_list_view(Widget *a_widget_ptr)
{
  a_widget_ptr->set_widget_id(d->m_list_layout->count());

  d->m_list_layout->addItem(a_widget_ptr);
  d->m_list_layout->updateGeometry();
  d->m_list_layout->activate();

  d->m_scroll_frame->setGeometry(d->m_list_layout->geometry());
}

void ItemView::insert_to_grid_view(Widget *a_widget_ptr)
{
  if (!d->m_grid_layout) {
    return;
  }

  a_widget_ptr->set_widget_id(d->m_grid_layout->count());

  int l_item_per_row =
    d->m_viewport_geometry.width() /
    a_widget_ptr->boundingRect().width();

  d->m_grid_layout->addItem(a_widget_ptr,
                            d->m_grid_layout->count() / (l_item_per_row - 1),
                            d->m_grid_layout->count() % (l_item_per_row - 1));

  d->m_grid_layout->activate();
  d->m_grid_layout->updateGeometry();

  d->m_scroll_frame->setGeometry(d->m_grid_layout->geometry());
}

void ItemView::insert_to_table_view(Widget *a_widget_ptr)
{
}

void ItemView::insert(Widget *a_widget_ptr)
{
  a_widget_ptr->setParentItem(d->m_scroll_frame);
  a_widget_ptr->setParent(d->m_scroll_frame);

  a_widget_ptr->on_input_event([this](int type, const Widget * widget) {
    if (type == Widget::kMouseReleaseEvent) {
      if (d->m_activation_handler) {
        d->m_activation_handler(widget->widget_id());
      }
    }
  });

  switch (d->m_model_view_type) {
  case kListModel:
    insert_to_list_view(a_widget_ptr);
    break;
  case kGridModel:
    insert_to_grid_view(a_widget_ptr);
    break;
  case kTableModel:
    insert_to_table_view(a_widget_ptr);
    break;
  }

  update();
}

void ItemView::remove(Widget *a_widget_ptr) {}

void ItemView::insert(ModelViewItem *a_item_ptr)
{
   insert(a_item_ptr->view());

   a_item_ptr->set_index(d->m_model_item_list.count());
   d->m_model_item_list.append(a_item_ptr);
}

void ItemView::remove(ModelViewItem *a_item_ptr)
{
}

ModelViewItem *ItemView::at(int index)
{
  return d->m_model_item_list.at(index);
}

void ItemView::clear()
{
  if (d->m_list_layout->count() <= 0) {
    return;
  }

  while (d->m_list_layout->count() > 0) {
    int index = d->m_list_layout->count() - 1;
    Widget *item = (Widget *)(d->m_list_layout->itemAt(index));
    d->m_list_layout->removeAt(index);

    if (item) {
      delete item;
    }
  }

  d->m_list_layout->invalidate();
  d->m_list_layout->updateGeometry();

  qDeleteAll(d->m_model_item_list);
}

void ItemView::set_view_geometry(const QRectF &a_rect)
{
  d->m_viewport_geometry = a_rect;
}

QRectF ItemView::boundingRect() const
{
  return d->m_viewport_geometry;
}

void ItemView::setGeometry(const QRectF &a_rect)
{
  setPos(a_rect.topLeft());
}

QSizeF ItemView::sizeHint(Qt::SizeHint which,
                           const QSizeF &a_constraint) const
{
  return d->m_list_layout->contentsRect().size();
}

void ItemView::on_activated(std::function<void (int)> a_callback)
{
  d->m_activation_handler = a_callback;
}

bool ItemView::event(QEvent *e)
{
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

bool ItemView::sceneEvent(QEvent *e)
{
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
