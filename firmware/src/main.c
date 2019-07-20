/*******************************************************************************
  Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This file contains the "main" function for a project.

  Description:
    This file contains the "main" function for a project.  The
    "main" function calls the "SYS_Initialize" function to initialize the state
    machines of all modules in the system
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes
#include "startup.h"
#include "satellite.h"
#include "led.h"
#include "timers.h"
#include "uart.h"

// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

int main(void) {
    /* Initialize all modules */
    SYS_Initialize(NULL);
    DetectStartupMode();
    DetectConnectedSatellites();
    if (startupMode == START_BIND) {
        SendBindPulses(DSMX_11);
    }
    GPIO_Initialize();
    if (startupMode == START_SERIAL) {
        while(true);
        //TODO Implement serial main loop
    }
    int blinks = 5;
    if (startupMode == START_BIND) {
        blinks = 10;
    }
    for (int i = 0; i < blinks; ++i) {
        LED3On();
        delay_us(200000);
        LED3Off();
        delay_us(200000);
    }
//    if (connectedSatellites[SAT1]) {
//        LED1On();
//    }
//    if (connectedSatellites[SAT2]) {
//        LED2On();
//    }
//    if (connectedSatellites[SAT3]) {
//        LED3On();
//    }
    __builtin_set_isr_state(0);
    __builtin_enable_interrupts();
    startSystemTickTimer();
    startOCTimer(PERIOD_11MS);
    //initUARTs();
    while (systemTickCount < 10000);
    while (true) {
        if (systemTickCount < 30000) {
            LED1On();
        } else {
            LED1Off();
        }
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE);
}


/*******************************************************************************
 End of File
 */

