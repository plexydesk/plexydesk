#include "button.h"

#include <widget.h>

#include <QGraphicsSceneEvent>
#include <QStyleOptionGraphicsItem>

#include <extensionmanager.h>
#include <themepackloader.h>

namespace UIKit
{

class Button::PrivateButton
{
public:
  typedef enum { NORMAL, PRESS, HOVER } ButtonState;

  PrivateButton() {}
  ~PrivateButton() {}

  ButtonState mState;
  QString mLabel;
  QVariant mData;
};

Button::Button(QGraphicsObject *a_parent_ptr) : Widget(a_parent_ptr),
  d(new PrivateButton)
{
  d->mState = PrivateButton::NORMAL;

  if (Theme::style()) {
    setSize(QSize(Theme::style()->attribute("button", "width").toFloat(),
                  Theme::style()->attribute("button", "height").toFloat()));
  }

  setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton);
  setFlag(QGraphicsItem::ItemIsMovable, false);
  setFlag(QGraphicsItem::ItemIsFocusable, true);
  setFiltersChildEvents(true);
  setAcceptHoverEvents(true);
}

Button::~Button() { delete d; }

void Button::setLabel(const QString &a_txt) { d->mLabel = a_txt; }

void Button::mousePressEvent(QGraphicsSceneMouseEvent *a_event_ptr)
{
  d->mState = PrivateButton::PRESS;
  update();
  Widget::mousePressEvent(a_event_ptr);
}

void Button::mouseReleaseEvent(QGraphicsSceneMouseEvent *a_event_ptr)
{
  d->mState = PrivateButton::NORMAL;
  update();

  Widget::mouseReleaseEvent(a_event_ptr);
}

void Button::paint_normal_button(QPainter *a_painter_ptr, const QRectF &a_rect)
{
  StyleFeatures feature;

  feature.text_data = d->mLabel;
  feature.geometry = a_rect;
  feature.render_state = StyleFeatures::kRenderElement;

  if (UIKit::Theme::style()) {
    UIKit::Theme::style()->draw("button", feature,
        a_painter_ptr);
  }
}

void Button::paint_sunken_button(QPainter *painter, const QRectF &a_rect)
{
  StyleFeatures feature;

  feature.text_data = d->mLabel;
  feature.geometry = a_rect;
  feature.render_state = StyleFeatures::kRenderPressed;

  if (UIKit::Theme::style()) {
    UIKit::Theme::style()->draw("button", feature,
        painter);
  }
}

StylePtr Button::style() const
{
  return UIKit::Theme::style();
}

void Button::setSize(const QSize &size)
{
  setGeometry(QRectF(0, 0, size.width(), size.height()));
}

QSizeF Button::sizeHint(Qt::SizeHint which, const QSizeF &a_constraint) const
{
  return boundingRect().size();
}

void Button::set_action_data(const QVariant &a_data) { d->mData = a_data; }

QVariant Button::action_data() const { return d->mData; }

void Button::on_button_pressed(std::function<void ()> a_handler)
{
}

void Button::paint_view(QPainter *a_painter_ptr, const QRectF &a_rect)
{
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
