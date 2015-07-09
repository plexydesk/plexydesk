/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.com>
*  Authored By  :
*
*  PlexyDesk is free software: you can redistribute it and/or modify
*  it under the terms of the GNU Lesser General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  PlexyDesk is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU Lesser General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with PlexyDesk. If not, see <http://www.gnu.org/licenses/lgpl.html>
*******************************************************************************/
#include "dialwidget.h"

#include <QDebug>
#include <resource_manager.h>

#include <iostream>
#include <cmath>

#define PI 3.14159258

namespace CherryKit {
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

DialWidget::DialWidget(Widget *parent)
    : CherryKit::Widget(parent), o_dial_widget(new PrivateDialWidget) {
  set_widget_flag(CherryKit::Widget::kRenderBackground, false);
  set_widget_flag(CherryKit::Widget::kConvertToWindowType, false);
  set_widget_flag(CherryKit::Widget::kRenderDropShadow, false);

  setFlag(QGraphicsItem::ItemIsMovable, false);
  setFlag(QGraphicsItem::ItemIsSelectable, true);

  o_dial_widget->mAngle = 0;
  o_dial_widget->mMaxAngle = 360;
  o_dial_widget->mProgressValue = 0;
  o_dial_widget->mStartAngle = 0;
  o_dial_widget->mMaxValue = 24;
}

DialWidget::~DialWidget() {}

void DialWidget::set_maximum_dial_value(float maxValue) {
  o_dial_widget->mMaxValue = maxValue;
}

float DialWidget::maximum_dial_value() const { return o_dial_widget->mMaxValue; }

float DialWidget::current_dial_value() const { return o_dial_widget->mProgressValue; }

void DialWidget::reset() {
  o_dial_widget->mAngle = 0;
  o_dial_widget->mMaxAngle = 360;
  o_dial_widget->mProgressValue = 0;
  o_dial_widget->mStartAngle = 0;
  o_dial_widget->mMaxValue = 24;
  o_dial_widget->m_is_set = 0;
  update();
}

void DialWidget::on_dialed(std::function<void(int)> a_callback) {
  o_dial_widget->m_on_dial_callback_list.push_back(a_callback);
}

void DialWidget::dragMoveEvent(QGraphicsSceneDragDropEvent *event) {
  qDebug() << Q_FUNC_INFO << event->pos();
}

void DialWidget::mousePressEvent(QGraphicsSceneMouseEvent *event) {
  o_dial_widget->mInitPos = mapToScene(event->pos());
  event->accept();
  QGraphicsItem::mousePressEvent(event);
  o_dial_widget->m_is_set = 1;
}

void DialWidget::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
  event->accept();
  // QGraphicsItem::mouseReleaseEvent(event);
  o_dial_widget->m_is_pressed = 0;
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

  return QString("%1:%2").arg(hours).arg(minutes); // time;
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

void DialWidget::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
  o_dial_widget->mInitPos = event->pos();
  QLineF line(boundingRect().center(), o_dial_widget->mInitPos);
  QLineF base_line(boundingRect().width() / 2, 0.0, boundingRect().center().x(),
                   boundingRect().center().y());
  o_dial_widget->mAngle = angle_to(line, base_line) - 180;
  o_dial_widget->m_is_pressed = 1;
  update();

  std::for_each(std::begin(o_dial_widget->m_on_dial_callback_list),
                std::end(o_dial_widget->m_on_dial_callback_list),
                [=](on_dial_callback_func a_func) {

    if (a_func)
      a_func(o_dial_widget->mProgressValue);
  });
}

void DialWidget::paint_view(QPainter *painter, const QRectF &rect) {
  float min_len = std::min(rect.width(), rect.height());
  min_len -= 16.0;
  QRectF r(rect.x() + ((rect.width() - min_len) / 2),
           rect.y() + ((rect.height() - min_len) / 2), min_len, min_len);
  double angle = o_dial_widget->mAngle;

  if (o_dial_widget->mInitPos.x() < (boundingRect().width() / 2)) {
    angle = 360 + o_dial_widget->mAngle;
  }

  double angle_percent = (std::abs(angle) / o_dial_widget->mMaxAngle);
  if (o_dial_widget->m_is_set)
    o_dial_widget->mProgressValue = angle_percent * o_dial_widget->mMaxValue;

  StyleFeatures feature;

  feature.geometry = r;
  feature.render_state = StyleFeatures::kRenderElement;
  if (o_dial_widget->m_is_set)
    feature.render_state = StyleFeatures::kRenderRaised;
  if (o_dial_widget->m_is_pressed)
    feature.render_state = StyleFeatures::kRenderPressed;

  feature.attributes["angle"] = angle_percent;
  feature.attributes["max_value"] = o_dial_widget->mMaxValue;
  feature.text_data = QString("%1").arg(o_dial_widget->mProgressValue);

  if (CherryKit::ResourceManager::style()) {
    CherryKit::ResourceManager::style()->draw("knob", feature, painter);
  }
}
}
