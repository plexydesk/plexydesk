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
#ifndef FBPOSTDIALOG_ACTIVITY_H
#define FBPOSTDIALOG_ACTIVITY_H

#include <QtCore>
#include <plexy.h>
#include <abstractplugininterface.h>
#include <datasource.h>
#include <QtNetwork>
#include <desktopactivity.h>
#include <widget.h>

class FBPostDialogActivity : public PlexyDesk::DesktopActivity {
  Q_OBJECT

public:
  FBPostDialogActivity(QGraphicsObject *object = 0);

  virtual ~FBPostDialogActivity();

  void createWindow(const QRectF &window_geometry, const QString &window_title,
                    const QPointF &window_pos);

  virtual QRectF geometry() const;

  PlexyDesk::Widget *window() const;

  QVariantMap result() const;

private
Q_SLOTS:
  void onWidgetClosed(PlexyDesk::Widget *widget);
  void onMotionAnimFinished();

private:
  class PrivateFBPostDialog;
  PrivateFBPostDialog *const d;
};

#endif
