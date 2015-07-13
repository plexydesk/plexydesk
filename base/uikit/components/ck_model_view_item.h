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
#ifndef MODELVIEWITEM_H
#define MODELVIEWITEM_H

#include <functional>

#include <ck_widget.h>

namespace cherry_kit {
class DECL_UI_KIT_EXPORT model_view_item {
public:
  explicit model_view_item();
  virtual ~model_view_item();

  virtual void set_view(widget *a_widget);
  virtual void remove_view();
  virtual widget *view() const;

  virtual void set_data(const QString &a_key, const QVariant &a_value);
  virtual QVariant data(const QString &a_key) const;

  virtual void set_index(int a_index);
  virtual int index() const;

  virtual bool is_a_match(const QString &a_keyword);

  virtual void on_activated(std::function<void(model_view_item *)> a_handler);
  virtual void
  on_view_removed(std::function<void(model_view_item *)> a_handler);
  virtual void
  on_filter(std::function<bool(const widget *, const QString &)> a_handler);

private:
  class PrivateModelViewItem;
  PrivateModelViewItem *const o_model_view_item;
};
}

#endif // MODELVIEWITEM_H
