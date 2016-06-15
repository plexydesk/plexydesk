#include "time_event.h"

int time_event::duration() const { return m_duration; }

void time_event::set_duration(int duration) { m_duration = duration; }
std::string time_event::name() const { return m_name; }

void time_event::set_name(const std::string &name) { m_name = name; }
std::string time_event::note() const { return m_note; }

void time_event::set_note(const std::string &note) { m_note = note; }
std::string time_event::start_time() const { return m_start_time; }

void time_event::set_start_time(const std::string &start_time) {
  m_start_time = start_time;
}

