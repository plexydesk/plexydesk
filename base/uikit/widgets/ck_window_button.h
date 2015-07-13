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

#ifndef WINDOWBUTTON_H
#define WINDOWBUTTON_H

#include <ck_button.h>
#include <plexydesk_ui_exports.h>
#include <ck_widget.h>

namespace cherry_kit {

class DECL_UI_KIT_EXPORT window_button : public cherry_kit::button {
  Q_OBJECT
public:
  typedef enum {
    CLOSE,
    ZOOM,
    MINIMIZE
  } WindowButtonType;

  explicit window_button(widget *a_parent_ptr = 0);
  ~window_button();

  void set_button_type(WindowButtonType a_type);

  virtual QRectF boundingRect() const;

protected:
  virtual void paint_normal_button(QPainter *a_painter_ptr,
                                   const QRectF &a_rect);
  virtual void paint_sunken_button(QPainter *a_painter_ptr,
                                   const QRectF &a_rect);
  virtual void paint_hover_button(QPainter *a_painter, const QRectF &a_rect);

private:
  class PrivateWindowButton;
  PrivateWindowButton *const o_button;
};
} // PlexyDesk
#endif // WINDOWBUTTON_H
