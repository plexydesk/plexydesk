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
#include "asyncimagecreator.h"
#include <QCryptographicHash>
#include <QFile>
#include <QDebug>
#include <QByteArray>
#include <QBuffer>
#include <QPainter>
#include <QDir>

namespace QuetzalSocialKit {

class AsyncImageCreator::PrivateAsyncImageCreator {
public:
  PrivateAsyncImageCreator() {}
  ~PrivateAsyncImageCreator() {}

  void saveFile(const QString &path, const QImage &img);

  QImage mImage;
  QImage mThumbNail;
  QImage mCropedImage;
  QImage mWidthScaledImage;
  QImage mHeightScaledImage;

  QByteArray mDataArray;
  bool mOffline;
  QString mStorePrefix;
  QString mFileName;
  QString mInputFileName;

  QVariantMap mMetaData;

  int mScaleWidth;
  int mScaleHeight;
  QSize mThumbNailSize;
  QRect mCropRect;
  bool mCropImage;

  std::function<void(AsyncImageCreator *)> m_on_complete_call;
};

AsyncImageCreator::AsyncImageCreator(QObject *parent)
    : QThread(parent), d(new PrivateAsyncImageCreator) {
  d->mScaleWidth = 0;
  d->mScaleHeight = 0;
  d->mThumbNailSize = QSize(150, 150);
  d->mCropImage = false;
  setTerminationEnabled(true);
  connect(this, SIGNAL(finished()), this, SLOT(on_finished()));
}

AsyncImageCreator::~AsyncImageCreator() {
    qDebug() << Q_FUNC_INFO;
    delete d;
}

void AsyncImageCreator::setMetaData(const QVariantMap &data) {
  d->mMetaData = data;
}

QVariantMap AsyncImageCreator::metaData() const { return d->mMetaData; }

void AsyncImageCreator::setData(const QString &path, const QString &prefix,
                                bool save) {
  d->mInputFileName = path;
  d->mOffline = save;
  d->mStorePrefix = prefix;
}

void AsyncImageCreator::setData(const QByteArray &data, const QString &path,
                                bool save) {
  d->mDataArray = data;
  d->mOffline = save;
  d->mStorePrefix = path;
}

void AsyncImageCreator::setData(const QImage &data, const QString &path,
                                bool save) {
  d->mImage = data;
  d->mOffline = save;
  d->mStorePrefix = path;
}

void AsyncImageCreator::setCrop(const QRectF &cropRect) {
  d->mCropRect =
      QRect(cropRect.x(), cropRect.y(), cropRect.width(), cropRect.height());
  d->mCropImage = true;
}

void AsyncImageCreator::setScaleToHeight(int height) {
  d->mScaleHeight = height;
}

void AsyncImageCreator::setScaleToWidth(int width) { d->mScaleWidth = width; }

void AsyncImageCreator::setThumbNailSize(const QSize &size) {
  d->mThumbNailSize = size;
}

QImage AsyncImageCreator::thumbNail() const { return d->mThumbNail; }

QImage AsyncImageCreator::image() const {
  if (d->mScaleHeight > 0) {
    return d->mHeightScaledImage;
  }

  if (d->mScaleWidth > 0) {
    return d->mWidthScaledImage;
  }

  if (d->mCropedImage.width() > 0) {
    return d->mCropedImage;
  }

  return d->mImage;
}

QString AsyncImageCreator::imagePath() const { return d->mFileName; }

bool AsyncImageCreator::offline() const { return d->mOffline; }

QByteArray AsyncImageCreator::imageToByteArray(const QImage &img) const {
  QByteArray array;
  QBuffer buffer(&array);
  buffer.open(QIODevice::WriteOnly);
  img.save(&buffer, "PNG"); // writes image into ba in PNG format
  buffer.close();

  return array;
}

void AsyncImageCreator::on_task_complete(
    std::function<void(AsyncImageCreator *)> a_callback) {
  d->m_on_complete_call = a_callback;
}

void AsyncImageCreator::on_finished() {
  qDebug() << Q_FUNC_INFO << "Finished";
  //quit();
  if (d->m_on_complete_call)
    d->m_on_complete_call(this);
}

void AsyncImageCreator::run() {
  // first we make sure we have an image
  QString cacheFileName;
  bool hasPreview = false;

  if (d->mImage.isNull() && d->mInputFileName.isEmpty() &&
      (!d->mDataArray.isEmpty() || !d->mDataArray.isNull())) {
    d->mImage.loadFromData(d->mDataArray);
    cacheFileName = QCryptographicHash::hash(d->mDataArray,
                                             QCryptographicHash::Md5).toHex();
  } else if (d->mImage.isNull() && !d->mInputFileName.isEmpty()) {
    cacheFileName =
        QCryptographicHash::hash(QByteArray().append(d->mInputFileName),
                                 QCryptographicHash::Md5).toHex();
    d->mFileName = d->mInputFileName;
    // cache preview
    QString cachePreviewFilePath = QDir::toNativeSeparators(
        d->mStorePrefix + "/" + cacheFileName + "_preview.png");
    QFile cachePreviewFile(cachePreviewFilePath);
    if (cachePreviewFile.exists()) {
      d->mThumbNail = QImage(cachePreviewFilePath);
    } else {
      d->mImage = QImage(d->mInputFileName);
      d->mThumbNail =
          d->mImage.scaled(d->mThumbNailSize, Qt::KeepAspectRatioByExpanding);
      d->saveFile(cachePreviewFilePath, d->mThumbNail);
    }

    hasPreview = true;
  } else {
    d->mDataArray = imageToByteArray(d->mImage);
    cacheFileName = QCryptographicHash::hash(d->mDataArray,
                                             QCryptographicHash::Md5).toHex();
  }

  QString cacheFilePath =
      QDir::toNativeSeparators(d->mStorePrefix + "/" + cacheFileName + ".png");

  if (d->mOffline && !d->mStorePrefix.isEmpty()) {
    d->saveFile(cacheFilePath, d->mImage);
    d->mFileName = cacheFilePath;
  }

  // create the thumbnail
  if (!hasPreview)
    d->mThumbNail =
        d->mImage.scaled(d->mThumbNailSize, Qt::KeepAspectRatioByExpanding);

  // scale operations
  if (d->mScaleHeight > 0) {
    d->mHeightScaledImage = d->mImage.scaledToHeight(d->mScaleHeight);
  }

  if (d->mScaleWidth > 0) {
    d->mWidthScaledImage = d->mImage.scaledToWidth(d->mScaleWidth);
  }

  if (d->mCropImage) {
    QImage imageToCrop = d->mImage.scaledToHeight(d->mCropRect.height());
    d->mCropedImage = imageToCrop.copy(d->mCropRect);
  }

  qDebug() << Q_FUNC_INFO << "Image Is Null within Thread : " <<
              d->mImage.isNull();
  Q_EMIT ready();
}

QImage AsyncImageCreator::genThumbNail(const QImage &img) const {
  QImage canvas(QSize(img.height(), img.height()),
                QImage::Format_ARGB32_Premultiplied);

  QPainter painter(&canvas);
  painter.fillRect(canvas.rect(), Qt::blue);

  painter.end();

  return canvas;
}

void AsyncImageCreator::PrivateAsyncImageCreator::saveFile(const QString &path,
                                                           const QImage &img) {
  QFile file(path);

  if (!file.exists()) {
    file.open(QIODevice::WriteOnly);
    bool saved = img.save(&file);
    file.close();
  }
}
}
