#include "imagecelladaptor.h"
#include "imagecell.h"
#include <themepackloader.h>
#include <QHash>

#include <webservice.h>
#include <asyncdatadownloader.h>
#include <asyncimagecreator.h>
#include <asyncimageloader.h>
#include <QFileInfo>
#include <QDir>
#include <plexyconfig.h>

class ResultCache {
public:
  QString id;
  QString url;
  QString size;
  int width;
  int height;
};

class ImageCellAdaptor::PrivateImageCellAdaptor {
public:
  PrivateImageCellAdaptor() {}
  ~PrivateImageCellAdaptor() { mData.clear(); }

  QHash<QString, QImage> mData;
  QMap<QString, QVariantMap> mMetaData;
  QSize mCellSize;
  bool mLablelVisibility;
  int mCompleted;

  QMultiMap<QString, ResultCache> mSizesMap;
  QList<QuetzalSocialKit::AsyncImageCreator *> mCreatorList;
};

ImageCellAdaptor::ImageCellAdaptor(QGraphicsObject *parent)
    : UIKit::TableModel(parent), d(new PrivateImageCellAdaptor) {
  d->mCompleted = 0;
  setCellSize(QSize(96, 96));
}

ImageCellAdaptor::~ImageCellAdaptor() { delete d; }

float ImageCellAdaptor::margin() const { return 0.0; }

float ImageCellAdaptor::padding() const { return 0.0; }

float ImageCellAdaptor::left_margin() const { return 0.0; }

float ImageCellAdaptor::right_margin() const { return 0.0; }

bool ImageCellAdaptor::init() { return true; }

TableModel::TableRenderMode ImageCellAdaptor::render_type() const {
  return ImageCellAdaptor::kRenderAsGridView;
}

void ImageCellAdaptor::addDataItem(const QString &label, const QImage &pixmap,
                                   bool selected, const QVariantMap &metaData) {
  d->mData[label] = pixmap;
  d->mMetaData[label] = metaData;

  ImageCell *item = new ImageCell(
      QRectF(0.0, 0.0, d->mCellSize.width(), d->mCellSize.height()),
      ImageCell::Grid, 0);
  item->setLabelVisibility(d->mLablelVisibility);
  item->addDataItem(pixmap, label, metaData);

  Q_EMIT add(item);
}

void ImageCellAdaptor::setLabelVisibility(bool visibility) {
  d->mLablelVisibility = visibility;
}

void ImageCellAdaptor::setCellSize(const QSize &size) { d->mCellSize = size; }

void ImageCellAdaptor::setSearchQuery(const QString &query, int pageNumber) {}

void ImageCellAdaptor::addPathList(const QStringList &pathList) {
  Q_FOREACH(const QString & str, pathList) {
    qDebug() << Q_FUNC_INFO << str;
    getImageFromPath(str);
  }
}

bool ImageCellAdaptor::hasRunningThreads() {
  Q_FOREACH(const QuetzalSocialKit::AsyncImageCreator * ptr, d->mCreatorList) {
    if (ptr && ptr->isRunning()) {
      return true;
    }
  }

  return false;
}

void ImageCellAdaptor::onImageRady() {
  if (!sender()) {
    return;
  }

  QuetzalSocialKit::AsyncImageCreator *creator =
      qobject_cast<QuetzalSocialKit::AsyncImageCreator *>(sender());

  if (!creator) {
    return;
  }

  qDebug() << Q_FUNC_INFO << creator->imagePath();
  addDataItem(creator->imagePath(), creator->thumbNail());

  creator->quit();
  creator->deleteLater();
  d->mCreatorList.removeOne(creator);

  // delete creator;
  Q_EMIT completed(100);
}

void ImageCellAdaptor::loadLocalImageFiles(QList<QUrl> &list) {}

void ImageCellAdaptor::getImageFromPath(const QString &str) {
  QFileInfo info(str);
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
      QString imageFile = QDir::toNativeSeparators(
          localPictureDir.absolutePath() + "/" + pictureName);
      QuetzalSocialKit::AsyncImageCreator *creator =
          new QuetzalSocialKit::AsyncImageCreator(this);
      creator->setData(imageFile, UIKit::Config::cache_dir());
      connect(creator, SIGNAL(ready()), this, SLOT(onImageRady()));

      d->mCreatorList << creator;
      creator->start();
    }
  }
}
