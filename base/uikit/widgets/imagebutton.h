#ifndef IMAGEBUTTON_H
#define IMAGEBUTTON_H

#include <plexydesk_ui_exports.h>
#include <QGraphicsLayoutItem>
#include <QGraphicsObject>
#include <widget.h>

namespace UIKit
{

class DECL_UI_KIT_EXPORT ImageButton : public Widget
{
  Q_OBJECT
public:
  explicit ImageButton(QGraphicsObject *parent = 0);
  virtual ~ImageButton();

  virtual void setSize(const QSize &size);
  virtual QSizeF sizeHint(Qt::SizeHint which,
                          const QSizeF &constraint = QSizeF()) const;

  virtual void setPixmap(const QPixmap &pixmap);

  virtual void setLable(const QString &text);
  virtual QString label() const;

  void setBackgroundColor(const QColor &color);
  virtual StylePtr style() const;

Q_SIGNALS:
  void selected(bool);
private Q_SLOTS:
  void onZoomDone();
  void onZoomOutDone();
  void createZoomAnimation();

protected:
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
  virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
  virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
  virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
  virtual void paintView(QPainter *painter, const QRectF &rect);

private:
  class PrivateImageButton;
  PrivateImageButton *const d;
};
}

#endif // IMAGEBUTTON_H
