/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.com>
*  Authored By  : *
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
#include "cocoa.h"
#include <widget.h>
#include <plexyconfig.h>
#include <QTimer>
#include <view_controller.h>
#include <stylefeatures.h>

#include <cmath>
#include <resource_manager.h>

#include <px_bench.h>

// for the clock
int angle_between_hands(double h, double m) {
  // validate the input
  if (h < 0 || m < 0 || h > 12 || m > 60)
    printf("Wrong input");

  if (h == 12)
    h = 0;
  if (m == 60)
    m = 0;

  // Calculate the angles moved by hour and minute hands
  // with reference to 12:00
  int hour_angle = 0.5 * (h * 60 + m);
  int minute_angle = 6 * m;

  // Find the difference between two angles
  int angle = abs(hour_angle - minute_angle);

  // Return the smaller angle of two possible angles
  angle = std::min(360 - angle, angle);

  return angle;
}

class CocoaStyle::PrivateCocoa {
public:
  PrivateCocoa() {}
  ~PrivateCocoa() {}

  QColor color(resource_manager::ColorName a_name);
  void set_pen_color(QPainter *painter, resource_manager::ColorName a_name,
                     int a_thikness = 0);
  void set_default_font_size(QPainter *painter, int a_size = 11,
                             bool a_highlight = false);

  QHash<QString, int> m_type_map;
  QVariantMap m_attribute_map;
  QVariantMap m_color_map;
};

void CocoaStyle::load_default_widget_style_properties() {
  d->m_type_map["button"] = 1;
  d->m_type_map["vertical_list_item"] = 2;
  d->m_type_map["window_button"] = 3;
  d->m_type_map["seperator"] = 4;
  d->m_type_map["text_edit"] = 5;
  d->m_type_map["line_edit"] = 6;
  d->m_type_map["image_view"] = 7;
  d->m_type_map["image_frame"] = 8;
  d->m_type_map["label"] = 9;
  d->m_type_map["clock"] = 10;
  d->m_type_map["cell"] = 11;
  d->m_type_map["dial"] = 12;
  d->m_type_map["knob"] = 13;
  d->m_type_map["tool_button"] = 14;
  d->m_type_map["tool_bar"] = 15;
  d->m_type_map["dock_background"] = 16;
  d->m_type_map["desktop_background"] = 17;
  d->m_type_map["desktop_side_panel"] = 18;
  d->m_type_map["linear_progress_bar"] = 19;
  d->m_type_map["circular_progress_bar"] = 20;
  d->m_type_map["window_frame"] = 21;
  d->m_type_map["window_title"] = 22;
  d->m_type_map["window_resize_handle"] = 23;
  d->m_type_map["image_button"] = 23;

  // style attributes. this could be read from a xml file or a stylesheet.
  QVariantMap _frame_attributes;
  QVariantMap _widget_attributes;
  QVariantMap _size_attributes;
  QVariantMap _button_attributes;

  _frame_attributes["window_title_height"] = 64.0;
  _frame_attributes["window_minimized_height"] = 128.0;
  _frame_attributes["window_minimized_width"] = 96.0;
  _frame_attributes["window_blur_radius"] = 24.0;
  _frame_attributes["window_close_button_width"] = 16.0;
  _frame_attributes["window_close_button_height"] = 16.0;
  _frame_attributes["window_shadow_color"] = QString("#323232");
  _frame_attributes["padding"] = 5.0;

  _widget_attributes["line_edit_width"] = 256.0;
  _widget_attributes["line_edit_height"] = 32.0;

  _size_attributes["small"] = 0.5;
  _size_attributes["original"] = 1.0;
  _size_attributes["large"] = 2.0;
  _size_attributes["huge"] = 4.0;

  _button_attributes["width"] = 128.0;
  _button_attributes["height"] = 32.0;

  d->m_attribute_map["frame"] = _frame_attributes;
  d->m_attribute_map["widget"] = _widget_attributes;
  d->m_attribute_map["size"] = _size_attributes;
  d->m_attribute_map["button"] = _button_attributes;
}

CocoaStyle::CocoaStyle() : d(new PrivateCocoa) {
  load_default_widget_style_properties();
}

CocoaStyle::~CocoaStyle() { delete d; }

QVariantMap CocoaStyle::attribute_map(const QString &type) const {
  return d->m_attribute_map[type].toMap();
}

