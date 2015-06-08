#include "classicbackgroundrender.h"

#include <QUrl>
#include <QDebug>
#include <QGraphicsDropShadowEffect>
#include <themepackloader.h>
#include <QPixmapCache>

ClassicBackgroundRender::ClassicBackgroundRender(const QRectF &rect,
                                                 QGraphicsObject *parent,
                                                 const QImage &background_image)
    : UIKit::Window(parent) {
  setFlag(QGraphicsItem::ItemIsMovable, false);
  setFlag(QGraphicsItem::ItemIsFocusable, true);
  mBackgroundImage = background_image;
  mScalingMode = None;
  mSeamLessMode = false;
  set_window_type(UIKit::Window::kFramelessWindow);
  enable_window_background(false);
}

void ClassicBackgroundRender::setBackgroundImage(const QString &path) {
  if (path.isEmpty() || path.isNull()) {
    return;
  }

  mScalingMode = None;
  mBackgroundImage.load(path);
  qDebug() << Q_FUNC_INFO << path << "Image Loader ?"
           << mBackgroundImage.isNull();
  update();
}

void ClassicBackgroundRender::setBackgroundImage(const QImage &img) {
  mScalingMode = None;
  mBackgroundImage = img;
  QSizeF size = mBackgroundImage.size();
  setCacheMode(QGraphicsItem::ItemCoordinateCache, mBackgroundImage.size());
  QPixmapCache::setCacheLimit((size.width() * size.height() * 32) / 8);
  update();
}

void ClassicBackgroundRender::setBackgroundImage(const QUrl &url) {
  mScalingMode = None;
  qDebug() << Q_FUNC_INFO << url;
  setBackgroundImage(url.toLocalFile());
}

void ClassicBackgroundRender::setBackgroundGeometry(const QRectF &rect) {
  mGeometry = rect;
  setGeometry(rect);
}

void ClassicBackgroundRender::setBackgroundMode(
    ClassicBackgroundRender::ScalingMode mode) {
  mScalingMode = mode;

  if (mode == FitHeight) {
    mBackgroundImageHeightScaled =
        mBackgroundImage.scaledToHeight(this->boundingRect().height());
  }

  if (mode == FitWidth) {
    mBackgroundImageWidthScaled =
        mBackgroundImage.scaledToWidth(this->boundingRect().width());
  }

  update();
}

StylePtr ClassicBackgroundRender::style() const {
  return UIKit::Theme::style();
}

void ClassicBackgroundRender::drawBackroundFrame(QPainter *painter,
                                                 const QRectF &rect) {
  painter->fillRect(rect, QColor(0, 0, 0));
  painter->save();
  float xoffset = (rect.width() - mBackgroundImage.width()) / 2;
  float yoffset = (rect.height() - mBackgroundImage.height()) / 2;
  QRectF imageRect(xoffset, yoffset, mBackgroundImage.width(),
                   mBackgroundImage.height());
  QPainterPath path;
  path.addRoundRect(imageRect, 8.0, 8.0);
  painter->setClipPath(path);
  painter->drawImage(imageRect, mBackgroundImage);
  painter->restore();
}
bool ClassicBackgroundRender::isSeamlessModeSet() const {
  return mSeamLessMode;
}

void ClassicBackgroundRender::setSeamLessMode(bool value) {
  mSeamLessMode = value;
  update();
}

void ClassicBackgroundRender::paint_view(QPainter *painter,
                                         const QRectF &rect /*rect*/) {
  if (mSeamLessMode) {
    painter->save();
    painter->setRenderHints(QPainter::SmoothPixmapTransform |
                            QPainter::Antialiasing |
                            QPainter::HighQualityAntialiasing);
    painter->setBackgroundMode(Qt::TransparentMode);
    painter->setCompositionMode(QPainter::CompositionMode_Source);
    painter->fillRect(rect, Qt::transparent);
    painter->restore();
    return;
  }

  painter->save();
  painter->setRenderHint(QPainter::SmoothPixmapTransform);
  painter->setRenderHint(QPainter::Antialiasing);

  switch (mScalingMode) {
    case None: {
      painter->drawImage(rect, mBackgroundImage);
    } break;
    case Tile: {
      QBrush brush(mBackgroundImage);
      painter->setBrush(brush);
      painter->setBackground(brush);
      painter->fillRect(rect, painter->background());
    } break;
    case Streach: {
      painter->drawImage(rect, mBackgroundImage);
    } break;
    case Frame: {
      drawBackroundFrame(painter, rect);
    } break;
    case FitWidth: {
      painter->drawImage(rect, mBackgroundImageWidthScaled);
    } break;
    case FitHeight: {
      painter->drawImage(rect, mBackgroundImageHeightScaled);
    } break;
    case CenterFocus:
      painter->drawImage(rect, mBackgroundImage, rect,
                         Qt::ColorOnly | Qt::DiffuseAlphaDither |
                             Qt::DiffuseDither | Qt::PreferDither);
      break;
    default:
      break;
  }
  painter->restore();
}
