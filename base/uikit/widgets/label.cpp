#include "label.h"

#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <QFontMetrics>
#include <QFont>
#include <QDebug>
#include <QTextOption>
#include <themepackloader.h>

namespace UIKit {

class Label::PrivateLabel {
public:
  PrivateLabel() {}
  ~PrivateLabel() {}

  QSizeF pixelSizeOfText(const QString &txt);

  QSizeF m_label_size;
  QString m_label_string;
  uint m_font_size;
  Qt::Alignment m_alignment;
};

Label::Label(QGraphicsObject *parent) : Widget(parent), d(new PrivateLabel) {
  d->m_font_size = 14;
  d->m_alignment = Qt::AlignCenter;

  setFlag(QGraphicsItem::ItemIsMovable, false);
}

Label::~Label() {
  delete d;
}

void Label::set_label(const QString &a_txt) {
  d->m_label_string = a_txt;
  update();
}

QString Label::label() const { return d->m_label_string; }

QRectF Label::boundingRect() const {
  return QRectF(0.0, 0.0, d->m_label_size.width(), d->m_label_size.height());
}

void Label::set_size(const QSizeF &_asize) { d->m_label_size = _asize; }

void Label::set_font_size(uint pixelSize) { d->m_font_size = pixelSize; }

QSizeF Label::sizeHint(Qt::SizeHint which, const QSizeF &a_constraint) const {
  return boundingRect().size();
}

void Label::setGeometry(const QRectF &a_rect) { setPos(a_rect.topLeft()); }

QRectF Label::contents_bounding_rect() const {
  QFont font;
  font.setFamily(font.defaultFamily());
  font.setPixelSize(d->m_font_size);
  QFontMetrics metic(font);
  QRectF rect = metic.boundingRect(d->m_label_string);
  rect.setX(0.0);
  rect.setY(0.0);

  return rect;
}

void Label::set_label_style(const QColor &a_backgroundColor,
                            const QColor &a_textColor) {
  update();
}

int Label::alignment() { return d->m_alignment; }

void Label::set_alignment(int a_alignment) {
  d->m_alignment = (Qt::Alignment)a_alignment;
}

void Label::paint_view(QPainter *a_painter_ptr, const QRectF &a_rect) {
  StyleFeatures feature;

  feature.text_data = d->m_label_string;
  feature.geometry = a_rect;
  feature.render_state = StyleFeatures::kRenderElement;
  QTextOption text_option;
  text_option.setAlignment(d->m_alignment);
  feature.text_options = text_option;
  feature.attributes["font_size"] = d->m_font_size;

  if (UIKit::Theme::style()) {
    UIKit::Theme::style()->draw("label", feature, a_painter_ptr);
  }
}

QSizeF Label::PrivateLabel::pixelSizeOfText(const QString &txt) {
  QFont font("Times", 20, QFont::Bold);
  QFontMetrics metic(font);
  QRectF rect = metic.boundingRect(txt);

  return rect.size();
}
}
