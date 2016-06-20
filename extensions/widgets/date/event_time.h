#ifndef CK_EVENT_TIME_H
#define CK_EVENT_TIME_H

class event_time {
public:
    typedef enum {
        kPMTime,
        kAMTime,
        kNoon,
        kMidNight
    } event_time_t;

  event_time();
  event_time(const event_time &a_copy);
  virtual ~event_time();

  void set_time(int a_hour, int a_min);
  int hour() const;
  int minute() const;

  void set_type(event_time_t a_type);
  event_time_t type() const;
private:
  int m_hour;
  int m_min;
  event_time::event_time_t m_type;

};

#endif // CK_EVENT_TIME_H
