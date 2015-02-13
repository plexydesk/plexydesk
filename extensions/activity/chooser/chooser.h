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
#ifndef CHOOSER_ACTIVITY_H
#define CHOOSER_ACTIVITY_H

#include <QtCore>
#include <plexy.h>

#include <datasource.h>
#include <QtNetwork>
#include <desktopactivity.h>
#include <widget.h>
#include <window.h>
#include <abstractcellcomponent.h>

using namespace UIKit;

class IconGridActivity : public UIKit::DesktopActivity
{
  Q_OBJECT

public:
  IconGridActivity(QGraphicsObject *object = 0);

  virtual ~IconGridActivity();

  void createWindow(const QRectF &window_geometry, const QString &window_title,
                    const QPointF &window_pos);
  QVariantMap result() const;
  Window *window() const;

private Q_SLOTS:
  void onWidgetClosed(UIKit::Widget *widget);
  void onDiscard();
  void onClicked(TableViewItem *item);
  void onArgumentChanged();

private:
  class PrivateIconGrid;
  PrivateIconGrid *const d;
};

#endif
