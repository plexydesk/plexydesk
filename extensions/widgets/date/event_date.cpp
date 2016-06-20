#include "event_date.h"

event_date::event_date() {}

event_date::event_date(const event_date &a_copy) {}

event_date::~event_date() {}

int event_date::year() const {
    return m_year;
}

void event_date::set_year(int a_year) {
    m_year = a_year;
}

int event_date::month() const {
    return m_month;
}

void event_date::set_month(int a_month) {
    m_month = a_month;
}

int event_date::day() const {
    return m_day;
}

void event_date::set_day(int a_day) {
    m_day = a_day;
}
