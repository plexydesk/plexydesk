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
#include "datepicker.h"
#include <widget.h>
#include <plexyconfig.h>
#include <QTimer>
#include <view_controller.h>
#include <tableview.h>
#include <default_table_model.h>
#include <themepackloader.h>

#include "calendarwidget.h"
#include "datecellfactory.h"

class DatePickerActivity::PrivateDatePicker
{
public:
  PrivateDatePicker() {}
  ~PrivateDatePicker() {}

  UIKit::Window *mFrame;
  DateCellFactory *mFactory;
  UIKit::TableView *mTable;
  QRectF mBoundingRect;
  QString mSelection;
  UIKit::Theme *mLoader;

  QVariantMap m_result_data;

  CalendarWidget *mCalendarWidget;
};

DatePickerActivity::DatePickerActivity(QGraphicsObject *object)
  : UIKit::DesktopActivity(object), d(new PrivateDatePicker) {}

DatePickerActivity::~DatePickerActivity() { delete d; }

void DatePickerActivity::createWindow(const QRectF &window_geometry,
                                      const QString &window_title,
                                      const QPointF &window_pos)
{
  d->mFrame = new UIKit::Window();
  d->mFrame->setGeometry(window_geometry);
  d->mFrame->setWindowTitle(window_title);
  setGeometry(window_geometry);

  d->mCalendarWidget = new CalendarWidget(d->mFrame);
  d->mCalendarWidget->setGeometry(window_geometry);

  d->mFrame->setWindowContent(d->mCalendarWidget);
  exec(window_pos);
}

QVariantMap DatePickerActivity::result() const { return d->m_result_data; }

UIKit::Window *DatePickerActivity::window() const { return d->mFrame; }

void DatePickerActivity::onWidgetClosed(UIKit::Widget *widget)
{
  connect(this, SIGNAL(discarded()), this, SLOT(onHideAnimationFinished()));
  discardActivity();
}

void DatePickerActivity::onHideAnimationFinished() { Q_EMIT finished(); }

void DatePickerActivity::onImageReady(const QImage &img)
{
  d->mCalendarWidget->setBackgroundImage(img);
}

void DatePickerActivity::onCalendarReady()
{
  if (!d->mCalendarWidget) {
    d->m_result_data["date"] = QVariant(QDate::currentDate().toString());
    d->m_result_data["hour"] = QVariant(0);
    d->m_result_data["minute"] = QVariant(0);
  } else {
    d->m_result_data["date"] = QVariant(d->mCalendarWidget->currentDate());
    d->m_result_data["hour"] = QVariant(d->mCalendarWidget->currentHour());
    d->m_result_data["minute"] = QVariant(d->mCalendarWidget->currentMinute());
  }

  connect(this, SIGNAL(discarded()), this, SLOT(onHideAnimationFinished()));
  discardActivity();
}
