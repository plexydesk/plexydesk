/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.com>
*  Authored By  : *
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
#include "template.h"
#include <ck_config.h>

class template_controller::PrivateTemplate {
public:
  PrivateTemplate() {}
  ~PrivateTemplate() {}
};

template_controller::template_controller(QObject *object)
    : cherry_kit::desktop_controller_interface(object),
      priv(new PrivateTemplate) {}

template_controller::~template_controller() { delete priv; }

void template_controller::init() {}

void template_controller::session_data_ready(
    const cherry_kit::sync_object &a_session_root) {}

void template_controller::submit_session_data(cherry_kit::sync_object *a_obj) {}

void template_controller::set_view_rect(const QRectF &a_rect) {}

bool template_controller::remove_widget(cherry_kit::widget *a_widget_ptr) {
  return false;
}

cherry_kit::ActionList template_controller::actions() const {
  return cherry_kit::ActionList();
}

void template_controller::request_action(const QString &a_name,
                                            const QVariantMap &a_args) {}
