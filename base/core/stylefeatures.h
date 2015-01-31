#ifndef STYLE_FEATURES_H
#define STYLE_FEATURES_H

#include <QtCore/QObject>
#include <QtCore/QRectF>
#include <QtGui/QPainter>
#include <plexy_core_exports.h>

namespace PlexyDesk {

class CORE_DECL_EXPORT StyleFeatures {

public:
  typedef enum {
    kRenderBackground,
    kRenderForground,
    kRenderElement,
    kRenderRaised,
    kRenderPressed
  } RenderState;

  QRectF geometry;
  QString text_data;
  QPixmap image_data;
  RenderState render_state;
};
}
#endif
