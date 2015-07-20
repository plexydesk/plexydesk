#ifndef SCROLLBAR_H
#define SCROLLBAR_H

#include <ck_widget.h>
#include <plexydesk_ui_exports.h>

namespace cherry_kit {
typedef std::function<void(int a_move_by)> value_changed_func_t;

class DECL_UI_KIT_EXPORT scrollbar : public widget {
public:
  scrollbar(widget *a_parent_ptr = 0);
  virtual ~scrollbar();

  virtual void set_size(const QSizeF &a_size);
  virtual QRectF contents_geometry() const;
  virtual void set_page_step(int a_step);
  virtual void set_maximum_value(int a_value);

  virtual void set_auto_hide(bool a_state);

  virtual void on_value_changed(value_changed_func_t callback);

protected:
  virtual float scale_factor();

  virtual void paint_view(QPainter *ctx, const QRectF &expose_rect);
  virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
  virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
  virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
  virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

  virtual void invoke_value_changed_callback(int a_value);

private:
  void move_up(int direction);
  void move_slider(bool a_progress_front, int a_distance);

  void update_slider_geometry();

  class private_scrollbar;
  private_scrollbar *const priv;
};
}

#endif // SCROLLBAR_H
