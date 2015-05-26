#include "calendarwidget.h"
#include <QGraphicsLinearLayout>
#include <QGraphicsWidget>
#include <imagebutton.h>
#include <imageview.h>
#include <label.h>
#include <tableview.h>
#include <toolbar.h>
#include <themepackloader.h>
#include "datecell.h"
#include "datecellfactory.h"
#include <components/dialwidget.h>

class CalendarWidget::PrivateCalendarWidget {
public:
  PrivateCalendarWidget() {}
  ~PrivateCalendarWidget() {}

  QGraphicsWidget *mBaseTooloBarWidget;

  UIKit::ToolBar *m_toolbar;

  UIKit::ImageButton *mNextBtn;
  UIKit::ImageButton *mPrevBtn;
  UIKit::ImageButton *mOkBtn;
  UIKit::Label *mYearLable;

  UIKit::Widget *mFrame;
  UIKit::TableView *mDayTable;
  UIKit::TableView *mHeaderTable;
  QImage mBackgroundImage;
  QDate mCurrentDate;

  UIKit::DialWidget *mClockHourWidget;
  UIKit::DialWidget *mClockMinWidget;
  UIKit::Label *mTimeLable;

  int mHour;
  int mMin;

  QString mHourLable;
  QString mMinLable;
};

CalendarWidget::CalendarWidget(QGraphicsObject *parent)
    : UIKit::Widget(parent), d(new PrivateCalendarWidget) {


  d->mHour = 0;
  d->mMin = 0;

  d->mHourLable = "00";
  d->mMinLable = "00";

  d->mBaseTooloBarWidget = new QGraphicsWidget(this);
  d->mBaseTooloBarWidget->setMinimumSize(QSizeF(300, 128));
  d->mBaseTooloBarWidget->setGeometry(0.0, 0.0, 300.0, 128);
  d->mBaseTooloBarWidget->setPos(300.0, 0.0);

  d->mYearLable = new UIKit::Label(d->mBaseTooloBarWidget);
  d->mYearLable->setMinimumSize(QSize(220, 24));
  d->mYearLable->set_size(QSize(220, 24));

  d->mYearLable->set_label_style(Qt::transparent, Qt::white);
  d->mYearLable->set_font_size(14);

  d->mYearLable->set_label(
      QString("%1 %2")
          .arg(QDate::longMonthName(QDate::currentDate().month()))
          .arg(QDate::currentDate().year()));

  d->m_toolbar = new UIKit::ToolBar(d->mBaseTooloBarWidget);
  d->m_toolbar->set_icon_resolution("mdpi");
  d->m_toolbar->set_icon_size(QSize(24, 24));

  d->m_toolbar->add_action("Previous", "pd_prev_icon_white", false);
  d->m_toolbar->insert_widget(d->mYearLable);
  d->m_toolbar->add_action("Next", "pd_next_icon_white", false);
  d->m_toolbar->setGeometry(QRectF(0, 0, 380, 48));
  d->m_toolbar->show();

  d->m_toolbar->on_item_activated([this](const QString &a_action) {
      qDebug() << Q_FUNC_INFO << a_action;
      if (a_action == "Next")
        onNextClicked();

      if (a_action == "Previous")
        onPrevClicked();
  });

  d->mFrame = new UIKit::Widget(this);
  d->mFrame->setGeometry(QRectF(0, 0, 300, 480));

  d->mCurrentDate.setDate(QDate::currentDate().year(),
                          QDate::currentDate().month(), 1);
  /*header */
  d->mHeaderTable = new UIKit::TableView(d->mFrame);
  d->mHeaderTable->setGeometry(QRectF(0, 0, 300, 48));
  d->mHeaderTable->setPos(0, 24);
  DateCellFactory *headerList = new DateCellFactory(this);
  headerList->setHeaderMode(true);
  d->mHeaderTable->set_model(headerList);

  // data
  DateCellFactory *factory = new DateCellFactory(this);

  d->mDayTable = new UIKit::TableView(d->mFrame);
  d->mDayTable->setGeometry(QRectF(0, 0, 300, 480));
  factory->setLabelVisibility(true);

  d->mDayTable->set_model(factory);
  d->mFrame->setPos(300.0, 48.0);
  d->mDayTable->setPos(0.0, 48.0);

  headerList->addDataItem(QDate::shortDayName(7), QPixmap(), true);

  for (int i = 1; i <= 6; i++) {
    headerList->addDataItem(QDate::shortDayName(i), QPixmap(), true);
  }

  // clearTableValues(factory);
  for (int i = 0; i < 43; i++) {
    factory->addDataItem("", QPixmap(), true);
  }

  this->changeDate(d->mCurrentDate);

  d->mClockHourWidget = new UIKit::DialWidget(this);
  d->mClockHourWidget->setGeometry(QRectF(0, 0, 300, 300));
  d->mClockHourWidget->show();

  d->mClockMinWidget = new UIKit::DialWidget(this);
  d->mClockMinWidget->setGeometry(QRectF(0, 0, 150, 150));
  d->mClockMinWidget->setPos(70, 70 + 48);
  d->mClockHourWidget->setPos(0.0, 48);

  d->mClockMinWidget->set_maximum_dial_value(60);

  d->mTimeLable = new UIKit::Label(this);
  d->mTimeLable->setMinimumSize(QSizeF(300.0, 50));
  d->mTimeLable->set_size(QSizeF(290.0, 50));
  d->mTimeLable->set_label_style(QColor("#f0f0f0"), QColor(81, 81, 81));
  d->mTimeLable->set_label(QString(d->mHourLable + ":" + d->mMinLable));
  d->mTimeLable->setPos(0.0, d->mClockHourWidget->boundingRect().height() + 32);
  d->mTimeLable->set_font_size(32);

  d->mOkBtn = new UIKit::ImageButton(this);
  d->mOkBtn->set_pixmap(
      UIKit::Theme::instance()->drawable("pd_arrow_icon.png", "mdpi"));
  d->mOkBtn->set_size(QSize(48, 48));
  d->mOkBtn->setMinimumSize(QSize(48, 48));
  d->mOkBtn->setGeometry(QRectF(0, 0, 48, 48));
  d->mOkBtn->show();
  d->mOkBtn->setPos(600 - 52, 320);
  d->mOkBtn->setZValue(1000);

  connect(d->mClockHourWidget, SIGNAL(value(float)), this,
          SLOT(onHourValueChanged(float)));
  connect(d->mClockMinWidget, SIGNAL(value(float)), this,
          SLOT(onMinValueChanged(float)));
  connect(d->mOkBtn, SIGNAL(clicked()), this, SLOT(onOkButtonClicked()));

  d->mOkBtn->on_input_event([this](UIKit::Widget::InputEvent a_event,
                            const UIKit::Widget *a_widget) {
    if (a_event == UIKit::Widget::kMouseReleaseEvent)
      onOkButtonClicked();
  });

  for (int s = 0; s < 43; s++) {
    DateCellFactory *_delegate =
        dynamic_cast<DateCellFactory *>(d->mDayTable->model());

    TableViewItem *com = _delegate->itemAt(s);

    if (!com) {
      continue;
    }

    connect(com, SIGNAL(clicked(TableViewItem *)), this, SLOT(onCellClicked()));
  }
}

