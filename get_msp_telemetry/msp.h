//
//  msp.h
//  get_msp_telemetry
//
//  Created by Maarten ter Mors on 27-12-15.
//  Copyright © 2015 Morse Digital Media. All rights reserved.
//

#ifndef msp_h
#define msp_h

#include <stdio.h>


int sendMspCommand(int fd, uint8_t opcode);
int getMspResponse(int fd, void *response, size_t expectedNumBytes);

#endif /* msp_h */
