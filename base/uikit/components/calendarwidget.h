#ifndef CALENDARWIDGET_H
#define CALENDARWIDGET_H

#include <widget.h>

namespace CherryKit {
class Widget;

class DECL_UI_KIT_EXPORT CalendarView : public CherryKit::Widget {
public:
  explicit CalendarView(Widget *a_parent_ptr = 0);
  virtual ~CalendarView();

  virtual void set_date(const QDate &a_date);
  virtual QDate a_date() const;

  virtual void set_geometry(float a_x, float a_y, float a_width,
                            float a_height);
  virtual QRectF geometry() const;

  virtual void next();
  virtual void previous();

protected:
  virtual void clear();
  virtual void reset();
  virtual void paint_view(QPainter *painter, const QRectF &rect);

private:
  class PrivateCalendarWidget;
  PrivateCalendarWidget *const o_calendar_widget;
};
}

#endif // CALENDARWIDGET_H
