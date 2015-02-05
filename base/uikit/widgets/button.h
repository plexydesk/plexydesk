#ifndef UI_BUTTON_H
#define UI_BUTTON_H

#include <plexy.h>
#include <QGraphicsScene>
#include <QGraphicsObject>
#include <QGraphicsLayoutItem>

#include <widget.h>
#include <widget.h>
#include <style.h>
#include <plexydesk_ui_exports.h>

namespace UI
{

class DECL_UI_KIT_EXPORT Button : public Widget
{
  Q_OBJECT

public:
  explicit Button(QGraphicsObject *parent = 0);

  virtual ~Button();

  virtual void setLabel(const QString &txt);

  virtual QString label() const;

  virtual void setIcon(const QImage &img);

  virtual StylePtr style() const;

  virtual void setSize(const QSize &size);

  virtual QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint) const;

  void setActionData(const QVariant &data);

  QVariant actionData() const;

Q_SIGNALS:
  void clicked();

protected:
  virtual void paintView(QPainter *painter, const QRectF &rect);

  virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);

  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

  virtual void paintNormalButton(QPainter *painter, const QRectF &rect);

  virtual void paintSunkenButton(QPainter *painter, const QRectF &rect);

private:
  class PrivateButton;
  PrivateButton *const d;
};
}
#endif // UI_BUTTON_H
