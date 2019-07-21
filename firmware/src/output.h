/* 
 * File:   output.h
 * Author: bemcg
 *
 * Created on July 20, 2019, 8:10 PM
 */

#ifndef OUTPUT_H
#define	OUTPUT_H

#include "servos.h"

#ifdef	__cplusplus
extern "C" {
#endif

    extern const unsigned int channelStartOffsets[MAX_CHANNEL];
    
    void initOutputs(void);


#ifdef	__cplusplus
}
#endif

#endif	/* OUTPUT_H */

