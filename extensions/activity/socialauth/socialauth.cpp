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
#include <desktopwidget.h>
#include <plexyconfig.h>
#include <QTimer>
#include <controllerinterface.h>

class SocialAuthActivity::PrivateSocialAuth {
public:
  PrivateSocialAuth() {}
  ~PrivateSocialAuth() {}

  PlexyDesk::UIWidget *mFrame;
};

SocialAuthActivity::SocialAuthActivity(QGraphicsObject *object)
    : PlexyDesk::DesktopActivity(object), d(new PrivateSocialAuth) {}

SocialAuthActivity::~SocialAuthActivity() { delete d; }

void SocialAuthActivity::createWindow(const QRectF &window_geometry,
                                      const QString &window_title,
                                      const QPointF &window_pos) {
  setGeometry(window_geometry);

  d->mFrame = new PlexyDesk::UIWidget();
  d->mFrame->setGeometry(geometry());
  d->mFrame->setVisible(true);
  d->mFrame->setLabelName("Message Dialog");
  d->mFrame->setWindowTitle(window_title);

  d->mFrame->setWindowFlag(PlexyDesk::UIWidget::kRenderBackground);
  d->mFrame->setWindowFlag(PlexyDesk::UIWidget::kTopLevelWindow);
  d->mFrame->setWindowFlag(PlexyDesk::UIWidget::kConvertToWindowType);
  d->mFrame->setWindowFlag(PlexyDesk::UIWidget::kRenderWindowTitle);
  d->mFrame->setWindowFlag(PlexyDesk::UIWidget::kRenderDropShadow);

  updateContentGeometry(d->mFrame);
  exec();

  showActivity();
  connect(d->mFrame, SIGNAL(closed(PlexyDesk::Widget *)), this,
          SLOT(onWidgetClosed(PlexyDesk::Widget *)));
}

QVariantMap SocialAuthActivity::result() const { return QVariantMap(); }

PlexyDesk::Widget *SocialAuthActivity::window() const { return d->mFrame; }

void SocialAuthActivity::onWidgetClosed(PlexyDesk::Widget *widget) {}

void SocialAuthActivity::onHideAnimationFinished() {}
