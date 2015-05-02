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
#ifndef FLIKRSEARCH_ACTIVITY_H
#define FLIKRSEARCH_ACTIVITY_H

#include <QtCore>
#include <plexy.h>

#include <datasource.h>
#include <QtNetwork>
#include <desktopactivity.h>
#include <webservice.h>
#include <abstractcellcomponent.h>
#include <window.h>

using namespace UIKit;
class FlickrSearchActivity : public UIKit::DesktopActivity {
  Q_OBJECT
public:
  FlickrSearchActivity(QGraphicsObject *object = 0);
  virtual ~FlickrSearchActivity();

  void create_window(const QRectF &aWindowGeometry, const QString &window_title,
                     const QPointF &window_pos);
  Window *window() const;

  QVariantMap result() const;
  void cleanup();
public
Q_SLOTS:
  void photoItemClicked(TableViewItem *item);

private
Q_SLOTS:
  void onWidgetClosed(UIKit::Widget *widget);
  void onButtonClicked();
  void onNextButtonClicked();
  void onPreviousButtonClicked();
  void onHideAnimationFinished();
  void onPageCount(int count);
  void onProgressRange(int range);
  void onCompleted(int count);
  void onToolbarAction(const QString &action);

private:
  class PrivateFlikrSearch;
  PrivateFlikrSearch *const d;
};

#endif
