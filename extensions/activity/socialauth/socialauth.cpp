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

class SocialAuthActivity::PrivateSocialAuth
{
public:
  PrivateSocialAuth() {}
  ~PrivateSocialAuth() {}

  UI::Window *mFrame;
};

SocialAuthActivity::SocialAuthActivity(QGraphicsObject *object)
  : UI::DesktopActivity(object), d(new PrivateSocialAuth) {}

SocialAuthActivity::~SocialAuthActivity() { delete d; }

void SocialAuthActivity::createWindow(const QRectF &window_geometry,
                                      const QString &window_title,
                                      const QPointF &window_pos)
{
  setGeometry(window_geometry);

  d->mFrame = new UI::Window();
  d->mFrame->setGeometry(geometry());
  d->mFrame->setVisible(true);
  d->mFrame->setLabelName("Message Dialog");

  d->mFrame->setWindowFlag(UI::Widget::kRenderBackground);
  d->mFrame->setWindowFlag(UI::Widget::kConvertToWindowType);
  d->mFrame->setWindowFlag(UI::Widget::kRenderDropShadow);

  updateContentGeometry(d->mFrame);
  exec();

  showActivity();
  connect(d->mFrame, SIGNAL(closed(UI::Widget *)), this,
          SLOT(onWidgetClosed(UI::Widget *)));
}

QVariantMap SocialAuthActivity::result() const { return QVariantMap(); }

UI::Window *SocialAuthActivity::window() const { return d->mFrame; }

void SocialAuthActivity::onWidgetClosed(UI::Widget *widget) {}

void SocialAuthActivity::onHideAnimationFinished() {}
