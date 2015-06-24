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
#include <imagebutton.h>
#include <button.h>
#include <calendarwidget.h>

class DatePickerActivity::PrivateDatePicker {
public:
  PrivateDatePicker() {}
  ~PrivateDatePicker() {}

  UIKit::Window *m_activity_window;
  UIKit::Widget *m_window_content;
  UIKit::Button *m_done_button;
  UIKit::CalendarView *mCalendarWidget;

  QVariantMap m_result_data;
};

DatePickerActivity::DatePickerActivity(QGraphicsObject *object)
    : UIKit::DesktopActivity(object), d(new PrivateDatePicker) {}

DatePickerActivity::~DatePickerActivity() { delete d; }

void DatePickerActivity::create_window(const QRectF &window_geometry,
                                       const QString &window_title,
                                       const QPointF &window_pos) {
  d->m_activity_window = new UIKit::Window();
  d->m_activity_window->setGeometry(window_geometry);
  d->m_activity_window->set_window_title(window_title);

  d->m_window_content = new UIKit::Widget(d->m_activity_window);
  d->m_window_content->setGeometry(window_geometry);

  d->mCalendarWidget = new UIKit::CalendarView(d->m_window_content);
  d->mCalendarWidget->setGeometry(window_geometry);
  d->mCalendarWidget->setPos(0, 0);

  d->m_done_button = new UIKit::Button(d->m_window_content);
  d->m_done_button->set_label(tr("Done"));
  d->m_done_button->show();

  d->m_done_button->setPos(
        d->mCalendarWidget->geometry().width() / 2
        - (d->m_done_button->boundingRect().width() + 10) / 2,
        310);

  d->m_done_button->on_input_event([this](UIKit::Widget::InputEvent a_event,
                            const UIKit::Widget *a_widget) {
    if (a_event == UIKit::Widget::kMouseReleaseEvent) {
        qDebug() << Q_FUNC_INFO << "Activity complete";
        end_calendar();
        notify_done();
    }
  });

  d->m_activity_window->set_window_content(d->m_window_content);

  QRectF view_geometry = window_geometry;
  view_geometry.setHeight(window_geometry.height() + 64);

  set_geometry(view_geometry);

  exec(window_pos);
}

QVariantMap DatePickerActivity::result() const { return d->m_result_data; }

UIKit::Window *DatePickerActivity::window() const {
    return d->m_activity_window;
}

void DatePickerActivity::cleanup() {
  if (d->m_activity_window) {
    delete d->m_activity_window;
  }

  d->m_activity_window = 0;
}

void DatePickerActivity::onImageReady(const QImage &img) {
}

void DatePickerActivity::end_calendar() {
  if (!d->mCalendarWidget) {
    d->m_result_data["date"] = QVariant(QDate::currentDate().toString());
  } else {
    d->m_result_data["date"] = QVariant(d->mCalendarWidget->a_date());
  }
}
