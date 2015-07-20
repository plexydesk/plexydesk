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
#ifndef PROGRESS_BAR_H
#define PROGRESS_BAR_H

#include <ck_style.h>
#include <ck_widget.h>

#include <plexydesk_ui_exports.h>

namespace cherry_kit {
class DECL_UI_KIT_EXPORT progress_bar : public widget {
public:
  explicit progress_bar(widget *a_parent_ptr = 0);
  virtual ~progress_bar();

  virtual void set_size(const QSizeF &a_size);
  virtual QRectF contents_geometry() const;

  virtual int max_range();
  virtual int min_range();

  void set_range(int a_min, int a_max);

  virtual void set_value(int a_value);
  virtual int value() const;

  void on_value_changed(const QVariant &a_value);

protected:
  virtual void paint_view(QPainter *a_painter_ptr, const QRectF &a_rect);

private:
  class PrivateProgressBar;
  PrivateProgressBar *const priv;
};
}

#endif // PROGRESS_BAR_H
