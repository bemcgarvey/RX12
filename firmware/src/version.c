/////////////////////////////////////////////////////
// Project: RX12                                   //
// File: version.c                                 //
// Target: PIC32MKxxxxGPD/E064                     // 
// Compiler: XC32 (Tested with 2.10)               //
// Author: Brad McGarvey                           //
// License: GNU General Public License v3.0        //
// Description: firmware version data              //
/////////////////////////////////////////////////////
#include <xc.h>
#include "version.h"


const unsigned int firmwareVersion = (MAJOR_VERSION << 8) | MINOR_VERSION;
