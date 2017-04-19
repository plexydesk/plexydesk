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
#include "deskcontrol.h"
#include <ck_config.h>

class deskcontrol_controller::Privatedeskcontrol {
public:
  Privatedeskcontrol() {}
  ~Privatedeskcontrol() {}
};

deskcontrol_controller::deskcontrol_controller(QObject *object)
    : cherry_kit::desktop_controller_interface(object),
      priv(new Privatedeskcontrol) {}

deskcontrol_controller::~deskcontrol_controller() { delete priv; }

void deskcontrol_controller::init() {}

void deskcontrol_controller::session_data_ready(
    const cherry_kit::sync_object &a_session_root) {}

void deskcontrol_controller::submit_session_data(cherry_kit::sync_object *a_obj) {}

void deskcontrol_controller::set_view_rect(const QRectF &a_rect) {
  if (a_rect.width() < 1 && a_rect.height() < 1) {
    return;
  }

  show_style_ui();
}

bool deskcontrol_controller::remove_widget(cherry_kit::widget *a_widget_ptr) {
  return false;
}

void deskcontrol_controller::show_style_ui() {
  cherry_kit::desktop_dialog_ref ck_activity =
          viewport()->create_activity("style_dialog");

  ck_activity->on_notify([=](const std::string &key, const std::string &value) {
        /*
        cherry_kit::sync_object *obj = new cherry_kit::sync_object();
        cherry_kit::data_sync *sync = new cherry_kit::data_sync("Global");
        cherry_kit::disk_engine *engine = new cherry_kit::disk_engine();

        sync->set_sync_engine(engine);

        obj->set_name("social");

        obj->set_property("service_name", "plexydesk");
        obj->set_property(key, value);

        sync->add_object(*obj);

        delete sync;

        request_pixabay_key(value);
        */
  });
}
