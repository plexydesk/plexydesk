#include "calendarwidget.h"
#include <QGraphicsLinearLayout>
#include <QGraphicsWidget>
#include <imagebutton.h>
#include <imageview.h>
#include <label.h>
#include <tableview.h>
#include <themepackloader.h>
#include "datecell.h"
#include "datecellfactory.h"
#include <components/dialwidget.h>

class CalendarWidget::PrivateCalendarWidget
{
public:
  PrivateCalendarWidget() {}
  ~PrivateCalendarWidget() {}

  QGraphicsWidget *mBaseTooloBarWidget;
  QGraphicsLinearLayout *mLayout;

  UI::ImageButton *mNextBtn;
  UI::ImageButton *mPrevBtn;
  UI::ImageButton *mOkBtn;
  UI::Label *mYearLable;

  UI::UIWidget *mFrame;
  UI::TableView *mDayTable;
  UI::TableView *mHeaderTable;
  QImage mBackgroundImage;
  QDate mCurrentDate;

  UI::DialWidget *mClockHourWidget;
  UI::DialWidget *mClockMinWidget;
  UI::Label *mTimeLable;

  int mHour;
  int mMin;

  QString mHourLable;
  QString mMinLable;
};

CalendarWidget::CalendarWidget(QGraphicsObject *parent)
  : UI::UIWidget(parent), d(new PrivateCalendarWidget)
{

  d->mHour = 0;
  d->mMin = 0;

  d->mHourLable = "00";
  d->mMinLable = "00";

  d->mBaseTooloBarWidget = new QGraphicsWidget(this);

  d->mBaseTooloBarWidget->setGeometry(0.0, 0.0, 300.0, 48);
  d->mBaseTooloBarWidget->setPos(300.0, 0.0);

  d->mLayout = new QGraphicsLinearLayout(d->mBaseTooloBarWidget);
  d->mLayout->setOrientation(Qt::Horizontal);

  d->mNextBtn = new UI::ImageButton(d->mBaseTooloBarWidget);
  d->mPrevBtn = new UI::ImageButton(d->mBaseTooloBarWidget);

  d->mNextBtn->setLable("Next");
  d->mNextBtn->setPixmap(
    UI::Theme::instance()->drawable("pd_next_icon_white.png", "mdpi"));

  d->mPrevBtn->setLable("Previous");
  d->mPrevBtn->setPixmap(
    UI::Theme::instance()->drawable("pd_prev_icon_white.png", "mdpi"));

  d->mPrevBtn->setSize(QSize(24, 24));
  d->mNextBtn->setSize(QSize(24, 24));

  d->mYearLable = new UI::Label(d->mBaseTooloBarWidget);
  d->mYearLable->setSize(QSize(258 - (d->mNextBtn->boundingRect().width() * 2),
                               d->mNextBtn->boundingRect().height()));
  d->mYearLable->setLabelStyle(Qt::transparent, Qt::white);
  d->mYearLable->setFontSize(14);
  d->mYearLable->setLabel(
    QString("%1 %2")
    .arg(QDate::longMonthName(QDate::currentDate().month()))
    .arg(QDate::currentDate().year()));

  d->mLayout->addItem(d->mPrevBtn);
  d->mLayout->addItem(d->mYearLable);
  d->mLayout->addItem(d->mNextBtn);

  connect(d->mNextBtn, SIGNAL(clicked()), this, SLOT(onNextClicked()));
  connect(d->mPrevBtn, SIGNAL(clicked()), this, SLOT(onPrevClicked()));

  d->mFrame = new UI::UIWidget(this);
  d->mFrame->setWindowFlag(UI::Window::kRenderBackground, false);
  d->mFrame->setFlag(QGraphicsItem::ItemIsMovable, false);
  this->setFlag(QGraphicsItem::ItemIsMovable, false);

  d->mCurrentDate.setDate(QDate::currentDate().year(),
                          QDate::currentDate().month(), 1);

  /*header */
  d->mHeaderTable = new UI::TableView(d->mFrame);
  DateCellFactory *headerList = new DateCellFactory(this);
  headerList->setHeaderMode(true);
  d->mHeaderTable->setModel(headerList);

  // data
  DateCellFactory *factory = new DateCellFactory(this);

  QRectF _day_table_geometry;

  /*
  _day_table_geometry.setHeight(rect.height());
  _day_table_geometry.setWidth(rect.width() / 2);
  _day_table_geometry.setX(0.0);
  _day_table_geometry.setY(0.0);
  */

  d->mDayTable = new UI::TableView(d->mFrame);
  factory->setLabelVisibility(true);

  d->mDayTable->setModel(factory);
  d->mFrame->setPos(300.0, d->mNextBtn->boundingRect().height() + 24);
  d->mDayTable->setPos(0.0, 28.0);

  headerList->addDataItem(QDate::shortDayName(7), QPixmap(), true);

  for (int i = 1; i <= 6; i++) {
    headerList->addDataItem(QDate::shortDayName(i), QPixmap(), true);
  }

  //
  // clearTableValues(factory);
  for (int i = 0; i < 43; i++) {
    factory->addDataItem("", QPixmap(), true);
  }

  this->changeDate(d->mCurrentDate);

  d->mClockHourWidget = new UI::DialWidget(this);
  d->mClockHourWidget->show();

  d->mClockMinWidget = new UI::DialWidget(this);

  d->mClockMinWidget->setPos(70, 70 + 48);
  d->mClockHourWidget->setPos(0.0, 48);

  d->mClockMinWidget->setMaxValue(60);
  d->mClockMinWidget->show();

  d->mTimeLable = new UI::Label(this);
  d->mTimeLable->setSize(QSizeF(300.0, 50));
  d->mTimeLable->setLabelStyle(Qt::white, QColor("#F28585"));
  // d->mTimeLable->setLabelStyle(Qt::white, QColor("#79BCD3"));
  d->mTimeLable->setLabel(QString(d->mHourLable + ":" + d->mMinLable));
  d->mTimeLable->setPos(0.0, d->mClockHourWidget->boundingRect().height() + 32);
  d->mTimeLable->setFontSize(32);
  d->mTimeLable->show();

  d->mOkBtn = new UI::ImageButton(this);
  d->mOkBtn->setPixmap(
    UI::Theme::instance()->drawable("pd_arrow_icon.png", "mdpi"));
  d->mOkBtn->setSize(QSize(48, 48));
  d->mOkBtn->setPos(this->boundingRect().width() -
                    d->mOkBtn->boundingRect().width(),
                    d->mTimeLable->pos().y() + 10);
  d->mOkBtn->show();

  connect(d->mClockHourWidget, SIGNAL(value(float)), this,
          SLOT(onHourValueChanged(float)));
  connect(d->mClockMinWidget, SIGNAL(value(float)), this,
          SLOT(onMinValueChanged(float)));
  connect(d->mOkBtn, SIGNAL(clicked()), this, SLOT(onOkButtonClicked()));

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

void CalendarWidget::clearTableValues()
{
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

uint CalendarWidget::currentMinute() const
{
  return d->mClockMinWidget->currentValue();
}

uint CalendarWidget::currentHour() const
{
  return d->mClockHourWidget->currentValue();
}

void CalendarWidget::setBackgroundImage(const QImage &img)
{
  d->mBackgroundImage = img;
  update();
}

void CalendarWidget::changeDate(const QDate &date)
{
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

void CalendarWidget::onNextClicked()
{
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
  d->mYearLable->setLabel(
    QString("%1 %2").arg(QDate::longMonthName(d->mCurrentDate.month())).arg(
      d->mCurrentDate.year()));
  changeDate(d->mCurrentDate);
}

void CalendarWidget::onPrevClicked()
{
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
  d->mYearLable->setLabel(
    QString("%1 %2").arg(QDate::longMonthName(d->mCurrentDate.month())).arg(
      d->mCurrentDate.year()));
  changeDate(d->mCurrentDate);
}

void CalendarWidget::onHourValueChanged(float value)
{
  d->mHour = value;

  if (value < 10) {
    d->mHourLable = QString("0%1").arg(value);
  } else {
    d->mHourLable = QString("%1").arg(value);
  }

  d->mTimeLable->setLabel(QString(d->mHourLable + ":" + d->mMinLable));
}

void CalendarWidget::onMinValueChanged(float value)
{
  d->mMin = value;

  if (value < 10) {
    d->mMinLable = QString("0%1").arg(value);
  } else {
    d->mMinLable = QString("%1").arg(value);
  }

  d->mTimeLable->setLabel(QString(d->mHourLable + ":" + d->mMinLable));
}

void CalendarWidget::onCellClicked()
{
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

void CalendarWidget::paintView(QPainter *painter, const QRectF &rect)
{
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
                      QColor("#ffffff")); // green
  }

  QRectF dateRect(300.0, 0.0, 300.0, rect.height());

  painter->save();
  painter->setOpacity(1.0);
  painter->fillRect(dateRect, QColor("#F28585"));
  painter->restore();

  // painter->setOpacity(1.0);
  // painter->fillRect(backgroundRect, QColor("#F28585"));

  painter->restore();
}
