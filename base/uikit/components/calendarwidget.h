#ifndef CALENDARWIDGET_H
#define CALENDARWIDGET_H

#include <widget.h>

namespace UIKit {
class CalendarView : public UIKit::Widget {
public:
  explicit CalendarView(QGraphicsObject *a_parent_ptr = 0);
  virtual ~CalendarView();

  virtual void set_date(const QDate &a_date);
  virtual QDate a_date() const;

  virtual void next();
  virtual void previous();
protected:
  virtual void clear();
  virtual void reset();

  void add_weekday_header(int i);
  virtual void paint_view(QPainter *painter, const QRectF &rect);
private:
  class PrivateCalendarWidget;
  PrivateCalendarWidget *const d;
};
}

#endif // CALENDARWIDGET_H
