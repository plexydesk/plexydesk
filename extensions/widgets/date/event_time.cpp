#include "event_time.h"

event_time::event_time() {}

event_time::event_time(const event_time &a_copy) {
  m_hour = a_copy.m_hour;
  m_min = a_copy.m_min;
  m_type = a_copy.m_type;
}

event_time::~event_time() {}

void event_time::set_time(int a_hour, int a_min) {
  m_hour = a_hour;
  m_min = a_min;
}

int event_time::hour() const { return m_hour; }

int event_time::minute() const { return m_min; }

void event_time::set_type(event_time::event_time_t a_type) {
  m_type = a_type;
}

event_time::event_time_t event_time::type() const { return m_type; }