QVariantMap CocoaStyle::color_scheme_map() const { return d->m_color_map; }

void CocoaStyle::draw(const QString &type, const style_data &options,
                      QPainter *painter, const widget *aWidget) {
  switch (d->m_type_map[type]) {
  case 1:
    draw_push_button(options, painter);
    break;
  case 2:
    drawVListItem(options, painter);
    break;
  case 3:
    draw_window_button(options, painter);
    break;
  case 6:
    draw_line_edit(options, painter);
    break;
  case 9:
    draw_label(options, painter);
    break;
  case 10:
    draw_clock_surface(options, painter);
    // draw_clock_surface_to_buffer(options, painter);
    break;
  case 13:
    draw_knob(options, painter);
    break;
  case 19:
    draw_progress_bar(options, painter);
    break;
  case 21:
    draw_window_frame(options, painter);
    break;
  case 23:
    draw_image_button(options, painter);
    break;
  default:
    qWarning() << Q_FUNC_INFO << "Unknown Element:" << type;
  }
}

void CocoaStyle::PrivateCocoa::set_pen_color(QPainter *painter,
                                             resource_manager::ColorName a_name,
                                             int a_thikness) {
  painter->setPen(QPen(color(a_name), a_thikness, Qt::SolidLine, Qt::RoundCap,
                       Qt::RoundJoin));
}

void CocoaStyle::set_default_painter_hints(QPainter *painter) {
  painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing |
                              QPainter::SmoothPixmapTransform |
                              QPainter::HighQualityAntialiasing,
                          true);
}

void CocoaStyle::draw_push_button(const style_data &features,
                                  QPainter *painter) {
  QRectF rect = features.geometry;

  /* Painter settings */
  painter->save();
  set_default_painter_hints(painter);

  QPainterPath button_background_path;
  button_background_path.addRoundedRect(rect, 4.0, 4.0);

  if (features.render_state == style_data::kRenderPressed) {
    painter->fillPath(button_background_path,
                      d->color(resource_manager::kPrimaryColor));
    d->set_pen_color(painter, resource_manager::kSecondryTextColor);
    d->set_default_font_size(painter);
    painter->drawPath(button_background_path);
  } else if (features.render_state == style_data::kRenderRaised) {
    painter->fillPath(button_background_path,
                      d->color(resource_manager::kDarkPrimaryColor));
    d->set_pen_color(painter, resource_manager::kSecondryTextColor);
    d->set_default_font_size(painter, 11, true);
    painter->drawPath(button_background_path);
  } else {
    painter->fillPath(button_background_path,
                      d->color(resource_manager::kDarkPrimaryColor));
    d->set_pen_color(painter, resource_manager::kSecondryTextColor);
    d->set_default_font_size(painter);
    painter->drawPath(button_background_path);
  }

  painter->drawText(features.geometry, Qt::AlignCenter, features.text_data);
  painter->restore();
}

void CocoaStyle::draw_window_button(const style_data &features,
                                    QPainter *painter) {
  QRectF rect = features.geometry.adjusted(0, 0, 0, 0);

  painter->save();
  set_default_painter_hints(painter);

  QPainterPath background;
  background.addRoundedRect(rect, 4.0, 4.0);

  if (features.render_state == style_data::kRenderElement) {
    painter->fillPath(background, d->color(resource_manager::kAccentColor));
  } else {
    painter->fillPath(background, d->color(resource_manager::kAccentColor));
  }

  painter->save();

  d->set_pen_color(painter, resource_manager::kSecondryTextColor, 2);
  QRectF cross_rect(6.0, 6.0, rect.width() - 12, rect.height() - 12);

  painter->drawLine(cross_rect.topLeft(), cross_rect.bottomRight());
  painter->drawLine(cross_rect.topRight(), cross_rect.bottomLeft());

  painter->restore();

  painter->restore();
}

void CocoaStyle::PrivateCocoa::set_default_font_size(QPainter *painter,
                                                     int a_size,
                                                     bool a_highlight) {
  QFont _font = painter->font();
  _font.setBold(a_highlight);
  _font.setPixelSize(a_size * scale_factor());
  painter->setFont(_font);
}

