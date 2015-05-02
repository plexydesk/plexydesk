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
#ifndef TIMEZONE_ACTIVITY_H
#define TIMEZONE_ACTIVITY_H

#include <QtCore>
#include <plexy.h>

#include <datasource.h>
#include <QtNetwork>
#include <desktopactivity.h>
#include <window.h>

class TimeZoneActivity : public UIKit::DesktopActivity {
  Q_OBJECT

public:
  TimeZoneActivity(QGraphicsObject *aParent = 0);

  virtual ~TimeZoneActivity();

  void create_window(const QRectF &aWindowGeometry, const QString &aWindowTitle,
                     const QPointF &aWindowPos);
  UIKit::Window *window() const;
  void cleanup();

  QVariantMap result() const;
  virtual void update_attribute(const QString &aName,
                                const QVariant &aVariantData);

private:
  class PrivateTimeZone;
  PrivateTimeZone *const m_priv_ptr;
  void loadTimeZones();
};

#endif
