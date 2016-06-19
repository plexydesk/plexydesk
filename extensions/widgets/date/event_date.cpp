#include "event_date.h"

class event_date::event_date_context {
public:
  event_date_context(int a_day, int a_month, int a_year)
      : m_year(a_year), m_month(a_month), m_day(a_day) {}
  ~event_date_context() {}

  int m_year;
  int m_month;
  int m_day;
};

event_date::event_date()
    : ctx(new event_date_context(1900, 0, 0)) {}

event_date::event_date(const event_date &a_copy)
    : ctx(new event_date_context(a_copy.ctx->m_day, a_copy.ctx->m_month,
                                 a_copy.ctx->m_day)) {}

event_date::~event_date() { delete ctx; }

int event_date::year() const {
	return ctx->m_year;
}

void event_date::set_year(int a_year) {
	ctx->m_year = a_year;
}

int event_date::month() const {
	return ctx->m_month;
}

void event_date::set_month(int a_month) {
	ctx->m_month = a_month;
}

int event_date::day() const {
	return ctx->m_day;
}

void event_date::set_day(int a_day) {
	ctx->m_day = a_day;
}