void CocoaStyle::draw_window_frame(const style_data &features,
                                   QPainter *a_ctx) {
  QRectF rect = features.geometry.adjusted(4, 4, -4, -4);

  a_ctx->save();
  set_default_painter_hints(a_ctx);

  /* draw shadow */
  QPainterPath drop_shadow;
  drop_shadow.addRoundRect(features.geometry, 4.0, 4.0);

  /* draw a border around the window */
  // todo : do this only when drop shadows are disabled
  /*
  a_ctx->save();
  a_ctx->setOpacity(0.3);
  a_ctx->fillPath(drop_shadow, d->color(ResourceManager::kTextColor));
  a_ctx->restore();
  */

  /* draw the adjusted window frame */
  QPainterPath window_background_path;
  window_background_path.addRoundedRect(rect, 4.0, 4.0);
  a_ctx->fillPath(window_background_path,
                  d->color(resource_manager::kLightPrimaryColor));

  /* draw seperator */
  if (!features.text_data.isNull() || !features.text_data.isEmpty()) {
    QRectF window_title_rect(8, 4, rect.width() - 8, 54.0);

    QLinearGradient seperator_line_grad(window_title_rect.bottomLeft(),
                                        window_title_rect.bottomRight());
    seperator_line_grad.setColorAt(
        0.0, d->color(resource_manager::kLightPrimaryColor));
    seperator_line_grad.setColorAt(0.5,
                                   d->color(resource_manager::kDividerColor));
    seperator_line_grad.setColorAt(
        1.0, d->color(resource_manager::kLightPrimaryColor));

    // draw frame text;
    a_ctx->save();

    d->set_default_font_size(a_ctx, 18, true);
    d->set_pen_color(a_ctx, resource_manager::kTextColor);
    a_ctx->drawText(window_title_rect, features.text_data,
                    QTextOption(Qt::AlignCenter));

    a_ctx->restore();

    a_ctx->save();

    QPen linePen = QPen(seperator_line_grad, 1, Qt::SolidLine, Qt::RoundCap,
                        Qt::RoundJoin);
    a_ctx->setPen(linePen);
    a_ctx->drawLine(window_title_rect.bottomLeft(),
                    window_title_rect.bottomRight());
    a_ctx->restore();
  }

  a_ctx->restore();
}

void CocoaStyle::draw_clock_hands(
    QPainter *a_ctx, QRectF rect, int factor, float angle,
    resource_manager::ColorName a_clock_hand_color, int a_thikness) {
  float _adjustment = rect.width() / factor;

  QRectF _clock_hour_rect(rect.x() + _adjustment, rect.y() + _adjustment,
                          rect.width() - (_adjustment * 2),
                          rect.height() - (_adjustment * 2));

  QTransform _xform_hour;
  QPointF _transPos = _clock_hour_rect.center();
  _xform_hour.translate(_transPos.x(), _transPos.y());
  _xform_hour.rotate(45 + angle);
  _xform_hour.translate(-_transPos.x(), -_transPos.y());
  a_ctx->setTransform(_xform_hour);

  d->set_pen_color(a_ctx, a_clock_hand_color, a_thikness);
  a_ctx->drawLine(_clock_hour_rect.topLeft(), _clock_hour_rect.center());
}

void CocoaStyle::draw_range_marker(QRectF rect, QTransform _xform_hour,
                                   QPainter *a_ctx, double mark_start,
                                   double mark_end,
                                   QPointF current_marker_location,
                                   QPointF _transPos,
                                   QPointF current_marker_location_for_min) {
  double end_angle = (mark_end / 60) * 360;
  double start_angle = (mark_start / 60) * 360;

  QPainterPath clock_path;
  clock_path.addEllipse(rect);

  _xform_hour.reset();

  _transPos = rect.center();
  _xform_hour.translate(_transPos.x(), _transPos.y());
  _xform_hour.rotate(-90);
  _xform_hour.translate(-_transPos.x(), -_transPos.y());

  a_ctx->setTransform(_xform_hour);

  QPainterPath timer_path;

  timer_path.moveTo(rect.center());
  timer_path.arcTo(rect, start_angle, -(end_angle));

  a_ctx->setOpacity(0.5);
  a_ctx->fillPath(timer_path, d->color(resource_manager::kPrimaryColor));
  a_ctx->setOpacity(1.0);
}

