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

class date_dialog::PrivateDatePicker {
public:
  PrivateDatePicker() {}
  ~PrivateDatePicker() {}

  cherry_kit::window *m_activity_window;
  cherry_kit::widget *m_window_content;
  cherry_kit::Button *m_done_button;
  cherry_kit::CalendarView *mCalendarWidget;

  QVariantMap m_result_data;
};

date_dialog::date_dialog(QGraphicsObject *object)
    : cherry_kit::desktop_dialog(object),
      o_desktop_dialog(new PrivateDatePicker) {}

date_dialog::~date_dialog() { delete o_desktop_dialog; }

void date_dialog::create_window(const QRectF &window_geometry,
                                       const QString &window_title,
                                       const QPointF &window_pos) {
  o_desktop_dialog->m_activity_window = new cherry_kit::window();
  o_desktop_dialog->m_activity_window->setGeometry(window_geometry);
  o_desktop_dialog->m_activity_window->set_window_title(window_title);

  o_desktop_dialog->m_window_content =
      new cherry_kit::widget(o_desktop_dialog->m_activity_window);
  o_desktop_dialog->m_window_content->setGeometry(window_geometry);

  o_desktop_dialog->mCalendarWidget =
      new cherry_kit::CalendarView(o_desktop_dialog->m_window_content);
  o_desktop_dialog->mCalendarWidget->setGeometry(window_geometry);
  o_desktop_dialog->mCalendarWidget->setPos(0, 0);

  o_desktop_dialog->m_done_button =
      new cherry_kit::Button(o_desktop_dialog->m_window_content);
  o_desktop_dialog->m_done_button->set_label(tr("Done"));
  o_desktop_dialog->m_done_button->show();

  o_desktop_dialog->m_done_button->setPos(
      o_desktop_dialog->mCalendarWidget->geometry().width() / 2 -
          (o_desktop_dialog->m_done_button->boundingRect().width() + 10) / 2,
      310);

  o_desktop_dialog->m_done_button->on_input_event([this](
      cherry_kit::widget::InputEvent a_event,
      const cherry_kit::widget *a_widget) {
    if (a_event == cherry_kit::widget::kMouseReleaseEvent) {
      qDebug() << Q_FUNC_INFO << "Activity complete";
      end_calendar();
      notify_done();
    }
  });

  o_desktop_dialog->m_activity_window->set_window_content(
      o_desktop_dialog->m_window_content);

  QRectF view_geometry = window_geometry;
  view_geometry.setHeight(window_geometry.height() + 64);

  set_geometry(view_geometry);

  exec(window_pos);
}

QVariantMap date_dialog::result() const {
  return o_desktop_dialog->m_result_data;
}

cherry_kit::window *date_dialog::activity_window() const {
  return o_desktop_dialog->m_activity_window;
}

void date_dialog::cleanup() {
  if (o_desktop_dialog->m_activity_window) {
    delete o_desktop_dialog->m_activity_window;
  }

  o_desktop_dialog->m_activity_window = 0;
}

void date_dialog::onImageReady(const QImage &img) {}

void date_dialog::end_calendar() {
  if (!o_desktop_dialog->mCalendarWidget) {
    o_desktop_dialog->m_result_data["date"] =
        QVariant(QDate::currentDate().toString());
  } else {
    o_desktop_dialog->m_result_data["date"] =
        QVariant(o_desktop_dialog->mCalendarWidget->a_date());
  }
}
