#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <widget.h>
#include <plexydesk_ui_exports.h>

namespace UIKit {
class DECL_UI_KIT_EXPORT ToolBar : public Widget {
  Q_OBJECT
public:
  ToolBar(QGraphicsObject *a_parent_ptr = 0);
  virtual ~ToolBar();

  virtual void add_action(const QString &a_lable, const QString &a_icon,
                          bool a_togle_action = false);
  virtual void insert_widget(Widget *a_widget_ptr);

  virtual void set_orientation(Qt::Orientation a_orientation);

  virtual void set_icon_resolution(const QString &a_res);
  virtual void set_icon_size(const QSize &a_size);

  virtual StylePtr style() const;

  // virtual void setGeometry(const QRectF &a_rect);
  virtual QRectF contents_geometry() const;
  virtual QRectF frame_geometry() const;
  virtual QSizeF sizeHint(Qt::SizeHint which, const QSizeF &a_constraint) const;

  virtual void on_item_activated(
      std::function<void (const QString &)> a_handler);

Q_SIGNALS:
  void action(const QString &a_actionName);

protected:
  virtual void paint_view(QPainter *a_painter_ptr, const QRectF &a_expose_rect);

  void tool_button_press_handler(const Widget *a_widget_ptr);

private:
  class PrivateToolBar;
  PrivateToolBar *const d;
};
}

#endif // TOOLBAR_H
