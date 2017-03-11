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
#include "simple_gray.h"
#include <QTimer>
#include <ck_config.h>
#include <ck_desktop_controller_interface.h>
#include <ck_style_data.h>
#include <ck_widget.h>

#include <ck_line_edit.h>
#include <ck_resource_manager.h>
#include <cmath>

#include <px_bench.h>

#include <QPaintDevice>
#include <QPaintEngine>

#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#include <malloc/malloc.h>
#include <stdlib.h>

#endif

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

class SimpleGrayStyle::PrivateSimpleGray {
public:
  PrivateSimpleGray() {}
  ~PrivateSimpleGray() {}

  QColor color(resource_manager::ColorName a_name);
  void set_pen_color(QPainter *painter, resource_manager::ColorName a_name,
                     int a_thikness = 0);
  void set_default_font_size(QPainter *painter, int a_size = 8,
                             bool a_highlight = false);

  QHash<QString, int> m_type_map;
  QVariantMap m_attribute_map;
  QVariantMap m_color_map;
#ifdef __APPLE__
  QImage *get_drawable_surface(QPainter *a_ctx);
#endif
};

void SimpleGrayStyle::load_default_widget_style_properties() {
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
  d->m_type_map["image_button"] = 24;
  d->m_type_map["scrollbar"] = 25;
  d->m_type_map["scrollbar_slider"] = 26;

  // style attributes. this could be read from a xml file or a stylesheet.
  QVariantMap _frame_attributes;
  QVariantMap _widget_attributes;
  QVariantMap _size_attributes;
  QVariantMap _button_attributes;

  _frame_attributes["window_title_height"] = 48;
  _frame_attributes["window_minimized_height"] = 128.0;
  _frame_attributes["window_minimized_width"] = 96.0;
  _frame_attributes["window_blur_radius"] = 12.0;
  _frame_attributes["window_close_button_width"] = 24.0;
  _frame_attributes["window_close_button_height"] = 24.0;
  _frame_attributes["window_shadow_color"] = QString("#323232");
  _frame_attributes["padding"] = 2.0;

  _widget_attributes["line_edit_width"] = 128.0;
  _widget_attributes["line_edit_height"] = 16.0;

  _size_attributes["small"] = 0.5;
  _size_attributes["original"] = 1.0;
  _size_attributes["large"] = 2.0;
  _size_attributes["huge"] = 4.0;

  _button_attributes["width"] = 64.0;
  _button_attributes["height"] = 16.0;

  d->m_attribute_map["frame"] = _frame_attributes;
  d->m_attribute_map["widget"] = _widget_attributes;
  d->m_attribute_map["size"] = _size_attributes;
  d->m_attribute_map["button"] = _button_attributes;
}

SimpleGrayStyle::SimpleGrayStyle() : d(new PrivateSimpleGray) {
  load_default_widget_style_properties();
}

SimpleGrayStyle::~SimpleGrayStyle() {
  qDebug() << Q_FUNC_INFO;
  delete d;
}

QVariantMap SimpleGrayStyle::attribute_map(const QString &type) const {
  return d->m_attribute_map[type].toMap();
}

QVariantMap SimpleGrayStyle::color_scheme_map() const { return d->m_color_map; }

void SimpleGrayStyle::draw(const QString &type, const style_data &options,
                           QPainter *painter, const widget *aWidget) {
  switch (d->m_type_map[type]) {
  case 1:
    draw_push_button(options, painter, aWidget);
    break;
  case 2:
    drawVListItem(options, painter, aWidget);
    break;
  case 3:
    draw_window_button(options, painter, aWidget);
    break;
  case 6:
    draw_line_edit(options, painter, aWidget);
    break;
  case 9:
    draw_label(options, painter, aWidget);
    break;
  case 10:
    draw_clock_surface(options, painter, aWidget);
    // draw_clock_surface_to_buffer(options, painter);
    break;
  case 13:
    draw_knob(options, painter, aWidget);
    break;
  case 19:
    draw_progress_bar(options, painter, aWidget);
    break;
  case 21:
    draw_window_frame(options, painter, aWidget);
    break;
  case 24:
    draw_image_button(options, painter, aWidget);
    break;
  case 25:
    draw_scrollbar(options, painter, aWidget);
    break;
  case 26:
    draw_scrollbar_slider(options, painter, aWidget);
    break;
  default:
    qWarning() << Q_FUNC_INFO << "Unknown Element:" << type;
  }
}

