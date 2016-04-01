#include "ck_item_view.h"

#include <QDebug>

#include <QScroller>
#include <ck_scrollbar.h>

namespace cherry_kit {

typedef std::pair<int, int> item_coordinate_t;
typedef widget *widget_ref_t;

class grid_model_container {
public:
  grid_model_container()
      : m_row_count(0), m_column_count(0), m_container_width(0),
        m_container_height(0), m_item_count_per_row(5), m_v_padding(5),
        m_h_padding(5) {}
  ~grid_model_container() {}

  void insert(widget *w_ref);
  void remove_item(const widget *w_ref);

  int row_count() const;
  int column_count() const;

  void set_geometry(float a_x, float a_y, float a_width, float a_height);

  void set_grid_size(int a_width, int a_height);
  int grid_size() const;

  void set_item_count_per_row(int a_count);

  void clear();
  float content_height();
  float content_width();

  int count();

  void set_padding(float a_padding) {
    float grid_width = m_grid_width - m_h_padding;
    float grid_height = m_grid_height - m_v_padding;

    m_h_padding = a_padding;
    m_v_padding = a_padding;

    set_grid_size(grid_width, grid_height);
  };

private:
  std::map<item_coordinate_t, widget_ref_t> m_widget_map;
  int m_row_count;
  int m_column_count;
  float m_container_width;
  float m_container_height;
  float m_x;
  float m_y;

  float m_item_count_per_row;

  int m_grid_width;
  int m_grid_height;

  float m_h_padding;
  float m_v_padding;
};

class linear_model_container {
public:
  linear_model_container() : m_height(0), m_width(0) {}
  ~linear_model_container() {}

  void insert(widget_ref_t a_widget);
  void remove(widget_ref_t a_widget);
  void clear();

  float container_height() const;
  float container_width() const;

  void set_grid_size(int a_width, int a_height);

  int count() const;

private:
  std::vector<widget_ref_t> m_widget_list;
  float m_width;
  float m_height;

  int m_grid_width;
  int m_grid_height;
};

class item_view::PrivateModelView {
public:
  PrivateModelView()
      : m_needs_scrollbars(0), m_item_count(0), m_margin_left(0),
        m_margin_right(0), m_margin_top(0), m_margin_bottom(0) {}
  ~PrivateModelView() {}

  ModelType m_model_view_type;
  cherry_kit::scrollbar *m_verticle_scrollbar;
  bool m_needs_scrollbars;

  QRectF m_viewport_geometry;

  QList<model_view_item *> m_model_item_list;

  std::function<void(int index)> m_activation_handler;
  std::function<void(model_view_item *)> m_item_remove_handler;
  int m_item_count;

  std::vector<widget *> m_widget_vector;
  widget *m_frame;

  grid_model_container m_grid_model_container;
  linear_model_container m_linear_model_container;

