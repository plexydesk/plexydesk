#ifndef ASYNCIMAGECREATOR_H
#define ASYNCIMAGECREATOR_H

#include <QThread>
#include <QImage>
#include <QuetzalSocialKit_export.h>

#include <functional>
#include <string>

namespace QuetzalSocialKit {

class QuetzalSocialKit_EXPORT AsyncImageCreator : public QThread {
  Q_OBJECT
public:
  explicit AsyncImageCreator(QObject *a_parent_ptr = 0);
  virtual ~AsyncImageCreator();

  void setMetaData(const QVariantMap &data);

  QVariantMap metaData() const;

  void setData(const QString &path, const QString &prefix, bool save = false);
  void setData(const QByteArray &data, const QString &path, bool save = false);
  void setData(const QImage &data, const QString &path, bool save = false);

  void setCrop(const QRectF &cropRect);
  void setScaleToHeight(int height);
  void setScaleToWidth(int width);

  void setThumbNailSize(const QSize &size);
  QImage thumbNail() const;

  QImage image() const;
  QString imagePath() const;

  bool offline() const;

  QByteArray imageToByteArray(const QImage &img) const;

  virtual void on_task_complete(
          std::function<void (AsyncImageCreator *)> a_callback);

Q_SIGNALS:
  void ready();

private Q_SLOTS:
  void on_finished();

protected:
  void run();

private:
  QImage genThumbNail(const QImage &img) const;

  class PrivateAsyncImageCreator;
  PrivateAsyncImageCreator *const d;
};
}

#endif // ASYNCIMAGECREATOR_H
