#include "calendarwidget.h"

#include <label.h>
#include <button.h>
#include <viewbuilder.h>
#include <imagebutton.h>
#include <resource_manager.h>

namespace UIKit {
class CalendarView::PrivateCalendarWidget {
public:
  PrivateCalendarWidget() {}
  ~PrivateCalendarWidget() {}

  int days_in_month(int a_day, int a_month, int a_year);
  int days_of_week(int a_day, int a_month, int a_year);

  QDate m_current_date;
  UIKit::HybridLayout *m_ui;
};

CalendarView::CalendarView(Widget *parent)
    : UIKit::Widget(parent), d(new PrivateCalendarWidget) {

  d->m_ui = new HybridLayout(this);
  d->m_ui->set_content_margin(10, 10, 10, 10);
  d->m_ui->set_geometry(0, 0, 320, 320);

  d->m_ui->set_horizontal_segment_count(9);

  d->m_ui->add_horizontal_segments(0, 3);
  d->m_ui->add_horizontal_segments(1, 7);

  d->m_ui->set_horizontal_height(0, "10%");
  d->m_ui->set_horizontal_height(1, "10%");

  for (int i = 2; i < 9; i++) {
    d->m_ui->add_horizontal_segments(i, 7);
    d->m_ui->set_horizontal_height(i, "8%");
  }

  d->m_ui->set_column_width(0, 0, "10%");
  d->m_ui->set_column_width(0, 1, "80%");
  d->m_ui->set_column_width(0, 2, "10%");

  WidgetProperties ui_data;

  ui_data["label"] = "";
  ui_data["icon"] = "actions/pd_previous.png";
  d->m_ui->add_widget(0, 0, "image_button", ui_data);

  ui_data["label"] = "Year";
  d->m_ui->add_widget(0, 1, "label", ui_data);

  ui_data["label"] = "";
  ui_data["icon"] = "actions/pd_next.png";
  d->m_ui->add_widget(0, 2, "image_button", ui_data);

  char day_name_table[7][4] = {
    "Sun", "Mon", "Tue", "Wed", "Thr", "Fri", "Sat"
  };

  for (int i = 0; i < 7; i++) {
    ui_data["label"] = day_name_table[i];
    d->m_ui->add_widget(1, i, "label", ui_data);
  }

  for (int r = 2; r < 9; r++) {
    for (int c = 0; c < 7; c++) {
      ui_data["label"] = "";
      d->m_ui->add_widget(r, c, "label", ui_data);
    }
  }

  set_date(QDate());
}

CalendarView::~CalendarView() { delete d; }

void CalendarView::clear() {
  for (int r = 2; r < 9; r++) {
    for (int c = 0; c < 7; c++) {
      UIKit::Widget *widget = d->m_ui->at(r, c);

      if (!widget)
        continue;

      UIKit::Label *label = dynamic_cast<UIKit::Label *>(widget);

      if (!label)
        continue;

      label->set_label("");
      label->set_highlight(false);
    }
  }
}

void CalendarView::reset() {
  for (int r = 2; r < 9; r++) {
    for (int c = 0; c < 7; c++) {
      UIKit::Widget *widget = d->m_ui->at(r, c);

      if (!widget)
        continue;

      UIKit::Label *label = dynamic_cast<UIKit::Label *>(widget);

      if (!label)
        continue;

      label->set_label("");
      label->set_highlight(false);
    }
  }
}

QDate CalendarView::a_date() const { return d->m_current_date; }

void CalendarView::set_geometry(float a_x, float a_y, float a_width,
                                float a_height) {
  d->m_ui->set_geometry(a_x, a_y, a_width, a_height);
}

QRectF CalendarView::geometry() const { return d->m_ui->viewport()->geometry(); }

void CalendarView::set_date(const QDate &date) {
  clear();
  int day = 1;
  int month = 7;
  int year = 2015;

  int week_num = 2;
  for (int i = 1; i <= d->days_in_month(day, month, year); i++) {
    if (d->days_of_week(i, month, year) == 0)
      week_num++;

    UIKit::Widget *widget =
        d->m_ui->at(week_num, d->days_of_week(i, month, year));

    if (!widget)
      continue;

    UIKit::Label *label = dynamic_cast<UIKit::Label *>(widget);

    if (!label)
      continue;

    label->set_label(QString("%1").arg(i));
  }
}

void CalendarView::next() {}

void CalendarView::previous() {}

void CalendarView::paint_view(QPainter *painter, const QRectF &rect) {}

int CalendarView::PrivateCalendarWidget::days_in_month(int day, int month,
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

int CalendarView::PrivateCalendarWidget::days_of_week(int a_day, int a_month,
                                                      int a_year) {
  static int t[] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };
  a_year -= a_month < 3;
  return (a_year + a_year / 4 - a_year / 100 + a_year / 400 + t[a_month - 1] +
          a_day) %
         7;
}
}
