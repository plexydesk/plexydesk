#include <config.h>

#include "photocelladaptor.h"
#include "photocell.h"
#include <resource_manager.h>
#include <QHash>

#include <webservice.h>
#include <asyncdatadownloader.h>
#include <asyncimagecreator.h>
#include <asyncimageloader.h>
#include <plexyconfig.h>

class ResultCache {
public:
  QString id;
  QString url;
  QString size;
  int width;
  int height;
};

class PhotoCellAdaptor::PrivatePhotoCellAdaptor {
public:
  PrivatePhotoCellAdaptor() {}
  ~PrivatePhotoCellAdaptor() { mData.clear(); }

  QHash<QString, QImage> mData;
  QMap<QString, QVariantMap> mMetaData;
  QSize mCellSize;
  bool mLablelVisibility;
  int mCompleted;

  QMultiMap<QString, ResultCache> mSizesMap;
};

PhotoCellAdaptor::PhotoCellAdaptor(QObject *parent)
    : cherry_kit::TableModel(parent), d(new PrivatePhotoCellAdaptor) {
  d->mCompleted = 0;
  setCellSize(QSize(96, 96));
}

PhotoCellAdaptor::~PhotoCellAdaptor() { delete d; }

QList<TableViewItem *> PhotoCellAdaptor::componentList() {
  QList<TableViewItem *> list;
  Q_FOREACH(QString key, d->mData.keys()) {
    PhotoCell *item = new PhotoCell(
        QRectF(0.0, 0.0, d->mCellSize.width(), d->mCellSize.height()),
        PhotoCell::Grid, 0);
    item->setLabelVisibility(d->mLablelVisibility);
    item->addDataItem(d->mData[key], key, d->mMetaData[key]);
    list.append(item);
  }

  return list;
}

float PhotoCellAdaptor::margin() const { return 0.0; }

float PhotoCellAdaptor::padding() const { return 0.0; }

float PhotoCellAdaptor::left_margin() const { return 0.0; }

float PhotoCellAdaptor::right_margin() const { return 0.0; }

bool PhotoCellAdaptor::init() { return true; }

TableModel::TableRenderMode PhotoCellAdaptor::render_type() const {
  return PhotoCellAdaptor::kRenderAsGridView;
}

void PhotoCellAdaptor::addDataItem(const QString &label, const QImage &pixmap,
                                   bool selected, const QVariantMap &metaData) {
  PhotoCell *item = new PhotoCell(
      QRectF(0.0, 0.0, d->mCellSize.width(), d->mCellSize.height()),
      PhotoCell::Grid, 0);

  item->setLabelVisibility(d->mLablelVisibility);
  item->addDataItem(pixmap, label, metaData);

  Q_EMIT add(item);
}

void PhotoCellAdaptor::setLabelVisibility(bool visibility) {
  d->mLablelVisibility = visibility;
}

void PhotoCellAdaptor::setCellSize(const QSize &size) { d->mCellSize = size; }

void PhotoCellAdaptor::setSearchQuery(const QString &query, int pageNumber) {
  Q_EMIT completed(0);
  d->mCompleted = 0;
  d->mData.clear();

  Q_EMIT cleared();

  Q_EMIT progressRange(100);

  QuetzalSocialKit::WebService *service =
      new QuetzalSocialKit::WebService(this);
  service->create("com.flickr.json.api");

  QVariantMap args;
  args["api_key"] = K_SOCIAL_KIT_FLICKR_API_KEY;
  args["text"] = query;
  args["safe_search"] = "1";
  args["tags"] = "wallpaper,wallpapers";
  args["tag_mode"] = "all";
  args["page"] = QString::number(pageNumber);

  service->queryService("flickr.photos.search", args);

  connect(service, SIGNAL(finished(QuetzalSocialKit::WebService *)), this,
          SLOT(onServiceComplete(QuetzalSocialKit::WebService *)));

  Q_EMIT completed(2);
}

QUrl PhotoCellAdaptor::requestImageUrl(const QString &id,
                                       const QString &size) const {
  QUrl rv;
  QMultiMap<QString, ResultCache>::iterator i = d->mSizesMap.find(id);

  while (i != d->mSizesMap.end() && i.key() == id) {
    // rv.append(i.value());
    ResultCache cache = i.value();
    qDebug() << Q_FUNC_INFO << "Size:  " << cache.size << " " << cache.width
             << "x" << cache.height;
    if (cache.id == id && cache.size == size) {
      qDebug() << Q_FUNC_INFO << "Url Found in Cache:  " << cache.url;
      rv = cache.url;
    }
    ++i;
  }

  return rv;
}

