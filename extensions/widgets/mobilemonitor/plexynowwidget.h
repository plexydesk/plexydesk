#ifndef PLEXYNOW_WIDGET_H
#define PLEXYNOW_WIDGET_H

#include <plexy.h>
#include <desktopwidget.h>
#include <QGraphicsDropShadowEffect>

class PlexyNowWidget : public PlexyDesk::DesktopWidget {
  Q_OBJECT
public:
  explicit PlexyNowWidget(const QRectF &rect);
  virtual ~PlexyNowWidget();

  void setContentImage(const QPixmap &pixmap);

Q_SIGNALS:
  void approvedToken(const QString &token);

public
Q_SLOTS:
  void onClicked();

private:
  virtual void paintRotatedView(QPainter *painter, const QRectF &rect);
  virtual void paintFrontView(QPainter *painter, const QRectF &rect);
  virtual void paintDockView(QPainter *painter, const QRectF &rect);

  QPixmap mContentPixmap;
  QPixmap mFramePixmap;
  QPixmap mFrameContentPixmap;
  class PrivatePhotoWidget;
  PrivatePhotoWidget *const d;
};

#endif // PLEXYNOW_WIDGET_H
