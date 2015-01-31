#ifndef LOCALWALLPAPERS_H
#define LOCALWALLPAPERS_H

#include <QThread>

class LocalWallPapers : public QThread {
  Q_OBJECT
public:
  explicit LocalWallPapers(QObject *parent = 0);
  virtual ~LocalWallPapers();

  QList<QImage> thumbNails() const;

  QList<QImage> allImages();

  QImage imageByName(const QString &key) const;

  QImage imageThumbByName(const QString &key) const;

  QString filePathFromName(const QString &key) const;

  void run();

signals:
  void ready();
  void ready(const QString &key);

public slots:
  void onDirectoryReady(const QString &);

private:
  class PrivateLocalWallPapers;
  PrivateLocalWallPapers *const d;
};

#endif // LOCALWALLPAPERS_H