void CocoaStyle::draw_timer_marker(QRectF rect, QTransform _xform_hour,
                                   QPainter *a_ctx, double mark_minutes,
                                   double mark_hour,
                                   QPointF current_marker_location,
                                   QPointF _transPos,
                                   QPointF current_marker_location_for_min) {
  if (mark_hour > 12)
    mark_hour = mark_hour - 12;
  double hour_angle = (((60.0 * mark_hour) + mark_minutes) / 2);
  double min_angle = (6.0 * mark_minutes);

  QPainterPath clock_path;
  clock_path.addEllipse(rect);

  signed int multiply = 1;
  if (std::abs(min_angle) > std::abs(hour_angle)) {
    multiply = -multiply;
  }

  _xform_hour.reset();

  _transPos = rect.center();
  _xform_hour.translate(_transPos.x(), _transPos.y());
  _xform_hour.rotate(-90);
  _xform_hour.translate(-_transPos.x(), -_transPos.y());

  a_ctx->setTransform(_xform_hour);

  QPainterPath timer_path;

  timer_path.moveTo(rect.center());
  timer_path.arcTo(rect, -hour_angle,
                   multiply * angle_between_hands(mark_hour, mark_minutes));

  a_ctx->setOpacity(0.3);
  a_ctx->fillPath(timer_path, d->color(resource_manager::kLightPrimaryColor));
  a_ctx->setOpacity(1.0);
}

void CocoaStyle::draw_clock_surface(const style_data &features,
                                    QPainter *a_ctx) {
  /* please note that the clock is drawn with the inverted color scheme */
  float border_len = features.geometry.width() - 16;

  QRectF rect = QRectF(
      features.geometry.x() + ((features.geometry.width() - border_len) / 2),
      (features.geometry.y() + ((features.geometry.height() - border_len) / 2)),
      border_len, border_len);

  double second_value = features.attributes["seconds"].toDouble();
  double minutes_value = features.attributes["minutes"].toDouble();
  double hour_value = features.attributes["hour"].toDouble();
  double mark_hour = features.attributes["mark_hour"].toDouble();
  double mark_minutes = features.attributes["mark_minutes"].toDouble();
  double mark_start = features.attributes["mark_start"].toDouble();
  double mark_end = features.attributes["mark_end"].toDouble();

  set_default_painter_hints(a_ctx);

  d->set_pen_color(a_ctx, resource_manager::kTextColor, 10);

  QPainterPath _clock_background;
  _clock_background.addEllipse(rect);

  a_ctx->fillPath(_clock_background, d->color(resource_manager::kTextColor));
  a_ctx->drawEllipse(rect);

  // draw second markers.
  for (int i = 0; i < 60; i++) {
    double percent = (i / 60.0);
    QPointF marker_location = _clock_background.pointAtPercent(percent);

    d->set_pen_color(a_ctx, resource_manager::kLightPrimaryColor);
    a_ctx->drawPoint(marker_location);
  }

  // draw minute markers
  for (int i = 0; i < 360; i = i + 30) {
    float percent = (i / 360.0);
    QPointF marker_location = _clock_background.pointAtPercent(percent);

    d->set_pen_color(a_ctx, resource_manager::kLightPrimaryColor, 4);
    a_ctx->drawPoint(marker_location);
  }

  // draw hour markers
  for (int i = 0; i < 360; i = i + 90) {
    float percent = (i / 360.0);
    QPointF marker_location = _clock_background.pointAtPercent(percent);

    d->set_pen_color(a_ctx, resource_manager::kLightPrimaryColor, 8);
    a_ctx->drawPoint(marker_location);
  }

  // draw marker
  double current_percent = (mark_hour) / 24.0;

  QTransform _xform_hour;
  QPointF _transPos = rect.center();
  _xform_hour.translate(_transPos.x(), _transPos.y());
  _xform_hour.rotate(-90);
  _xform_hour.translate(-_transPos.x(), -_transPos.y());
  a_ctx->setTransform(_xform_hour);

  QPointF current_marker_location =
      _clock_background.pointAtPercent(current_percent);

  // draw timer marker.
  double current_percent_min = (mark_minutes) / 60.0;

  QPointF current_marker_location_for_min =
      _clock_background.pointAtPercent(current_percent_min);

  // experiment
  draw_timer_marker(rect, _xform_hour, a_ctx, mark_minutes, mark_hour,
                    current_marker_location, _transPos,
                    current_marker_location_for_min);

  if (std::abs(mark_start) >= 0) {
    double current_percent = (mark_start) / 60.0;
    QPointF current_marker_location =
        _clock_background.pointAtPercent(current_percent);

    // draw timer marker.
    double current_percent_min = (mark_end) / 60.0;

    _xform_hour.reset();
    _transPos = rect.center();
    _xform_hour.translate(_transPos.x(), _transPos.y());
    _xform_hour.rotate(-90);
    _xform_hour.translate(-_transPos.x(), -_transPos.y());
    a_ctx->setTransform(_xform_hour);

    QPointF current_marker_location_for_min =
        _clock_background.pointAtPercent(current_percent_min);

    // experiment
    draw_range_marker(rect, _xform_hour, a_ctx, mark_start, mark_end,
                      current_marker_location, _transPos,
                      current_marker_location_for_min);
  }

  /* Draw Hour Hand */
  draw_clock_hands(a_ctx, rect, 3, hour_value,
                   resource_manager::kSecondryTextColor, 6);
  draw_clock_hands(a_ctx, rect, 4, minutes_value,
                   resource_manager::kSecondryTextColor, 3);

  QRectF _clock_wheel_rect(rect.center().x() - 8, rect.center().y() - 8, 16,
                           16);
  QRectF _clock_wheel_inner_rect(rect.center().x() - 4, rect.center().y() - 4,
                                 8, 8);

  QPainterPath _clock_wheel_path;
  QPainterPath _clock_wheel_inner_path;

  _clock_wheel_path.addEllipse(_clock_wheel_rect);
  _clock_wheel_inner_path.addEllipse(_clock_wheel_inner_rect);

  a_ctx->fillPath(_clock_wheel_path,
                  d->color(resource_manager::kLightPrimaryColor));

  /* second hand */
  d->set_pen_color(a_ctx, resource_manager::kLightPrimaryColor);
  a_ctx->drawPath(_clock_wheel_path);
  a_ctx->fillPath(_clock_wheel_inner_path,
                  d->color(resource_manager::kAccentColor));

  draw_clock_hands(a_ctx, rect, 5, second_value, resource_manager::kAccentColor,
                   2);
}

