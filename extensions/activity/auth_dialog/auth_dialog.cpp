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
#include "auth_dialog.h"

#include <ck_config.h>

#include <ck_widget.h>
#include <ck_button.h>
#include <ck_text_editor.h>
#include <ck_line_edit.h>
#include <ck_fixed_layout.h>
#include <ck_image_view.h>
#include <ck_progress_bar.h>
#include <ck_resource_manager.h>

#include <ck_remote_service.h>
#include <webservice.h>
#include <iostream>

#include <ck_image_io.h>
#include <ck_url.h>

#include <config.h>


class auth_dialog::Privateauth {
public:
  Privateauth() {}
  ~Privateauth() {}

  cherry_kit::window *m_main_window;
  cherry_kit::fixed_layout *m_layout;
  cherry_kit::button *m_done_btn;
  cherry_kit::button *m_register_btn;
  cherry_kit::line_edit *m_username;
  cherry_kit::line_edit *m_password;
  cherry_kit::image_view *m_logo;

  cherry_kit::progress_bar *m_progress_widget;
  cherry_kit::window *m_progress_window;
};

auth_dialog::auth_dialog(QGraphicsObject *object)
    : cherry_kit::desktop_dialog(object),
      priv(new Privateauth) {}

auth_dialog::~auth_dialog() { delete priv; }

void auth_dialog::create_window() {
  QRectF window_geometry(0, 0, 320, 500);
  priv->m_main_window = new cherry_kit::window();

  priv->m_main_window->set_widget_flag(
      cherry_kit::widget::kRenderBackground);
  priv->m_main_window->set_widget_flag(
      cherry_kit::widget::kConvertToWindowType);
  priv->m_main_window->set_widget_flag(
      cherry_kit::widget::kRenderDropShadow);

  priv->m_main_window->set_window_title("plexydesk.org account login");

  /* create the layout */
  priv->m_layout = new cherry_kit::fixed_layout(priv->m_main_window);
  priv->m_layout->set_content_margin(0, 0, 2, 0);
  priv->m_layout->set_geometry(0, 0, window_geometry.width(),
                               window_geometry.height());
  priv->m_layout->set_verticle_spacing(16);

  priv->m_layout->add_rows(5);

  priv->m_layout->add_segments(0, 1);
  priv->m_layout->add_segments(1, 1);

  priv->m_layout->add_segments(2, 3);
  priv->m_layout->set_segment_width(2, 0, "10%");
  priv->m_layout->set_segment_width(2, 1, "80%");
  priv->m_layout->set_segment_width(2, 2, "10%");

  priv->m_layout->add_segments(3, 3);
  priv->m_layout->set_segment_width(3, 0, "10%");
  priv->m_layout->set_segment_width(3, 1, "80%");
  priv->m_layout->set_segment_width(3, 2, "10%");

  priv->m_layout->add_segments(4, 4);
  priv->m_layout->set_segment_width(4, 0, "10%");
  priv->m_layout->set_segment_width(4, 1, "40%");
  priv->m_layout->set_segment_width(4, 2, "40%");
  priv->m_layout->set_segment_width(4, 3, "10%");

  priv->m_layout->set_row_height(0, "65%");
  priv->m_layout->set_row_height(1, "5%");
  priv->m_layout->set_row_height(2, "10%");
  priv->m_layout->set_row_height(3, "10%");
  priv->m_layout->set_row_height(4, "10%");

  cherry_kit::widget_properties_t prop;
  prop["label"] = "Login";

  priv->m_done_btn = dynamic_cast<cherry_kit::button *>(
      priv->m_layout->add_widget(4, 2, "button", prop, [=]() {}));

  prop["label"] = "Register";
  priv->m_register_btn = dynamic_cast<cherry_kit::button *>(
      priv->m_layout->add_widget(4, 1, "button", prop, [=]() {}));

  prop["text"] = "";
  priv->m_username = dynamic_cast<cherry_kit::line_edit *>(
      priv->m_layout->add_widget(2, 1, "line_edit", prop, [=]() {}));

  prop["text"] = "";
  priv->m_password = dynamic_cast<cherry_kit::line_edit *>(
      priv->m_layout->add_widget(3, 1, "line_edit", prop, [=]() {}));
  priv->m_password->set_password_input(true);

  priv->m_logo = dynamic_cast<cherry_kit::image_view *>(
      priv->m_layout->add_widget(0, 0, "image_view", prop, [=]() {}));

  priv->m_progress_widget = dynamic_cast<cherry_kit::progress_bar *>(
      priv->m_layout->add_widget(1, 0, "progress_bar", prop, [=]() {}));

  priv->m_progress_widget->hide();

  if (priv->m_logo) {
    QString default_wallpaper_file =
      cherry_kit::resource_manager::instance()->drawable_file_name(
          "mdpi", "desktop/ck_login_ui_splash.png");

      priv->m_logo->set_pixmap(default_wallpaper_file);
  }

  priv->m_username->set_focus_buddy(priv->m_password);
  priv->m_password->set_focus_buddy(priv->m_username);

  priv->m_main_window->set_window_content(priv->m_layout->viewport());

  /* actions */
  priv->m_done_btn->on_click([=]() {
    priv->m_progress_widget->show();
    priv->m_progress_widget->set_value(35);
    request_login();
  });
}

cherry_kit::window *auth_dialog::dialog_window() const {
  return priv->m_main_window;
}

bool auth_dialog::purge() {
  if (priv->m_layout) {
      delete priv->m_layout;
  }

  if (priv->m_main_window) {
    delete priv->m_main_window;
  }

  priv->m_main_window = 0;

  return true;
}

void auth_dialog::request_login() {
  social_kit::web_service *service = new social_kit::web_service(0);
  social_kit::service_query_parameters input_data;

  service->create("org.plexydesk.api.xml");

  input_data.insert("grant_type", "password");
  input_data.insert("username", priv->m_username->text().toStdString());
  input_data.insert("password", priv->m_password->text().toStdString());
  input_data.insert("client_id", K_SOCIAL_KIT_CLIENT_ID);
  input_data.insert("client_secret", K_SOCIAL_KIT_CLIENT_SECRET);

  input_data.set_mime_type(
      social_kit::url_request_context::kMimeTypeUrlEncoded);

  service->on_response_ready([this](const social_kit::remote_result &a_result,
                                 const social_kit::web_service *a_web_service) {
      priv->m_progress_widget->set_value(100);
      if (a_result.get("access_token").size() == 0) {
          priv->m_main_window->set_window_title("Sorry, No Such Account");

          priv->m_username->set_input_focus(true);
          priv->m_progress_widget->hide();

          return;
      }

      social_kit::remote_result_data token_data =
        a_result.get("access_token").at(0);
     std::string _access_token = token_data.get("access_token").value();

     priv->m_main_window->hide();

     notify_message("token", _access_token);
     notify_done();
     priv->m_main_window->close();
  });

  priv->m_progress_widget->set_value(65);
  service->submit("plexydesk.oauth2.token", &input_data);
}
