#include "progressbar.h"

#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <QFontMetrics>
#include <QFont>
#include <QDebug>
#include <QTextOption>
#include <QVariantAnimation>
#include <themepackloader.h>

namespace UI
{

class ProgressBar::PrivateProgressBar
{
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
  : UIWidget(parent), d(new PrivateProgressBar)
{
  d->mMaxValue = 100.0;
  d->mMinValue = 1.0;
  d->mValue = (float)0.01;

  d->mProgressAnimation = new QVariantAnimation(this);
  d->mProgressAnimation->setDuration(100);
  d->mProgressAnimation->setStartValue(d->mMinValue);
  d->mProgressAnimation->setEndValue(d->mMaxValue);

  setSize(
    QSize(Theme::style()->attrbute("widget", "line_edit_width").toInt(),
          Theme::style()->attrbute("widget", "line_edit_height").toInt()));

  connect(d->mProgressAnimation, SIGNAL(valueChanged(QVariant)), this,
          SLOT(valueChanged(QVariant)));

  setFlag(QGraphicsItem::ItemIsMovable, false);
}

ProgressBar::~ProgressBar()
{
  qDebug() << Q_FUNC_INFO;
  delete d;
}

void ProgressBar::setLabel(const QString &txt)
{
  d->mString = txt;
  Q_EMIT contentBoundingRectChaned();
}

/*
     this is a comment
     */

QString ProgressBar::label() const { return d->mString; }

void ProgressBar::setSize(const QSizeF &size)
{
  setGeometry(QRectF(0, 0, size.width(), size.height()));
}

QSizeF ProgressBar::sizeHint(Qt::SizeHint which,
                             const QSizeF &constraint) const
{
  return boundingRect().size();
}

int ProgressBar::maxRange() { return d->mMaxValue; }

int ProgressBar::minRange() { return d->mMinValue; }

void ProgressBar::setRange(int min, int max)
{
  d->mMinValue = min;
  d->mMaxValue = max;

  d->mProgressAnimation->setStartValue(d->mMinValue);
  d->mProgressAnimation->setEndValue(d->mMaxValue);

  update();
}

void ProgressBar::setValue(int value)
{
  if (value > d->mMaxValue) {
    d->mValue = d->mMaxValue;
    d->mProgressAnimation->setEndValue(d->mValue);
    d->mProgressAnimation->start();
    update();
    return;
  }

  if (value < d->mMinValue) {
    d->mValue = d->mMinValue;
    d->mProgressAnimation->setEndValue(d->mValue);
    d->mProgressAnimation->start();
    update();
    return;
  }

  d->mValue = value;
  d->mProgressAnimation->setEndValue(d->mValue);
  d->mProgressAnimation->start();
  update();
}

void ProgressBar::onValueChanged(const QVariant &value)
{
  d->mValue = value.toFloat();
  update();
}

void ProgressBar::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
  Q_EMIT clicked();
}

void ProgressBar::paint(QPainter *painter,
                        const QStyleOptionGraphicsItem *option,
                        QWidget *widget)
{
  painter->save();
  StyleFeatures features;
  features.render_state = StyleFeatures::kRenderBackground;
  features.geometry = option->exposedRect;

  if (UI::Theme::style()) {
    UI::Theme::style()->draw("linear_progress_bar", features, painter);
  }

  float percentage = (d->mValue / d->mMaxValue) * 100;
  float progressLevel = (option->exposedRect.width() / 100) * percentage;

  StyleFeatures progressFeatures;
  progressFeatures.geometry =
    QRectF(0.0, 0.0, progressLevel, option->exposedRect.height());

  progressFeatures.render_state = StyleFeatures::kRenderForground;

  if (UI::Theme::style()) {
    UI::Theme::style()->draw("linear_progress_bar", progressFeatures,
                             painter);
  }

  painter->restore();
}

QSizeF ProgressBar::PrivateProgressBar::pixelSizeOfText(const QString &txt)
{
  QFont font("Times", 20, QFont::Bold);
  QFontMetrics metic(font);
  QRectF rect = metic.boundingRect(txt);

  return rect.size();
}
}
