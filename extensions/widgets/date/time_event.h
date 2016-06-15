#ifndef TIME_EVENT_H
#define TIME_EVENT_H

#include <string>
#include <vector>

class time_event {
public:
  time_event() {}
  virtual ~time_event() {}

  int duration() const;
  void set_duration(int duration);

  std::string name() const;
  void set_name(const std::string &name);

  std::string note() const;
  void set_note(const std::string &note);

  std::string start_time() const;
  void set_start_time(const std::string &start_time);

private:
  int m_duration;
  std::string m_name;
  std::string m_note;
  std::string m_start_time;
};

typedef std::vector<time_event> event_list_t;

#endif // TIME_EVENT_H
