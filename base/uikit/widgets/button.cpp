#include "button.h"

#include <widget.h>

#include <QGraphicsSceneEvent>
#include <QStyleOptionGraphicsItem>

#include <extensionmanager.h>
#include <resource_manager.h>

namespace UIKit {

class Button::PrivateButton {
public:
  typedef enum {
    NORMAL,
    PRESS,
    HOVER
  } ButtonState;

  PrivateButton() {}
  ~PrivateButton() {}

  QSizeF m_button_size;
  ButtonState mState;
  QString mLabel;
  QVariant mData;
};

Button::Button(QGraphicsObject *a_parent_ptr)
    : Widget(a_parent_ptr), d(new PrivateButton) {
  d->mState = PrivateButton::NORMAL;

  if (ResourceManager::style()) {
    set_size(QSize(ResourceManager::style()->attribute("button", "width").toFloat(),
                  ResourceManager::style()->attribute("button", "height").toFloat()));
  }

  setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton);
  setFlag(QGraphicsItem::ItemIsMovable, false);
  setFlag(QGraphicsItem::ItemIsFocusable, true);
  setFiltersChildEvents(true);
  setAcceptHoverEvents(true);
}

Button::~Button() { delete d; }

void Button::set_label(const QString &a_txt) {
  d->mLabel = a_txt;
  update();
}

void Button::mousePressEvent(QGraphicsSceneMouseEvent *a_event_ptr) {
  d->mState = PrivateButton::PRESS;
  update();
  Widget::mousePressEvent(a_event_ptr);
}

void Button::mouseReleaseEvent(QGraphicsSceneMouseEvent *a_event_ptr) {
  d->mState = PrivateButton::NORMAL;
  update();

  Widget::mouseReleaseEvent(a_event_ptr);
}

void Button::paint_normal_button(QPainter *a_painter_ptr,
                                 const QRectF &a_rect) {
  StyleFeatures feature;

  feature.text_data = d->mLabel;
  feature.geometry = a_rect;
  feature.render_state = StyleFeatures::kRenderElement;

  if (UIKit::ResourceManager::style()) {
    UIKit::ResourceManager::style()->draw("button", feature, a_painter_ptr);
  }
}

void Button::paint_sunken_button(QPainter *painter, const QRectF &a_rect) {
  StyleFeatures feature;

  feature.text_data = d->mLabel;
  feature.geometry = a_rect;
  feature.render_state = StyleFeatures::kRenderPressed;

  if (UIKit::ResourceManager::style()) {
    UIKit::ResourceManager::style()->draw("button", feature, painter);
  }
}

StylePtr Button::style() const { return UIKit::ResourceManager::style(); }

void Button::set_size(const QSizeF &a_size) {
  prepareGeometryChange();
  d->m_button_size = a_size;
  setMinimumSize(a_size);
  update();
}

QSizeF Button::sizeHint(Qt::SizeHint which, const QSizeF &a_constraint) const {
  return boundingRect().size();
}

QRectF Button::boundingRect() const {
  return QRectF(0, 0, d->m_button_size.width(),
                d->m_button_size.height());
}

void Button::setGeometry(const QRectF &a_rect) {
  setPos(a_rect.topLeft());
}

void Button::set_action_data(const QVariant &a_data) { d->mData = a_data; }

QVariant Button::action_data() const { return d->mData; }

void Button::on_button_pressed(std::function<void()> a_handler) {}

void Button::paint_view(QPainter *a_painter_ptr, const QRectF &a_rect) {
  switch (d->mState) {
    case PrivateButton::NORMAL:
      paint_normal_button(a_painter_ptr, a_rect);
      break;
    case PrivateButton::PRESS:
      paint_sunken_button(a_painter_ptr, a_rect);
      break;
    default:
      qDebug() << Q_FUNC_INFO << "Unknown Button State";
  }
}

void Button::setIcon(const QImage & /*img*/) {}

QString Button::label() const { return d->mLabel; }
}
