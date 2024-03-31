#include "timers_for_func.h"

void set_timer_for_function(void (*foo)(), int time, unsigned long *counter) {
  if(millis() - *counter > time) {
    *counter = millis();
    foo();
  }
}
