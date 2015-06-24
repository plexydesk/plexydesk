#include "progressbar.h"

#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <QFontMetrics>
#include <QFont>
#include <QDebug>
#include <QTextOption>
#include <QVariantAnimation>
#include <resource_manager.h>

namespace UIKit {

class ProgressBar::PrivateProgressBar {
public:
  PrivateProgressBar() {}
  ~PrivateProgressBar() {}

  QSizeF pixelSizeOfText(const QString &txt);
  QString mString;
  float mMaxValue;
  float mMinValue;
  float mValue;

  // progress animation
  QVariantAnimation *mProgressAnimation;
};

ProgressBar::ProgressBar(QGraphicsObject *parent)
    : Widget(parent), d(new PrivateProgressBar) {
  d->mMaxValue = 100.0;
  d->mMinValue = 1.0;
  d->mValue = (float)0.01;

  d->mProgressAnimation = new QVariantAnimation(this);
  d->mProgressAnimation->setDuration(100);
  d->mProgressAnimation->setStartValue(d->mMinValue);
  d->mProgressAnimation->setEndValue(d->mMaxValue);

  set_size(
      QSize(ResourceManager::style()->attribute("widget", "line_edit_width").toInt(),
            ResourceManager::style()->attribute("widget", "line_edit_height").toInt()));

  connect(d->mProgressAnimation, SIGNAL(valueChanged(QVariant)), this,
          SLOT(valueChanged(QVariant)));

  setFlag(QGraphicsItem::ItemIsMovable, false);
}

ProgressBar::~ProgressBar() {
  qDebug() << Q_FUNC_INFO;
  delete d;
}

void ProgressBar::set_label(const QString &a_txt) {
  d->mString = a_txt;
  Q_EMIT contentBoundingRectChaned();
}

/*
     this is a comment
     */

QString ProgressBar::label() const { return d->mString; }

void ProgressBar::set_size(const QSizeF &size) {
  setGeometry(QRectF(0, 0, size.width(), size.height()));
}

QSizeF ProgressBar::sizeHint(Qt::SizeHint which,
                             const QSizeF &a_constraint) const {
  return boundingRect().size();
}

int ProgressBar::max_range() { return d->mMaxValue; }

int ProgressBar::min_range() { return d->mMinValue; }

void ProgressBar::set_range(int a_min, int a_max) {
  d->mMinValue = a_min;
  d->mMaxValue = a_max;

  d->mProgressAnimation->setStartValue(d->mMinValue);
  d->mProgressAnimation->setEndValue(d->mMaxValue);

  update();
}

void ProgressBar::set_value(int a_value) {
  if (a_value > d->mMaxValue) {
    d->mValue = d->mMaxValue;
    d->mProgressAnimation->setEndValue(d->mValue);
    d->mProgressAnimation->start();
    update();
    return;
  }

  if (a_value < d->mMinValue) {
    d->mValue = d->mMinValue;
    d->mProgressAnimation->setEndValue(d->mValue);
    d->mProgressAnimation->start();
    update();
    return;
  }

  d->mValue = a_value;
  d->mProgressAnimation->setEndValue(d->mValue);
  d->mProgressAnimation->start();
  update();
}

void ProgressBar::on_value_changed(const QVariant &a_value) {
  d->mValue = a_value.toFloat();
  update();
}

void ProgressBar::mouseReleaseEvent(QGraphicsSceneMouseEvent *a_event_ptr) {
  Widget::mouseReleaseEvent(a_event_ptr);
}

void ProgressBar::paint(QPainter *a_painter_ptr,
                        const QStyleOptionGraphicsItem *a_option_ptr,
                        QWidget *a_widget_ptr) {
  a_painter_ptr->save();
  StyleFeatures features;
  features.render_state = StyleFeatures::kRenderBackground;
  features.geometry = a_option_ptr->exposedRect;

  if (UIKit::ResourceManager::style()) {
    UIKit::ResourceManager::style()->draw("linear_progress_bar", features, a_painter_ptr);
  }

  float percentage = (d->mValue / d->mMaxValue) * 100;
  float progressLevel = (a_option_ptr->exposedRect.width() / 100) * percentage;

  StyleFeatures progressFeatures;
  progressFeatures.geometry =
      QRectF(0.0, 0.0, progressLevel, a_option_ptr->exposedRect.height());

  progressFeatures.render_state = StyleFeatures::kRenderForground;

  if (UIKit::ResourceManager::style()) {
    UIKit::ResourceManager::style()->draw("linear_progress_bar", progressFeatures,
                                a_painter_ptr);
  }

  a_painter_ptr->restore();
}

QSizeF ProgressBar::PrivateProgressBar::pixelSizeOfText(const QString &txt) {
  QFont font("Times", 20, QFont::Bold);
  QFontMetrics metic(font);
  QRectF rect = metic.boundingRect(txt);

  return rect.size();
}
}
