#include "localwallpapers.h"
#include <QDebug>
#include <QDir>
#include <QPixmap>
#include <QImage>
#include <QString>

class LocalWallPapers::PrivateLocalWallPapers {
public:
  PrivateLocalWallPapers() {}
  ~PrivateLocalWallPapers() {
    mData.clear();
    mThumbData.clear();
    mPathData.clear();
  }

  QMap<QString, QImage> mData;
  QMap<QString, QImage> mThumbData;
  QMap<QString, QString> mPathData;
};

LocalWallPapers::LocalWallPapers(QObject *parent)
    : QThread(parent), d(new PrivateLocalWallPapers) {}

LocalWallPapers::~LocalWallPapers() {
  qDebug() << Q_FUNC_INFO;
  delete d;
}

QList<QImage> LocalWallPapers::thumbNails() const { return d->mData.values(); }

QList<QImage> LocalWallPapers::allImages() { return d->mData.values(); }

QImage LocalWallPapers::imageByName(const QString &key) const {
  return d->mData[key];
}

QImage LocalWallPapers::imageThumbByName(const QString &key) const {
  return d->mThumbData[key];
}

QString LocalWallPapers::filePathFromName(const QString &key) const {
  return d->mPathData[key];
}

void LocalWallPapers::run() {
  qDebug() << Q_FUNC_INFO << "New Thread";
  QList<QPixmap> rv;
#ifdef Q_OS_MAC
  QDir localPictureDir("/Library/Desktop Pictures");
#endif

#ifdef Q_OS_LINUX
  QDir localPictureDir("/usr/share/backgrounds");
#endif

#ifdef Q_OS_WINDOWS
  QDir localPictureDir(QDir::toNativeSeparators(QDir::rootPath() + QString("Web\Wallpaper"));
#endif

                       QStringList filters;
                       filters << "*.png" << "*.jpg" << "*.jpeg" << "*.tiff" << "*.svg";

                       localPictureDir.setNameFilters(filters);

                       QStringList localPictureList = localPictureDir.entryList();

  Q_FOREACH(const QString & pictureName, localPictureList) {
    QImage *wallpaperImage = new QImage(QDir::toNativeSeparators(
                                          localPictureDir.absolutePath() + "/" + pictureName));
    if (!wallpaperImage->isNull()) {
      // d->mData[pictureName] = wallpaperImage;
      d->mThumbData[pictureName] = wallpaperImage->scaledToWidth(72);
      d->mPathData[pictureName] = QDir::toNativeSeparators(
                                    localPictureDir.absolutePath() + "/" + pictureName);
      delete wallpaperImage;
      Q_EMIT ready(pictureName);
    }
  }
  Q_EMIT ready();
}

void LocalWallPapers::onDirectoryReady(const QString &) {}
