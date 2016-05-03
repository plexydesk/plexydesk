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
#include "asyncimageloader.h"
#include <QDebug>
#include <QDir>
#include <QPixmap>
#include <QImage>
#include <QUrl>
#include <QString>
#include <QFileInfo>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

namespace social_kit {

class AsyncImageLoader::PrivateAsyncImageLoader {
public:
  PrivateAsyncImageLoader() {}
  ~PrivateAsyncImageLoader() {
    mThumbData.clear();
    mPathData.clear();
  }

  QUrl mUrl;
  QMap<QString, QImage> mThumbData;
  QMap<QString, QString> mPathData;
  QNetworkAccessManager *nm;
};

AsyncImageLoader::AsyncImageLoader(QObject *parent)
    : QThread(parent), d(new PrivateAsyncImageLoader) {}

AsyncImageLoader::~AsyncImageLoader() {
  qDebug() << Q_FUNC_INFO;
  delete d;
}

void AsyncImageLoader::setUrl(const QUrl &url) { d->mUrl = url; }

QList<QImage> AsyncImageLoader::thumbNails() const {
  return d->mThumbData.values();
}

QImage AsyncImageLoader::imageThumbByName(const QString &key) const {
  return d->mThumbData[key];
}

QString AsyncImageLoader::filePathFromName(const QString &key) const {
  return d->mPathData[key];
}

void AsyncImageLoader::run() {
  qDebug() << Q_FUNC_INFO;
  if (!d->mUrl.isValid()) {
    qWarning() << Q_FUNC_INFO << "Invalid URL";
    Q_EMIT ready();
    return;
  }

  if (d->mUrl.isLocalFile()) {
    qDebug() << Q_FUNC_INFO << "Local File";
    QString localFile = d->mUrl.toLocalFile();
    QFileInfo info(localFile);

    if (info.isDir()) {
      qDebug() << Q_FUNC_INFO << "Local Dir";
      QDir localPictureDir(info.filePath());
      QStringList filters;
      filters << "*.png"
              << "*.jpg"
              << "*.jpeg"
              << "*.tiff"
              << "*.svg";

      localPictureDir.setNameFilters(filters);

      QStringList localPictureList = localPictureDir.entryList();

      Q_FOREACH(const QString & pictureName, localPictureList) {
        QImage wallpaperImage(QDir::toNativeSeparators(
            localPictureDir.absolutePath() + "/" + pictureName));
        if (!wallpaperImage.isNull()) {
          // d->mData[pictureName] = wallpaperImage;
          d->mThumbData[pictureName] = wallpaperImage;
          d->mPathData[pictureName] = QDir::toNativeSeparators(
              localPictureDir.absolutePath() + "/" + pictureName);
          qDebug() << Q_FUNC_INFO << "Adding new Image" << pictureName;
          // delete wallpaperImage;
          Q_EMIT ready(pictureName);
        }
      }
      Q_EMIT ready();
    } else if (info.isFile()) {
      QImage *wallpaperImage = new QImage(info.filePath() + info.fileName());
      d->mThumbData[info.fileName()] = wallpaperImage->scaledToWidth(72);
      d->mPathData[info.fileName()] =
          QDir::toNativeSeparators(info.filePath() + info.fileName());
      delete wallpaperImage;
      Q_EMIT ready(info.fileName());
      Q_EMIT ready();
    }
  } else {

    d->nm = new QNetworkAccessManager(this);
    connect(d->nm, SIGNAL(finished(QNetworkReply *)), this,
            SLOT(onNetworkRequestFinished(QNetworkReply *)));
    qDebug() << Q_FUNC_INFO << "Remote File";
    // remote file.
    d->nm->get(QNetworkRequest(d->mUrl));
  }
}

void AsyncImageLoader::onNetworkRequestFinished(QNetworkReply *reply) {
  if (reply) {
    QByteArray data = reply->readAll();

    QImage *img = new QImage(data);
    if (!img->isNull()) {
      d->mThumbData[reply->url().toString()] = img->scaledToWidth(72);
      d->mPathData[reply->url().toString()] = reply->url().toString();
      Q_EMIT ready(reply->url().toString());
      Q_EMIT ready();
    }

    delete img;
  }

  reply->deleteLater();
}

} // namespace Social
