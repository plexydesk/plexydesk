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
#include <ck_widget.h>
#include <ck_config.h>
#include <QTimer>
#include <ck_desktop_controller_interface.h>
#include <ck_style_data.h>

#include <cmath>
#include <ck_resource_manager.h>
#include <ck_line_edit.h>

#include <px_bench.h>

#include <QPaintEngine>
#include <QPaintDevice>
#include <ck_button.h>

#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
//#include <CoreGraphics/CGBitmapContext.h>
//#include <CoreGraphics/CGDirectDisplay.h>
#include <stdlib.h>
#include <malloc/malloc.h>

#include "ck_platform_context.h"

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

class CocoaStyle::PrivateCocoa {
public:
  PrivateCocoa() {}
  ~PrivateCocoa() {}

  QColor color(resource_manager::ColorName a_name);
  void set_pen_color(QPainter *painter, resource_manager::ColorName a_name,
                     int a_thikness = 0);
  void set_default_font_size(QPainter *painter, int a_size = 8,
                             bool a_highlight = false);

#ifdef __APPLE__
  QImage *get_drawable_surface(QPainter *a_ctx);
  CGContextRef copy_cg_context(QImage *a_surface_ptr);
#endif

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
  d->m_type_map["image_button"] = 24;
  d->m_type_map["scrollbar"] = 25;
  d->m_type_map["scrollbar_slider"] = 26;

  // style attributes. this could be read from a xml file or a stylesheet.
  QVariantMap _frame_attributes;
  QVariantMap _widget_attributes;
  QVariantMap _size_attributes;
  QVariantMap _button_attributes;

  _frame_attributes["window_title_height"] = 48.0;
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

CocoaStyle::CocoaStyle() : d(new PrivateCocoa) {
  load_default_widget_style_properties();
}

CocoaStyle::~CocoaStyle() {
  delete d;
}

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
  case 24:
    draw_image_button(options, painter);
    break;
  case 25:
    draw_scrollbar(options, painter);
    break;
  case 26:
    draw_scrollbar_slider(options, painter);
    break;
  default:
    qWarning() << Q_FUNC_INFO << "Unknown Element:" << type;
  }
}

