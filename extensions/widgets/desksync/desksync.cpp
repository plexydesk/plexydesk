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
#include "desksync.h"
#include <ck_config.h>

#include <ck_sync_object.h>
#include <ck_disk_engine.h>
#include <ck_data_sync.h>
#include <ck_url.h>
#include <ck_remote_service.h>

class desksync_controller::Privatedesksync {
public:
  Privatedesksync() {}
  ~Privatedesksync() {}
};

desksync_controller::desksync_controller(QObject *object)
    : cherry_kit::desktop_controller_interface(object),
      priv(new Privatedesksync) {}

desksync_controller::~desksync_controller() { delete priv; }

void desksync_controller::init() {}

void desksync_controller::session_data_ready(
    const cherry_kit::sync_object &a_session_root) {
}

void desksync_controller::submit_session_data(cherry_kit::sync_object *a_obj) {}

void desksync_controller::set_view_rect(const QRectF &a_rect) {
  if (a_rect.width() < 1 && a_rect.height() < 1) {
    return;
  }

  cherry_kit::data_sync *sync = new cherry_kit::data_sync("Global");
  cherry_kit::disk_engine *engine = new cherry_kit::disk_engine();

  sync->set_sync_engine(engine);

  sync->on_object_found([&](cherry_kit::sync_object &a_object,
                            const std::string &a_app_name, bool a_found) {

    if (!a_found) {
        show_auth_dialog();
    }

    delete sync;
  });

  sync->find("social", "service_name", "plexydesk");
}

bool desksync_controller::remove_widget(cherry_kit::widget *a_widget_ptr) {
    return false;
}

void desksync_controller::show_auth_dialog() {
  cherry_kit::desktop_dialog_ref ck_activity =
          viewport()->create_activity("auth_dialog");

  ck_activity->on_notify([=](const std::string &key, const std::string &value) {
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
  });
}


void desksync_controller::request_pixabay_key(const std::string &a_token) {
    social_kit::url_request *request = new social_kit::url_request();

    request->on_response_ready([&](const social_kit::url_response &response) {

        if(response.status_code() != 200)
            return;

        social_kit::remote_service srv_query("org.plexydesk.api.xml");

        /* get access token */
        const social_kit::remote_result result =
                srv_query.response("plexydesk.pixabay.token", response);

        social_kit::remote_result_data token_type = result.get("key_id").at(0);
        social_kit::remote_result_data token_value =
                result.get("client_token").at(0);

        std::string web_token = token_value.get("client_token").value();

        cherry_kit::sync_object *obj = new cherry_kit::sync_object();
        cherry_kit::data_sync *sync = new cherry_kit::data_sync("Global");
        cherry_kit::disk_engine *engine = new cherry_kit::disk_engine();

        sync->set_sync_engine(engine);

        obj->set_name("social");

        obj->set_property("service_name", "pixabay");
        obj->set_property("token", web_token);

        sync->add_object(*obj);

        delete sync;
    });

  /* service data */
  social_kit::url_request_context input_data;

  input_data.add_header("Authorization", "Bearer " + a_token);
  input_data.set_mime_type(
      social_kit::url_request_context::kMimeTypeUrlEncoded);

  request->submit(social_kit::url_request::kPOSTRequest,
                  "https://plexydesk.org/api/social/pixabay/token/",
                  input_data);
}
