/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.org>
*  Authored By  :
*
*  PlexyDesk is free software: you can redistribute it and/or modify
*  it under the terms of the GNU Lesser General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  PlexyDesk is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU Lesser General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with PlexyDesk. If not, see <http://www.gnu.org/licenses/lgpl.html>
*******************************************************************************/

#ifndef WINDOW_H
#define WINDOW_H

#include <functional>

#include <widget.h>

class QRectF;
class QPointF;
class QSizeF;

namespace CherryKit {
class Space;
class DECL_UI_KIT_EXPORT Window : public Widget {
public:
  typedef enum {
    kFramelessWindow = 1,
    kPanelWindow = 2,
    kApplicationWindow = 3,
    kNotificationWindow = 4,
    kDialogWindow = 5,
    kMessageWindow = 6,
    kPopupWindow = 7
  } WindowType;

  typedef std::function<void(Window *, int, int)> ResizeCallback;

  Window(Widget *a_parent_ptr = 0);
  virtual ~Window();

  virtual void set_window_content(Widget *a_widget_ptr);
  virtual void set_window_viewport(Space *a_space);

  virtual void set_window_title(const QString &a_window_title);
  virtual QString window_title() const;

  virtual WindowType window_type();
  virtual void set_window_type(WindowType a_window_type);

  virtual void on_window_resized(ResizeCallback a_handler);
  virtual void
  on_window_moved(std::function<void(const QPointF &pos)> a_handler);
  virtual void on_window_closed(std::function<void(Window *)> a_handler);
  virtual void on_window_discarded(std::function<void(Window *)> a_handler);
  virtual void on_window_focused(std::function<void(Window *)> a_handler);

  virtual void raise();
  virtual void close();
  virtual void show();
  virtual void hide();
  virtual void discard();

  virtual void resize(float a_width, float a_height);

  virtual void enable_window_background(bool a_visibility = true);

protected:
  virtual void mousePressEvent(QGraphicsSceneMouseEvent *a_event_ptr);
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *a_event_ptr);

  virtual void paint_view(QPainter *a_painter_ptr, const QRectF &a_rect_ptr);

  void invoke_focus_handlers();
  void invoke_window_closed_action();
  void invoke_window_moved_action();

private:
  class PrivateWindow;
  PrivateWindow *const o_window;
};
}
#endif // WINDOW_H
