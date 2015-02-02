#ifndef CALENDARWIDGET_H
#define CALENDARWIDGET_H

#include <desktopwidget.h>

class CalendarWidget : public UI::UIWidget {
  Q_OBJECT

public:
  CalendarWidget(QGraphicsObject *parent = 0);

  virtual ~CalendarWidget();

  void setBackgroundImage(const QImage &img);

  void changeDate(const QDate &date);

  void clearTableValues();

  QDate currentDate() const;

  uint currentMinute() const;

  uint currentHour() const;

Q_SIGNALS:
  void done();

private Q_SLOTS:
  void onNextClicked();
  void onPrevClicked();
  void onHourValueChanged(float value);
  void onMinValueChanged(float value);
  void onCellClicked();
  void onOkButtonClicked();

protected:
  virtual void paintView(QPainter *painter, const QRectF &rect);

private:
  class PrivateCalendarWidget;
  PrivateCalendarWidget *const d;
};

#endif // CALENDARWIDGET_H
