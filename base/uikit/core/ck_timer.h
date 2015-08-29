#ifndef CK_TIMER_H
#define CK_TIMER_H

#include <functional>
#include <chrono>

namespace cherry_kit {
class timer {
public:
  timer(int a_duration);
  virtual ~timer();

  virtual void start() const;
  virtual void stop();

  int elapsed_seconds();
  int elapsed_minutes();

  virtual bool is_active() const;

  static void start_once(int duration, std::function<void ()> a_callback);
  virtual void on_timeout(std::function<void()> a_callback);

private:
  class private_timer;
  private_timer * const priv;
};
}
#endif // CK_TIMER_H
