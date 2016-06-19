#include "basic_event.h"
#include "event_date.h"

class basic_event::basic_event_context {
public:
  basic_event_context() {}
  ~basic_event_context() {}

  std::string m_event_title;
  std::string m_event_note;

	event_date m_date;
};

basic_event::basic_event() : ctx(new basic_event_context) {}

basic_event::basic_event(const basic_event &a_copy)
    : ctx(new basic_event_context) {}
basic_event::~basic_event() { delete ctx; }
