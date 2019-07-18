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

// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

int main ( void )
{
    /* Initialize all modules */
    SYS_Initialize ( NULL );
    DetectStartupMode();
    DetectConnectedSatellites();
    GPIO_Initialize();
    if (activeSatellites[SAT1]) {
        LED1On();
    }
    if (activeSatellites[SAT2]) {
        LED2On();
    }
    if (activeSatellites[SAT3]) {
        LED3On();
    }
    while ( true )
    {
//        int i;
//        for (i = 0; i < 30000000; ++i);
//        LATEbits.LATE13 ^= 1;
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

