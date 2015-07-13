#include "ck_item_view.h"

#include <QGraphicsLinearLayout>
#include <QDebug>
#include <QGraphicsWidget>
#include <QGraphicsGridLayout>

#include <QScroller>

namespace cherry_kit {

class item_view::PrivateModelView {
public:
  PrivateModelView() {}
  ~PrivateModelView() {}

  ModelType m_model_view_type;
  QGraphicsWidget *m_scroll_frame;

  QGraphicsLinearLayout *m_list_layout;
  QGraphicsGridLayout *m_grid_layout;
  QRectF m_viewport_geometry;

  QList<model_view_item *> m_model_item_list;

  std::function<void(int index)> m_activation_handler;
  std::function<void(model_view_item *)> m_item_remove_handler;
};

item_view::item_view(widget *parent, ModelType a_model_type)
    : widget(parent), d(new PrivateModelView) {
  d->m_model_view_type = a_model_type;
  d->m_scroll_frame = new QGraphicsWidget(this);

  if (d->m_model_view_type == kListModel) {
    d->m_list_layout = new QGraphicsLinearLayout(d->m_scroll_frame);
    d->m_list_layout->setOrientation(Qt::Vertical);
    d->m_list_layout->setContentsMargins(0, 0, 0, 0);
  }

  if (d->m_model_view_type == kGridModel) {
    d->m_grid_layout = new QGraphicsGridLayout(d->m_scroll_frame);
    d->m_grid_layout->setContentsMargins(4, 4, 4, 4);
  }

  d->m_viewport_geometry = QRectF();
  d->m_scroll_frame->setGeometry(QRectF());

  setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton);
  setFlag(QGraphicsItem::ItemIsMovable, false);
  setFlag(QGraphicsItem::ItemIsFocusable, true);
  setFlag(QGraphicsItem::ItemClipsChildrenToShape, true);

  QScroller::grabGesture(this, QScroller::LeftMouseButtonGesture);
}

item_view::~item_view() {
  QScroller::ungrabGesture(this);
  delete d;
}

void item_view::insert_to_list_view(widget *a_widget_ptr) {
  a_widget_ptr->set_widget_id(d->m_list_layout->count());

  d->m_list_layout->addItem(a_widget_ptr);
  d->m_list_layout->updateGeometry();
  d->m_list_layout->activate();

  d->m_scroll_frame->setGeometry(d->m_list_layout->geometry());
}

void item_view::remove_from_list_view(widget *a_widget_ptr) {
  if (!d->m_list_layout)
    return;
  d->m_list_layout->removeItem(a_widget_ptr);
}

void item_view::insert_to_grid_view(widget *a_widget_ptr) {
  if (!d->m_grid_layout) {
    return;
  }

  a_widget_ptr->set_widget_id(d->m_grid_layout->count());
  int l_item_per_row = (d->m_viewport_geometry.width() - 4) /
                       a_widget_ptr->boundingRect().width();

  d->m_grid_layout->addItem(a_widget_ptr,
                            d->m_grid_layout->count() / (l_item_per_row - 1),
                            d->m_grid_layout->count() % (l_item_per_row - 1));

  d->m_grid_layout->activate();
  d->m_grid_layout->updateGeometry();

  d->m_scroll_frame->setGeometry(d->m_grid_layout->geometry());
}

void item_view::insert_to_table_view(widget *a_widget_ptr) {}

void item_view::insert(widget *a_widget_ptr) {
  a_widget_ptr->setParentItem(d->m_scroll_frame);
  a_widget_ptr->setParent(d->m_scroll_frame);

  a_widget_ptr->on_input_event([this](int type, const widget *widget) {
    if (type == widget::kMouseReleaseEvent) {
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

void item_view::remove(widget *a_widget_ptr) {
  switch (d->m_model_view_type) {
  case kListModel:
    remove_from_list_view(a_widget_ptr);
    break;
  case kGridModel:
    // insert_to_grid_view(a_widget_ptr);
    break;
  case kTableModel:
    // insert_to_table_view(a_widget_ptr);
    break;
  }
}

void item_view::insert(model_view_item *a_item_ptr) {
  insert(a_item_ptr->view());

  a_item_ptr->set_index(d->m_model_item_list.count());
  d->m_model_item_list.append(a_item_ptr);
}

void item_view::remove(model_view_item *a_item_ptr) {}

model_view_item *item_view::at(int index) {
  return d->m_model_item_list.at(index);
}

void item_view::set_filter(const QString &a_keyword) {
  foreach(model_view_item * _item, d->m_model_item_list) {
    if (!_item)
      continue;
    if (!_item->view())
      continue;

    if (!_item->is_a_match(a_keyword)) {
      _item->view()->hide();
      remove(_item->view());
      continue;
    }

    insert(_item->view());
    _item->view()->show();
  }
}

void item_view::clear() {
  if (d->m_model_item_list.count() <= 0)
    return;

  if (d->m_model_view_type == kListModel) {
    if (d->m_list_layout->count() <= 0) {
      return;
    }

    while (d->m_list_layout->count() > 0) {
      d->m_list_layout->removeAt(d->m_list_layout->count() - 1);
    }

    d->m_list_layout->invalidate();
    d->m_list_layout->updateGeometry();
  }

  if (d->m_model_view_type == kGridModel) {
    if (d->m_grid_layout->count() <= 0)
      return;

    while (d->m_grid_layout->count() > 0) {
      d->m_grid_layout->removeAt(d->m_grid_layout->count() - 1);
    }
  }

  Q_FOREACH(model_view_item * item, d->m_model_item_list) {
    if (!item)
      continue;

    item->remove_view();
    if (d->m_item_remove_handler)
      d->m_item_remove_handler(item);
  }

  d->m_model_item_list.clear();
}

void item_view::set_view_geometry(const QRectF &a_rect) {
  d->m_viewport_geometry = a_rect;
  if (d->m_model_view_type == kGridModel)
    d->m_grid_layout->setGeometry(a_rect);
}

QRectF item_view::boundingRect() const {
  if (d->m_model_view_type == kGridModel)
    return d->m_grid_layout->contentsRect();

  return d->m_viewport_geometry;
}

void item_view::setGeometry(const QRectF &a_rect) { setPos(a_rect.topLeft()); }

QSizeF item_view::sizeHint(Qt::SizeHint which,
                          const QSizeF &a_constraint) const {
  return d->m_list_layout->contentsRect().size();
}

void item_view::on_activated(std::function<void(int)> a_callback) {
  d->m_activation_handler = a_callback;
}

void item_view::on_item_removed(std::function<void(model_view_item *)> a_handler) {
  // please note that the item can be set only once, and can't be over-ridden
  // once set. without destruction of the class.
  if (d->m_item_remove_handler)
    return;

  d->m_item_remove_handler = a_handler;
}

bool item_view::event(QEvent *e) {
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
    d->m_scroll_frame->setPos(-se->contentPos() - se->overshootDistance());
    return true;
  }

  default:
    break;
  }
  return QGraphicsObject::event(e);
}

bool item_view::sceneEvent(QEvent *e) {
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
