#include "ck_button.h"

#include <ck_widget.h>

#include <QGraphicsSceneEvent>
#include <QStyleOptionGraphicsItem>

#include <ck_extension_manager.h>
#include <ck_resource_manager.h>

namespace cherry_kit {

class button::PrivateButton {
public:
  typedef enum {
    NORMAL,
    PRESS,
    HOVER
  } ButtonState;

  PrivateButton() {}
  ~PrivateButton() {}

  void invoke_click_actions();

  QSizeF m_button_size;
  ButtonState m_button_state;
  QString m_button_text;
  QVariant m_button_data;

  std::vector<std::function<void ()>> m_click_action_list;
};

button::button(widget *a_parent_ptr)
    : widget(a_parent_ptr), priv(new PrivateButton) {
  priv->m_button_state = PrivateButton::NORMAL;

  if (resource_manager::style()) {
    set_size(QSize(
        resource_manager::style()->attribute("button", "width").toFloat(),
        resource_manager::style()->attribute("button", "height").toFloat()));
  }

  setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton);
  setFlag(QGraphicsItem::ItemIsMovable, false);
  setFlag(QGraphicsItem::ItemIsFocusable, true);
  setFiltersChildEvents(true);
  setAcceptHoverEvents(true);
}

button::~button() { delete priv; }

void button::set_label(const QString &a_txt) {
  priv->m_button_text = a_txt;
  update();
}

void button::mousePressEvent(QGraphicsSceneMouseEvent *a_event_ptr) {
  priv->m_button_state = PrivateButton::PRESS;
  update();
  widget::mousePressEvent(a_event_ptr);
}

void button::mouseReleaseEvent(QGraphicsSceneMouseEvent *a_event_ptr) {
  priv->m_button_state = PrivateButton::NORMAL;
  update();

  priv->invoke_click_actions();
}

void button::hoverEnterEvent(QGraphicsSceneHoverEvent *a_event_ptr) {
  priv->m_button_state = PrivateButton::HOVER;
  update();

  widget::hoverEnterEvent(a_event_ptr);
}

void button::hoverLeaveEvent(QGraphicsSceneHoverEvent *a_event_ptr) {
  priv->m_button_state = PrivateButton::NORMAL;
  update();

  widget::hoverLeaveEvent(a_event_ptr);
}

void button::paint_normal_button(QPainter *a_painter_ptr,
                                 const QRectF &a_rect) {
  style_data feature;

  feature.text_data = priv->m_button_text;
  feature.geometry = a_rect;
  feature.render_state = style_data::kRenderElement;

  if (cherry_kit::resource_manager::style()) {
    cherry_kit::resource_manager::style()->draw("button", feature,
                                                a_painter_ptr);
  }
}

void button::paint_sunken_button(QPainter *painter, const QRectF &a_rect) {
  style_data feature;

  feature.text_data = priv->m_button_text;
  feature.geometry = a_rect;
  feature.render_state = style_data::kRenderPressed;

  if (cherry_kit::resource_manager::style()) {
    cherry_kit::resource_manager::style()->draw("button", feature, painter);
  }
}

void button::paint_hover_button(QPainter *a_painter, const QRectF &a_rect) {
  style_data feature;

  feature.text_data = priv->m_button_text;
  feature.geometry = a_rect;
  feature.render_state = style_data::kRenderRaised;

  if (cherry_kit::resource_manager::style()) {
    cherry_kit::resource_manager::style()->draw("button", feature, a_painter);
  }
}

StylePtr button::style() const { return cherry_kit::resource_manager::style(); }

void button::set_size(const QSizeF &a_size) {
  prepareGeometryChange();
  priv->m_button_size = a_size;
  update();
}

QSizeF button::sizeHint(Qt::SizeHint which, const QSizeF &a_constraint) const {
  return contents_geometry().size();
}

void button::set_geometry(const QRectF &a_rect) {
    setPos(a_rect.topLeft());
    widget::set_geometry(a_rect);
}

void button::set_action_data(const QVariant &a_data) {
  priv->m_button_data = a_data;
}

QVariant button::action_data() const { return priv->m_button_data; }

void button::on_click(std::function<void()> a_callback) {
    priv->m_click_action_list.push_back(a_callback);
}

void button::paint_view(QPainter *a_painter_ptr, const QRectF &a_rect) {
  switch (priv->m_button_state) {
  case PrivateButton::NORMAL:
    paint_normal_button(a_painter_ptr, a_rect);
    break;
  case PrivateButton::PRESS:
    paint_sunken_button(a_painter_ptr, a_rect);
    break;
  case PrivateButton::HOVER:
    paint_hover_button(a_painter_ptr, a_rect);
  default:
    qDebug() << Q_FUNC_INFO << "Unknown Button State";
  }
}

void button::setIcon(const QImage & /*img*/) {}

QString button::text() const { return priv->m_button_text; }

void button::PrivateButton::invoke_click_actions()
{
    std::for_each(std::begin(m_click_action_list),
                  std::end(m_click_action_list),
                  [&](std::function<void()> a_func) {
        if (a_func)
            a_func();
    });
}

}
