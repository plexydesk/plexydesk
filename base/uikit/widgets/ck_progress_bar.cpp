#include "ck_progress_bar.h"

#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <QFontMetrics>
#include <QFont>
#include <QDebug>
#include <QTextOption>
#include <QVariantAnimation>
#include <ck_resource_manager.h>

namespace cherry_kit {

class progress_bar::PrivateProgressBar {
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

progress_bar::progress_bar(widget *parent)
    : widget(parent), o_progress_bar(new PrivateProgressBar) {
  o_progress_bar->mMaxValue = 100.0;
  o_progress_bar->mMinValue = 1.0;
  o_progress_bar->mValue = (float)0.01;

  o_progress_bar->mProgressAnimation = new QVariantAnimation(this);
  o_progress_bar->mProgressAnimation->setDuration(100);
  o_progress_bar->mProgressAnimation->setStartValue(o_progress_bar->mMinValue);
  o_progress_bar->mProgressAnimation->setEndValue(o_progress_bar->mMaxValue);

  set_size(QSize(
      resource_manager::style()->attribute("widget", "line_edit_width").toInt(),
      resource_manager::style()
          ->attribute("widget", "line_edit_height")
          .toInt()));

  connect(o_progress_bar->mProgressAnimation, SIGNAL(valueChanged(QVariant)),
          this, SLOT(valueChanged(QVariant)));

  setFlag(QGraphicsItem::ItemIsMovable, false);
}

progress_bar::~progress_bar() {
  qDebug() << Q_FUNC_INFO;
  delete o_progress_bar;
}

void progress_bar::set_label(const QString &a_txt) {
  o_progress_bar->mString = a_txt;
  Q_EMIT contentBoundingRectChaned();
}

/*
     this is a comment
     */

QString progress_bar::text() const { return o_progress_bar->mString; }

void progress_bar::set_size(const QSizeF &size) {
  setGeometry(QRectF(0, 0, size.width(), size.height()));
}

QSizeF progress_bar::sizeHint(Qt::SizeHint which,
                             const QSizeF &a_constraint) const {
  return boundingRect().size();
}

int progress_bar::max_range() { return o_progress_bar->mMaxValue; }

int progress_bar::min_range() { return o_progress_bar->mMinValue; }

void progress_bar::set_range(int a_min, int a_max) {
  o_progress_bar->mMinValue = a_min;
  o_progress_bar->mMaxValue = a_max;

  o_progress_bar->mProgressAnimation->setStartValue(o_progress_bar->mMinValue);
  o_progress_bar->mProgressAnimation->setEndValue(o_progress_bar->mMaxValue);

  update();
}

void progress_bar::set_value(int a_value) {
  if (a_value > o_progress_bar->mMaxValue) {
    o_progress_bar->mValue = o_progress_bar->mMaxValue;
    o_progress_bar->mProgressAnimation->setEndValue(o_progress_bar->mValue);
    o_progress_bar->mProgressAnimation->start();
    update();
    return;
  }

  if (a_value < o_progress_bar->mMinValue) {
    o_progress_bar->mValue = o_progress_bar->mMinValue;
    o_progress_bar->mProgressAnimation->setEndValue(o_progress_bar->mValue);
    o_progress_bar->mProgressAnimation->start();
    update();
    return;
  }

  o_progress_bar->mValue = a_value;
  o_progress_bar->mProgressAnimation->setEndValue(o_progress_bar->mValue);
  o_progress_bar->mProgressAnimation->start();
  update();
}

void progress_bar::on_value_changed(const QVariant &a_value) {
  o_progress_bar->mValue = a_value.toFloat();
  update();
}

void progress_bar::mouseReleaseEvent(QGraphicsSceneMouseEvent *a_event_ptr) {
  widget::mouseReleaseEvent(a_event_ptr);
}

void progress_bar::paint(QPainter *a_painter_ptr,
                        const QStyleOptionGraphicsItem *a_option_ptr,
                        QWidget *a_widget_ptr) {
  a_painter_ptr->save();
  style_data features;
  features.render_state = style_data::kRenderBackground;
  features.geometry = a_option_ptr->exposedRect;

  if (cherry_kit::resource_manager::style()) {
    cherry_kit::resource_manager::style()->draw("linear_progress_bar", features,
                                                a_painter_ptr);
  }

  float percentage = (o_progress_bar->mValue / o_progress_bar->mMaxValue) * 100;
  float progressLevel = (a_option_ptr->exposedRect.width() / 100) * percentage;

  style_data progressFeatures;
  progressFeatures.geometry =
      QRectF(0.0, 0.0, progressLevel, a_option_ptr->exposedRect.height());

  progressFeatures.render_state = style_data::kRenderForground;

  if (cherry_kit::resource_manager::style()) {
    cherry_kit::resource_manager::style()->draw(
        "linear_progress_bar", progressFeatures, a_painter_ptr);
  }

  a_painter_ptr->restore();
}

QSizeF progress_bar::PrivateProgressBar::pixelSizeOfText(const QString &txt) {
  QFont font("Times", 20, QFont::Bold);
  QFontMetrics metic(font);
  QRectF rect = metic.boundingRect(txt);

  return rect.size();
}
}
