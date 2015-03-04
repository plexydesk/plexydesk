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

Button::Button(QGraphicsObject *parent) : Widget(parent),
  d(new PrivateButton)
{
  d->mState = PrivateButton::NORMAL;

  if (Theme::style()) {
    setSize(QSize(Theme::style()->attrbute("button", "width").toFloat(),
                  Theme::style()->attrbute("button", "height").toFloat()));
  }

  setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton);
  setFlag(QGraphicsItem::ItemIsMovable, false);
  setFlag(QGraphicsItem::ItemIsFocusable, true);
  setFiltersChildEvents(true);
  setAcceptHoverEvents(true);
}

Button::~Button() { delete d; }

void Button::setLabel(const QString &txt) { d->mLabel = txt; }

void Button::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
  d->mState = PrivateButton::PRESS;
  update();
  Widget::mousePressEvent(event);
}

void Button::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
  d->mState = PrivateButton::NORMAL;
  update();

  Widget::mouseReleaseEvent(event);
}

void Button::paintNormalButton(QPainter *painter, const QRectF &rect)
{
  StyleFeatures feature;

  feature.text_data = d->mLabel;
  feature.geometry = rect;
  feature.render_state = StyleFeatures::kRenderElement;

  if (UIKit::Theme::instance()->defaultDesktopStyle()) {
    UIKit::Theme::instance()->defaultDesktopStyle()->draw("button", feature,
        painter);
  }
}

void Button::paintSunkenButton(QPainter *painter, const QRectF &rect)
{
  StyleFeatures feature;

  feature.text_data = d->mLabel;
  feature.geometry = rect;
  feature.render_state = StyleFeatures::kRenderPressed;

  if (UIKit::Theme::instance()->defaultDesktopStyle()) {
    UIKit::Theme::instance()->defaultDesktopStyle()->draw("button", feature,
        painter);
  }
}

StylePtr Button::style() const
{
  return UIKit::Theme::instance()->defaultDesktopStyle();
}

void Button::setSize(const QSize &size)
{
  setGeometry(QRectF(0, 0, size.width(), size.height()));
}

QSizeF Button::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
{
  return boundingRect().size();
}

void Button::setActionData(const QVariant &data) { d->mData = data; }

QVariant Button::actionData() const { return d->mData; }

void Button::setButtonPressCallback(std::function<void ()> handler)
{

}

void Button::paintView(QPainter *painter, const QRectF &rect)
{
  switch (d->mState) {
  case PrivateButton::NORMAL:
    paintNormalButton(painter, rect);
    break;
  case PrivateButton::PRESS:
    paintSunkenButton(painter, rect);
    break;
  default:
    qDebug() << Q_FUNC_INFO << "Unknown Button State";
  }
}

void Button::setIcon(const QImage & /*img*/) {}

QString Button::label() const { return d->mLabel; }
}
