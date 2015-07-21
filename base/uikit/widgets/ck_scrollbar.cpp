#include "ck_scrollbar.h"

#include <QGraphicsSceneMouseEvent>
#include <ck_style.h>
#include <ck_resource_manager.h>

namespace cherry_kit {
class scrollbar::private_scrollbar {
public:
  private_scrollbar()
      : m_show_scrollbar(false), m_page_step(1), m_max_value(1),
        m_auto_hide(false) {}
  ~private_scrollbar() {}

  int get_current_top();

  float m_width;
  float m_height;
  float m_page_step;
  float m_max_value;

  QRectF m_handler_rect;
  QPointF m_current_pos;

  bool m_show_scrollbar;
  bool m_auto_hide;

  std::vector<value_changed_func_t> m_call_on_value_changed;
};

scrollbar::scrollbar(widget *a_parent_ptr)
    : widget(a_parent_ptr), priv(new private_scrollbar) {

  setFlag(QGraphicsItem::ItemIsMovable, false);
  setFlag(QGraphicsItem::ItemIsSelectable, true);

  setAcceptHoverEvents(true);
}

scrollbar::~scrollbar() { delete priv; }

void scrollbar::set_size(const QSizeF &a_size) {
  priv->m_width = a_size.width();
  priv->m_height = a_size.height();

  update_slider_geometry();
  update();
}

QRectF scrollbar::contents_geometry() const {
  return QRectF(0, 0, priv->m_width, priv->m_height);
}

void scrollbar::set_page_step(int a_step) {
  priv->m_page_step = a_step;

  update_slider_geometry();
  update();
}

float scrollbar::scale_factor() {
  float unit = 1;
  if (priv->m_max_value > priv->m_height) {
    unit = priv->m_max_value / priv->m_height;
  }

  return unit;
}

void scrollbar::update_slider_geometry() {
  priv->m_handler_rect.setX(0);
  priv->m_handler_rect.setY(priv->m_current_pos.y());
  priv->m_handler_rect.setWidth(priv->m_width - 2);
  priv->m_handler_rect.setHeight(priv->m_height / scale_factor());
}

void scrollbar::set_maximum_value(int a_value) {
  priv->m_max_value = a_value;

  priv->m_current_pos.setY(0);
  update_slider_geometry();
}

void scrollbar::set_auto_hide(bool a_state) {}

void scrollbar::on_value_changed(value_changed_func_t callback) {
  priv->m_call_on_value_changed.push_back(callback);
}

void scrollbar::paint_view(QPainter *ctx, const QRectF &expose_rect) {
  if (priv->m_auto_hide && !priv->m_show_scrollbar)
    return;

  style_data data;

  data.geometry = expose_rect;
  data.render_state = style_data::kRenderElement;

  if (cherry_kit::resource_manager::style()) {
    cherry_kit::resource_manager::style()->draw("scrollbar", data, ctx);

    data.geometry = priv->m_handler_rect;
    data.render_state = style_data::kRenderElement;

    cherry_kit::resource_manager::style()->draw("scrollbar_slider", data, ctx);
  }
}

void scrollbar::mousePressEvent(QGraphicsSceneMouseEvent *event) {
  event->accept();
}

void scrollbar::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
  event->accept();
}

void scrollbar::invoke_value_changed_callback(int a_value) {
  std::for_each(std::begin(priv->m_call_on_value_changed),
                std::end(priv->m_call_on_value_changed),
                [&](value_changed_func_t callback) {

    if (callback)
      callback(a_value);
  });
}

void scrollbar::move_slider(bool a_progress_front, int a_distance) {
  int move_handler_by = 0;

  if (a_progress_front) {
    if (std::abs(a_distance) > 10)
      a_distance = (-10);

    move_handler_by = priv->m_handler_rect.y() - std::abs(a_distance);
    if (move_handler_by < 0)
      move_handler_by = 0;

    priv->m_handler_rect.setY(move_handler_by);
  } else {
    if (std::abs(a_distance) > 10)
      a_distance = 10;

    move_handler_by = priv->m_handler_rect.y() + std::abs(a_distance);

    if ((move_handler_by + priv->m_handler_rect.height()) > priv->m_height)
      move_handler_by = priv->m_height - priv->m_handler_rect.height();

    priv->m_handler_rect.setY(move_handler_by);
  }

  invoke_value_changed_callback(-(move_handler_by * scale_factor()));

  priv->m_handler_rect.setHeight(priv->m_height / scale_factor());
}

void scrollbar::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
  /* calcualte the current pos from event*/
  QPointF pos = event->pos();
  int direction = pos.y() - priv->m_current_pos.y();
  bool go_up = (direction < 0);

  priv->m_current_pos = pos;

  move_slider(go_up, direction);

  update();
  event->accept();
}

void scrollbar::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
  event->accept();
  if (priv->m_auto_hide)
    priv->m_show_scrollbar = true;
  update();
}

void scrollbar::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
  event->accept();
  if (priv->m_auto_hide)
    priv->m_show_scrollbar = false;
  update();
}

int scrollbar::private_scrollbar::get_current_top() { return 0; }
}
