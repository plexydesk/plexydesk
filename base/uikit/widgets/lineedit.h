#ifndef LINEEDIT_H
#define LINEEDIT_H

#include <plexy.h>

#include <style.h>
#include <widget.h>

#include <plexydesk_ui_exports.h>

namespace UIKit
{
class DECL_UI_KIT_EXPORT LineEdit : public Widget
{
  Q_OBJECT
public:
  explicit LineEdit(QGraphicsObject *a_parent_ptr = 0);
  virtual ~LineEdit();

  virtual void set_text(const QString &a_txt);
  virtual QString text() const;

  virtual void style(StylePtr a_style);

  virtual void set_size(const QSizeF &a_size);
  virtual QSizeF sizeHint(Qt::SizeHint which, const QSizeF &a_constraint) const;
Q_SIGNALS:
  void submit();
  void text(const QString &a_text);
protected:
  virtual bool eventFilter(QObject *a_object_ptr, QEvent *a_event_ptr);
  virtual void paint(QPainter *a_painter_ptr, const QStyleOptionGraphicsItem *a_option_ptr,
                     QWidget *a_widget_ptr = 0);
  virtual void mousePressEvent(QGraphicsSceneMouseEvent *a_event_ptr);
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *a_event_ptr);
  virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *a_event_ptr);
  virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *a_event_ptr);
  virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *a_event_ptr);
  virtual void hoverMoveEvent(QGraphicsSceneHoverEvent *a_event_ptr);
  virtual void keyPressEvent(QKeyEvent *a_event_ptr);
private:
  class PrivateLineEdit;
  PrivateLineEdit *const d;
};
}
#endif // LINEEDIT_H
