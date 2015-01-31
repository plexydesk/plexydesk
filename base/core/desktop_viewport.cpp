#include "desktop_viewport.h"

#include <QDebug>

namespace PlexyDesk {

QPointF DesktopViewport::center(
    const QRectF &viewGeometry,
    const DesktopViewport::ViewportLocation &loc) const {
  QPointF _rv;
  float _x_location;
  float _y_location;

  switch (loc) {
    case kCenterOnViewport:
      _y_location = (geometry().height() / 2) - (viewGeometry.height() / 2);
      _x_location = (geometry().width() / 2) - (viewGeometry.width() / 2);
      break;
    case kCenterOnViewportLeft:
      _y_location = (geometry().height() / 2) - (viewGeometry.height() / 2);
      _x_location = (geometry().topLeft().x());
      break;
    case kCenterOnViewportRight:
      _y_location = (geometry().height() / 2) - (viewGeometry.height() / 2);
      _x_location = (geometry().width() - (viewGeometry.width()));
      break;
    case kCenterOnViewportTop:
      _y_location = (0.0);
      _x_location = ((geometry().width() / 2) - (viewGeometry.width() / 2));
      break;
    case kCenterOnViewportBottom:
      _y_location = (geometry().height() - (viewGeometry.height()));
      _x_location = ((geometry().width() / 2) - (viewGeometry.width() / 2));
      break;
    default:
      qWarning() << Q_FUNC_INFO << "Error : Unknown Viewprot Location Type:";
  }

  _rv.setY(geometry().y() + _y_location);
  _rv.setX(_x_location);

  return _rv;
}

DesktopViewport::DesktopViewport(QObject *parent) : QObject(parent) {}
}
