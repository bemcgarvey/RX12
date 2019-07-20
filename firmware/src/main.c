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
    initUARTs();
    while (systemTickCount < 100);
    while (true) {
        if (systemTickCount - lastRxTime[SAT1] < 100) {
            LED1On();
        } else {
            LED1Off();
        }
        if (systemTickCount - lastRxTime[SAT2] < 100) {
            LED2On();
        } else {
            LED2Off();
        }
        if (systemTickCount - lastRxTime[SAT3] < 100) {
            LED3On();
        } else {
            LED3Off();
        }
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE);
}


/* Code identifying the cause of the exception (CP0 Cause register). */
static unsigned int _excep_code;

/* Address of instruction that caused the exception. */
static unsigned int _excep_addr;

/* Pointer to the string describing the cause of the exception. */
static char *_cause_str;

/* Array identifying the cause (indexed by _exception_code). */
static char *cause[] =
{
    "Interrupt",
    "Undefined",
    "Undefined",
    "Undefined",
    "Load/fetch address error",
    "Store address error",
    "Instruction bus error",
    "Data bus error",
    "Syscall",
    "Breakpoint",
    "Reserved instruction",
    "Coprocessor unusable",
    "Arithmetic overflow",
    "Trap",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

// </editor-fold>

/*******************************************************************************
  Function:
    void _general_exception_handler ( void )
  Summary:
    Overrides the XC32 _weak_ _generic_exception_handler.
  Description:
    This function overrides the XC32 default _weak_ _generic_exception_handler.
  Remarks:
    Refer to the XC32 User's Guide for additional information.
 */

void _general_exception_handler ( void )
{
    /* Mask off Mask of the ExcCode Field from the Cause Register
    Refer to the MIPs Software User's manual */
    _excep_code = (_CP0_GET_CAUSE() & 0x0000007C) >> 2;
    _excep_addr = _CP0_GET_EPC();

    _cause_str  = cause[_excep_code];
    //printf("\n\rGeneral Exception %s (cause=%d, addr=%x).\n\r", _cause_str, _excep_code, _excep_addr);
    while (1)
    {
        #if defined(__DEBUG) || defined(__DEBUG_D) && defined(__XC32)
            __builtin_software_breakpoint();
        #endif
    }
}

/*******************************************************************************
 End of File
*/


/*******************************************************************************
 End of File
 */

