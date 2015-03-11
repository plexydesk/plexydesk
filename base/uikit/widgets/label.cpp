#include "label.h"

#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <QFontMetrics>
#include <QFont>
#include <QDebug>
#include <QTextOption>

namespace UIKit
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

void Label::set_label(const QString &a_txt)
{
  d->mString = a_txt;
  update();
  Q_EMIT contentBoundingRectChaned();
}

QString Label::label() const { return d->mString; }

QRectF Label::boundingRect() const
{
  // QSizeF size = d->pixelSizeOfText(d->mString);

  /*
  if (d->mSize.width() <= 0) {
    return contentBoundingRect();
  }
  */

  return QRectF(0.0, 0.0, d->mSize.width(), d->mSize.height());
}

void Label::set_size(const QSizeF &_asize) { d->mSize = _asize; }

void Label::set_font_size(uint pixelSize) { d->mFontSize = pixelSize; }

QSizeF Label::sizeHint(Qt::SizeHint which, const QSizeF &a_constraint) const
{
  return boundingRect().size();
}

void Label::setGeometry(const QRectF &a_rect) { setPos(a_rect.topLeft()); }

QRectF Label::contents_bounding_rect() const
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

void Label::set_label_style(const QColor &a_backgroundColor,
                          const QColor &a_textColor)
{
  d->mBgColor = a_backgroundColor;
  d->mFgColor = a_textColor;
  update();
}

void Label::paint(QPainter *a_painter_ptr, const QStyleOptionGraphicsItem *a_option_ptr,
                  QWidget *a_widget_ptr)
{
  a_painter_ptr->save();
  a_painter_ptr->setPen(d->mFgColor);

  /*
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
  */
  //painter->fillRect(option->exposedRect, QColor("#ffffff"));
  a_painter_ptr->drawText(a_option_ptr->exposedRect, d->mString,
                    QTextOption(Qt::AlignCenter));
  a_painter_ptr->restore();
}

QSizeF Label::PrivateLabel::pixelSizeOfText(const QString &txt)
{
  QFont font("Times", 20, QFont::Bold);
  QFontMetrics metic(font);
  QRectF rect = metic.boundingRect(txt);

  return rect.size();
}
}
