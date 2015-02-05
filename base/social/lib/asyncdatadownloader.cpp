#include "asyncdatadownloader.h"

#include <QNetworkAccessManager>
#include <QImage>
#include <QVariantMap>
#include <QDebug>

namespace QuetzalSocialKit
{

class AsyncDataDownloader::PrivateAsyncDataDownloader
{
public:
  PrivateAsyncDataDownloader() {}
  ~PrivateAsyncDataDownloader() {}

  QByteArray mData;
  QNetworkAccessManager *mNetworkAccessManager;
  QVariantMap mMetaData;
};

AsyncDataDownloader::AsyncDataDownloader(QObject *parent)
  : QObject(parent), d(new PrivateAsyncDataDownloader)
{
  d->mNetworkAccessManager = new QNetworkAccessManager(this);

  connect(d->mNetworkAccessManager, SIGNAL(finished(QNetworkReply *)), this,
          SLOT(onDownloadComplete(QNetworkReply *)));
}

AsyncDataDownloader::~AsyncDataDownloader() { delete d; }

void AsyncDataDownloader::setMetaData(const QVariantMap &metaData)
{
  d->mMetaData = metaData;
}

QVariantMap AsyncDataDownloader::metaData() const { return d->mMetaData; }

void AsyncDataDownloader::setUrl(const QUrl &url)
{
  QNetworkRequest request(url);

  QNetworkReply *reply = d->mNetworkAccessManager->get(request);

  connect(reply, SIGNAL(downloadProgress(qint64, qint64)), this,
          SLOT(onDownloadProgress(qint64, qint64)));
}

QByteArray AsyncDataDownloader::data() const { return d->mData; }

void AsyncDataDownloader::onDownloadComplete(QNetworkReply *reply)
{
  if (reply->error() != QNetworkReply::NoError) {
    qDebug() << Q_FUNC_INFO << "Network Error" << reply->errorString();
  }
  d->mData = reply->readAll();
  reply->deleteLater();
  Q_EMIT ready();
}

void AsyncDataDownloader::onDownloadProgress(qint64 current, qint64 total)
{
  float _progress = ((float)current / (float)total) * 100.0;
  qDebug() << "Download in Progress : " << _progress;
  Q_EMIT progress(_progress);
}
}