void CocoaStyle::draw_clock_surface_to_buffer(const style_data &features,
                                              QPainter *a_ctx) {
  QRectF src_rect = features.geometry;
  int width = src_rect.width();
  int height = src_rect.height();

  QRectF target_rect =
      QRectF((src_rect.x() / 2), (src_rect.y() / 2), width, height);

  unsigned char *imgbuf = (unsigned char *)malloc(width * height * 4);
  memset(imgbuf, 0, 4 * width * height);

  QImage surface(imgbuf, width, height, QImage::Format_ARGB32_Premultiplied);

  QPainter proxy;
  proxy.begin(&surface);
  draw_clock_surface(features, &proxy);
  proxy.end();

  a_ctx->drawImage(src_rect, surface, target_rect);

  free(imgbuf);
}

void CocoaStyle::draw_knob(const style_data &features, QPainter *a_ctx) {
  set_default_painter_hints(a_ctx);

  float border_len = features.geometry.width() - 8;
  float outer_len = features.geometry.width() - 10;
  float inner_len = features.geometry.width() - 24;

  QRectF border_rect = QRectF(
      features.geometry.x() + ((features.geometry.width() - border_len) / 2),
      (features.geometry.y() + ((features.geometry.height() - border_len) / 2)),
      border_len, border_len);

  QRectF rect = QRectF(
      features.geometry.x() + ((features.geometry.width() - outer_len) / 2),
      (features.geometry.y() + ((features.geometry.height() - outer_len) / 2)),
      outer_len, outer_len);

  QRectF handle_rect = QRectF(
      features.geometry.x() + ((features.geometry.width() - inner_len) / 2),
      (features.geometry.y() + ((features.geometry.height() - inner_len) / 2)),
      inner_len, inner_len);

  double angle_percent = features.attributes["angle"].toDouble();
  double max_value = features.attributes["max_value"].toDouble();

  d->set_pen_color(a_ctx, resource_manager::kTextColor, 1);
  QPainterPath _clock_background;
  QPainterPath _marker_background;
  QPainterPath _border_background;

  _marker_background.addEllipse(features.geometry);
  _border_background.addEllipse(border_rect);

  _clock_background.addEllipse(rect);
  a_ctx->fillPath(_marker_background,
                  d->color(resource_manager::kLightPrimaryColor));
  a_ctx->fillPath(_clock_background, d->color(resource_manager::kTextColor));
  a_ctx->drawPath(_clock_background);

  d->set_pen_color(a_ctx, resource_manager::kDividerColor, 1);
  a_ctx->drawPath(_border_background);

  // draw segement markers.
  for (int i = 0; i < max_value; i++) {
    double percent = (i / max_value);
    QPointF marker_location = _marker_background.pointAtPercent(percent);

    d->set_pen_color(a_ctx, resource_manager::kTextColor, 1);
    a_ctx->drawPoint(marker_location);
  }

  /*
  // main points.
  for (int i = 0; i < max_value; i = i + (max_value / 4)) {
    double percent = (i / max_value);
    QPointF marker_location = _clock_background.pointAtPercent(percent);

    a_ctx->save();
    d->set_pen_color(a_ctx, ResourceManager::kLightPrimaryColor, 4);
    a_ctx->drawPoint(marker_location);
    a_ctx->restore();
  }
  */

  QPointF transPos = handle_rect.center();
  QPainterPath border_path;
  QPainterPath knob_path;
  QRectF dial_handle = handle_rect;

  border_path.addEllipse(rect);
  knob_path.addEllipse(handle_rect);

  QPointF current_marker_location_for_min =
      knob_path.pointAtPercent(angle_percent);

  /* draw the dial */
  a_ctx->save();
  set_default_painter_hints(a_ctx);
  a_ctx->fillPath(border_path, d->color(resource_manager::kLightPrimaryColor));

  QTransform xform;
  xform.translate(transPos.x(), transPos.y());
  xform.rotate(-90);
  xform.translate(-transPos.x(), -transPos.y());

  a_ctx->setTransform(xform);

  a_ctx->save();
  d->set_pen_color(a_ctx, resource_manager::kTextColor, 8);
  a_ctx->drawPoint(current_marker_location_for_min);

  if (features.render_state == style_data::kRenderRaised) {
    d->set_pen_color(a_ctx, resource_manager::kAccentColor, 8);
    a_ctx->drawPoint(current_marker_location_for_min);
  }

  if (features.render_state == style_data::kRenderPressed) {
    d->set_pen_color(a_ctx, resource_manager::kPrimaryColor, 4);
    a_ctx->drawPoint(current_marker_location_for_min);
  }

  a_ctx->restore();
  a_ctx->restore();
}