  int m_margin_left;
  int m_margin_right;
  int m_margin_top;
  int m_margin_bottom;
};

void item_view::set_content_margin(int a_left, int a_right, int a_top,
                                   int a_bottom) {
  d->m_margin_left = a_left;
  d->m_margin_right = a_right;
  d->m_margin_top = a_top;
  d->m_margin_bottom = a_bottom;

  d->m_frame->set_contents_geometry(
      a_left, a_top, d->m_frame->geometry().width() + (a_right + a_left),
      d->m_frame->geometry().height() + (a_top + a_bottom));
}

void item_view::set_content_spacing(int a_distance) {
  if (d->m_model_view_type == kGridModel) {
    d->m_grid_model_container.set_padding(a_distance);
  }
}

void item_view::set_content_size(int a_width, int a_height) {
  if (d->m_model_view_type == kGridModel) {
    d->m_grid_model_container.set_grid_size(a_width, a_height);
    d->m_verticle_scrollbar->set_page_step(a_height * 2);
  } else if (d->m_model_view_type == kListModel) {
    d->m_linear_model_container.set_grid_size(a_width, a_height);
  }
}

void item_view::set_enable_scrollbars(bool a_state) {
  d->m_needs_scrollbars = a_state;

  if (a_state)
    d->m_verticle_scrollbar->show();
  else
    d->m_verticle_scrollbar->hide();

  d->m_verticle_scrollbar->setZValue(d->m_frame->zValue() + 1);
}

item_view::item_view(widget *parent, ModelType a_model_type)
    : widget(parent), d(new PrivateModelView) {
  d->m_model_view_type = a_model_type;
  // d->m_scroll_frame->hide();
  d->m_frame = new widget(this);
  d->m_frame->setParentItem(this);

  d->m_verticle_scrollbar = new scrollbar(this);
  d->m_verticle_scrollbar->setZValue(d->m_frame->zValue() + 1000);
  d->m_verticle_scrollbar->set_page_step(60);
  d->m_verticle_scrollbar->show();

  d->m_verticle_scrollbar->on_value_changed(
      [this](int value) { d->m_frame->setPos(d->m_frame->x(), value); });

  if (d->m_model_view_type == kListModel) {
    d->m_linear_model_container.set_grid_size(96, 240);
  }

  if (d->m_model_view_type == kGridModel) {
    d->m_grid_model_container.set_grid_size(96.0, 96.0);
    set_content_margin(0, 0, 0, 0);
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
  a_widget_ptr->setParentItem(d->m_frame);
  a_widget_ptr->set_widget_id(d->m_linear_model_container.count());

  d->m_linear_model_container.insert(a_widget_ptr);

  d->m_frame->set_geometry(
      QRectF(0, 0, d->m_linear_model_container.container_width(),
             d->m_linear_model_container.container_height()));

  d->m_verticle_scrollbar->set_maximum_value(
      d->m_linear_model_container.container_height());

  check_needs_scrolling();
}

void item_view::remove_from_list_view(widget *a_widget_ptr) {
  d->m_verticle_scrollbar->set_maximum_value(
      d->m_linear_model_container.container_height());
}

float item_view::content_width() const {
  float current_content_width = 0;
  std::for_each(std::begin(d->m_widget_vector), std::end(d->m_widget_vector),
                [&](widget *w_ref) {
                  current_content_width += w_ref->geometry().width();
                });

  return current_content_width;
}

float item_view::content_height() const {
  float current_content_height = 0;
  std::for_each(std::begin(d->m_widget_vector), std::end(d->m_widget_vector),
                [&](widget *w_ref) {
                  current_content_height += w_ref->geometry().height();
                });

  return current_content_height;
}

void item_view::insert_to_grid_view(widget *a_widget_ref) {

  a_widget_ref->setParentItem(d->m_frame);

  a_widget_ref->set_widget_id(d->m_item_count++);
  d->m_widget_vector.push_back(a_widget_ref);

  d->m_grid_model_container.insert(a_widget_ref);

  d->m_frame->set_geometry(QRectF(d->m_frame->x() + d->m_margin_left,
                                  d->m_frame->y() + d->m_margin_top,
                                  d->m_grid_model_container.content_width() +
                                      (d->m_margin_left + d->m_margin_right),
                                  d->m_grid_model_container.content_height() +
                                      (d->m_margin_top + d->m_margin_bottom)));
#ifdef ENABLE_QT_DEBUG_ON
  qDebug() << Q_FUNC_INFO << "ScrollView Geometry :" << d->m_frame->geometry();
  qDebug() << Q_FUNC_INFO << "Viewport : " << d->m_viewport_geometry;
  qDebug() << Q_FUNC_INFO << "Item Geometry " << a_widget_ref->boundingRect();
  qDebug() << Q_FUNC_INFO << "Item Location : " << a_widget_ref->pos();
  qDebug() << Q_FUNC_INFO << "Frame Location : " << d->m_frame->pos();
  qDebug() << Q_FUNC_INFO << "Item Contents Geometry "
           << a_widget_ref->contents_geometry();
#endif

  d->m_verticle_scrollbar->set_maximum_value(
      d->m_grid_model_container.content_height());
  check_needs_scrolling();
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
  if (d->m_model_view_type == kGridModel) {
    return d->m_grid_model_container.row_count() *
           d->m_grid_model_container.column_count();
  } else if (d->m_model_view_type == kListModel) {
    return d->m_linear_model_container.count();
  }
  return 0;
}

void item_view::set_column_count(int a_count) {
  d->m_grid_model_container.set_item_count_per_row(a_count);
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
  if (d->m_model_item_list.count() <= 0) {
    qDebug() << Q_FUNC_INFO << "No Items in list";
    return;
  }

  if (d->m_model_view_type == kListModel) {
    if (d->m_linear_model_container.count() <= 0) {
      return;
    }

    d->m_linear_model_container.clear();

    d->m_frame->set_contents_geometry(
        0, 0, d->m_linear_model_container.container_width(),
        d->m_linear_model_container.container_height());
    check_needs_scrolling();
  }

  if (d->m_model_view_type == kGridModel) {
    if (d->m_grid_model_container.count() <= 0) {
      qDebug() << Q_FUNC_INFO
               << "Model Has no Items : " << d->m_grid_model_container.count();
      return;
    }

    d->m_grid_model_container.clear();

    d->m_frame->set_contents_geometry(
        0, 0, d->m_grid_model_container.content_width(),
        d->m_grid_model_container.content_height());
    check_needs_scrolling();
  }

  Q_FOREACH (model_view_item *item, d->m_model_item_list) {
    qDebug() << Q_FUNC_INFO << "Delete Request";

    if (!item)
      continue;

    // item->remove_view();
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

  if (d->m_model_view_type == kGridModel) {
    d->m_verticle_scrollbar->set_maximum_value(
        d->m_grid_model_container.content_height());
  }

  adjust_scrollbar(a_rect);
  check_needs_scrolling();
}

QRectF item_view::boundingRect() const { return d->m_viewport_geometry; }

void item_view::check_needs_scrolling() {
  if (!d->m_needs_scrollbars)
    return;

  if (d->m_model_view_type == kGridModel) {
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

// grid model class defs;
void grid_model_container::insert(widget *w_ref) {
  if (!w_ref)
    return;

  int item_count = m_widget_map.size();

  m_row_count = item_count / m_item_count_per_row;

  float x_coord = ((m_column_count++) * m_grid_width);
  float y_coord = (m_row_count * m_grid_height) + m_h_padding;

  float h_align = (m_grid_width - w_ref->contents_geometry().width()) / 2;
  float v_align = (m_grid_height - w_ref->contents_geometry().height()) / 2;

  w_ref->set_coordinates(h_align + x_coord, v_align + y_coord);

  item_coordinate_t pos(m_row_count, m_column_count);
  m_widget_map[pos] = w_ref;

  if (m_column_count >= m_item_count_per_row) {
    m_column_count = 0;
  }
}

void grid_model_container::remove_item(const widget *w_ref) {}

int grid_model_container::row_count() const { return m_row_count + 1; }

int grid_model_container::column_count() const { return m_column_count + 1; }

void grid_model_container::set_geometry(float a_x, float a_y, float a_width,
                                        float a_height) {
  m_x = a_x;
  m_y = a_y;
  m_container_width = a_width;
  m_container_height = a_height;
}

void grid_model_container::set_grid_size(int a_width, int a_height) {
  m_grid_width = a_width + m_h_padding;
  m_grid_height = a_height + m_v_padding;
}

int grid_model_container::grid_size() const { return m_grid_width; }

void grid_model_container::set_item_count_per_row(int a_count) {
  m_item_count_per_row = a_count;
}

void grid_model_container::clear() {
  m_column_count = 0;
  m_row_count = 0;
  m_widget_map.clear();
}

float grid_model_container::content_height() {
  return (m_grid_height * (m_row_count + 1));
}

float grid_model_container::content_width() {
  return (m_grid_width * m_item_count_per_row);
}

int grid_model_container::count() { return (row_count() * column_count()); }

void linear_model_container::insert(widget_ref_t a_widget) {
  if (!a_widget)
    return;

  m_widget_list.push_back(a_widget);
  a_widget->set_coordinates(0, m_height);

  m_height += a_widget->contents_geometry().height();
}

void linear_model_container::remove(widget_ref_t a_widget) {
  int item_index = -1;
  int count = 0;
  for (widget *_widget : m_widget_list) {
    count++;

    if (!_widget)
      continue;

    if (_widget == a_widget) {
      item_index = count;
      break;
    }
  }

  m_widget_list.erase(m_widget_list.begin() + item_index);
}

void linear_model_container::clear() { m_widget_list.clear(); }

float linear_model_container::container_height() const { return m_height; }

float linear_model_container::container_width() const { return m_grid_width; }

void linear_model_container::set_grid_size(int a_width, int a_height) {
  m_grid_width = a_width;
  m_grid_height = a_height;
}

int linear_model_container::count() const { return m_widget_list.size(); }

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
