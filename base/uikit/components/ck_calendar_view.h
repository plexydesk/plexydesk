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
#ifndef CALENDARWIDGET_H
#define CALENDARWIDGET_H

#include <ck_widget.h>
#include <ck_label.h>

namespace cherry_kit {
class widget;

class DECL_UI_KIT_EXPORT calendar_view : public cherry_kit::widget {
public:
  explicit calendar_view(widget *a_parent_ptr = 0);
  virtual ~calendar_view();

  virtual void set_date(const QDate &selected_date);
  virtual QDate selected_date() const;

  virtual void set_view_geometry(float a_x, float a_y, float a_width,
                            float a_height);
  virtual QRectF geometry() const;

  virtual void next();
  virtual void previous();

  void clear_selection();

 protected:
  virtual void clear();
  virtual void reset();
  void next_view(label *ck_year_label);
  void previous_view(label *ck_year_label);
  void reset_view(label *ck_year_label);

  virtual void paint_view(QPainter *painter, const QRectF &rect);

private:
  class PrivateCalendarWidget;
  PrivateCalendarWidget *const priv;
};
}

#endif // CALENDARWIDGET_H