void CocoaStyle::draw_line_edit(const style_data &features, QPainter *painter) {

  QRectF rect = features.geometry.adjusted(4, 4, 0, 0);

  set_default_painter_hints(painter);

  QPainterPath background_path;
  background_path.addRoundedRect(rect, 0, 0);

  painter->fillPath(background_path,
                    d->color(resource_manager::kLightPrimaryColor));

  d->set_default_font_size(painter, 18);

  if (features.render_state == style_data::kRenderRaised) {
    d->set_pen_color(painter, resource_manager::kDividerColor);
  } else {
    d->set_pen_color(painter, resource_manager::kPrimaryColor);
  }
  painter->setOpacity(0.8);
  painter->drawPath(background_path);
  painter->setOpacity(1.0);

  painter->save();

  d->set_pen_color(painter, resource_manager::kTextColor);
  painter->drawText(features.geometry.adjusted(10.0, 4.0, 0.0, 0.0),
                    Qt::AlignLeft | Qt::AlignVCenter, features.text_data);
  // cursor handling.
  int cursor_pos = features.attributes["cursor_location"].toInt();
  int selection_cursor = features.attributes["selection_cursor"].toInt();

  QFontMetrics m = painter->fontMetrics();
  int _text_pixel_width = m.width(features.text_data);

  int _text_cursor_width_to_left = 10;
  int _text_cursor_width_to_right = 10;

  if (cursor_pos == features.text_data.count()) {
    _text_cursor_width_to_left += _text_pixel_width;
  } else {
    _text_cursor_width_to_left += m.width(features.text_data.left(cursor_pos));
  }

  if (selection_cursor == features.text_data.count()) {
    _text_cursor_width_to_right += _text_pixel_width;
  } else {
    _text_cursor_width_to_right +=
        m.width(features.text_data.left(selection_cursor));
  }

  QPointF line1(_text_cursor_width_to_left, 10);
  QPointF line2(_text_cursor_width_to_left, m.height() + 4);

  d->set_pen_color(painter, resource_manager::kPrimaryColor, 2);
  painter->drawLine(line1, line2);

  if (features.render_state == style_data::kRenderPressed) {
    /*
     * if current text cursor is at x1 and selection cursor at x2
     * distence between the cursors (x1 - x2)
     * */
    int diff = (_text_cursor_width_to_left - _text_cursor_width_to_right);

    QRectF selection_rect =
        QRectF(_text_cursor_width_to_right, (rect.height() - m.height()) / 2,
               diff, m.height());

    painter->save();
    painter->setOpacity(0.3);
    painter->fillRect(selection_rect, d->color(resource_manager::kAccentColor));

    painter->restore();
  }
  painter->restore();
}

