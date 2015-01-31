#ifndef LINEEDIT_H
#define LINEEDIT_H

#include <plexy.h>
#include <style.h>

#include <QGraphicsObject>
#include <QGraphicsLayoutItem>
#include <plexydesk_ui_exports.h>
#include <desktopwidget.h>

namespace PlexyDesk {
class DECL_UI_KIT_EXPORT LineEdit : public UIWidget {
  Q_OBJECT
  Q_INTERFACES(QGraphicsLayoutItem)

public:
  explicit LineEdit(QGraphicsObject *parent = 0);

  virtual ~LineEdit();

  virtual void setText(const QString &txt);

  virtual QString text() const;

  virtual void style(StylePtr style);

  virtual void setSize(const QSizeF &size);

  virtual QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint) const;

Q_SIGNALS:
  void clicked();
  void submit();
  void text(const QString &text);

protected:
  virtual bool eventFilter(QObject *object, QEvent *event);
  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                     QWidget *widget = 0);
  virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
  virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
  virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
  virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
  virtual void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
  virtual void keyPressEvent(QKeyEvent *event);

private:
  class PrivateLineEdit;
  PrivateLineEdit *const d;
};
}

#endif // LINEEDIT_H
