#ifndef CK_TIMER_H
#define CK_TIMER_H

namespace cherry_kit {
class timer {
public:
  timer(int a_duration);
  virtual ~timer();
private:
  class private_timer;
  private_timer * const priv;
};
}
#endif // CK_TIMER_H
