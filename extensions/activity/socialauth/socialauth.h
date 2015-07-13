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
#ifndef SOCIALAUTH_ACTIVITY_H
#define SOCIALAUTH_ACTIVITY_H

#include <QtCore>

#include <window.h>
#include <datasource.h>
#include <QtNetwork>
#include <desktopactivity.h>

class social_auth_dialog : public cherry_kit::desktop_dialog {
  Q_OBJECT

public:
  social_auth_dialog(QGraphicsObject *object = 0);

  virtual ~social_auth_dialog();

  void create_window(const QRectF &window_geometry, const QString &window_title,
                     const QPointF &window_pos);

  QVariantMap result() const;

  cherry_kit::window *activity_window() const;
  void cleanup();

private
Q_SLOTS:
  void onWidgetClosed(cherry_kit::widget *widget);

  void onHideAnimationFinished();

private:
  class PrivateSocialAuth;
  PrivateSocialAuth *const o_desktop_dialog;
};

#endif
