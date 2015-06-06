#ifndef CALENDARWIDGET_H
#define CALENDARWIDGET_H

#include <widget.h>

class CalendarWidget : public UIKit::Widget {
  Q_OBJECT

public:
  CalendarWidget(QGraphicsObject *a_parent_ptr = 0);
  virtual ~CalendarWidget();

  void setBackgroundImage(const QImage &img);

  void changeDate(const QDate &date);
  QDate currentDate() const;

  void add_weekday_header(int i);
private Q_SLOTS:
  void onNextClicked();
  void onPrevClicked();
  void onHourValueChanged(float value);
  void onMinValueChanged(float value);
  void onCellClicked();
  void onOkButtonClicked();

protected:
  virtual void paint_view(QPainter *painter, const QRectF &rect);
  void clearTableValues();

  void clear_highlight();
private:
  class PrivateCalendarWidget;
  PrivateCalendarWidget *const d;
};

#endif // CALENDARWIDGET_H
