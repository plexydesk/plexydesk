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
#include "socialauth.h"
#include <ck_widget.h>
#include <ck_config.h>
#include <QTimer>
#include <ck_desktop_controller_interface.h>

class social_auth_dialog::PrivateSocialAuth {
public:
  PrivateSocialAuth() {}
  ~PrivateSocialAuth() {}

  cherry_kit::window *mFrame;
};

social_auth_dialog::social_auth_dialog(QGraphicsObject *object)
    : cherry_kit::desktop_dialog(object),
      o_desktop_dialog(new PrivateSocialAuth) {}

social_auth_dialog::~social_auth_dialog() { delete o_desktop_dialog; }

void social_auth_dialog::create_window(const QRectF &window_geometry,
                                       const QString &window_title,
                                       const QPointF &window_pos) {
  set_geometry(window_geometry);

  o_desktop_dialog->mFrame = new cherry_kit::window();
  o_desktop_dialog->mFrame->setGeometry(geometry());
  o_desktop_dialog->mFrame->setVisible(true);
  o_desktop_dialog->mFrame->set_widget_name("Message Dialog");

  o_desktop_dialog->mFrame->set_widget_flag(
      cherry_kit::widget::kRenderBackground);
  o_desktop_dialog->mFrame->set_widget_flag(
      cherry_kit::widget::kConvertToWindowType);
  o_desktop_dialog->mFrame->set_widget_flag(
      cherry_kit::widget::kRenderDropShadow);

  update_content_geometry(o_desktop_dialog->mFrame);
  exec();

  show_activity();
}

QVariantMap social_auth_dialog::result() const { return QVariantMap(); }

cherry_kit::window *social_auth_dialog::activity_window() const {
  return o_desktop_dialog->mFrame;
}

void social_auth_dialog::cleanup() {
  if (o_desktop_dialog->mFrame) {
    delete o_desktop_dialog->mFrame;
  }
  o_desktop_dialog->mFrame = 0;
}

void social_auth_dialog::onWidgetClosed(cherry_kit::widget *widget) {}

void social_auth_dialog::onHideAnimationFinished() {}
