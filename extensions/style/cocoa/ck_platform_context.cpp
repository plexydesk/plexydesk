#include "ck_platform_context.h"

#include <QPainter>
#include <QPaintEngine>
#include <QPaintDevice>

#ifdef __APPLE__
#include <CoreGraphics/CGContext.h>
#include <CoreGraphics/CGBitmapContext.h>
#include <CoreGraphics/CGDirectDisplay.h>
#include <stdlib.h>
#include <malloc/malloc.h>
#endif

class ck_platform_context::private_platform_context {
public:
  private_platform_context(QImage *a_surface) { m_surface_ref = a_surface; }
  ~private_platform_context() {}

  QImage *get_drawable_surface(QPainter *a_ctx);
  CGContextRef copy_cg_context(QImage *a_surface_ptr);

  QImage *m_surface_ref;
};

ck_platform_context::ck_platform_context(QImage *a_surface_ref)
    : ctx(new private_platform_context(a_surface_ref)) {}

ck_platform_context::~ck_platform_context() { delete ctx; }

bool ck_platform_context::is_valid() const {
  return (ctx->m_surface_ref && ctx->copy_cg_context(ctx->m_surface_ref));
}

void ck_platform_context::draw_round_rect(float x, float y, float width,
                                          float height, float x_radious,
                                          float y_radious) {
  CGContextRef p = nil;

  if (!ctx->m_surface_ref)
    return;

  p = ctx->copy_cg_context(ctx->m_surface_ref);

  if (!p)
    return;

  CGRect rect = {{x, y}, { x + (width - 2), y + (height -2 )}};

  CGContextSetLineWidth(p, 2.0);
  CGContextBeginPath(p);

  CGContextSaveGState(p);
  CGContextAddRect(p, rect);
  CGContextRestoreGState(p);

  CGContextSetRGBStrokeColor(p, 1., 0., 0., 1.);
  CGContextDrawPath(p, kCGPathStroke);

  CGContextRelease(p);
}

QImage *ck_platform_context::private_platform_context::get_drawable_surface(
    QPainter *a_ctx) {
  QPaintDevice *current_paint_device = a_ctx->paintEngine()->paintDevice();
  QImage *rv = NULL;

  if (current_paint_device) {
    if (current_paint_device->devType() == QInternal::Image) {
      rv = static_cast<QImage *>(current_paint_device);
    }
  }

  return rv;
}

CGContextRef ck_platform_context::private_platform_context::copy_cg_context(
    QImage *a_surface_ptr) {
  CGContextRef rv = nil;
  CGColorSpaceRef colorspace =
      CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB);

  uint flags = kCGImageAlphaPremultipliedFirst;
  flags |= kCGBitmapByteOrder32Host;
  rv = CGBitmapContextCreate(a_surface_ptr->bits(), a_surface_ptr->width(),
                             a_surface_ptr->height(), 8,
                             a_surface_ptr->bytesPerLine(), colorspace, flags);
  CGContextTranslateCTM(rv, 0, a_surface_ptr->height());
  CGContextScaleCTM(rv, 1, -1);

  CGColorSpaceRelease(colorspace);

  return rv;
}
