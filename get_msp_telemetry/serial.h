//
//  serial.h
//  get_msp_telemetry
//
//  Created by Maarten ter Mors on 27-12-15.
//  Copyright © 2015 Morse Digital Media. All rights reserved.
//

#ifndef serial_h
#define serial_h

#include <stdio.h>

#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

int openPort(void);
void closePort(int fd);
int readPort(int fd, void *buf, size_t num);
int writePort(int fd, void *buf, size_t num);



#endif /* serial_h */