CalendarWidget::~CalendarWidget() { delete d; }

void CalendarWidget::clearTableValues() {
  for (int i = 0; i < 43; i++) {
    DateCellFactory *_delegate =
        dynamic_cast<DateCellFactory *>(d->mDayTable->model());

    TableViewItem *com = _delegate->itemAt(i);

    if (!com) {
      continue;
    }

    DateCell *cell = qobject_cast<DateCell *>(com);

    if (!cell) {
      continue;
    }

    qDebug() << Q_FUNC_INFO << "Blank Label;";
    cell->setLabel("");
  }
}

QDate CalendarWidget::currentDate() const { return d->mCurrentDate; }

uint CalendarWidget::currentMinute() const {
  return d->mClockMinWidget->current_dial_value();
}

uint CalendarWidget::currentHour() const {
  return d->mClockHourWidget->current_dial_value();
}

void CalendarWidget::setBackgroundImage(const QImage &img) {
  d->mBackgroundImage = img;
  update();
}

void CalendarWidget::changeDate(const QDate &date) {
  clearTableValues();
  for (int s = date.dayOfWeek(); s < 43; s++) {
    DateCellFactory *_delegate =
        qobject_cast<DateCellFactory *>(d->mDayTable->model());

    TableViewItem *com = _delegate->itemAt(s);

    if (!com) {
      continue;
    }

    DateCell *cell = qobject_cast<DateCell *>(com);

    if (!cell) {
      continue;
    }

    int value = s - date.dayOfWeek() + 1;
    if (value > date.daysInMonth()) {
      continue;
    }

    qDebug() << Q_FUNC_INFO << "Value of date:;" << value;
    cell->setLabel(QString("%1").arg(value));
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
  d->mHour = value;

  if (value < 10) {
    d->mHourLable = QString("0%1").arg(value);
  } else {
    d->mHourLable = QString("%1").arg(value);
  }

  d->mTimeLable->set_label(QString(d->mHourLable + ":" + d->mMinLable));
}

void CalendarWidget::onMinValueChanged(float value) {
  d->mMin = value;

  if (value < 10) {
    d->mMinLable = QString("0%1").arg(value);
  } else {
    d->mMinLable = QString("%1").arg(value);
  }

  d->mTimeLable->set_label(QString(d->mHourLable + ":" + d->mMinLable));
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

void CalendarWidget::onOkButtonClicked() { Q_EMIT done(); }

void CalendarWidget::paint_view(QPainter *painter, const QRectF &rect) {
  painter->save();

  painter->setRenderHint(QPainter::Antialiasing);
  painter->setRenderHint(QPainter::HighQualityAntialiasing);
  painter->setRenderHint(QPainter::SmoothPixmapTransform);

  // QRectF backgroundRect(300.0, d->mNextBtn->boundingRect().height(), 300.0,
  // 440 - d->mNextBtn->boundingRect().height());

  if (!d->mBackgroundImage.isNull()) {
    painter->drawImage(rect, d->mBackgroundImage);

  } else {
    // painter->fillRect(rect, QColor("#79BCD3"));
    // painter->fillRect(rect, QColor("#79BCD3"));
    // painter->fillRect(rect, QColor("#F28585")) // pink;
    // painter->fillRect(rect, QColor("#A6C024"));
    // painter->fillRect(rect, QColor("#02C2B5"));// green
    painter->fillRect(QRectF(0.0, 0.0, 300.0, rect.height()),
                      QColor("#f0f0f0")); // green
  }

  QRectF dateRect(300.0, 0.0, 300.0, rect.height());

  painter->save();
  painter->setOpacity(1.0);
  painter->fillRect(dateRect, QColor(100, 100, 100));
  painter->restore();

  // painter->setOpacity(1.0);
  // painter->fillRect(backgroundRect, QColor("#F28585"));

  painter->restore();
}