QList<QString> PhotoCellAdaptor::availableSizesForImage(const QString &id) {
  QList<QString> rv;

  QMultiMap<QString, ResultCache>::iterator i = d->mSizesMap.find(id);

  while (i != d->mSizesMap.end() && i.key() == id) {
    // rv.append(i.value());
    ResultCache cache = i.value();
    qDebug() << Q_FUNC_INFO << "Size:  " << cache.size << " " << cache.width
             << "x" << cache.height;
    if (cache.id == id) {
      qDebug() << Q_FUNC_INFO << "Url Found in Cache:  " << cache.url;
      rv << cache.size;
    }
    ++i;
  }

  return rv;
}

void
PhotoCellAdaptor::onServiceComplete(QuetzalSocialKit::WebService *service) {
  QList<QVariantMap> photoList = service->methodData("photo");

  if (service->methodData("photos").count() > 0) {
    QString pageCountStr = service->methodData("photos")[0]["pages"].toString();
    bool ok;
    int _pageCount = pageCountStr.toInt(&ok, 10);

    Q_EMIT pageCount(_pageCount);
  }

  foreach(const QVariantMap & map, photoList) {
    QuetzalSocialKit::WebService *service =
        new QuetzalSocialKit::WebService(this);

    service->create("com.flickr.json.api");

    QVariantMap args;
    args["api_key"] = K_SOCIAL_KIT_FLICKR_API_KEY;
    args["photo_id"] = map["id"].toString();
    service->queryService("flickr.photos.getSizes", args);

    connect(service, SIGNAL(finished(QuetzalSocialKit::WebService *)), this,
            SLOT(onSizeServiceComplete(QuetzalSocialKit::WebService *)));
  }

  service->deleteLater();
  ;
}

void
PhotoCellAdaptor::onSizeServiceComplete(QuetzalSocialKit::WebService *service) {
  if (service->methodData("sizes").count() == 0) {
    Q_EMIT completed(10);
    service->deleteLater();
    return;
  }

  QString canDownloadStr =
      service->methodData("sizes")[0]["candownload"].toString();
  bool ok;
  bool canDownload = canDownloadStr.toInt(&ok, 10);

  if (!canDownload) {
    service->deleteLater();
    return;
  }

  Q_FOREACH(const QVariantMap & map, service->methodData("size")) {
    if (map["label"].toString() == "Large Square") {
      QuetzalSocialKit::AsyncDataDownloader *downloader =
          new QuetzalSocialKit::AsyncDataDownloader(this);
      QVariantMap metaData;
      metaData["method"] = service->methodName();
      metaData["id"] =
          service->inputArgumentForMethod(service->methodName())["photo_id"];
      metaData["data"] = service->inputArgumentForMethod(service->methodName());

      downloader->setMetaData(metaData);
      downloader->setUrl(map["source"].toString());
      connect(downloader, SIGNAL(ready()), this, SLOT(onImageReady()));
    }

    ResultCache cache;
    cache.id = service->inputArgumentForMethod(
                            service->methodName())["photo_id"].toString();
    cache.size = map["label"].toString();
    cache.url = map["source"].toString();

    QString heightStr = map["height"].toString();
    QString widthStr = map["width"].toString();

    cache.height = heightStr.toInt();
    cache.width = widthStr.toInt();

    d->mSizesMap.insert(cache.id, cache);
  }

  Q_EMIT completed(20);

  service->deleteLater();
}

void PhotoCellAdaptor::onImageReady() {
  QuetzalSocialKit::AsyncDataDownloader *downloader =
      qobject_cast<QuetzalSocialKit::AsyncDataDownloader *>(sender());

  if (downloader) {
    QuetzalSocialKit::AsyncImageCreator *imageSave =
        new QuetzalSocialKit::AsyncImageCreator(this);

    connect(imageSave, SIGNAL(ready()), this, SLOT(onImageSaveReady()));

    imageSave->setMetaData(downloader->metaData());
    imageSave->setData(downloader->data(), cherry_kit::Config::cache_dir(),
                       false);
    imageSave->start();
    downloader->deleteLater();

    Q_EMIT completed(30);
    d->mCompleted++;
  }
}

void PhotoCellAdaptor::onImageSaveReady() {
  QuetzalSocialKit::AsyncImageCreator *c =
      qobject_cast<QuetzalSocialKit::AsyncImageCreator *>(sender());

  if (c) {
    // qDebug() << Q_FUNC_INFO << "File Saved to: " << c->imagePath();
    // qDebug() << Q_FUNC_INFO << c->metaData()["id"] << ": " << "File Details:
    // " << c->image();
    addDataItem(c->metaData()["id"].toString(), c->thumbNail(), false,
                c->metaData());
    c->quit();
    c->deleteLater();
    Q_EMIT completed(100);
  }
}
