/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.org>
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
#ifndef COCOA_ACTIVITY_H
#define COCOA_ACTIVITY_H

#include <QtCore>
#include <plexy.h>

#include <datasource.h>
#include <QtNetwork>
#include <style.h>

using namespace UIKit;

class CocoaStyle : public UIKit::Style {
public:
  CocoaStyle();

  virtual ~CocoaStyle();


  virtual void draw(const QString &type, const StyleFeatures &options,
                    QPainter *painter, const UIKit::Widget *aWidget = 0);
  void load_default_widget_style_properties();

  void draw_timer_marker(QRectF rect, QTransform _xform_hour, QPainter *p,
                         double mark_minutes,
                         double mark_hour,
                         QPen current_dot_min_pen,
                         QPointF current_marker_location,
                         QPointF _transPos,
                         QPointF current_marker_location_for_min);
  void draw_range_marker(QRectF rect, QTransform _xform_hour,
                         QPainter *p, double mark_start,
                         double mark_end, QPen current_dot_min_pen,
                         QPointF current_marker_location, QPointF _transPos,
                         QPointF current_marker_location_for_min);
protected:
  QVariantMap attribute_map(const QString &type) const;
  virtual QVariantMap color_scheme_map() const;


private:
  class PrivateCocoa;
  PrivateCocoa *const d;
  void drawPushButton(const StyleFeatures &features, QPainter *painter);
  void drawWindowButton(const StyleFeatures &features, QPainter *painter);
  void drawFrame(const StyleFeatures &features, QPainter *painter);
  void draw_clock_hands(QPainter *p, QRectF rect, int factor, float angle,
                        QColor hand_color, int thikness);
  void drawClock(const StyleFeatures &features, QPainter *painter);
  void draw_knob(const StyleFeatures &features, QPainter *painter);
  void drawPushButtonText(const StyleFeatures &features, const QString &text,
                          QPainter *painter);
  void drawLineEdit(const StyleFeatures &features, QPainter *painter);
  void drawLineEditText(const StyleFeatures &features, const QString &text,
                        QPainter *painter);
  void drawLabelEditText(const StyleFeatures &features, const QString &text,
                         QPainter *painter);
  void drawSeperatorLine(const StyleFeatures &features, QPainter *painter);
  void drawProgressBar(const StyleFeatures &features, QPainter *painter);
  void drawVListItem(const StyleFeatures &features, QPainter *painter);
  void draw_image_button(const StyleFeatures &features, QPainter *a_ctx);

  void drawLabel(const StyleFeatures &aFeatures, QPainter *aPainterPtr,
                 const UIKit::Widget *aWidget);
};

#endif
