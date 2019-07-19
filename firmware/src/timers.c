#include <xc.h>
#include "timers.h"

void delay_us(unsigned int us) {
    unsigned int delay;
    delay = US_COUNT * us;
    _CP0_SET_COUNT(0);
    while (_CP0_GET_COUNT() < delay);
}
