#ifndef CK_EVENT_DATE_H
#define CK_EVENT_DATE_H

#include <string>

class event_date {
public:
  event_date();
  event_date(const event_date &a_copy);
  ~event_date();

	int year() const;
	void set_year(int a_year);

	int month() const;
	void set_month(int a_month);

	int day() const;
	void set_day(int a_day);

private:
  int m_year;
  int m_month;
  int m_day;

};

#endif // CK_EVENT_DATE_H