void CocoaStyle::PrivateCocoa::set_pen_color(QPainter *painter,
                                             resource_manager::ColorName a_name,
                                             int a_thikness) {
  painter->setPen(QPen(color(a_name), a_thikness * scale_factor(),
                       Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
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
  cherry_kit::button *ck_button =
          dynamic_cast<cherry_kit::button *>(features.style_object);

  /* Painter settings */
  painter->save();
  set_default_painter_hints(painter);

  QPainterPath button_background_path;

  if (ck_button && ck_button->is_tool_button())
    button_background_path.addRoundedRect(rect, 0.0, 0.0);
  else
    button_background_path.addRoundedRect(rect, 4.0, 4.0);

  if (features.render_state == style_data::kRenderPressed) {
    painter->fillPath(button_background_path,
                      d->color(resource_manager::kPrimaryColor));
    d->set_pen_color(painter, resource_manager::kSecondryTextColor);
    d->set_default_font_size(painter, 12);
    painter->drawPath(button_background_path);
  } else if (features.render_state == style_data::kRenderRaised || (ck_button && ck_button->is_tool_button())) {
    /*layer 1*/
    QLinearGradient aqua_blue_layer1(QPointF(rect.width() / 2, rect.y()),
                               QPointF(rect.width() / 2, rect.height()));

    aqua_blue_layer1.setColorAt(0, QColor("#A5D6F7"));
    aqua_blue_layer1.setColorAt(0.10, QColor("#84C6F7"));
    aqua_blue_layer1.setColorAt(0.40, QColor("#429CE7"));
    aqua_blue_layer1.setColorAt(1, QColor("#C6FFFF"));

    painter->fillPath(button_background_path, QBrush(aqua_blue_layer1));

    /* layer 2 : gloss left */
    QRectF left_gloss_rect(rect.x() , features.geometry.y(), (rect.width()),
                           rect.height() / 2);

    QPainterPath left_gloss;
    if (ck_button && ck_button->is_tool_button()) {
        left_gloss.addRoundedRect(left_gloss_rect,0, 0);
    }  else {
        left_gloss.addRoundedRect(left_gloss_rect, 4, 4);

    }
    painter->save();
    QLinearGradient aqua_blue_layer2(QPointF(rect.width() / 2, rect.y()),
                                     QPointF(rect.width() / 2, rect.height()));

    aqua_blue_layer2.setColorAt(0, QColor("#ffffff"));
    //aqua_blue_layer2.setColorAt(0.50, QColor("#429CE7"));
    //aqua_blue_layer2.setColorAt(0.70, QColor("#84C6F7"));
    aqua_blue_layer2.setColorAt(1, Qt::transparent);

    painter->setOpacity(0.9);
    painter->fillPath(left_gloss, QBrush(aqua_blue_layer2));
    painter->restore();
  } else {
    /*layer 1*/
    QLinearGradient aqua_blue_layer1(QPointF(rect.width() / 2, rect.y()),
                               QPointF(rect.width() / 2, rect.height()));

    aqua_blue_layer1.setColorAt(0, QColor("#FBFBFB"));
    aqua_blue_layer1.setColorAt(0.50, QColor("#E0E0E0"));
    aqua_blue_layer1.setColorAt(0.70, QColor("#EBEBEB"));
    aqua_blue_layer1.setColorAt(1, QColor("#FFFFFF"));

    painter->fillPath(button_background_path, QBrush(aqua_blue_layer1));

    /* layer 2 : gloss left */
    QRectF left_gloss_rect(rect.x() , features.geometry.y(), (rect.width()),
              rect.height() / 2);

    QPainterPath left_gloss;
    left_gloss.addRoundedRect(left_gloss_rect, 4, 4);

    painter->save();
    QLinearGradient aqua_blue_layer2(QPointF(rect.width() / 2, rect.y()),
                               QPointF(rect.width() / 2, rect.height()));

    aqua_blue_layer2.setColorAt(0, QColor("#ffffff"));
    //aqua_blue_layer2.setColorAt(0.50, QColor("#429CE7"));
    //aqua_blue_layer2.setColorAt(0.70, QColor("#84C6F7"));
    aqua_blue_layer2.setColorAt(1, Qt::transparent);

    painter->setOpacity(0.9);
    painter->fillPath(left_gloss, QBrush(aqua_blue_layer2));
    painter->restore();
  }

  if ((ck_button && !ck_button->is_tool_button())) {
    painter->save();
    QPen shadow_pen;
    shadow_pen.setColor(QColor("#888888"));
    painter->setPen(shadow_pen);
    painter->setOpacity(0.4);
    painter->drawPath(button_background_path);
    painter->restore();
  }

  painter->drawText(features.geometry, Qt::AlignCenter, features.text_data);
  painter->restore();
}

void CocoaStyle::draw_window_button(const style_data &features,
                                    QPainter *painter) {
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
  background.addRoundedRect(rect, 8.0 * scale_factor(), 8.0 * scale_factor());

  if (features.render_state == style_data::kRenderElement) {
    QLinearGradient aqua_blue_fill(QPointF(rect.width() / 2, 0),
                                 QPointF(rect.width() / 2, rect.height()));

    aqua_blue_fill.setColorAt(0, QColor("#EFEFEF"));
    aqua_blue_fill.setColorAt(0.50, QColor("#A0C7F1"));
    aqua_blue_fill.setColorAt(0.70, QColor("#87BAF2"));
    aqua_blue_fill.setColorAt(1, QColor("#C9F5FC"));

    painter->save();
    painter->setPen(QColor("#ffffff"));
    painter->setOpacity(0.5);
    painter->drawPath(background);
    painter->restore();
    painter->fillPath(background, QBrush(aqua_blue_fill));
  } else {
    QLinearGradient aqua_blue_fill(QPointF(rect.width() / 2, 0),
                                 QPointF(rect.width() / 2, rect.height()));

    aqua_blue_fill.setColorAt(0, QColor("#C9F5FC"));
    aqua_blue_fill.setColorAt(0.50, QColor("#87BAF2"));
    aqua_blue_fill.setColorAt(0.70, QColor("#A0C7F1"));
    aqua_blue_fill.setColorAt(1, QColor("#EFEFEF"));

    painter->fillPath(background, QBrush(aqua_blue_fill));

  }

  painter->save();

  d->set_pen_color(painter, resource_manager::kDarkPrimaryColor,
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

  painter->drawPath(background);

  painter->restore();

  painter->restore();
}

#ifdef __APPLE__
static CGContextRef _create_cg_context(int pixelsWide, int pixelsHigh) {
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
  context = CGBitmapContextCreate(bitmapData, // 4
                                  pixelsWide, pixelsHigh,
                                  8, // bits per component
                                  bitmapBytesPerRow, colorSpace,
                                  kCGImageAlphaPremultipliedLast);
  if (context == NULL) {
    free(bitmapData); // 5
    fprintf(stderr, "Context not created!");
    return NULL;
  }
  CGColorSpaceRelease(colorSpace); // 6

  return context; // 7
}

static QImage _cg_image_to_qimage(CGImageRef image) {
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
void CocoaStyle::PrivateCocoa::set_default_font_size(QPainter *painter,
                                                     int a_size,
                                                     bool a_highlight) {
  QFont _font = painter->font();
  _font.setBold(a_highlight);
  _font.setPixelSize(a_size * scale_factor());
  painter->setFont(_font);
}

#ifdef __APPLE__
QImage *CocoaStyle::PrivateCocoa::get_drawable_surface(QPainter *a_ctx) {
  QPaintDevice *current_paint_device = a_ctx->paintEngine()->paintDevice();
  QImage *rv = NULL;

  if (current_paint_device) {
    if (current_paint_device->devType() == QInternal::Image) {
      rv = static_cast<QImage *>(current_paint_device);
    }
  }

  return rv;
}

CGContextRef
CocoaStyle::PrivateCocoa::copy_cg_context(QImage *a_surface_ptr) {
  CGContextRef rv = nil;
  CGColorSpaceRef colorspace =
      CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);

  uint flags = kCGImageAlphaPremultipliedFirst;
  flags |= kCGBitmapByteOrder32Host;
  rv = CGBitmapContextCreate(
      a_surface_ptr->bits(), a_surface_ptr->width(), a_surface_ptr->height(), 8,
      a_surface_ptr->bytesPerLine(), colorspace, flags);
  CGContextTranslateCTM(rv, 0, a_surface_ptr->height());
  CGContextScaleCTM(rv, 1, -1);

  CGColorSpaceRelease(colorspace);

  return rv;
}

#endif

void CocoaStyle::draw_window_frame(const style_data &features,
                                   QPainter *a_ctx) {
#ifdef __APPLE__
  QRectF rect = features.geometry.adjusted(10, 10, -10, -10);
  QRectF _shadow_rect = features.geometry;
#else
  QRectF rect = features.geometry.adjusted(1, 1, -1, -1);
#endif

  a_ctx->save();
  set_default_painter_hints(a_ctx);
  /* draw seperator */
  window *ck_window = dynamic_cast<window *>(features.style_object);

#ifdef __APPLE__
  if (ck_window && ck_window->window_type() != window::kFramelessWindow) {
  /* draw shadow */
  QPaintDevice *current_paint_device = a_ctx->paintEngine()->paintDevice();
  CGContextRef bitmap_ctx = 0;

  if (current_paint_device) {
    if (current_paint_device->devType() == QInternal::Image) {
      QImage *qt_surface = static_cast<QImage *>(current_paint_device);

      if (!qt_surface)
        return;

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

      //CGContextSetShadow(bitmap_ctx, myShadowOffset, 15);

#if MAC_OS_X_VERSION_MIN_REQUIRED > MAC_OS_X_VERSION_10_6
      CGFloat _color_data[] = {0.0f, 0.0f, 0.0f, 1.0f};
#else
      float _color_data[] = {0.0f, 0.0f, 0.0f, 1.0f};
#endif
      CGColorSpaceRef cg_shadow_color_space = CGColorSpaceCreateDeviceRGB();
      CGColorRef cg_shadow_color = CGColorCreate(cg_shadow_color_space, _color_data);

      CGContextSetShadowWithColor(bitmap_ctx, myShadowOffset, 10, cg_shadow_color);

      QColor window_color = d->color(resource_manager::kLightPrimaryColor);

      CGContextSetRGBFillColor(bitmap_ctx, window_color.red(),
                               window_color.blue(), window_color.green(),
                               features.opacity);
      CGContextFillRect(bitmap_ctx, CGRectMake(12, 12, qt_surface->width() - 24,
                                               qt_surface->height() - 24));

      CGContextRestoreGState(bitmap_ctx);
      CGContextRelease(bitmap_ctx);
    } else if (current_paint_device->devType() == QInternal::Pixmap) {
      a_ctx->setClipping(false);
      QPixmap *_pixmap = static_cast<QPixmap *>(current_paint_device);

      if (!_pixmap) {
        qDebug() << Q_FUNC_INFO << "Error Loding Qt Pixmap";
        return;
      }

      QImage _offscreen_buffer = _pixmap->toImage();

      CGColorSpaceRef colorspace =
          CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);
      uint flags = kCGImageAlphaPremultipliedFirst;
      flags |= kCGBitmapByteOrder32Host;

      bitmap_ctx = CGBitmapContextCreate(
          _offscreen_buffer.bits(), _offscreen_buffer.width(), _offscreen_buffer.height(), 8, _offscreen_buffer.bytesPerLine(), colorspace, flags);

      CGContextTranslateCTM(bitmap_ctx, 0, _pixmap->height());
      CGContextScaleCTM(bitmap_ctx, 1, -1);

      CGColorSpaceRelease(colorspace);

      // draw
      CGSize myShadowOffset = CGSizeMake(0.0, 0.0);
      CGContextSaveGState(bitmap_ctx);

#if MAC_OS_X_VERSION_MIN_REQUIRED > MAC_OS_X_VERSION_10_6
      CGFloat _color_data[] = {0.0f, 0.0f, 0.0f, 1.0f};
#else
      float _color_data[] = {0.0f, 0.0f, 0.0f, 1.0f};
#endif
      CGColorSpaceRef cg_shadow_color_space = CGColorSpaceCreateDeviceRGB();
      CGColorRef cg_shadow_color = CGColorCreate(cg_shadow_color_space, _color_data);

      CGContextSetShadowWithColor(bitmap_ctx, myShadowOffset, 15, cg_shadow_color);

      QColor window_color = d->color(resource_manager::kLightPrimaryColor);

      CGContextSetRGBFillColor(bitmap_ctx, window_color.red(),
                               window_color.blue(), window_color.green(),
                               features.opacity);
      CGContextFillRect(bitmap_ctx, CGRectMake(11, 11, _pixmap->width() - 22,
                                               _pixmap->height() - 22));

      CGContextRestoreGState(bitmap_ctx);
      CGContextRelease(bitmap_ctx);
      CGColorSpaceRelease(cg_shadow_color_space);
      CGColorRelease(cg_shadow_color);

      a_ctx->drawImage(_shadow_rect, _offscreen_buffer);
    } else if (current_paint_device->devType() == QInternal::Widget) {
      qDebug() << Q_FUNC_INFO << "Device Type Is Widget";
    }
  } else {
    qDebug() << Q_FUNC_INFO << "Invalide Paint Device";
  }
}
#endif
  /* white aqua gradient */
  QPointF _aqua_start(rect.width() /2, rect.y());
  QPointF _aqua_end(rect.width() /2, rect.y() + 6);
  QLinearGradient aqua_blue_fill(_aqua_start, _aqua_end);

  aqua_blue_fill.setColorAt(0, QColor("#FFFFFF"));
  aqua_blue_fill.setColorAt(0.50, QColor("#FBFBFB"));
  aqua_blue_fill.setColorAt(0.70, QColor("#F6F6F6"));
  aqua_blue_fill.setColorAt(1, QColor("#FFFFFF"));
 
  aqua_blue_fill.setSpread(QGradient::RepeatSpread);
  //aqua_blue_fill.setSpread(QGradient::ReflectSpread);

  a_ctx->setOpacity(features.opacity / 2);
  /* draw the adjusted window frame */
  QPainterPath window_background_path;
  window_background_path.addRoundedRect(rect, 2.0, 2.0);
  a_ctx->fillPath(window_background_path, QBrush(aqua_blue_fill));
  a_ctx->save();
  QPen _outline_pen;
  _outline_pen.setColor(QColor("#ffffff"));
  a_ctx->setPen(_outline_pen);
  a_ctx->drawPath(window_background_path);
  a_ctx->restore();

  if (ck_window && ck_window->window_type() != window::kPanelWindow) {
    QRectF window_title_rect(12, 5, rect.width() - 16, 48.0 * scale_factor());
    QRectF window_title_fill(0, 0, rect.width() , 48.0 * scale_factor());

    /* window border */
    QPainterPath border_path;
    border_path.addRoundedRect(window_title_fill, 4.0, 4.0);
    /*
    if (ck_window->window_type() != window::kPopupWindow) {
        a_ctx->save();
        a_ctx->setPen(QColor("#ffffff"));
        a_ctx->setOpacity(0.8);
        a_ctx->drawPath(window_background_path);
        a_ctx->restore();
    }
    */

    QLinearGradient seperator_line_grad(window_title_rect.bottomLeft(),
                                        window_title_rect.bottomRight());
    seperator_line_grad.setColorAt(
        0.0, d->color(resource_manager::kLightPrimaryColor));
    seperator_line_grad.setColorAt(0.5,
                                   d->color(resource_manager::kDarkPrimaryColor));
    seperator_line_grad.setColorAt(
        1.0, QColor("#ffffff"));

    // draw frame text;
    a_ctx->save();

    d->set_default_font_size(a_ctx, 16.0, true);
    d->set_pen_color(a_ctx, resource_manager::kTextColor);
    a_ctx->setOpacity(1.0);
    a_ctx->drawText(window_title_rect, features.text_data,
                    QTextOption(Qt::AlignCenter));

    a_ctx->restore();

    a_ctx->save();

    /*
    QPointF _aqua_start(window_title_rect.width()/2, window_title_rect.x());
    QPointF _aqua_end(window_title_rect.width() /2, window_title_rect.y() + window_title_rect.height());
    QLinearGradient aqua_blue_fill(_aqua_start, _aqua_end);

    aqua_blue_fill.setColorAt(0, QColor("#FFFFFF"));
    aqua_blue_fill.setColorAt(0.50, QColor("#FBFBFB"));
    aqua_blue_fill.setColorAt(0.70, QColor("#F6F6F6"));
    aqua_blue_fill.setColorAt(1, QColor("#FFFFFF"));

    QPen linePen = QPen(seperator_line_grad, 1, Qt::SolidLine, Qt::RoundCap,
                        Qt::RoundJoin);
    a_ctx->setPen(linePen);
  
    a_ctx->drawLine(QPointF(rect.x() + 10 , window_title_rect.height()),
                    QPointF(rect.width(), window_title_rect.height()));
    */
    //a_ctx->drawLine(window_title_rect.bottomLeft(), window_title_rect.bottomRight());

    a_ctx->restore();
   
  }

  a_ctx->setOpacity(0.6);
  a_ctx->restore();
#ifdef __APPLE__
#endif
    //
}

void CocoaStyle::draw_clock_hands(
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

  d->set_pen_color(a_ctx, a_clock_hand_color, a_thikness + 1);
  a_ctx->drawLine(_clock_hour_rect.topLeft(), _clock_hour_rect.center());
  a_ctx->restore();
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
  float border_len = features.geometry.width() - (48);

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


  /* aqua */
  QLinearGradient aqua_blue_fill(QPointF(rect.width() / 2, 0),
                                 QPointF(rect.width() / 2, rect.height()));

  aqua_blue_fill.setColorAt(0, QColor("#FFFFFF"));
  aqua_blue_fill.setColorAt(0.60, QColor("#FFFFFF"));
  aqua_blue_fill.setColorAt(0.75, QColor("#FFFFFF"));
  aqua_blue_fill.setColorAt(1, QColor("#FFFFFF"));
    
  a_ctx->save();
  a_ctx->setOpacity(0.9);
  a_ctx->fillPath(_clock_background, QBrush(aqua_blue_fill));
  a_ctx->restore();  
  
  a_ctx->save();
  QPen _outline_pen;
  float _frame_adjust = 8;
  _outline_pen.setColor(d->color(resource_manager::kDarkPrimaryColor));
  _outline_pen.setWidth(10);
  a_ctx->setPen(_outline_pen);
  QRectF frame_rect(features.geometry.x() + _frame_adjust,
                    features.geometry.y() + _frame_adjust,
                    features.geometry.width() - (_frame_adjust * 2),
                    features.geometry.height() - (_frame_adjust * 2));
  a_ctx->drawEllipse(frame_rect);
  a_ctx->restore();

  a_ctx->save();
  QRectF glass_rect(features.geometry.x() + _frame_adjust ,
                    features.geometry.y() + _frame_adjust ,
                    features.geometry.width() - (_frame_adjust * 2),
                    features.geometry.height() - (_frame_adjust* 2));
  qreal rad = (glass_rect.width() / 2);
  QRadialGradient gr(rad + 16 + _frame_adjust , rad + _frame_adjust, rad, rad , rad );

  gr.setColorAt(0.0, QColor(255, 255, 255, 89));
  gr.setColorAt(0.2, QColor(255, 255, 255, 89));
  gr.setColorAt(0.9, QColor(255, 255, 255, 89));
  gr.setColorAt(0.95, QColor(100, 100, 100, 127));
  gr.setColorAt(1, QColor(0, 0, 0, 0));

  a_ctx->setBrush(gr);
  a_ctx->setPen(Qt::NoPen);
  //a_ctx->drawEllipse(glass_rect);
  a_ctx->drawRect(glass_rect);
  a_ctx->restore();

  // draw second markers.
  for (int i = 0; i < 60; i++) {
    double percent = (i / 60.0);
    QPointF marker_location = _clock_background.pointAtPercent(percent);

    d->set_pen_color(a_ctx, resource_manager::kDarkPrimaryColor, 1);
    a_ctx->drawPoint(marker_location);
  }

  // draw minute markers
  for (int i = 0; i < 360; i = i + 30) {
    float percent = (i / 360.0);
    QPointF marker_location = _clock_background.pointAtPercent(percent);

    d->set_pen_color(a_ctx, resource_manager::kDarkPrimaryColor, 3);
    a_ctx->drawPoint(marker_location);
  }

  // draw hour markers
  for (int i = 0; i < 360; i = i + 90) {
    float percent = (i / 360.0);
    QPointF marker_location = _clock_background.pointAtPercent(percent);

    d->set_pen_color(a_ctx, resource_manager::kDarkPrimaryColor, 4);
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
                   resource_manager::kDarkPrimaryColor, 3 * scale_factor());
  draw_clock_hands(a_ctx, rect, 4, minutes_value,
                   resource_manager::kDarkPrimaryColor, 2 * scale_factor());

  QRectF _clock_wheel_rect(rect.center().x() - (4 * scale_factor()),
                           rect.center().y() - (4 * scale_factor()),
                           8 * scale_factor(), 8 * scale_factor());

  QRectF _clock_wheel_inner_rect(rect.center().x() - (2 * scale_factor()),
                                 rect.center().y() - (2 * scale_factor()),
                                 4 * scale_factor(), 4 * scale_factor());

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

  d->set_pen_color(a_ctx, resource_manager::kDarkPrimaryColor, 1);
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

  d->set_pen_color(a_ctx, resource_manager::kDarkPrimaryColor, 1);
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
  qreal rad = border_rect.width() / 2;

  QRadialGradient aqua_white_inverted(border_rect.center(), rad, border_rect.center());

  aqua_white_inverted.setColorAt(0, QColor("#FFFFFF"));
  //aqua_white_inverted.setColorAt(0.50, QColor("#F4F4F4"));
  //aqua_white_inverted.setColorAt(0.70, QColor("#FDFDFD"));
  aqua_white_inverted.setColorAt(1, QColor("#F2F2F2"));

  a_ctx->fillPath(border_path, QBrush(aqua_white_inverted));

  QTransform xform = a_ctx->transform();
  xform.translate(transPos.x(), transPos.y());
  xform.rotate(-90);
  xform.translate(-transPos.x(), -transPos.y());

  a_ctx->setTransform(xform);

  a_ctx->save();

  QPainterPath handle_path;
  QLinearGradient aqua_blue_layer1(QPointF(0, handle_rect.height() / 2),
                                   QPointF(handle_rect.x() + (handle_rect.width()), handle_rect.height() / 2));

  aqua_blue_layer1.setColorAt(0, Qt::transparent);
  //aqua_blue_layer1.setColorAt(0.50, QColor("#429CE7"));
  //aqua_blue_layer1.setColorAt(0.70, QColor("#84C6F7"));
  aqua_blue_layer1.setColorAt(1, QColor("#FFFFFF"));

  //handle_path.addEllipse(current_marker_location_for_min, 6, 6);
  handle_path.addEllipse(handle_rect);
  a_ctx->setOpacity(0.5);
  a_ctx->fillPath(handle_path, QBrush(aqua_blue_layer1));

  a_ctx->restore();

  d->set_pen_color(a_ctx, resource_manager::kDarkPrimaryColor, 8);
  a_ctx->drawPoint(current_marker_location_for_min);

  if (features.render_state == style_data::kRenderRaised) {
    d->set_pen_color(a_ctx, resource_manager::kDarkPrimaryColor, 8);
    //a_ctx->drawPoint(current_marker_location_for_min);
  }

  if (features.render_state == style_data::kRenderPressed) {
    d->set_pen_color(a_ctx, resource_manager::kDarkPrimaryColor, 4);
    //a_ctx->drawPoint(current_marker_location_for_min);
  }
}

void CocoaStyle::draw_line_edit(const style_data &features, QPainter *painter) {
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

    if (ck_line_edit && !ck_line_edit->readonly()) {
      painter->setOpacity(0.8);
      painter->drawPath(background_path);
      painter->setOpacity(1.0);
    }

    painter->save();

    d->set_pen_color(painter, resource_manager::kTextColor);

    if (ck_line_edit && ck_line_edit->readonly()) {
        /*layer 1*/
        QLinearGradient aqua_blue_layer1(QPointF(rect.width() / 2, rect.y()),
                                   QPointF(rect.width() / 2, rect.height()));

        aqua_blue_layer1.setColorAt(0, QColor("#FBFBFB"));
        aqua_blue_layer1.setColorAt(0.50, QColor("#E0E0E0"));
        aqua_blue_layer1.setColorAt(0.70, QColor("#EBEBEB"));
        aqua_blue_layer1.setColorAt(1, QColor("#FFFFFF"));

        QPainterPath readonly_path;
        readonly_path.addRect(rect);
        painter->fillPath(readonly_path, QBrush(aqua_blue_layer1));
    }


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

  painter->save();
  QPen shadow_pen;
  shadow_pen.setColor(d->color(resource_manager::kDarkPrimaryColor));
  painter->setOpacity(0.3);
  painter->setPen(shadow_pen);
  painter->drawPath(background_path);
  painter->restore();

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
  a_ctx->save();
  set_default_painter_hints(a_ctx);
  a_ctx->setRenderHint(QPainter::HighQualityAntialiasing, true);
  a_ctx->setRenderHint(QPainter::Antialiasing, true);
  a_ctx->setRenderHint(QPainter::TextAntialiasing, true);
  qDebug() << Q_FUNC_INFO << "target @: " << icon_draw_rect << " src @: " << a_features.image_data.rect();


  if (icon_area.width() < 16 || icon_area.height() < 16)
    a_ctx->setRenderHint(QPainter::SmoothPixmapTransform, false);
  else
    a_ctx->setRenderHint(QPainter::SmoothPixmapTransform, true);

  a_ctx->translate(icon_draw_rect.x(), icon_draw_rect.y());
  a_ctx->drawImage(a_features.image_data.rect(), a_features.image_data, a_features.image_data.rect(), Qt::DiffuseDither | Qt::DiffuseAlphaDither);
  a_ctx->restore();

  a_ctx->save();
  d->set_default_font_size(a_ctx, 12, true);
  d->set_pen_color(a_ctx, resource_manager::kTextColor);
  a_ctx->setRenderHint(QPainter::Antialiasing, true);
  a_ctx->setRenderHint(QPainter::TextAntialiasing, true);
  a_ctx->drawText(text_draw_rect, a_features.text_data,
                  Qt::AlignCenter | Qt::AlignVCenter);
  a_ctx->restore(); 

  a_ctx->restore();
  a_ctx->restore();
 }

void CocoaStyle::draw_scrollbar(const style_data &a_data, QPainter *a_ctx) {
  QRectF rect = a_data.geometry;

  a_ctx->save();
  a_ctx->setOpacity(0.9);
  a_ctx->setRenderHints(
      QPainter::HighQualityAntialiasing | QPainter::Antialiasing, true);
  QPainterPath path;
  path.addRoundedRect(rect, 4, 4);

  QLinearGradient aqua_white_inverted(QPointF(rect.x(), rect.height() / 2),
                               QPointF(rect.x() + rect.width(), rect.height() / 2));

  aqua_white_inverted.setColorAt(0, QColor("#C5C5C5"));
  aqua_white_inverted.setColorAt(0.50, QColor("#F4F4F4"));
  aqua_white_inverted.setColorAt(0.70, QColor("#FDFDFD"));
  aqua_white_inverted.setColorAt(1, QColor("#F2F2F2"));

  a_ctx->fillPath(path, QBrush(aqua_white_inverted));
  a_ctx->restore();
}

void CocoaStyle::draw_scrollbar_slider(const style_data &a_data,
                                       QPainter *a_ctx) {
  QRectF rect(2, a_data.geometry.y(), a_data.geometry.width() - 4,
              a_data.geometry.height());
  a_ctx->save();
  a_ctx->setRenderHints(
      QPainter::HighQualityAntialiasing | QPainter::Antialiasing, true);
  QPainterPath path;
  path.addRoundedRect(rect, 3, 3);

  /*layer 1*/
  QLinearGradient aqua_blue_layer1(QPointF(rect.x(), rect.height() / 2),
                               QPointF(rect.x() + rect.width(), rect.height() / 2));

  aqua_blue_layer1.setColorAt(0, QColor("#0051C0"));
  aqua_blue_layer1.setColorAt(0.50, QColor("#429CE7"));
  aqua_blue_layer1.setColorAt(0.70, QColor("#84C6F7"));
  aqua_blue_layer1.setColorAt(1, QColor("#A5D6F7"));

  a_ctx->fillPath(path, QBrush(aqua_blue_layer1));

  /* layer 2 : gloss left */
  QRectF left_gloss_rect(rect.x() + 1, a_data.geometry.y() + 1, (rect.width() / 2) - 2,
              rect.height() - 2);

  QPainterPath left_gloss;
  left_gloss.addRoundedRect(left_gloss_rect, 4, 4);

  a_ctx->save();
  QLinearGradient aqua_blue_layer2(QPointF(rect.x(), rect.height() / 2),
                               QPointF(rect.x() + rect.width(), rect.height() / 2));

  aqua_blue_layer2.setColorAt(0, QColor("#ffffff"));
  //aqua_blue_layer2.setColorAt(0.50, QColor("#429CE7"));
  //aqua_blue_layer2.setColorAt(0.70, QColor("#84C6F7"));
  aqua_blue_layer2.setColorAt(1, Qt::transparent);
  a_ctx->setOpacity(0.9);
  a_ctx->fillPath(left_gloss, QBrush(aqua_blue_layer2));
  a_ctx->restore();

  a_ctx->restore();
}

void CocoaStyle::draw_label(const style_data &aFeatures, QPainter *a_ctx) {
  a_ctx->save();

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
    QRectF rect = aFeatures.geometry;
    QLinearGradient aqua_blue_fill(QPointF(rect.width() / 2, 0),
                                 QPointF(rect.width() / 2, rect.height()));

    aqua_blue_fill.setColorAt(0, QColor("#697AF0"));
    aqua_blue_fill.setColorAt(0.50, QColor("#5768F7"));
    aqua_blue_fill.setColorAt(1, QColor("#4351f6"));

    a_ctx->save();
    a_ctx->setOpacity(0.4);
    a_ctx->fillRect(aFeatures.geometry, QBrush(aqua_blue_fill));
    a_ctx->restore();
  } else {
    d->set_pen_color(a_ctx, resource_manager::kTextColor);
  }

  set_default_painter_hints(a_ctx);
  a_ctx->drawText(aFeatures.geometry, text, aFeatures.text_options);
  a_ctx->restore();
}

QColor CocoaStyle::PrivateCocoa::color(resource_manager::ColorName a_name) {
  return resource_manager::color(a_name);
}
