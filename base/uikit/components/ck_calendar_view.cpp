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
#include "ck_calendar_view.h"

#include <ck_label.h>
#include <ck_button.h>
#include <ck_fixed_layout.h>
#include <ck_icon_button.h>
#include <ck_resource_manager.h>

namespace cherry_kit {
class calendar_view::PrivateCalendarWidget {
public:
  PrivateCalendarWidget() {}
  ~PrivateCalendarWidget() {}

  int days_in_month(int a_day, int a_month, int a_year);
  int days_of_week(int a_day, int a_month, int a_year);

  QDate m_current_date;
  cherry_kit::fixed_layout *m_ui;
};

calendar_view::calendar_view(widget *parent)
    : cherry_kit::widget(parent), o_calendar_widget(new PrivateCalendarWidget) {

  o_calendar_widget->m_ui = new fixed_layout(this);
  o_calendar_widget->m_ui->set_content_margin(5, 5, 5, 5);
  o_calendar_widget->m_ui->set_geometry(0, 0, 300, 300);

  o_calendar_widget->m_ui->add_rows(10);

  o_calendar_widget->m_ui->add_segments(0, 3);
  o_calendar_widget->m_ui->add_segments(2, 7);

  o_calendar_widget->m_ui->set_row_height(0, "7%");
  o_calendar_widget->m_ui->set_row_height(1, "6%");
  o_calendar_widget->m_ui->set_row_height(2, "7%");

  for (int i = 3; i < 10; i++) {
    o_calendar_widget->m_ui->add_segments(i, 7);
    o_calendar_widget->m_ui->set_row_height(i, "8%");
  }

  o_calendar_widget->m_ui->set_segment_width(0, 0, "10%");
  o_calendar_widget->m_ui->set_segment_width(0, 1, "80%");
  o_calendar_widget->m_ui->set_segment_width(0, 2, "10%");

  widget_properties_t ui_data;

  ui_data["label"] = "";
  ui_data["icon"] = "toolbar/ck_previous.png";
  o_calendar_widget->m_ui->add_widget(0, 0, "image_button", ui_data);

  ui_data["label"] = "Year";
  o_calendar_widget->m_ui->add_widget(0, 1, "label", ui_data);

  ui_data["label"] = "";
  ui_data["icon"] = "toolbar/ck_next.png";
  o_calendar_widget->m_ui->add_widget(0, 2, "image_button", ui_data);

  char day_name_table[7][4] = {
    "Sun", "Mon", "Tue", "Wed", "Thr", "Fri", "Sat"
  };

  for (int i = 0; i < 7; i++) {
    ui_data["label"] = day_name_table[i];
    o_calendar_widget->m_ui->add_widget(2, i, "label", ui_data);
  }

  for (int r = 3; r < 10; r++) {
    for (int c = 0; c < 7; c++) {
      ui_data["label"] = "";
      o_calendar_widget->m_ui->add_widget(r, c, "label", ui_data);
    }
  }

  set_date(QDate());
}

calendar_view::~calendar_view() { delete o_calendar_widget; }

void calendar_view::clear() {
  for (int r = 3; r < 10; r++) {
    for (int c = 0; c < 7; c++) {
      cherry_kit::widget *widget = o_calendar_widget->m_ui->at(r, c);

      if (!widget)
        continue;

      cherry_kit::label *label = dynamic_cast<cherry_kit::label *>(widget);

      if (!label)
        continue;

      label->set_text("");
      label->set_highlight(false);
    }
  }
}

void calendar_view::reset() {
  for (int r = 3; r < 10; r++) {
    for (int c = 0; c < 7; c++) {
      cherry_kit::widget *widget = o_calendar_widget->m_ui->at(r, c);

      if (!widget)
        continue;

      cherry_kit::label *label = dynamic_cast<cherry_kit::label *>(widget);

      if (!label)
        continue;

      label->set_text("");
      label->set_highlight(false);
    }
  }
}

QDate calendar_view::selected_date() const { return o_calendar_widget->m_current_date; }

void calendar_view::set_geometry(float a_x, float a_y, float a_width,
                                float a_height) {
  o_calendar_widget->m_ui->set_geometry(a_x, a_y, a_width, a_height);
}

QRectF calendar_view::geometry() const {
  return o_calendar_widget->m_ui->viewport()->geometry();
}

void calendar_view::set_date(const QDate &date) {
  clear();
  int day = 1;
  int month = 7;
  int year = 2015;

  int week_num = 3;
  for (int i = 1; i <= o_calendar_widget->days_in_month(day, month, year);
       i++) {
    if (o_calendar_widget->days_of_week(i, month, year) == 0)
      week_num++;

    cherry_kit::widget *widget = o_calendar_widget->m_ui->at(
        week_num, o_calendar_widget->days_of_week(i, month, year));

    if (!widget)
      continue;

    cherry_kit::label *label = dynamic_cast<cherry_kit::label *>(widget);

    if (!label)
      continue;

    label->set_text(QString("%1").arg(i));
  }
}

void calendar_view::next() {}

void calendar_view::previous() {}

void calendar_view::paint_view(QPainter *painter, const QRectF &rect) {}

int calendar_view::PrivateCalendarWidget::days_in_month(int day, int month,
                                                       int year) {
  if (month == 4 || month == 6 || month == 9 || month == 11)
    return 30;

  if (month == 2) {
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
      return 29;
    }
    return 28;
  }

  return 31;
}

int calendar_view::PrivateCalendarWidget::days_of_week(int a_day, int a_month,
                                                      int a_year) {
  static int t[] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };
  a_year -= a_month < 3;
  return (a_year + a_year / 4 - a_year / 100 + a_year / 400 + t[a_month - 1] +
          a_day) %
         7;
}
}
