#ifndef LABEL_H
#define LABEL_H

#include "desktopwidget.h"

#include <QGraphicsObject>
#include <QGraphicsLayoutItem>

#include <style.h>
#include <plexydesk_ui_exports.h>

namespace UI {

class DECL_UI_KIT_EXPORT Label : public UIWidget {
  Q_OBJECT
  Q_INTERFACES(QGraphicsLayoutItem)

public:
  explicit Label(QGraphicsObject *parent = 0);

  virtual ~Label();

  virtual void setLabel(const QString &txt);

  virtual QString label() const;

  virtual QRectF boundingRect() const;

  virtual void setSize(const QSizeF &size);

  virtual void setFontSize(uint pixelSize);

  virtual QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint) const;

  virtual void setGeometry(const QRectF &rect);

  virtual QRectF contentBoundingRect() const;

  virtual void setLabelStyle(const QColor &backgroundColor,
                             const QColor &textColor);

Q_SIGNALS:
  void clicked();
  void contentBoundingRectChaned();

protected:
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                     QWidget *widget = 0);

private:
  class PrivateLabel;
  PrivateLabel *const d;
};
}

#endif // LABEL_H
