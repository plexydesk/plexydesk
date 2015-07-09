/*******************************************************************************
* This file is part of PlexyDesk.
*  Maintained by : Siraj Razick <siraj@plexydesk.com>
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
#include <resource_manager.h>
#include <imagebutton.h>
#include <button.h>
#include <calendarwidget.h>

class DatePickerActivity::PrivateDatePicker {
public:
  PrivateDatePicker() {}
  ~PrivateDatePicker() {}

  CherryKit::Window *m_activity_window;
  CherryKit::Widget *m_window_content;
  CherryKit::Button *m_done_button;
  CherryKit::CalendarView *mCalendarWidget;

  QVariantMap m_result_data;
};

DatePickerActivity::DatePickerActivity(QGraphicsObject *object)
    : CherryKit::DesktopActivity(object), o_desktop_activity(new PrivateDatePicker) {}

DatePickerActivity::~DatePickerActivity() { delete o_desktop_activity; }

void DatePickerActivity::create_window(const QRectF &window_geometry,
                                       const QString &window_title,
                                       const QPointF &window_pos) {
  o_desktop_activity->m_activity_window = new CherryKit::Window();
  o_desktop_activity->m_activity_window->setGeometry(window_geometry);
  o_desktop_activity->m_activity_window->set_window_title(window_title);

  o_desktop_activity->m_window_content = new CherryKit::Widget(o_desktop_activity->m_activity_window);
  o_desktop_activity->m_window_content->setGeometry(window_geometry);

  o_desktop_activity->mCalendarWidget = new CherryKit::CalendarView(o_desktop_activity->m_window_content);
  o_desktop_activity->mCalendarWidget->setGeometry(window_geometry);
  o_desktop_activity->mCalendarWidget->setPos(0, 0);

  o_desktop_activity->m_done_button = new CherryKit::Button(o_desktop_activity->m_window_content);
  o_desktop_activity->m_done_button->set_label(tr("Done"));
  o_desktop_activity->m_done_button->show();

  o_desktop_activity->m_done_button->setPos(o_desktop_activity->mCalendarWidget->geometry().width() / 2 -
                               (o_desktop_activity->m_done_button->boundingRect().width() + 10) /
                                   2,
                           310);

  o_desktop_activity->m_done_button->on_input_event([this](CherryKit::Widget::InputEvent a_event,
                                          const CherryKit::Widget *a_widget) {
    if (a_event == CherryKit::Widget::kMouseReleaseEvent) {
      qDebug() << Q_FUNC_INFO << "Activity complete";
      end_calendar();
      notify_done();
    }
  });

  o_desktop_activity->m_activity_window->set_window_content(o_desktop_activity->m_window_content);

  QRectF view_geometry = window_geometry;
  view_geometry.setHeight(window_geometry.height() + 64);

  set_geometry(view_geometry);

  exec(window_pos);
}

QVariantMap DatePickerActivity::result() const { return o_desktop_activity->m_result_data; }

CherryKit::Window *DatePickerActivity::window() const {
  return o_desktop_activity->m_activity_window;
}

void DatePickerActivity::cleanup() {
  if (o_desktop_activity->m_activity_window) {
    delete o_desktop_activity->m_activity_window;
  }

  o_desktop_activity->m_activity_window = 0;
}

void DatePickerActivity::onImageReady(const QImage &img) {}

void DatePickerActivity::end_calendar() {
  if (!o_desktop_activity->mCalendarWidget) {
    o_desktop_activity->m_result_data["date"] = QVariant(QDate::currentDate().toString());
  } else {
    o_desktop_activity->m_result_data["date"] = QVariant(o_desktop_activity->mCalendarWidget->a_date());
  }
}
