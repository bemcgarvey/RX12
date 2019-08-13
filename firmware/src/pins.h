/////////////////////////////////////////////////////
// Project: RX12                                   //
// File: pins.h                                    //
// Target: PIC32MKxxxxGPD/E064                     // 
// Compiler: XC32 (Tested with 2.10)               //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: I/O pin configuration              //
/////////////////////////////////////////////////////

#ifndef PINS_H
#define	PINS_H

#ifdef	__cplusplus
extern "C" {
#endif

    void initPins(void);
    void setPPS(void);


#ifdef	__cplusplus
}
#endif

#endif	/* PINS_H */

