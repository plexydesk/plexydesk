#include "ck_item_view.h"

#include <QDebug>
#include <QGraphicsGridLayout>
#include <QGraphicsLinearLayout>
#include <QGraphicsWidget>

#include <QScroller>
#include <ck_scrollbar.h>

namespace cherry_kit {

class item_view::PrivateModelView {
public:
  PrivateModelView() : m_needs_scrollbars(0), m_item_count(0) {}
  ~PrivateModelView() {}

  ModelType m_model_view_type;
  QGraphicsWidget *m_scroll_frame;
  cherry_kit::scrollbar *m_verticle_scrollbar;
  bool m_needs_scrollbars;

  QGraphicsLinearLayout *m_list_layout;
  QGraphicsGridLayout *m_grid_layout;
  QRectF m_viewport_geometry;

  QList<model_view_item *> m_model_item_list;

  std::function<void(int index)> m_activation_handler;
  std::function<void(model_view_item *)> m_item_remove_handler;
  int m_item_count;

  std::vector<widget *> m_widget_vector;
  widget *m_frame;
};

void item_view::set_content_margin(int a_left, int a_right, int a_top,
                                   int a_bottom) {
  if (d->m_grid_layout && d->m_model_view_type == kGridModel) {
    d->m_grid_layout->setContentsMargins(a_left, a_right, a_top, a_bottom);
  }
}

void item_view::set_content_spacing(int a_distance) {
  if (d->m_grid_layout && d->m_model_view_type == kGridModel) {
    d->m_grid_layout->setSpacing(a_distance);
  }
}

void item_view::set_enable_scrollbars(bool a_state) {
  d->m_needs_scrollbars = a_state;

  if (a_state)
    d->m_verticle_scrollbar->show();
  else
    d->m_verticle_scrollbar->hide();
}

item_view::item_view(widget *parent, ModelType a_model_type)
    : widget(parent), d(new PrivateModelView) {
  d->m_model_view_type = a_model_type;
  d->m_scroll_frame = new QGraphicsWidget(this);
  //d->m_scroll_frame->hide();
  d->m_frame = new widget(this);
  d->m_frame->setParentItem(this);

  d->m_verticle_scrollbar = new scrollbar(this);
  d->m_verticle_scrollbar->setZValue(d->m_frame->zValue() + 1000);
  d->m_verticle_scrollbar->hide();

  d->m_verticle_scrollbar->on_value_changed(
      [this](int value) { d->m_frame->setPos(d->m_frame->x(), value); });

  if (d->m_model_view_type == kListModel) {
    d->m_list_layout = new QGraphicsLinearLayout(d->m_scroll_frame);
    d->m_list_layout->setOrientation(Qt::Vertical);
    d->m_list_layout->setContentsMargins(0, 0, 0, 0);
  }

  if (d->m_model_view_type == kGridModel) {
    d->m_grid_layout = new QGraphicsGridLayout(d->m_scroll_frame);
    set_content_margin(4, 4, 4, 4);
  }

  d->m_viewport_geometry = QRectF();
  d->m_frame->set_geometry(QRectF());

  setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton);
  setFlag(QGraphicsItem::ItemIsMovable, false);
  setFlag(QGraphicsItem::ItemIsFocusable, true);
  setFlag(QGraphicsItem::ItemClipsChildrenToShape, true);

  // QScroller::grabGesture(this, QScroller::LeftMouseButtonGesture);
}

item_view::~item_view() {
  // QScroller::ungrabGesture(this);
  delete d;
}

void item_view::insert_to_list_view(widget *a_widget_ptr) {
  a_widget_ptr->set_widget_id(d->m_list_layout->count());

  d->m_list_layout->addItem(a_widget_ptr);
  d->m_list_layout->updateGeometry();
  d->m_list_layout->activate();

  d->m_frame->set_geometry(d->m_list_layout->geometry());

  d->m_verticle_scrollbar->set_maximum_value(
      d->m_list_layout->contentsRect().height());
}

void item_view::remove_from_list_view(widget *a_widget_ptr) {
  if (!d->m_list_layout)
    return;
  d->m_list_layout->removeItem(a_widget_ptr);
  d->m_list_layout->updateGeometry();
  d->m_list_layout->activate();

  d->m_verticle_scrollbar->set_maximum_value(
      d->m_list_layout->contentsRect().height());
}

