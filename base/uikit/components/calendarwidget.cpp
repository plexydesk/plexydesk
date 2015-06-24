#include "calendarwidget.h"
#include <QGraphicsLinearLayout>
#include <QGraphicsWidget>
#include <imagebutton.h>
#include <imageview.h>
#include <label.h>
#include <tableview.h>
#include <toolbar.h>
#include <resource_manager.h>
#include <item_view.h>
#include <button.h>

namespace UIKit {
class CalendarView::PrivateCalendarWidget {
public:
  PrivateCalendarWidget() {}
  ~PrivateCalendarWidget() {}

  QGraphicsWidget *m_calendar_base_frame;

  UIKit::ToolBar *m_toolbar;

  UIKit::ImageButton *m_next_btn;
  UIKit::ImageButton *m_previous_btn;
  UIKit::Label *mYearLable;
  UIKit::Widget *m_content_widget;
  UIKit::ItemView *m_header_view;
  UIKit::ItemView *m_cell_view;

  QDate m_current_date;
};

void CalendarView::add_weekday_header(int i)
{
  UIKit::Label *day_label = new UIKit::Label(d->m_header_view);
  day_label->set_label(QDate::shortDayName(i));
  day_label->set_size(QSizeF(32, 32));
  day_label->setMinimumSize(QSizeF(32, 32));
  UIKit::ModelViewItem *grid_item = new UIKit::ModelViewItem();
  grid_item->set_view(day_label);

  d->m_header_view->insert(grid_item);
}

CalendarView::CalendarView(QGraphicsObject *parent)
    : UIKit::Widget(parent), d(new PrivateCalendarWidget) {
  d->m_calendar_base_frame = new QGraphicsWidget(this);
  d->m_calendar_base_frame->setMinimumSize(QSizeF(300, 128));
  d->m_calendar_base_frame->setGeometry(0.0, 0.0, 300.0, 128);
  d->m_calendar_base_frame->setPos(0.0, 0.0);

  d->m_toolbar = new UIKit::ToolBar(this);
  d->m_toolbar->set_icon_resolution("hdpi");
  d->m_toolbar->set_icon_size(QSize(24, 24));

  d->mYearLable = new UIKit::Label(d->m_toolbar);
  d->mYearLable->setMinimumSize(QSize(220, 24));
  d->mYearLable->set_size(QSize(220, 24));

  d->mYearLable->set_label_style(Qt::transparent, Qt::white);
  d->mYearLable->set_font_size(14);

  d->mYearLable->set_label(
      QString("%1 %2")
          .arg(QDate::longMonthName(QDate::currentDate().month()))
          .arg(QDate::currentDate().year()));

  d->m_toolbar->add_action("Previous", "actions/pd_previous", false);
  d->m_toolbar->insert_widget(d->mYearLable);
  d->m_toolbar->add_action("Next", "actions/pd_next", false);
  d->m_toolbar->setGeometry(QRectF(0, 0, 380, 24));
  d->m_toolbar->show();

  d->m_toolbar->on_item_activated([this](const QString &a_action) {
      qDebug() << Q_FUNC_INFO << a_action;
      if (a_action == "Next")
        next();

      if (a_action == "Previous")
        previous();
  });

  d->m_content_widget = new UIKit::Widget(d->m_calendar_base_frame);
  d->m_content_widget->setGeometry(QRectF(0, 0, 300, 480));

  d->m_current_date.setDate(QDate::currentDate().year(),
                          QDate::currentDate().month(), 1);
  /*header */
  d->m_header_view = new UIKit::ItemView(d->m_content_widget,
                                         UIKit::ItemView::kGridModel);
  d->m_header_view->setGeometry(QRectF(0, 0, 300, 48));
  d->m_header_view->set_view_geometry(QRectF(0, 0, 300, 48));
  d->m_header_view->setPos(0, d->m_toolbar->frame_geometry().height());

  // data
  d->m_cell_view =
      new UIKit::ItemView(d->m_content_widget, UIKit::ItemView::kGridModel);
  d->m_cell_view->set_view_geometry(QRectF(0, 0, 300 - 20, 480));
  d->m_cell_view->setPos(0.0,
                       d->m_toolbar->frame_geometry().height() +
                       d->m_header_view->boundingRect().height());

  add_weekday_header(7);
  for (int i = 1; i <= 6; i++) {
    add_weekday_header(i);
  }

  for (int i = 0; i < 43; i++) {
      UIKit::Label *day_label = new UIKit::Label(d->m_cell_view);
      //day_label->set_label(QString("%1").arg(i));
      day_label->set_size(QSizeF(32, 32));
      day_label->setMinimumSize(QSizeF(32, 32));
      UIKit::ModelViewItem *grid_item = new UIKit::ModelViewItem();
      grid_item->set_view(day_label);

      d->m_cell_view->insert(grid_item);

      day_label->on_input_event([=](UIKit::Widget::InputEvent a_event,
                                const UIKit::Widget *a_widget) {
        if (a_event == UIKit::Widget::kMouseReleaseEvent) {
            if (day_label) {
              reset();
              day_label->set_highlight(1);
            }
        }
      });
   }

  this->set_date(d->m_current_date);
}

CalendarView::~CalendarView() { delete d; }

void CalendarView::clear() {
  for (int i = 0; i < 43; i++) {
    UIKit::ModelViewItem *com = d->m_cell_view->at(i);

    if (!com) {
      continue;
    }

    UIKit::Label *cell = qobject_cast<UIKit::Label *>(com->view());

    if (!cell) {
      continue;
    }

    cell->set_label("");
  }
}

void CalendarView::reset() {
  for (int i = 0; i < 43; i++) {
    UIKit::ModelViewItem *com = d->m_cell_view->at(i);

    if (!com) {
      continue;
    }

    UIKit::Label *cell = qobject_cast<UIKit::Label *>(com->view());

    if (!cell) {
      continue;
    }

    cell->set_highlight(0);
  }
}


QDate CalendarView::a_date() const { return d->m_current_date; }

void CalendarView::set_date(const QDate &date) {
  clear();
  for (int s = date.dayOfWeek(); s < 43; s++) {
    UIKit::ModelViewItem *com = d->m_cell_view->at(s);

    if (!com) {
      continue;
    }

    UIKit::Label *cell = qobject_cast<UIKit::Label *>(com->view());

    if (!cell) {
      continue;
    }

    int value = s - date.dayOfWeek()  + 1 ;
    if (value > date.daysInMonth()) {
      continue;
    }

    cell->set_label(QString("%1").arg(value));
  }
}

void CalendarView::next() {
  int currentMonth = d->m_current_date.month();
  int currentYear = d->m_current_date.year();
  int currentDay = d->m_current_date.day();

  if ((currentMonth + 1) > 12) {
    currentMonth = 1;
    currentYear += 1;
  } else {
    currentMonth += 1;
  }

  d->m_current_date.setDate(currentYear, currentMonth, currentDay);
  update();

  d->mYearLable->set_label(
      QString("%1 %2").arg(QDate::longMonthName(d->m_current_date.month())).arg(
          d->m_current_date.year()));
  set_date(d->m_current_date);
}

void CalendarView::previous() {
  int currentMonth = d->m_current_date.month();
  int currentYear = d->m_current_date.year();
  int currentDay = d->m_current_date.day();

  if ((currentMonth - 1) < 1) {
    currentMonth = 12;
    currentYear -= 1;
  } else {
    currentMonth -= 1;
  }

  d->m_current_date.setDate(currentYear, currentMonth, currentDay);
  update();
  d->mYearLable->set_label(
      QString("%1 %2").arg(QDate::longMonthName(d->m_current_date.month())).arg(
          d->m_current_date.year()));
  set_date(d->m_current_date);
}

void CalendarView::paint_view(QPainter *painter, const QRectF &rect) {
  painter->save();

  painter->setRenderHint(QPainter::Antialiasing);
  painter->setRenderHint(QPainter::HighQualityAntialiasing);
  painter->setRenderHint(QPainter::SmoothPixmapTransform);

  painter->restore();
}
}
