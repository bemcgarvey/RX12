/* 
 * File:   ppm.h
 * Author: bemcg
 *
 * Created on December 5, 2019, 10:35 PM
 */

#ifndef PPM_H
#define	PPM_H

#include "timers.h"

#ifdef	__cplusplus
extern "C" {
#endif

#define MAX_PPM_CHANNEL 8
#define PPM_OUT         12  //Channel for PPM output
#define PPM_SPACE  (300 * US_COUNT) 

    void initPPM(void);
    void startPPM(void);

#ifdef	__cplusplus
}
#endif

#endif	/* PPM_H */