void item_view::insert_to_grid_view(widget *a_widget_ref) {
  /*
  a_widget_ptr->set_widget_id(d->m_grid_layout->count());
  int l_item_per_row =
      (d->m_viewport_geometry.width()) /
  a_widget_ptr->contents_geometry().width();

  if (l_item_per_row <= 0)
      l_item_per_row = 1;

  l_item_per_row = 4;
  d->m_grid_layout->addItem(a_widget_ptr,
                            d->m_grid_layout->count() / (l_item_per_row),
                            d->m_grid_layout->count() % (l_item_per_row));
  d->m_grid_layout->setAlignment(a_widget_ptr, Qt::AlignVCenter);

  d->m_grid_layout->activate();
  d->m_grid_layout->updateGeometry();
  d->m_scroll_frame->setGeometry(d->m_grid_layout->geometry());

  qDebug() << Q_FUNC_INFO << d->m_grid_layout->geometry();

  d->m_verticle_scrollbar->set_maximum_value(
      d->m_grid_layout->geometry().height());

  check_needs_scrolling();
  */

  a_widget_ref->setParentItem(d->m_frame);

  /*
  a_widget_ref->setZValue(d->m_frame->zValue() + 1);
  a_widget_ref->show();
  d->m_frame->show();
  */

  d->m_frame->set_contents_geometry(0, 0, 1000, 1000);
  d->m_frame->setPos(0,0);

  a_widget_ref->set_widget_id(d->m_item_count);
  d->m_widget_vector.push_back(a_widget_ref);

  float current_content_width = 0;
  float current_content_height = 0;

  std::for_each(std::begin(d->m_widget_vector), std::end(d->m_widget_vector),
                [&](widget *w_ref) {
                  current_content_width += w_ref->geometry().width();
                });

  float grid_width = 96.0f;

  if (d->m_widget_vector.size() > 0) {
    grid_width = current_content_width / d->m_widget_vector.size();
  }

  int l_item_per_row = (d->m_viewport_geometry.width()) /
                       a_widget_ref->contents_geometry().width();

  if (l_item_per_row <= 0)
    l_item_per_row = 1;

  a_widget_ref->setPos(a_widget_ref->contents_geometry().width() *
                           (d->m_item_count % l_item_per_row),
                       a_widget_ref->contents_geometry().height() *
                           (d->m_item_count / l_item_per_row));
  /*
  d->m_frame->set_contents_geometry(0, 0,
                                    d->m_frame->geometry().width(),
                                    d->m_frame->geometry().height());
  */

  d->m_item_count++;
  check_needs_scrolling();

  qDebug() << Q_FUNC_INFO << "Items Per Row: " << l_item_per_row;
  qDebug() << Q_FUNC_INFO
           << "ScrollView Geometry :" << d->m_frame->geometry();
  qDebug() << Q_FUNC_INFO << "Viewport : " << d->m_viewport_geometry;
  qDebug() << Q_FUNC_INFO << "Item Geometry " << a_widget_ref->boundingRect();
  qDebug() << Q_FUNC_INFO << "Item Location : " << a_widget_ref->pos();
  qDebug() << Q_FUNC_INFO << "Frame Location : " << d->m_frame->pos();
  qDebug() << Q_FUNC_INFO << "Item Contents Geometry "
           << a_widget_ref->contents_geometry();
}

void item_view::insert_to_table_view(widget *a_widget_ptr) {}

void item_view::insert(widget *a_widget_ptr) {
  a_widget_ptr->setParentItem(d->m_frame);
  a_widget_ptr->setParent(d->m_frame);

  a_widget_ptr->on_click([=]() {
    if (d->m_activation_handler) {
      d->m_activation_handler(a_widget_ptr->widget_id());
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

  check_needs_scrolling();
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

int item_view::count() const {
  if (d->m_model_view_type == kGridModel && d->m_grid_layout) {
    return d->m_grid_layout->count();
  } else if (d->m_model_view_type == kListModel && d->m_list_layout) {
    return d->m_list_layout->count();
  }
  return 0;
}

void item_view::set_filter(const QString &a_keyword) {
  foreach (model_view_item *_item, d->m_model_item_list) {
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
  qDebug() << Q_FUNC_INFO << "cleaering";
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

  Q_FOREACH (model_view_item *item, d->m_model_item_list) {
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
  // if (d->m_model_view_type == kGridModel)

  d->m_frame->set_geometry(a_rect);
  d->m_verticle_scrollbar->set_size(QSizeF(16, a_rect.height()));

  int page_step = a_rect.height();

  if (d->m_model_view_type == kGridModel && d->m_grid_layout) {
    d->m_verticle_scrollbar->set_maximum_value(
        d->m_grid_layout->contentsRect().height());
  }

  d->m_verticle_scrollbar->set_page_step(20);

  adjust_scrollbar(a_rect);
  check_needs_scrolling();
}

QRectF item_view::boundingRect() const { return d->m_viewport_geometry; }

void item_view::check_needs_scrolling() {
  if (!d->m_needs_scrollbars)
    return;

  if (d->m_model_view_type == kGridModel && d->m_grid_layout) {
    int viewport_hight = d->m_viewport_geometry.height();
    int content_height = d->m_frame->geometry().height();

    if (viewport_hight < content_height)
      d->m_needs_scrollbars = false;
    else
      d->m_needs_scrollbars = true;
  }
}

void item_view::adjust_scrollbar(const QRectF &a_rect) {
  QPointF scrollbar_pos = QPointF(a_rect.width() - 16, a_rect.y());
  d->m_verticle_scrollbar->setPos(scrollbar_pos);
}

void item_view::set_geometry(const QRectF &a_rect) {
  set_contents_geometry(a_rect.x(), a_rect.y(), a_rect.width(),
                        a_rect.height());
  setPos(a_rect.topLeft());

  adjust_scrollbar(a_rect);
  check_needs_scrolling();
}

QSizeF item_view::sizeHint(Qt::SizeHint which,
                           const QSizeF &a_constraint) const {
  return d->m_viewport_geometry.size();
}

void item_view::on_activated(std::function<void(int)> a_callback) {
  d->m_activation_handler = a_callback;
}

void item_view::on_item_removed(
    std::function<void(model_view_item *)> a_handler) {
  // please note that the item can be set only once, and can't be over-ridden
  // once set. without destruction of the class.
  if (d->m_item_remove_handler)
    return;

  d->m_item_remove_handler = a_handler;
}
/*
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
*/
}
/*
  example:

  UiKit::ModelView model;

  model.setSource(ModelSource("rss"));
  model.setRenderer(ModelRender("rss"));

  model.onModelUpdate([](void) {});
*/
