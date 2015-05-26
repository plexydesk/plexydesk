#include "dialwidget.h"

#include <QDebug>
#include <themepackloader.h>

#include <iostream>
#include <cmath>

#define PI 3.14159258

namespace UIKit {
typedef std::function<void(int)> on_dial_callback_func;

class DialWidget::PrivateDialWidget {
 public:
  PrivateDialWidget() : m_is_set(0), m_is_pressed(0) {}
  ~PrivateDialWidget() {}

  QString convertAngleToTimeString(float angle);
  void calculateValue();

  float mAngle;
  int mMaxAngle;
  int mMinAngle;
  int mStartAngle;
  int mMaxValue;
  QPointF mInitPos;

  int mProgressValue;
  bool m_is_set;
  bool m_is_pressed;

  std::vector<on_dial_callback_func> m_on_dial_callback_list;
};

DialWidget::DialWidget(QGraphicsObject* parent)
    : UIKit::Widget(parent), d(new PrivateDialWidget) {
  set_widget_flag(UIKit::Widget::kRenderBackground, false);
  set_widget_flag(UIKit::Widget::kConvertToWindowType, false);
  set_widget_flag(UIKit::Widget::kRenderDropShadow, false);

  setFlag(QGraphicsItem::ItemIsMovable, false);
  setFlag(QGraphicsItem::ItemIsSelectable, true);

  d->mAngle = 0;
  d->mMaxAngle = 360;
  d->mProgressValue = 0;
  d->mStartAngle = 0;
  d->mMaxValue = 24;
}

DialWidget::~DialWidget() {}

void DialWidget::set_maximum_dial_value(float maxValue) { d->mMaxValue = maxValue; }

float DialWidget::maximum_dial_value() const { return d->mMaxValue; }

float DialWidget::current_dial_value() const { return d->mProgressValue; }

void DialWidget::reset() {
  d->mAngle = 0;
  d->mMaxAngle = 360;
  d->mProgressValue = 0;
  d->mStartAngle = 0;
  d->mMaxValue = 24;
  d->m_is_set = 0;
  update();
}

void DialWidget::on_dialed(std::function<void (int)> a_callback) {
    d->m_on_dial_callback_list.push_back(a_callback);
}

void DialWidget::dragMoveEvent(QGraphicsSceneDragDropEvent* event) {
  qDebug() << Q_FUNC_INFO << event->pos();
}

void DialWidget::mousePressEvent(QGraphicsSceneMouseEvent* event) {
  d->mInitPos = mapToScene(event->pos());
  event->accept();
  QGraphicsItem::mousePressEvent(event);
  d->m_is_set = 1;
}

void DialWidget::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
  event->accept();
  // QGraphicsItem::mouseReleaseEvent(event);
  d->m_is_pressed = 0;
}

void DialWidget::PrivateDialWidget::calculateValue() {
  //
  float decimalValue = (mAngle - mStartAngle) / (mMaxAngle / mMaxValue);
  if (decimalValue < 0) {
    decimalValue += mMaxValue;
  }

  int hours = (int)decimalValue;

  if (hours == mMaxValue) {
    hours = 0;
  }

  mProgressValue = hours;
}

QString DialWidget::PrivateDialWidget::convertAngleToTimeString(float angle) {
  QString time = "";
  float decimalValue = (angle - mStartAngle) / 15.0;
  if (decimalValue < 0) {
    decimalValue += mMaxValue;
  }

  int hours = (int)decimalValue;

  if (hours == mMaxValue) {
    hours = 0;
  }

  float minutes = ((int)angle % (int)15) / 0.25;

  return QString("%1:%2").arg(hours).arg(minutes);  // time;
}

static double angle_to(QLineF line1, QLineF line2) {
  // found on code-guru forum- thread 137249
  // calculate the angle between the line from p1 to p2
  // and the line from p3 to p4
  //
  // uses the theorem :
  //
  // given directional vectors v = ai + bj and w = ci + di
  //
  // then cos(angle) = (ac + bd) / ( |v| * |w| )
  //
  double a = (float)line1.p1().x() - line1.p2().x();
  double b = (float)line1.p1().y() - line1.p2().y();
  double c = (float)line2.p1().x() - line2.p2().x();
  double d = (float)line2.p1().y() - line2.p2().y();

  double cos_angle, angle;
  double mag_v1 = sqrt(a * a + b * b);
  double mag_v2 = sqrt(c * c + d * d);

  cos_angle = (a * c + b * d) / (mag_v1 * mag_v2);
  angle = acos(cos_angle);
  angle = angle * 180.0 / PI;

  return angle;
}

void DialWidget::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
  d->mInitPos = event->pos();
  QLineF line(boundingRect().center(), d->mInitPos);
  QLineF base_line(boundingRect().width() / 2,
                   0.0,
                   boundingRect().center().x(),
                   boundingRect().center().y());
  d->mAngle = angle_to(line, base_line) - 180;
  d->m_is_pressed = 1;
  update();

  std::for_each(std::begin(d->m_on_dial_callback_list),
                std::end(d->m_on_dial_callback_list),
                [=](on_dial_callback_func a_func) {

      if (a_func)
          a_func(d->mProgressValue);
  });
}

void DialWidget::paint_view(QPainter* painter, const QRectF& rect) {
  QRectF r(rect.x() + 16, rect.y() + 16, rect.width() - 32, rect.height() - 32);
  double angle = d->mAngle;

  if (d->mInitPos.x() < (boundingRect().width() / 2)) {
    angle = 360 + d->mAngle;
  }

  double angle_percent = (std::abs(angle) / d->mMaxAngle);
  if (d->m_is_set)
     d->mProgressValue = angle_percent * d->mMaxValue;

  StyleFeatures feature;

  feature.geometry = r;
  feature.render_state = StyleFeatures::kRenderElement;
  if (d->m_is_set)
      feature.render_state = StyleFeatures::kRenderRaised;
  if (d->m_is_pressed)
      feature.render_state = StyleFeatures::kRenderPressed;

  feature.attributes["angle"] = angle_percent;
  feature.attributes["max_value"] = d->mMaxValue;
  feature.text_data = QString("%1").arg(d->mProgressValue);

  if (UIKit::Theme::style()) {
    UIKit::Theme::style()->draw("knob", feature, painter);
  }
}
}
