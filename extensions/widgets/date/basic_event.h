#ifndef CK_BASIC_EVENT_H
#define CK_BASIC_EVENT_H

#include <string>

class event_time;
class event_date;

class basic_event {
public:
  basic_event();
  basic_event(const basic_event &a_copy);
  virtual ~basic_event();

  void set_note(const std::string &a_note);
  std::string note() const;

  void set_title(const std::string &a_title);
  std::string title() const;

  void set_date(const event_date &a_date);
  event_date date() const;

  void set_start_time(const event_time &a_time);
  event_time start_time() const;

  void set_end_time(const event_time &a_time);
  event_time end_time() const;

private:
  class basic_event_context;
  basic_event_context *const ctx;
};
#endif // CK_BASIC_EVENT_H
