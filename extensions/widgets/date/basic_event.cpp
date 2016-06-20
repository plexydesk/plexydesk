#include "basic_event.h"
#include "event_date.h"
#include "event_time.h"

class basic_event::basic_event_context {
public:
  basic_event_context() {}
  ~basic_event_context() {}

  std::string m_event_title;
  std::string m_event_note;

  event_date m_date;
  event_time m_start_time;
  event_time m_end_time;
};

basic_event::basic_event() : ctx(new basic_event_context) {}

basic_event::basic_event(const basic_event &a_copy)
    : ctx(new basic_event_context) {}
basic_event::~basic_event() { delete ctx; }

void basic_event::set_note(const std::string &a_note) {
  ctx->m_event_note = a_note;
}

std::string basic_event::note() const { return ctx->m_event_note; }

void basic_event::set_title(const std::string &a_title) {
  ctx->m_event_title = a_title;
}

std::string basic_event::title() const { return ctx->m_event_title; }

void basic_event::set_date(const event_date &a_date) { ctx->m_date = a_date; }

event_date basic_event::date() const { return ctx->m_date; }

void basic_event::set_start_time(const event_time &a_time) {
  ctx->m_start_time = a_time;
}

event_time basic_event::start_time() const { return ctx->m_start_time; }

void basic_event::set_end_time(const event_time &a_time) {
  ctx->m_end_time = a_time;
}

event_time basic_event::end_time() const { return ctx->m_end_time; }
