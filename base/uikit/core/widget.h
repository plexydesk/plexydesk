/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.com>
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
#ifndef PD_CORE_WIDGET_H
#define PD_CORE_WIDGET_H

#include <plexy.h>
#include <style.h>
#include <plexydesk_ui_exports.h>

#include <functional>

#include <QGraphicsLayoutItem>
#include <QGraphicsObject>

namespace CherryKit {
/*
 * @brief The Base Class for All types of Desktop Widgets.
 */
class ViewController;
typedef unsigned char *GraphicsSurface;
typedef std::function<void(const Widget *)> UpdateCallback;
typedef std::vector<Widget *> WidgetList;

class DECL_UI_KIT_EXPORT Widget : public QGraphicsObject,
                                  public QGraphicsLayoutItem {
  Q_OBJECT
  Q_INTERFACES(QGraphicsLayoutItem)
public:
  typedef enum {
    kRenderBackground = 1ul << 0,
    kRenderDropShadow = 1ul << 1,
    kDockWindowType = 1ul << 2,
    kTopLevelWindow = 1ul << 3,
    kConvertToWindowType = 1ul << 4,
    kRenderWindowTitle = 1ul << 5
  } WidgetFlags;

  typedef enum {
    kMousePressedEvent = 1ul << 0,
    kMouseReleaseEvent = 1ul << 1,
    kFocusOutEvent = 1ul << 2,
    kFocusInEvent = 1ul << 3,
    kKeyPressEvent = 1ul << 4
  } InputEvent;

  typedef enum {
    kRenderAtBackgroundLevel,
    kRenderAtForgroundLevel
  } RenderLevel;

  typedef std::function<void(InputEvent, const Widget *)> InputCallback;

  Widget(Widget *a_parent_ptr = 0);
  virtual ~Widget();

  virtual void setGeometry(const QRectF &a_rect);
  virtual QRectF boundingRect() const;

  virtual void set_controller(ViewController *a_view_controller_ptr);
  virtual ViewController *controller() const;

  virtual void set_widget_name(const QString &a_name);
  virtual QString label() const;

  virtual void set_widget_id(unsigned int a_id);
  virtual unsigned widget_id() const;

  virtual StylePtr style() const;

  virtual RenderLevel layer_type() const;
  virtual void set_layer_type(RenderLevel a_level);

  virtual void set_widget_flag(int a_flags, bool a_enable = true);

  virtual void on_input_event(std::function<
      void(InputEvent a_type, const Widget *a_widget_ptr)> a_callback);

  virtual void
  on_geometry_changed(std::function<void(const QRectF &)> a_callback);

  virtual void set_style_attribute(const QString &a_key, QVariant a_data);
  virtual QVariant style_attribute(const QString &a_key);

  // experimental.
  virtual void draw();
  virtual void render(unsigned char **a_ctx);
  virtual GraphicsSurface *surface();
  virtual void request_update();
  virtual void on_update(UpdateCallback a_callback);
  virtual WidgetList children();

protected:
  virtual void paint_view(QPainter *a_painter_ptr, const QRectF &a_rect);
  virtual void paint(QPainter *a_painter_ptr,
                     const QStyleOptionGraphicsItem *a_option_ptr,
                     QWidget *a_widget_ptr = 0);

  virtual QSizeF sizeHint(Qt::SizeHint a_which,
                          const QSizeF &a_constraint = QSizeF()) const;

  virtual void focusOutEvent(QFocusEvent *a_event_ptr);
  virtual void mousePressEvent(QGraphicsSceneMouseEvent *a_event_ptr);
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *a_event_ptr);

  virtual float scale_factor_for_width() const;
  virtual float scale_factor_for_height() const;
  virtual void set_child_widet_visibility(bool a_visibility);

private:
  class PrivateWidget;
  PrivateWidget *const p_widget;
};
} //

#endif