void SimpleGrayStyle::PrivateSimpleGray::set_pen_color(
    QPainter *painter, resource_manager::ColorName a_name, int a_thikness) {
  painter->setPen(QPen(color(a_name), a_thikness * scale_factor(),
                       Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
}

void SimpleGrayStyle::set_default_painter_hints(QPainter *painter) {
  painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing |
                              QPainter::SmoothPixmapTransform, true);
}

void SimpleGrayStyle::draw_push_button(const style_data &features,
                                       QPainter *painter,
                                       const widget *a_widget) {
  QRectF rect = features.geometry;

  /* Painter settings */
  painter->save();
  set_default_painter_hints(painter);

  QPainterPath button_background_path;
  button_background_path.addRoundedRect(rect, 2.0, 2.0);

  if (features.render_state == style_data::kRenderPressed) {
    painter->fillPath(button_background_path,
                      d->color(resource_manager::kPrimaryColor));
    d->set_pen_color(painter, resource_manager::kSecondryTextColor);
    d->set_default_font_size(painter, 12, true);
    painter->drawPath(button_background_path);
  } else if (features.render_state == style_data::kRenderRaised) {
    painter->fillPath(button_background_path,
                      d->color(resource_manager::kDarkPrimaryColor));
    d->set_pen_color(painter, resource_manager::kSecondryTextColor);
    d->set_default_font_size(painter, 12, true);
    painter->drawPath(button_background_path);
  } else {
    painter->fillPath(button_background_path,
                      d->color(resource_manager::kDarkPrimaryColor));
    d->set_pen_color(painter, resource_manager::kSecondryTextColor);
    d->set_default_font_size(painter, 12, true);
    painter->drawPath(button_background_path);
  }

  painter->drawText(features.geometry, Qt::AlignCenter, features.text_data);

  painter->restore();
}

void SimpleGrayStyle::draw_window_button(const style_data &features,
                                         QPainter *painter,
                                         const widget *a_widget) {
#ifdef __APPLE__
  QRectF rect = features.geometry.adjusted(
      6 * scale_factor(), 6 * scale_factor(), -2 * scale_factor(), -2);
#else
  QRectF rect =
      features.geometry.adjusted(2 * scale_factor(), 2 * scale_factor(),
                                 -2 * scale_factor(), -2 * scale_factor());
#endif

  painter->save();
  set_default_painter_hints(painter);

  QPainterPath background;
  background.addRoundedRect(rect, 4.0 * scale_factor(), 4.0 * scale_factor());

  if (features.render_state == style_data::kRenderElement) {
    painter->save();
    painter->setPen(QColor("#ffffff"));
    painter->setOpacity(0.5);
    painter->drawPath(background);
    painter->restore();
    painter->fillPath(background, d->color(resource_manager::kTextColor));
  } else {
    painter->fillPath(background, d->color(resource_manager::kAccentColor));
  }

  painter->save();

  d->set_pen_color(painter, resource_manager::kTextBackground,
                   2 * scale_factor());
#ifdef __APPLE__
  QRectF cross_rect(12.0 * scale_factor(), 12.0 * scale_factor(),
                    rect.width() - (12 * scale_factor()),
                    rect.height() - (12 * scale_factor()));
#else
  QRectF cross_rect(8.0 * scale_factor(), 8.0 * scale_factor(),
                    rect.width() - (12 * scale_factor()),
                    rect.height() - (12 * scale_factor()));
#endif

  painter->drawLine(cross_rect.topLeft(), cross_rect.bottomRight());
  painter->drawLine(cross_rect.topRight(), cross_rect.bottomLeft());

  painter->restore();

  painter->restore();
}

#ifdef __APPLE__
CGContextRef _create_cg_context(int pixelsWide, int pixelsHigh) {
  CGContextRef context = NULL;
  CGColorSpaceRef colorSpace;
  void *bitmapData;
  int bitmapByteCount;
  int bitmapBytesPerRow;

  bitmapBytesPerRow = (pixelsWide * 4); // 1
  bitmapByteCount = (bitmapBytesPerRow * pixelsHigh);

  colorSpace = CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB); // 2
  bitmapData = calloc(bitmapByteCount, sizeof(int));                // 3
  if (bitmapData == NULL) {
    fprintf(stderr, "Memory not allocated!");
    return NULL;
  }
  context = CGBitmapContextCreate(
      bitmapData,                // 4
      pixelsWide, pixelsHigh, 8, // bits per component
      bitmapBytesPerRow, colorSpace, kCGImageAlphaPremultipliedLast);
  if (context == NULL) {
    free(bitmapData); // 5
    fprintf(stderr, "Context not created!");
    return NULL;
  }
  CGColorSpaceRelease(colorSpace); // 6

  return context; // 7
}

