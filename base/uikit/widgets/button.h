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
#ifndef UI_BUTTON_H
#define UI_BUTTON_H

#include <plexy.h>

#include <QGraphicsScene>

#include <functional>

#include <widget.h>
#include <style.h>

#include <plexydesk_ui_exports.h>

namespace CherryKit {
class DECL_UI_KIT_EXPORT Button : public Widget {
  Q_OBJECT
public:
  explicit Button(Widget *a_parent_ptr = 0);
  virtual ~Button();

  virtual void set_label(const QString &a_txt);
  virtual QString label() const;

  virtual void setIcon(const QImage &a_img);

  virtual StylePtr style() const;

  virtual void set_size(const QSizeF &a_size);
  virtual QSizeF sizeHint(Qt::SizeHint which, const QSizeF &a_constraint) const;
  virtual QRectF boundingRect() const;
  virtual void setGeometry(const QRectF &a_rect);

  void set_action_data(const QVariant &a_data);
  QVariant action_data() const;

  virtual void on_button_pressed(std::function<void()> a_handler);

protected:
  virtual void mousePressEvent(QGraphicsSceneMouseEvent *a_event_ptr);
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *a_event_ptr);
  virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *a_event_ptr);
  virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *a_event_ptr);

  virtual void paint_view(QPainter *a_painter_ptr, const QRectF &a_rect);
  virtual void paint_normal_button(QPainter *a_painter_ptr,
                                   const QRectF &a_rect);
  virtual void paint_sunken_button(QPainter *a_painter, const QRectF &a_rect);
  virtual void paint_hover_button(QPainter *a_painter, const QRectF &a_rect);

private:
  class PrivateButton;
  PrivateButton *const o_button;
};
}
#endif // UI_BUTTON_H
