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
#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <widget.h>

#include <QGraphicsObject>
#include <QGraphicsLayoutItem>
#include <plexydesk_ui_exports.h>

namespace cherry_kit {
class DECL_UI_KIT_EXPORT ImageView : public widget {
  Q_OBJECT
public:
  explicit ImageView(widget *a_parent_ptr = 0);
  virtual ~ImageView();

  virtual void set_size(const QSizeF &a_size);
  virtual QSizeF sizeHint(Qt::SizeHint which,
                          const QSizeF &a_constraint = QSizeF()) const;

  virtual void set_pixmap(const QPixmap &a_pixmap);
  virtual StylePtr style() const;
Q_SIGNALS:
  void mouseOver();
  void mouseReleased();
  void mouseEnter();
  void mouseLeave();

private:
  virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *a_event_ptr);
  virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *a_event_ptr);
  virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *a_event_ptr);
  virtual void hoverMoveEvent(QGraphicsSceneHoverEvent *a_event_ptr);

  virtual void paint_view(QPainter *a_painter_ptr, const QRectF &a_exposeRect);

private:
  class PrivateImageView;
  PrivateImageView *const o_image_view;
};
}
#endif // IMAGEVIEW_H