QImage _cg_image_to_qimage(CGImageRef image) {
  const size_t w = CGImageGetWidth(image), h = CGImageGetHeight(image);
  QImage ret(w, h, QImage::Format_ARGB32_Premultiplied);
  ret.fill(Qt::transparent);
  CGRect rect = CGRectMake(0, 0, w, h);
  // CGContextRef ctx = qt_mac_cg_context(&ret);
  // qt_mac_drawCGImage(ctx, &rect, image);
  // CGContextRelease(ctx);
  return ret;
}

/*
void _draw_drop_shadow(CGContextRef myContext, // 1
                       CGFloat wd, CGFloat ht) {
  CGSize myShadowOffset = CGSizeMake(2, -5);         // 2
  CGContextSaveGState(myContext);                    // 6
  CGContextSetShadow(myContext, myShadowOffset, 30); // 7
  CGContextRestoreGState(myContext);                 // 15
}
*/
#endif
void SimpleGrayStyle::PrivateSimpleGray::set_default_font_size(
    QPainter *painter, int a_size, bool a_highlight) {
  QFont _font = painter->font();
  _font.setBold(a_highlight);
  _font.setPixelSize(a_size * scale_factor());
  painter->setFont(_font);
}

#ifdef __APPLE__
QImage *
SimpleGrayStyle::PrivateSimpleGray::get_drawable_surface(QPainter *a_ctx) {
  QPaintDevice *current_paint_device = a_ctx->paintEngine()->paintDevice();
  CGContextRef bitmap_ctx = 0;
  QImage *rv = NULL;

  if (current_paint_device) {
    if (current_paint_device->devType() == QInternal::Image) {
      rv = static_cast<QImage *>(current_paint_device);
    }
  }

  return rv;
}
#endif

void SimpleGrayStyle::draw_window_frame(const style_data &features,
                                        QPainter *a_ctx,
                                        const widget *a_widget) {
#ifdef __APPLE__
  QRectF rect = features.geometry.adjusted(10, 10, -10, -10);
#else
  QRectF rect = features.geometry.adjusted(1, 1, -1, -1);
#endif

  a_ctx->save();
  set_default_painter_hints(a_ctx);
#ifdef __APPLE__
  /* draw shadow */
  QPainterPath drop_shadow;
  drop_shadow.addRoundRect(features.geometry, 4.0, 4.0);
  QPaintDevice *current_paint_device = a_ctx->paintEngine()->paintDevice();
  CGContextRef bitmap_ctx = 0;

  if (current_paint_device) {
    if (current_paint_device->devType() == QInternal::Image) {
      QImage *qt_surface = static_cast<QImage *>(current_paint_device);

      if (!qt_surface)
        qDebug() << Q_FUNC_INFO << "Error Loding Qt Image";

      CGColorSpaceRef colorspace =
          CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);
      uint flags = kCGImageAlphaPremultipliedFirst;
      flags |= kCGBitmapByteOrder32Host;
      bitmap_ctx = CGBitmapContextCreate(
          qt_surface->bits(), qt_surface->width(), qt_surface->height(), 8,
          qt_surface->bytesPerLine(), colorspace, flags);
      CGContextTranslateCTM(bitmap_ctx, 0, qt_surface->height());
      CGContextScaleCTM(bitmap_ctx, 1, -1);

      CGColorSpaceRelease(colorspace);

      // draw
      CGSize myShadowOffset = CGSizeMake(0.0, 0.0);
      CGContextSaveGState(bitmap_ctx);
      CGContextSetShadow(bitmap_ctx, myShadowOffset, 15);

      QColor window_color = d->color(resource_manager::kLightPrimaryColor);

      CGContextSetRGBFillColor(bitmap_ctx, window_color.red(),
                               window_color.blue(), window_color.green(),
                               features.opacity);
      CGContextFillRect(bitmap_ctx, CGRectMake(15, 15, qt_surface->width() - 30,
                                               qt_surface->height() - 30));

      CGContextRestoreGState(bitmap_ctx);
      CGContextRelease(bitmap_ctx);
    } else if (current_paint_device->devType() == QInternal::Widget) {
      qDebug() << Q_FUNC_INFO << "Device Type Is Widget";
    }
  } else {
    qDebug() << Q_FUNC_INFO << "Invalide Paint Device";
  }
