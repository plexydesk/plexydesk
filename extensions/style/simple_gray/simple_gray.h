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
#ifndef COCOA_ACTIVITY_H
#define COCOA_ACTIVITY_H

#include <QtCore>

#include <ck_data_source.h>
#include <ck_style.h>
#include <ck_resource_manager.h>

using namespace cherry_kit;

class SimpleGrayStyle : public cherry_kit::style {
public:
  SimpleGrayStyle();

  virtual ~SimpleGrayStyle();

  virtual void draw(const QString &type, const style_data &options,
                    QPainter *painter, const cherry_kit::widget *aWidget = 0);
  void load_default_widget_style_properties();

  void draw_timer_marker(QRectF rect, QTransform _xform_hour, QPainter *p,
                         double mark_minutes, double mark_hour,
                         QPointF current_marker_location, QPointF _transPos,
                         QPointF current_marker_location_for_min);
  void draw_range_marker(QRectF rect, QTransform _xform_hour, QPainter *p,
                         double mark_start, double mark_end,
                         QPointF current_marker_location, QPointF _transPos,
                         QPointF current_marker_location_for_min);
  void set_default_painter_hints(QPainter *painter);

protected:
  QVariantMap attribute_map(const QString &type) const;
  virtual QVariantMap color_scheme_map() const;

private:
  class PrivateSimpleGray;
  PrivateSimpleGray *const d;
  void draw_push_button(const style_data &features, QPainter *painter);
  void draw_window_button(const style_data &features, QPainter *painter);
  void draw_window_frame(const style_data &features, QPainter *a_ctx);
  void
  draw_clock_hands(QPainter *a_ctx, QRectF rect, int factor, float angle,
                   cherry_kit::resource_manager::ColorName a_clock_hand_color,
                   int a_thikness);
  void draw_clock_surface(const style_data &features, QPainter *painter);
  void draw_clock_surface_to_buffer(const style_data &features,
                                    QPainter *a_ctx);

  void draw_knob(const style_data &features, QPainter *a_ctx);
  void draw_line_edit(const style_data &features, QPainter *painter);
  void draw_text_editor(const style_data &features, const QString &text,
                        QPainter *a_ctx);
  void drawSeperatorLine(const style_data &features, QPainter *a_ctx);
  void draw_progress_bar(const style_data &features, QPainter *a_ctx);
  void drawVListItem(const style_data &features, QPainter *painter);
  void draw_image_button(const style_data &features, QPainter *a_ctx);
  void draw_scrollbar(const style_data &features, QPainter *a_ctx);
  void draw_scrollbar_slider(const style_data &features, QPainter *a_ctx);

  void draw_label(const style_data &a_features, QPainter *a_ctx);
};

#endif
