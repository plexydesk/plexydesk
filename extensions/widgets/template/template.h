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
#ifndef TEMPLATE_DATA_H
#define TEMPLATE_DATA_H

#include <view_controller.h>

class TemplateControllerImpl : public CherryKit::ViewController {
public:
  explicit TemplateControllerImpl(QObject *object = 0);
  virtual ~TemplateControllerImpl();

  void init();

  void session_data_available(const cherry::sync_object &a_session_root);
  void submit_session_data(cherry::sync_object *a_obj);

  void set_view_rect(const QRectF &a_rect);

  bool remove_widget(CherryKit::Widget *a_widget_ptr);

  CherryKit::ActionList actions() const;
  void request_action(const QString &a_name, const QVariantMap &a_args);

  QString icon() const;

private:
  class PrivateTemplate;
  PrivateTemplate *const o_view_controller;
};
#endif