#endif

  a_ctx->setOpacity(features.opacity);
  /* draw the adjusted window frame */
  QPainterPath window_background_path;
  window_background_path.addRoundedRect(rect, 4.0, 4.0);
  a_ctx->fillPath(window_background_path,
                  d->color(resource_manager::kLightPrimaryColor));

  /* draw seperator */
  window *ck_window = dynamic_cast<window *>(features.style_object);

  if (ck_window && ck_window->window_type() != window::kPanelWindow) {
    QRectF window_title_rect(12, 5, rect.width() - 16, 48.0 * scale_factor());
    QRectF window_title_fill(0, 0, rect.width() , 48.0 * scale_factor());

    /* window border */
    QPainterPath border_path;
    border_path.addRoundedRect(window_title_fill, 4.0, 4.0);

    a_ctx->save();
    a_ctx->setPen(QColor("#ffffff"));
    a_ctx->setOpacity(0.8);
    a_ctx->drawPath(window_background_path);
    a_ctx->restore();

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

    d->set_default_font_size(a_ctx, 16.0, true);
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

  a_ctx->setOpacity(1.0);
  a_ctx->restore();
#ifdef __APPLE__
#endif
}

void SimpleGrayStyle::draw_clock_hands(
    QPainter *a_ctx, QRectF rect, int factor, float angle,
    resource_manager::ColorName a_clock_hand_color, int a_thikness) {
  float _adjustment = rect.width() / factor;

  QRectF _clock_hour_rect(rect.x() + _adjustment, rect.y() + _adjustment,
                          rect.width() - (_adjustment * 2),
                          rect.height() - (_adjustment * 2));

  a_ctx->save();
  QTransform _xform_hour = a_ctx->transform();
  QPointF _transPos = _clock_hour_rect.center();
  _xform_hour.translate(_transPos.x(), _transPos.y());
  _xform_hour.rotate(90 + 45 + angle);
  _xform_hour.translate(-_transPos.x(), -_transPos.y());
  a_ctx->setTransform(_xform_hour);

  d->set_pen_color(a_ctx, a_clock_hand_color, a_thikness);
  a_ctx->drawLine(_clock_hour_rect.topLeft(), _clock_hour_rect.center());
  a_ctx->restore();
}

