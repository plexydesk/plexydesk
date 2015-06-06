#include "calendarwidget.h"
#include <QGraphicsLinearLayout>
#include <QGraphicsWidget>
#include <imagebutton.h>
#include <imageview.h>
#include <label.h>
#include <tableview.h>
#include <toolbar.h>
#include <themepackloader.h>
#include <item_view.h>
#include <button.h>
#include "datecell.h"
#include "datecellfactory.h"
#include <components/dialwidget.h>

class CalendarWidget::PrivateCalendarWidget {
public:
  PrivateCalendarWidget() {}
  ~PrivateCalendarWidget() {}

  QGraphicsWidget *m_calendar_base_frame;

  UIKit::ToolBar *m_toolbar;

  UIKit::ImageButton *mNextBtn;
  UIKit::ImageButton *mPrevBtn;
  UIKit::Label *mYearLable;

  UIKit::Widget *mFrame;
  UIKit::ItemView *m_header_view;
  UIKit::ItemView *m_cell_view;

  QImage mBackgroundImage;
  QDate mCurrentDate;
};

void CalendarWidget::add_weekday_header(int i)
{
  UIKit::Label *day_label = new UIKit::Label(d->m_header_view);
  day_label->set_label(QDate::shortDayName(i));
  day_label->set_size(QSizeF(32, 32));
  day_label->setMinimumSize(QSizeF(32, 32));
  UIKit::ModelViewItem *grid_item = new UIKit::ModelViewItem();
  grid_item->set_view(day_label);

  d->m_header_view->insert(grid_item);
}

CalendarWidget::CalendarWidget(QGraphicsObject *parent)
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
        onNextClicked();

      if (a_action == "Previous")
        onPrevClicked();
  });

  d->mFrame = new UIKit::Widget(d->m_calendar_base_frame);
  d->mFrame->setGeometry(QRectF(0, 0, 300, 480));

  d->mCurrentDate.setDate(QDate::currentDate().year(),
                          QDate::currentDate().month(), 1);
  /*header */
  d->m_header_view = new UIKit::ItemView(d->mFrame,
                                         UIKit::ItemView::kGridModel);
  d->m_header_view->setGeometry(QRectF(0, 0, 300, 48));
  d->m_header_view->set_view_geometry(QRectF(0, 0, 300, 48));
  d->m_header_view->setPos(0, d->m_toolbar->frame_geometry().height());

  // data
  d->m_cell_view =
      new UIKit::ItemView(d->mFrame, UIKit::ItemView::kGridModel);
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
              clear_highlight();
              day_label->set_highlight(1);
            }
        }
      });
   }

  this->changeDate(d->mCurrentDate);
}

CalendarWidget::~CalendarWidget() { delete d; }

void CalendarWidget::clearTableValues() {
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

void CalendarWidget::clear_highlight() {
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


QDate CalendarWidget::currentDate() const { return d->mCurrentDate; }

void CalendarWidget::setBackgroundImage(const QImage &img) {
  d->mBackgroundImage = img;
  update();
}

void CalendarWidget::changeDate(const QDate &date) {
  clearTableValues();
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

void CalendarWidget::onNextClicked() {
  int currentMonth = d->mCurrentDate.month();
  int currentYear = d->mCurrentDate.year();
  int currentDay = d->mCurrentDate.day();

  if ((currentMonth + 1) > 12) {
    currentMonth = 1;
    currentYear += 1;
  } else {
    currentMonth += 1;
  }

  d->mCurrentDate.setDate(currentYear, currentMonth, currentDay);
  update();

  d->mYearLable->set_label(
      QString("%1 %2").arg(QDate::longMonthName(d->mCurrentDate.month())).arg(
          d->mCurrentDate.year()));
  changeDate(d->mCurrentDate);
}

void CalendarWidget::onPrevClicked() {
  int currentMonth = d->mCurrentDate.month();
  int currentYear = d->mCurrentDate.year();
  int currentDay = d->mCurrentDate.day();

  if ((currentMonth - 1) < 1) {
    currentMonth = 12;
    currentYear -= 1;
  } else {
    currentMonth -= 1;
  }

  d->mCurrentDate.setDate(currentYear, currentMonth, currentDay);
  update();
  d->mYearLable->set_label(
      QString("%1 %2").arg(QDate::longMonthName(d->mCurrentDate.month())).arg(
          d->mCurrentDate.year()));
  changeDate(d->mCurrentDate);
}

void CalendarWidget::onHourValueChanged(float value) {
}

void CalendarWidget::onMinValueChanged(float value) {
}

void CalendarWidget::onCellClicked() {
  if (sender()) {
    DateCell *cell = qobject_cast<DateCell *>(sender());
    if (cell) {
      qDebug() << Q_FUNC_INFO << cell->label().toInt();
      if (cell->label().toInt() > 0) {
        d->mCurrentDate.setDate(d->mCurrentDate.year(), d->mCurrentDate.month(),
                                cell->label().toInt());
      }
    }
  }
}

void CalendarWidget::onOkButtonClicked() {

}

void CalendarWidget::paint_view(QPainter *painter, const QRectF &rect) {
  painter->save();

  painter->setRenderHint(QPainter::Antialiasing);
  painter->setRenderHint(QPainter::HighQualityAntialiasing);
  painter->setRenderHint(QPainter::SmoothPixmapTransform);

  if (!d->mBackgroundImage.isNull()) {
    painter->drawImage(rect, d->mBackgroundImage);
  }

  painter->restore();
}
