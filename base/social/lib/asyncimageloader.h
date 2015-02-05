#ifndef ASYNC_IMAGE_LOADER_H
#define ASYNC_IMAGE_LOADER_H

#include <QThread>
#include <QNetworkReply>
#include <QuetzalSocialKit_export.h>

namespace QuetzalSocialKit
{

class QuetzalSocialKit_EXPORT AsyncImageLoader : public QThread
{
  Q_OBJECT
public:
  AsyncImageLoader(QObject *parent = 0);

  virtual ~AsyncImageLoader();

  void setUrl(const QUrl &url);

  QList<QImage> thumbNails() const;

  QImage imageThumbByName(const QString &key) const;

  QString filePathFromName(const QString &key) const;

  void run();

Q_SIGNALS:
  void ready();

  void ready(const QString &key);

private Q_SLOTS:
  void onNetworkRequestFinished(QNetworkReply *reply);

private:
  class PrivateAsyncImageLoader;
  PrivateAsyncImageLoader *const d;
};

} // namespace Social
#endif // ASYNC_IMAGE_LOADER_H
