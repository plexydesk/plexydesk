#ifndef STYLE_FEATURES_H
#define STYLE_FEATURES_H

#include <QObject>
#include <QPainter>
#include <QVariant>
#include <plexydesk_ui_exports.h>
#include <QtCore>

namespace cherry_kit {

class DECL_UI_KIT_EXPORT style_data {

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
  QTextOption text_options;
  QPixmap image_data;
  RenderState render_state;

  QVariantMap attributes;
};
}
#endif
