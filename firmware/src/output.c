#include <xc.h>
#include "output.h"
#include "timers.h"

const unsigned int channelStartOffsets[MAX_CHANNEL] = {0, 2097 * US_COUNT,
    0, 2097 * US_COUNT, 0, 2097 * US_COUNT, 0, 2097 * US_COUNT,
    2 * 2097 * US_COUNT, 2 * 2097 * US_COUNT, 2 * 2097 * US_COUNT, 
    2 * 2097 * US_COUNT};

void initOutputs(void) {
    //Ch 0 - Throttle
    OC11CONbits.OC32 = 1;
    OC11CONbits.OCM = 0b101;
    OC11R = 0;
    OC11RS = (903 + ((1194 * 1024)) / 2048) * US_COUNT;
    //Ch 4 - Gear
    OC2CONbits.OC32 = 1;
    OC2CONbits.OCM = 0b101;
    OC2R = 0;
    OC2RS = (903 + ((1194 * 1024)) / 2048) * US_COUNT;

    OC2CONbits.ON = 1;
    OC11CONbits.ON = 1;
}
