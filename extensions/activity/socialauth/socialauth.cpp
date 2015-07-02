/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.org>
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
#include <widget.h>
#include <plexyconfig.h>
#include <QTimer>
#include <view_controller.h>

class SocialAuthActivity::PrivateSocialAuth {
public:
  PrivateSocialAuth() {}
  ~PrivateSocialAuth() {}

  CherryKit::Window *mFrame;
};

SocialAuthActivity::SocialAuthActivity(QGraphicsObject *object)
    : CherryKit::DesktopActivity(object), o_desktop_activity(new PrivateSocialAuth) {}

SocialAuthActivity::~SocialAuthActivity() { delete o_desktop_activity; }

void SocialAuthActivity::create_window(const QRectF &window_geometry,
                                       const QString &window_title,
                                       const QPointF &window_pos) {
  set_geometry(window_geometry);

  o_desktop_activity->mFrame = new CherryKit::Window();
  o_desktop_activity->mFrame->setGeometry(geometry());
  o_desktop_activity->mFrame->setVisible(true);
  o_desktop_activity->mFrame->set_widget_name("Message Dialog");

  o_desktop_activity->mFrame->set_widget_flag(CherryKit::Widget::kRenderBackground);
  o_desktop_activity->mFrame->set_widget_flag(CherryKit::Widget::kConvertToWindowType);
  o_desktop_activity->mFrame->set_widget_flag(CherryKit::Widget::kRenderDropShadow);

  update_content_geometry(o_desktop_activity->mFrame);
  exec();

  show_activity();
}

QVariantMap SocialAuthActivity::result() const { return QVariantMap(); }

CherryKit::Window *SocialAuthActivity::window() const { return o_desktop_activity->mFrame; }

void SocialAuthActivity::cleanup() {
  if (o_desktop_activity->mFrame) {
    delete o_desktop_activity->mFrame;
  }
  o_desktop_activity->mFrame = 0;
}

void SocialAuthActivity::onWidgetClosed(CherryKit::Widget *widget) {}

void SocialAuthActivity::onHideAnimationFinished() {}
