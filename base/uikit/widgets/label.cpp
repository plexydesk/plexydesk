#include "label.h"

#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <QFontMetrics>
#include <QFont>
#include <QDebug>
#include <QTextOption>

namespace UI
{

class Label::PrivateLabel
{
public:
  PrivateLabel() {}
  ~PrivateLabel() {}

  QSizeF pixelSizeOfText(const QString &txt);

  QSizeF mSize;
  QString mString;
  QColor mBgColor, mFgColor;
  uint mFontSize;
};

Label::Label(QGraphicsObject *parent) : Widget(parent), d(new PrivateLabel)
{
  d->mBgColor = Qt::transparent;
  d->mFgColor = QColor(88, 88, 88);
  d->mFontSize = 14;

  setFlag(QGraphicsItem::ItemIsMovable, false);
}

Label::~Label()
{
  // qDebug() << Q_FUNC_INFO;
  delete d;
}

void Label::setLabel(const QString &txt)
{
  d->mString = txt;
  update();
  Q_EMIT contentBoundingRectChaned();
}

QString Label::label() const { return d->mString; }

QRectF Label::boundingRect() const
{
  // QSizeF size = d->pixelSizeOfText(d->mString);

  if (d->mSize.width() <= 0) {
    return contentBoundingRect();
  }

  return QRectF(0.0, 0.0, d->mSize.width(), d->mSize.height());
}

void Label::setSize(const QSizeF &size) { d->mSize = size; }

void Label::setFontSize(uint pixelSize) { d->mFontSize = pixelSize; }

QSizeF Label::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
{
  return boundingRect().size();
}

void Label::setGeometry(const QRectF &rect) { setPos(rect.topLeft()); }

QRectF Label::contentBoundingRect() const
{
  QFont font;
  font.setFamily(font.defaultFamily());
  font.setPointSize(d->mFontSize);
  QFontMetrics metic(font);
  QRectF rect = metic.boundingRect(d->mString);
  rect.setX(0.0);
  rect.setY(0.0);

  qDebug() << Q_FUNC_INFO << rect;
  return rect;
}

void Label::setLabelStyle(const QColor &backgroundColor,
                          const QColor &textColor)
{
  d->mBgColor = backgroundColor;
  d->mFgColor = textColor;
  update();
}

void Label::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
  Q_EMIT clicked();
}

void Label::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                  QWidget *widget)
{
  painter->save();
  painter->setPen(d->mFgColor);

  QFontMetrics metrics(painter->font());
  QString elidedText =
    metrics.elidedText(d->mString, Qt::ElideMiddle, d->mSize.width());

  QPainterPath path;
  painter->setRenderHints(QPainter::SmoothPixmapTransform |
                          QPainter::Antialiasing |
                          QPainter::HighQualityAntialiasing);
  path.addRoundedRect(option->exposedRect, 6, 6);
  QFont drawFont = painter->font();
  drawFont.setPixelSize(d->mFontSize);
  painter->setFont(drawFont);
  painter->fillPath(path, d->mBgColor);
  painter->drawText(option->exposedRect, d->mString,
                    QTextOption(Qt::AlignCenter));
  painter->restore();
}

QSizeF Label::PrivateLabel::pixelSizeOfText(const QString &txt)
{
  QFont font("Times", 20, QFont::Bold);
  QFontMetrics metic(font);
  QRectF rect = metic.boundingRect(txt);

  return rect.size();
}
}