void SimpleGrayStyle::draw_range_marker(
    QRectF rect, QTransform _xform_hour, QPainter *a_ctx, double mark_start,
    double mark_end, QPointF current_marker_location, QPointF _transPos,
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

void SimpleGrayStyle::draw_timer_marker(
    QRectF rect, QTransform _xform_hour, QPainter *a_ctx, double mark_minutes,
    double mark_hour, QPointF current_marker_location, QPointF _transPos,
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

void SimpleGrayStyle::draw_clock_surface(const style_data &features,
                                         QPainter *a_ctx,
                                         const widget *a_widget) {
  /* please note that the clock is drawn with the inverted color scheme */
  float border_len = features.geometry.width() - (48 * scale_factor());

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

  d->set_pen_color(a_ctx, resource_manager::kTextColor, 15);

  QPainterPath _clock_background;
  _clock_background.addEllipse(rect);

  a_ctx->fillPath(_clock_background, d->color(resource_manager::kTextColor));
  a_ctx->drawEllipse(rect);

  // draw second markers.
  for (int i = 0; i < 60; i++) {
    double percent = (i / 60.0);
    QPointF marker_location = _clock_background.pointAtPercent(percent);

    d->set_pen_color(a_ctx, resource_manager::kLightPrimaryColor, 1);
    a_ctx->drawPoint(marker_location);
  }

  // draw minute markers
  for (int i = 0; i < 360; i = i + 30) {
    float percent = (i / 360.0);
    QPointF marker_location = _clock_background.pointAtPercent(percent);

    d->set_pen_color(a_ctx, resource_manager::kLightPrimaryColor, 3);
    a_ctx->drawPoint(marker_location);
  }

  // draw hour markers
  for (int i = 0; i < 360; i = i + 90) {
    float percent = (i / 360.0);
    QPointF marker_location = _clock_background.pointAtPercent(percent);

    d->set_pen_color(a_ctx, resource_manager::kLightPrimaryColor, 4);
    a_ctx->drawPoint(marker_location);
  }

  // draw marker
  double current_percent = (mark_hour) / 24.0;

  const QTransform current_transform = a_ctx->transform();
  QTransform _xform_hour = current_transform;
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
  a_ctx->save();
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
    _xform_hour = current_transform;
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

  a_ctx->restore();
  /* Draw Hour Hand */
  /*
  qDebug() << Q_FUNC_INFO << "Hour : " << hour_value
           << " : " << minutes_value
           << " : " << second_value;
  */

  draw_clock_hands(a_ctx, features.geometry, 3, hour_value,
                   resource_manager::kSecondryTextColor, 3 * scale_factor());
  draw_clock_hands(a_ctx, rect, 4, minutes_value,
                   resource_manager::kSecondryTextColor, 2 * scale_factor());

  QRectF _clock_wheel_rect(rect.center().x() - (10 * scale_factor()),
                           rect.center().y() - (10 * scale_factor()),
                           20 * scale_factor(), 20 * scale_factor());

  QRectF _clock_wheel_inner_rect(rect.center().x() - (3 * scale_factor()),
                                 rect.center().y() - (3 * scale_factor()),
                                 6 * scale_factor(), 6 * scale_factor());

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
                   1 * scale_factor());
}

void SimpleGrayStyle::draw_clock_surface_to_buffer(const style_data &features,
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
  draw_clock_surface(features, &proxy, 0);
  proxy.end();

  a_ctx->drawImage(src_rect, surface, target_rect);

  free(imgbuf);
}

void SimpleGrayStyle::draw_knob(const style_data &features, QPainter *a_ctx,
                                const widget *a_widget) {
  set_default_painter_hints(a_ctx);

  float border_len = features.geometry.width() - (8 * scale_factor());
  float outer_len = features.geometry.width() - (10 * scale_factor());
  float inner_len = features.geometry.width() - (24 * scale_factor());

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

  QPointF transPos = handle_rect.center();
  QPainterPath border_path;
  QPainterPath knob_path;

  border_path.addEllipse(rect);
  knob_path.addEllipse(handle_rect);

  QPointF current_marker_location_for_min =
      knob_path.pointAtPercent(angle_percent);

  /* draw the dial */
  a_ctx->fillPath(border_path, d->color(resource_manager::kLightPrimaryColor));

  QTransform xform = a_ctx->transform();
  xform.translate(transPos.x(), transPos.y());
  xform.rotate(-90);
  xform.translate(-transPos.x(), -transPos.y());

  a_ctx->setTransform(xform);

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
}

void SimpleGrayStyle::draw_line_edit(const style_data &features,
                                     QPainter *painter,
                                     const widget *a_widget) {
  cherry_kit::line_edit *ck_line_edit =
      dynamic_cast<cherry_kit::line_edit *>(features.style_object);

  if (!ck_line_edit)
      return;

  QRectF rect = features.geometry.adjusted(0, 0, 0, 0);

  set_default_painter_hints(painter);

  QPainterPath background_path;
  background_path.addRoundedRect(rect, 0, 0);

  painter->fillPath(background_path,
                    d->color(resource_manager::kTextBackground));

  d->set_default_font_size(painter, 14);

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

  if (!ck_line_edit->is_password_input()) {
    painter->drawText(
        features.geometry.adjusted(4.0 * scale_factor(), 0.0, 0.0, 0.0),
        Qt::AlignLeft | Qt::AlignVCenter, features.text_data);
  } else {
      float x_pos = 10 * features.text_data.count();
      for (int x = 0; x < features.text_data.count(); x++) {
        int y_pos = features.geometry.adjusted(
                    (6.0 * scale_factor())+ x_pos, 0.0, 0.0, 0.0).center().toPoint().y();
        QPainterPath password_dot;
        password_dot.addEllipse(QPoint(8 + (x * 10), y_pos), 4, 4);
        painter->fillPath(password_dot, d->color(resource_manager::kPrimaryColor));
      }
  }
  // cursor handling.
  int cursor_pos = features.attributes["cursor_location"].toInt();
  int selection_cursor = features.attributes["selection_cursor"].toInt();

  QFontMetrics m = painter->fontMetrics();
  int _text_pixel_width = m.width(features.text_data);

  int _text_cursor_width_to_left = 4 * scale_factor();
  int _text_cursor_width_to_right = 4 * scale_factor();

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

  if (ck_line_edit && !ck_line_edit->readonly()
          && !ck_line_edit->is_password_input()
          && ck_line_edit->has_input_focus()) {
    QPointF line1(_text_cursor_width_to_left + 2, 10 * scale_factor());
    QPointF line2(_text_cursor_width_to_left + 2,
                  m.height() + (5 * scale_factor()));

    d->set_pen_color(painter, resource_manager::kPrimaryColor, 1);

    painter->drawLine(line1, line2);
  } else if (ck_line_edit->is_password_input()
             && ck_line_edit->has_input_focus()) {
    QPointF line1(4 + (features.text_data.count() * 10) * scale_factor(),
                  8 * scale_factor());
    QPointF line2(4 + (features.text_data.count() * 10) * scale_factor(),
                  m.height() + (4 * scale_factor()));

    d->set_pen_color(painter, resource_manager::kPrimaryColor, 1);

    painter->drawLine(line1, line2);
  }

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

void SimpleGrayStyle::draw_text_editor(const style_data &features,
                                       const QString &text, QPainter *a_ctx,
                                       const widget *a_widget) {
  /* Painter settings */
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

void SimpleGrayStyle::drawSeperatorLine(const style_data &features,
                                        QPainter *a_ctx,
                                        const widget *a_widget) {
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

void SimpleGrayStyle::draw_progress_bar(const style_data &features,
                                        QPainter *a_ctx,
                                        const widget *a_widget) {
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

void SimpleGrayStyle::drawVListItem(const style_data &features,
                                    QPainter *painter, const widget *a_widget) {
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

void SimpleGrayStyle::draw_image_button(const style_data &a_features,
                                        QPainter *a_ctx,
                                        const widget *a_widget) {
  a_ctx->save();

  set_default_painter_hints(a_ctx);

  QPainterPath background_path;
  QRectF a_rect = a_features.geometry;

  background_path.addRoundRect(a_rect, 6, 6);

  switch (a_features.render_state) {
  case style_data::kRenderElement:
    // a_ctx->fillPath(background_path,
    // d->color(resource_manager::kLightPrimaryColor));
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

  QRect expose_area = a_rect.toRect();
  QRect icon_area = a_features.image_data.rect();
  QRect icon_draw_rect = QRect((expose_area.width() - icon_area.width()) / 2,
                               (expose_area.height() - icon_area.height()) / 2,
                               icon_area.width(), icon_area.height());
  QRect text_draw_rect = QRect(0,
                               (icon_area.height() + 12),
                               expose_area.width(),
                               (expose_area.height() - icon_area.height())
                               );
  a_ctx->save();
  set_default_painter_hints(a_ctx);
  a_ctx->drawPixmap(icon_draw_rect, a_features.image_data);

  d->set_default_font_size(a_ctx, 12, true);
  d->set_pen_color(a_ctx, resource_manager::kTextColor);
  a_ctx->drawText(text_draw_rect, a_features.text_data,
                  Qt::AlignCenter | Qt::AlignVCenter);

  a_ctx->restore();

  a_ctx->restore();
}

void SimpleGrayStyle::draw_scrollbar(const style_data &a_data, QPainter *a_ctx,
                                     const widget *a_widget) {
  QRectF rect = a_data.geometry;

  a_ctx->save();
  a_ctx->setOpacity(0.9);

  QPainterPath path;
  path.addRoundedRect(rect, 0, 0);
  a_ctx->fillPath(path,
                  d->color(cherry_kit::resource_manager::kLightPrimaryColor));
  a_ctx->restore();
}

void SimpleGrayStyle::draw_scrollbar_slider(const style_data &a_data,
                                            QPainter *a_ctx,
                                            const widget *a_widget) {
  QRectF rect(2, a_data.geometry.y(), a_data.geometry.width() - 4,
              a_data.geometry.height());
  a_ctx->save();
  a_ctx->setOpacity(0.5);
  QPainterPath path;
  path.addRoundedRect(rect, 2, 2);
  a_ctx->fillPath(path, d->color(cherry_kit::resource_manager::kTextColor));
  a_ctx->restore();
}

void SimpleGrayStyle::draw_label(const style_data &aFeatures, QPainter *a_ctx,
                                 const widget *a_widget) {
  a_ctx->save();

  set_default_painter_hints(a_ctx);

  /* spacial case so doing it manually */
  QFont _font = a_ctx->font();
  _font.setBold(true);
  if (aFeatures.attributes.keys().contains("font_size"))
    _font.setPixelSize(aFeatures.attributes["font_size"].toInt() *
                       scale_factor());
  else
    _font.setPixelSize(8 * scale_factor());

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

QColor
SimpleGrayStyle::PrivateSimpleGray::color(resource_manager::ColorName a_name) {
  return resource_manager::color(a_name);
}
