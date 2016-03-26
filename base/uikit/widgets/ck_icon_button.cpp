#include "ck_icon_button.h"

#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <QGraphicsSceneHoverEvent>
#include <QPropertyAnimation>
#include <QAbstractAnimation>
#include <ck_resource_manager.h>

#include <QDebug>

namespace cherry_kit {

class icon_button::PrivateImageButton {
public:
  typedef enum {
    _kNormal,
    _kPressed,
    _kHover
  } ButtonState;

  PrivateImageButton() : m_state(_kNormal) {}
  ~PrivateImageButton() {}

  QPixmap m_button_icon;
  QString m_button_text;
  QColor mBgColor;

  ButtonState m_state;
};

void icon_button::set_background_color(const QColor &a_color) {
  o_image_button->mBgColor = a_color;
  update();
}

StylePtr icon_button::style() const { return resource_manager::style(); }

icon_button::icon_button(widget *a_parent_ptr)
    : widget(a_parent_ptr), o_image_button(new PrivateImageButton) {
  setFlag(QGraphicsItem::ItemIsMovable, false);
  setFlag(QGraphicsItem::ItemIsFocusable, true);
  setAcceptHoverEvents(true);

  o_image_button->mBgColor = Qt::transparent;
}

icon_button::~icon_button() { delete o_image_button; }

void icon_button::set_size(const QSize &a_size) {
  //setGeometry(QRectF(0, 0, a_size.width(), a_size.height()));
  set_contents_geometry(0, 0, a_size.width(), a_size.height());
}

QSizeF icon_button::sizeHint(Qt::SizeHint which,
                             const QSizeF &a_constraint) const {
  return geometry().size();
}

void icon_button::set_pixmap(const QPixmap &a_pixmap) {
  o_image_button->m_button_icon = a_pixmap;
  update();
}

void icon_button::set_lable(const QString &a_text) {
  o_image_button->m_button_text = a_text;
  update();
}

QString icon_button::text() const { return o_image_button->m_button_text; }

void icon_button::onZoomDone() {}

void icon_button::onZoomOutDone() {}

void icon_button::mouseReleaseEvent(QGraphicsSceneMouseEvent *a_event_ptr) {
  o_image_button->m_state = PrivateImageButton::_kNormal;
  update();
  widget::mouseReleaseEvent(a_event_ptr);
}

void icon_button::mousePressEvent(QGraphicsSceneMouseEvent *a_event_ptr) {
  Q_EMIT selected(true);
  o_image_button->m_state = PrivateImageButton::_kPressed;
  update();
  widget::mousePressEvent(a_event_ptr);
}

void icon_button::hoverEnterEvent(QGraphicsSceneHoverEvent *a_event_ptr) {
  a_event_ptr->accept();
  o_image_button->m_state = PrivateImageButton::_kHover;
  update();
  widget::hoverEnterEvent(a_event_ptr);
}

void icon_button::hoverLeaveEvent(QGraphicsSceneHoverEvent *a_event_ptr) {
  a_event_ptr->accept();
  o_image_button->m_state = PrivateImageButton::_kNormal;
  update();
  widget::hoverLeaveEvent(a_event_ptr);
}

void icon_button::paint_view(QPainter *a_painter_ptr, const QRectF &a_rect) {
  style_data feature;

  feature.text_data = o_image_button->m_button_text;
  feature.image_data = o_image_button->m_button_icon;
  feature.geometry = a_rect;
  feature.render_state = style_data::kRenderElement;

  switch (o_image_button->m_state) {
  case PrivateImageButton::_kNormal:
    feature.render_state = style_data::kRenderElement;
    break;
  case PrivateImageButton::_kPressed:
    feature.render_state = style_data::kRenderPressed;
    break;
  default:
    qDebug() << Q_FUNC_INFO << "Unknown Button State";
  }

  if (cherry_kit::resource_manager::style()) {
    cherry_kit::resource_manager::style()->draw("image_button", feature,
                                                a_painter_ptr, this);
  }
}
}
