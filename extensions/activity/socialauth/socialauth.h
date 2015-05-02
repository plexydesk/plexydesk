/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.org>
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
#ifndef SOCIALAUTH_ACTIVITY_H
#define SOCIALAUTH_ACTIVITY_H

#include <QtCore>
#include <plexy.h>

#include <window.h>
#include <datasource.h>
#include <QtNetwork>
#include <desktopactivity.h>

class SocialAuthActivity : public UIKit::DesktopActivity {
  Q_OBJECT

public:
  SocialAuthActivity(QGraphicsObject *object = 0);

  virtual ~SocialAuthActivity();

  void create_window(const QRectF &window_geometry, const QString &window_title,
                     const QPointF &window_pos);

  QVariantMap result() const;

  UIKit::Window *window() const;
  void cleanup();

private
Q_SLOTS:
  void onWidgetClosed(UIKit::Widget *widget);

  void onHideAnimationFinished();

private:
  class PrivateSocialAuth;
  PrivateSocialAuth *const d;
};

#endif
