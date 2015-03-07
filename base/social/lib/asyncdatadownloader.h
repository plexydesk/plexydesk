#ifndef ASYNCDATADOWNLOADER_H
#define ASYNCDATADOWNLOADER_H

#include <QObject>
#include <QNetworkReply>
#include <QuetzalSocialKit_export.h>

namespace QuetzalSocialKit
{

class QuetzalSocialKit_EXPORT AsyncDataDownloader : public QObject
{
  Q_OBJECT
public:
  explicit AsyncDataDownloader(QObject *a_parent_ptr = 0);
  virtual ~AsyncDataDownloader();

  void setMetaData(const QVariantMap &metaData);

  QVariantMap metaData() const;

  void setUrl(const QUrl &url);

  QByteArray data() const;

Q_SIGNALS:
  void progress(float progress);
  void ready();

private Q_SLOTS:
  void onDownloadComplete(QNetworkReply *reply);
  void onDownloadProgress(qint64, qint64);

private:
  class PrivateAsyncDataDownloader;
  PrivateAsyncDataDownloader *const d;
};
}

#endif // ASYNCDATADOWNLOADER_H
