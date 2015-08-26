#include "ck_timer.h"

namespace cherry_kit {
class timer::private_timer {
public:
  private_timer() {}
  ~private_timer() {}
};

timer::timer(int a_duration) : priv(new private_timer){
}

timer::~timer() {delete priv;}
}
