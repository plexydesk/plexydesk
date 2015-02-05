#ifndef QMLPIXMAPPROVIDER_H
#define QMLPIXMAPPROVIDER_H

#include <QtDeclarative/QDeclarativeImageProvider>
#include <PlexyDeskUICore_export.h>

class PlexyDeskUICore_EXPORT QmlPixmapProvider
  : public QDeclarativeImageProvider
{
public:
  QmlPixmapProvider(QDeclarativeImageProvider::ImageType type =
                      QDeclarativeImageProvider::Pixmap);
  virtual ~QmlPixmapProvider();

  QPixmap requestPixmap(const QString &id, QSize *size,
                        const QSize &requestedSize);

private:
  class Private;
  Private *const d;
};

#endif // QMLPIXMAPPROVIDER_H
