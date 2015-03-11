#ifndef WINDOW_H
#define WINDOW_H

#include <functional>
#include <widget.h>

class QRectF;
class QPointF;
class QSizeF;

namespace UIKit
{
class Space;
class Window : public Widget
{
public:
  typedef enum {
    kFramelessWindow,
    kPanelWindow,
    kApplicationWindow,
    kNotificationWindow,
    kDialogWindow,
    kMessageWindow,
    kPopupWindow
  } WindowType;

  Window(QGraphicsObject *a_parent_ptr = 0);
  virtual ~Window();

  virtual void set_window_content(Widget *a_widget_ptr);
  virtual void set_window_viewport(Space *a_space);

  virtual void set_window_title(const QString &a_window_title);
  virtual QString window_title() const;

  virtual WindowType window_type();
  virtual void set_window_type(WindowType a_window_type);

  virtual void on_window_resized(
    std::function<void (const QSizeF &size)> a_handler);
  virtual void on_window_moved(
    std::function<void (const QPointF &pos)> a_handler);
  virtual void on_window_closed(
    std::function<void (Window *)> a_handler);
  virtual void on_window_discarded(
    std::function<void (Window *)> a_handler);

  virtual void show();
  virtual void hide();
  virtual void discard();

  virtual void enable_window_background(bool a_visibility = true);
protected:
  virtual void mousePressEvent(QGraphicsSceneMouseEvent *a_event_ptr);
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *a_event_ptr);

  virtual void paint_view(QPainter *a_painter_ptr, const QRectF &a_rect_ptr);

private:
  class PrivateWindow;
  PrivateWindow *const m_priv_impl;
};
}
#endif // WINDOW_H