void CocoaStyle::draw_text_editor(const style_data &features,
                                  const QString &text, QPainter *a_ctx) {
  /* Painter settings */
  a_ctx->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing |
                            QPainter::HighQualityAntialiasing,
                        true);
  QPen pen;

  pen = QPen(QColor(255, 255, 255), 1, Qt::SolidLine, Qt::RoundCap,
             Qt::RoundJoin);
  QFont font = a_ctx->font();
  font.setPixelSize(18 * scale_factor());
  a_ctx->setFont(font);

  a_ctx->setPen(pen);
  a_ctx->drawText(features.geometry.adjusted(11.0, 1.0, 1.0, 1.0),
                  Qt::AlignLeft | Qt::AlignVCenter, text);

  pen = QPen(QColor(0, 0, 0), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
  a_ctx->setPen(pen);
  a_ctx->drawText(features.geometry.adjusted(10.0, 0.0, 0.0, 0.0),
                  Qt::AlignLeft | Qt::AlignVCenter, text);

  pen =
      QPen(QColor(98, 101, 108), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
  a_ctx->setPen(pen);
}

void CocoaStyle::drawSeperatorLine(const style_data &features,
                                   QPainter *a_ctx) {
  a_ctx->save();
  QPen pen = QPen(QColor(217, 217, 217), 1, Qt::SolidLine, Qt::RoundCap,
                  Qt::RoundJoin);
  a_ctx->setPen(pen);
  a_ctx->drawLine(
      QPoint(features.geometry.x(), features.geometry.y()),
      QPoint(features.geometry.width(), features.geometry.height()));
  pen = QPen(QColor(255, 255, 255), 1, Qt::SolidLine, Qt::RoundCap,
             Qt::RoundJoin);
  a_ctx->setPen(pen);
  a_ctx->drawLine(
      QPoint(features.geometry.x() + 1, features.geometry.y() + 1),
      QPoint(features.geometry.width() + 1, features.geometry.height() + 1));
  a_ctx->restore();
}

void CocoaStyle::draw_progress_bar(const style_data &features,
                                   QPainter *a_ctx) {
  a_ctx->save();

  set_default_painter_hints(a_ctx);

  switch (features.render_state) {
  case style_data::kRenderBackground: {
    QRectF rect = features.geometry;
    QLinearGradient background_grad(QPointF(0, 0), QPointF(0.0, 20));

    background_grad.setColorAt(1,
                               d->color(resource_manager::kLightPrimaryColor));
    background_grad.setColorAt(0.5,
                               d->color(resource_manager::kTextBackground));
    background_grad.setColorAt(0,
                               d->color(resource_manager::kLightPrimaryColor));

    a_ctx->setPen(
        QPen(background_grad, 4, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    QPointF background_line_begin(rect.topLeft().x(),
                                  rect.bottomLeft().y() / 2);
    QPointF background_line_end(rect.topRight().x(),
                                rect.bottomRight().y() / 2);

    a_ctx->drawLine(background_line_begin, background_line_end);
  } break;
  case style_data::kRenderForground: {
    QRectF rect = features.geometry;
    QLinearGradient progress_bar_grad(QPointF(0, 0), QPointF(0.0, 20));
    progress_bar_grad.setColorAt(1, d->color(resource_manager::kAccentColor));
    progress_bar_grad.setColorAt(0, d->color(resource_manager::kAccentColor));
    a_ctx->setPen(
        QPen(progress_bar_grad, 4, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    QPointF progress_bar_line_start(rect.topLeft().x(),
                                    rect.bottomLeft().y() / 2);
    QPointF progress_bar_line_end(rect.topRight().x(),
                                  rect.bottomRight().y() / 2);
    a_ctx->drawLine(progress_bar_line_start, progress_bar_line_end);
  } break;
  default:
    qDebug() << Q_FUNC_INFO << "Unknown progress bar state";
  }

  a_ctx->restore();
}

void CocoaStyle::drawVListItem(const style_data &features, QPainter *painter) {
  qWarning() << Q_FUNC_INFO << "This method is depricated and non-functional";
  // todo : Remove this method.
  /*
  QRectF rect = features.geometry;

  QPen backgroundPen(QColor(color("primary_forground")), 1, Qt::SolidLine,
                     Qt::RoundCap, Qt::RoundJoin);

  painter->save();
  if (features.render_state == StyleFeatures::kRenderElement) {
    painter->setPen(backgroundPen);
    painter->fillRect(rect, QColor(color("primary_background")));

    QLinearGradient _seperator_line_grad(rect.bottomLeft(), rect.bottomRight());
    _seperator_line_grad.setColorAt(0.0, QColor(color("primary_background")));
    _seperator_line_grad.setColorAt(0.5,
                                    QColor(color("accent_primary_forground")));
    _seperator_line_grad.setColorAt(1.0, QColor(color("primary_background")));

    QPen linePen = QPen(_seperator_line_grad, 1, Qt::SolidLine, Qt::RoundCap,
                        Qt::RoundJoin);
    painter->setPen(linePen);
    painter->drawLine(rect.bottomLeft(), rect.bottomRight());
  }
  painter->restore();
  */
}

void CocoaStyle::draw_image_button(const style_data &a_features,
                                   QPainter *a_ctx) {
  a_ctx->save();

  set_default_painter_hints(a_ctx);

  QPainterPath background_path;
  QRectF a_rect = a_features.geometry;

  background_path.addRoundRect(a_rect, 6, 6);

  switch (a_features.render_state) {
  case style_data::kRenderElement:
    a_ctx->fillPath(background_path,
                    d->color(resource_manager::kLightPrimaryColor));
    break;
  case style_data::kRenderPressed:
    a_ctx->fillPath(background_path, d->color(resource_manager::kPrimaryColor));
    break;
  case style_data::kRenderRaised:
    a_ctx->fillPath(background_path,
                    d->color(resource_manager::kLightPrimaryColor));
    break;
  default:
    qWarning() << Q_FUNC_INFO << "Unknown State";
  }

  QRect icon_rect = a_rect.toRect();
  if (a_features.text_data.isNull() || a_features.text_data.isEmpty()) {
    icon_rect.setX(a_rect.center().x() - (icon_rect.height() / 2));
  } else {
    icon_rect.setX(a_rect.center().x() - (icon_rect.width() / 2));
  }
  icon_rect.setWidth(icon_rect.height());

  QRect text_rect = a_rect.toRect();
  text_rect.setX(icon_rect.width() + 5);

  a_ctx->drawPixmap(icon_rect, a_features.image_data);
  d->set_default_font_size(a_ctx, 11);
  a_ctx->drawText(text_rect, a_features.text_data,
                  Qt::AlignLeft | Qt::AlignVCenter);
  a_ctx->restore();
}

void CocoaStyle::draw_label(const style_data &aFeatures, QPainter *a_ctx) {
  a_ctx->save();

  set_default_painter_hints(a_ctx);

  /* spacial case so doing it manually */
  QFont _font = a_ctx->font();
  _font.setBold(true);
  if (aFeatures.attributes.keys().contains("font_size"))
    _font.setPixelSize(aFeatures.attributes["font_size"].toInt() *
                       scale_factor());
  else
    _font.setPixelSize(14 * scale_factor());

  a_ctx->setFont(_font);

  QFontMetrics metrics(a_ctx->font());
  QString text = metrics.elidedText(aFeatures.text_data, Qt::ElideRight,
                                    aFeatures.geometry.width());

  if (aFeatures.render_state == style_data::kRenderRaised) {
    d->set_pen_color(a_ctx, resource_manager::kLightPrimaryColor);
    a_ctx->fillRect(aFeatures.geometry,
                    d->color(resource_manager::kAccentColor));
  } else {
    d->set_pen_color(a_ctx, resource_manager::kTextColor);
  }

  a_ctx->drawText(aFeatures.geometry, text, aFeatures.text_options);
  a_ctx->restore();
}

QColor CocoaStyle::PrivateCocoa::color(resource_manager::ColorName a_name) {
  return resource_manager::color(a_name);
}
