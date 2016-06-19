#ifndef CK_BASIC_EVENT_H
#define CK_BASIC_EVENT_H

#include <string>

class basic_event {
public:
  basic_event();
  basic_event(const basic_event &a_copy);

  virtual ~basic_event();

private:
  class basic_event_context;
  basic_event_context *const ctx;
};
#endif // CK_BASIC_EVENT_H
