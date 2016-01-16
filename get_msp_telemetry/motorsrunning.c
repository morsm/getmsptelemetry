//
//  motorsrunning.c
//  get_msp_telemetry
//
//  Created by Maarten ter Mors on 16-01-16.
//  Copyright © 2016 Morse Digital Media. All rights reserved.
//
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

// Include types from multiwii
#include "types.h"

// Include Serial io code
#include "serial.h"

#include "msp.h"

#include "motorsrunning.h"

int main(int argc, const char * argv[])
{
    MSP_MotorData md_MotorData;
    char buffer[10];
    int res, running = 0, fd = openPort();
    if (fd < 0) return 2;
    
    // Send command to read motor status
    res = sendMspCommand(fd, MSP_MOTOR);
    if (res < 0) { closePort(fd); return res; }
    // Read servo response straight into struct
    res = getMspResponse(fd, &md_MotorData, sizeof(md_MotorData));
    closePort(fd);
    if (res < 0) return 3;
    
    running = (md_MotorData.u16_MotorValue[0] > 1000);
    
    printf("Motors are %srunning\n", running ? "" : "not ");
    
    return running ? 0 : 1;
}
