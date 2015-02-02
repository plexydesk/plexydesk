#ifndef PHOTOWIDGET_H
#define PHOTOWIDGET_H

#include <plexy.h>
#include <desktopwidget.h>
#include <QGraphicsDropShadowEffect>

class PhotoWidget : public UI::UIWidget {
  Q_OBJECT

public:
  PhotoWidget(QGraphicsObject *parent = 0);

  virtual ~PhotoWidget();

  void setContentImage(const QPixmap &pixmap);

  void setPhotoURL(const QString &name);

  QString photoURL() const;

  bool validPhotoFrame();

private:
  QRectF scaledDockRect(const QPixmap &pixmap) const;

  // virtual void paintRotatedView(QPainter *painter, const QRectF &rect);

  virtual void paintView(QPainter *painter, const QRectF &rect);

  // virtual void paintMinimizedView(QPainter *painter, const QRectF &rect);

  class PrivatePhotoFrame;
  PrivatePhotoFrame *const d;

  QPixmap mContentPixmap;
  QPixmap mFramePixmap;
  QPixmap mFrameContentPixmap;
  bool mValid;
};

#endif // PHOTOWIDGET_H
