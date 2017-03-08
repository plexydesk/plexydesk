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

#include <algorithm>

namespace cherry_kit {
class calendar_view::PrivateCalendarWidget {
public:
  PrivateCalendarWidget() {}
  ~PrivateCalendarWidget() {
		if (m_ui)
			delete m_ui;
	}

  int days_in_month(int a_day, int a_month, int a_year);
  int days_of_week(int a_day, int a_month, int a_year);

  QDate m_current_date;
  cherry_kit::fixed_layout *m_ui;

  std::vector<on_date_change_t> m_notification_list;
};

void calendar_view::next_view(label *ck_year_label)
{
  priv->m_current_date = priv->m_current_date.addMonths(1);
  set_date(priv->m_current_date);
  ck_year_label->set_text(QString("%1 %2").
                           arg(priv->m_current_date.toString("MMM")).
                           arg(priv->m_current_date.year()));
}

void calendar_view::previous_view(label *ck_year_label)
{
  priv->m_current_date = priv->m_current_date.addMonths(-1);
  set_date(priv->m_current_date);

  ck_year_label->set_text(QString("%1 %2").
                          arg(priv->m_current_date.toString("MMM")).
                          arg(priv->m_current_date.year()));
}

void calendar_view::reset_view(label *ck_year_label)
{
  priv->m_current_date = QDate::currentDate();
  set_date(priv->m_current_date);

  ck_year_label->set_text(QString("%1 %2").
                          arg(priv->m_current_date.toString("MMM")).
                          arg(priv->m_current_date.year()));
}

calendar_view::calendar_view(widget *parent)
    : cherry_kit::widget(parent), priv(new PrivateCalendarWidget) {

  priv->m_ui = new fixed_layout(this);
  priv->m_ui->set_content_margin(5, 5, 5, 5);
  priv->m_ui->set_geometry(0, 0, 300, 300);

  priv->m_ui->add_rows(10);

  priv->m_ui->add_segments(0, 3);
  priv->m_ui->add_segments(2, 7);

  priv->m_ui->set_row_height(0, "7%");
  priv->m_ui->set_row_height(1, "6%");
  priv->m_ui->set_row_height(2, "7%");

  for (int i = 3; i < 10; i++) {
    priv->m_ui->add_segments(i, 7);
    priv->m_ui->set_row_height(i, "8%");
  }

  priv->m_ui->set_segment_width(0, 0, "10%");
  priv->m_ui->set_segment_width(0, 1, "80%");
  priv->m_ui->set_segment_width(0, 2, "10%");

  widget_properties_t ui_data;

  ui_data["label"] = "Year";
  label *ck_year_label = dynamic_cast<label*>
          (priv->m_ui->add_widget(0, 1, "label", ui_data, [=]() {}));
  ck_year_label->on_click([=]() {
    reset_view(ck_year_label);
  });

  ui_data["label"] = "";
  ui_data["icon"] = "toolbar/ck_arrow-left.png";
  icon_button *ck_prev_btn = dynamic_cast<icon_button*>
          (priv->m_ui->add_widget(0, 0, "image_button", ui_data, [=]() {}));
  ck_prev_btn->on_click([=] (){
    previous_view(ck_year_label);
  });

  //ui_data["label"] = "Year";
  //priv->m_ui->add_widget(0, 1, "label", ui_data);

  ui_data["label"] = "";
  ui_data["icon"] = "toolbar/ck_arrow-right.png";
  icon_button *ck_next_btn = dynamic_cast<icon_button*>
          (priv->m_ui->add_widget(0, 2, "image_button", ui_data, [=]() {}));
  ck_next_btn->on_click([=]() {
   next_view(ck_year_label);
  });

  char day_name_table[7][4] = {
    "Sun", "Mon", "Tue", "Wed", "Thr", "Fri", "Sat"
  };

  for (int i = 0; i < 7; i++) {
    ui_data["label"] = day_name_table[i];
    priv->m_ui->add_widget(2, i, "label", ui_data, [=]() {});
  }

  for (int r = 3; r < 10; r++) {
    for (int c = 0; c < 7; c++) {
      ui_data["label"] = "";
      label *ck_date_lbl = dynamic_cast<label*>
              (priv->m_ui->add_widget(r, c, "label", ui_data, [=]() {}));
      if (ck_date_lbl) {
          ck_date_lbl->on_click([=]() {
            clear_selection();
            //ck_date_lbl->set_highlight(1);
            int ck_click_date  = ck_date_lbl->text().toInt();
            priv->m_current_date.setDate(
                  priv->m_current_date.year(),
                  priv->m_current_date.month(),
                  ck_click_date);
            set_date(priv->m_current_date);
          });
      }
    }
  }

  reset_view(dynamic_cast<label*>(priv->m_ui->at(0,1)));
}

calendar_view::~calendar_view() { delete priv; }

void calendar_view::clear() {
  for (int r = 3; r < 10; r++) {
    for (int c = 0; c < 7; c++) {
      cherry_kit::widget *widget = priv->m_ui->at(r, c);

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

void calendar_view::clear_selection() {
  for (int r = 3; r < 10; r++) {
    for (int c = 0; c < 7; c++) {
      cherry_kit::widget *widget = priv->m_ui->at(r, c);

      if (!widget)
        continue;

      cherry_kit::label *label = dynamic_cast<cherry_kit::label *>(widget);

      if (!label)
        continue;

      label->set_highlight(false);
    }
  }
}

void calendar_view::reset() {
  for (int r = 3; r < 10; r++) {
    for (int c = 0; c < 7; c++) {
      cherry_kit::widget *widget = priv->m_ui->at(r, c);

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

void calendar_view::notify()
{
    std::for_each(priv->m_notification_list.begin(),
                  priv->m_notification_list.end(), [](on_date_change_t a_func) {
        if (a_func)
            a_func();
    });
}

QDate calendar_view::selected_date() const { return priv->m_current_date; }

void calendar_view::set_view_geometry(float a_x, float a_y, float a_width,
                                float a_height) {
  priv->m_ui->set_geometry(a_x, a_y, a_width, a_height);
}

QRectF calendar_view::geometry() const {
  return priv->m_ui->viewport()->geometry();
}

void calendar_view::set_date(const QDate &date) {
  clear();
  int day = date.day();
  int month = date.month();
  int year = date.year();
  priv->m_current_date = date;

  int week_num = 3;
  for (int i = 1; i <= priv->days_in_month(day, month, year);
       i++) {
    if (priv->days_of_week(i, month, year) == 0)
      week_num++;

    cherry_kit::widget *widget = priv->m_ui->at(
        week_num, priv->days_of_week(i, month, year));

    if (!widget)
      continue;

    cherry_kit::label *label = dynamic_cast<cherry_kit::label *>(widget);

    if (!label)
      continue;

    if(i == day)
      label->set_highlight(1);
    label->set_text(QString("%1").arg(i));
  }

  notify();
}

void calendar_view::next() {
  next_view(dynamic_cast<label*>(priv->m_ui->at(0,1)));
  notify();
}

void calendar_view::previous() {
    previous_view(dynamic_cast<label*>(priv->m_ui->at(0,1)));
    notify();
}

void calendar_view::on_date_change(on_date_change_t a_callback) {
  priv->m_notification_list.push_back(a_callback);
}

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
