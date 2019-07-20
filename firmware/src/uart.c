#include <xc.h>
#include "uart.h"
#include <sys/attribs.h>
#include "led.h"
#include "satellite.h"
#include "timers.h"


void initUARTs(void) {
    //SAT1 = UART1
    U1BRG = 129;  //115200 baud
    U1MODEbits.BRGH = 1;
    //TODO Should RUNOVF be set?
    U1STAbits.URXEN = 1;
    IPC9bits.U1RXIP = 1;
    IPC9bits.U1RXIS = 3;
    IFS1bits.U1RXIF = 0;
    IEC1bits.U1RXIE = 1;
    IPC9bits.U1EIP = 2;
    IPC9bits.U1EIS = 3;
    IFS1bits.U1EIF = 0;
    IEC1bits.U1EIE = 1;
    U1STAbits.OERR = 0;
    
    //SAT2 = UART6
    U6BRG = 129;  //115200 baud
    U6MODEbits.BRGH = 1;
    //TODO Should RUNOVF be set?
    U6STAbits.URXEN = 1;
    IPC41bits.U6RXIP = 1;
    IPC41bits.U6RXIS = 2;
    IFS5bits.U6RXIF = 0;
    IEC5bits.U6RXIE = 1;
    IPC41bits.U6EIP = 2;
    IPC41bits.U6EIS = 2;
    IFS5bits.U6EIF = 0;
    IEC5bits.U6EIE = 1;
    U6STAbits.OERR = 0;
    
    //SAT3 = UART3
    U3BRG = 129;  //115200 baud
    U3MODEbits.BRGH = 1;
    //TODO Should RUNOVF be set?
    U3STAbits.URXEN = 1;
    IPC15bits.U3RXIP = 1;
    IPC15bits.U3RXIS = 3;
    IFS1bits.U3RXIF = 0;
    IEC1bits.U3RXIE = 1;
    IPC15bits.U3EIP = 2;
    IPC15bits.U3EIS = 3;
    IFS1bits.U3EIF = 0;
    IEC1bits.U3EIE = 1;
    U3STAbits.OERR = 0;
    
    U1MODEbits.ON = 1;
    U6MODEbits.ON = 1;
    U3MODEbits.ON = 1;
}

void __ISR(_UART1_RX_VECTOR, IPL1SOFT) uart1Isr(void) {
    while (U1STAbits.URXDA == 1) {
        unsigned char data = U1RXREG;
    }
    lastRxTime[SAT1] = systemTickCount;
    IFS1bits.U1RXIF = 0;
}

void __ISR(_UART1_FAULT_VECTOR, IPL2SOFT) uart1ErrorIsr(void) {
    if (U1STAbits.OERR == 1) {
        U1STAbits.OERR = 0;
        //LED3On();
    }
    if (U1STAbits.FERR == 1) {
        //LED2On();
    }
    IFS1bits.U1EIF = 0;
}

void __ISR(_UART6_RX_VECTOR, IPL1SOFT) uart6Isr(void) {
    while (U6STAbits.URXDA == 1) {
        unsigned char data = U6RXREG;
    }
    lastRxTime[SAT2] = systemTickCount;
    IFS5bits.U6RXIF = 0;
}

void __ISR(_UART6_FAULT_VECTOR, IPL2SOFT) uart6ErrorIsr(void) {
    if (U6STAbits.OERR == 1) {
        U6STAbits.OERR = 0;
        //LED3On();
    }
    if (U6STAbits.FERR == 1) {
        //LED2On();
    }
    IFS5bits.U6EIF = 0;
}

void __ISR(_UART3_RX_VECTOR, IPL1SOFT) uart3Isr(void) {
    while (U3STAbits.URXDA == 1) {
        unsigned char data = U3RXREG;
    }
    lastRxTime[SAT3] = systemTickCount;
    IFS1bits.U3RXIF = 0;
}

void __ISR(_UART3_FAULT_VECTOR, IPL2SOFT) uart3ErrorIsr(void) {
    if (U3STAbits.OERR == 1) {
        U3STAbits.OERR = 0;
        //LED3On();
    }
    if (U3STAbits.FERR == 1) {
        //LED2On();
    }
    IFS1bits.U3EIF = 0;
}