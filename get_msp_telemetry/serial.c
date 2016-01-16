//
//  serial.c
//  get_msp_telemetry
//
//  Created by Maarten ter Mors on 27-12-15.
//  Copyright © 2015 Morse Digital Media. All rights reserved.
//

// Serial port code, inspired by Serial Programming HOWTO
// at http://tldp.org/HOWTO/Serial-Programming-HOWTO/x115.html

#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>

#include "serial.h"

// Probably shouldmake this command line configurable or something
#define BAUDRATE B115200
#define MODEMDEVICE "/dev/ttyAMA0"
#define TIMEOUT 10


volatile int STOP=FALSE;
struct termios g_oldtio;

int openPort()
{
    int fd;
    struct termios newtio;
    
    fd = open(MODEMDEVICE, O_RDWR | O_NOCTTY );
    if (fd <0) return fd;
    
    tcgetattr(fd,&g_oldtio); /* save current port settings */
    
    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE| CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;
    
    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;
    
    newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 5;   /* blocking read until 5 chars received */
    
    tcflush(fd, TCIFLUSH);
    tcsetattr(fd,TCSANOW,&newtio);
    
    return fd;
}

void closePort(int fd)
{
    if (fd<0) return;

    tcsetattr(fd,TCSANOW,&g_oldtio);
    close(fd);
}

int readPort(int fd, void *buf, size_t num)
{
    int res;
    struct termios tio;
    
    tcgetattr(fd, &tio);
    tio.c_cc[VTIME]    = TIMEOUT;   /* inter-character timer unused */
    tio.c_cc[VMIN]     = num;   /* blocking read until 5 chars received */
    tcsetattr(fd,TCSANOW,&tio);

//    printf("Reading %d characters from serial port\n", num);

    res = read(fd, buf, num);   /* returns after 5 chars have been input */
    if (res != num)
    {
        printf("res = %d\n", res);
        res = -1;
    }
    
    return res;
}

int writePort(int fd, void *buf, size_t num)
{
    int res;
    struct termios tio;
    
    tcgetattr(fd, &tio);
    tio.c_cc[VTIME]    = TIMEOUT;   /* inter-character timer unused */
    tio.c_cc[VMIN]     = num;   /* blocking read until 5 chars received */
    tcsetattr(fd,TCSANOW,&tio);
    
    res = write(fd, buf, num);   /* returns after 5 chars have been input */
    if (res != num) res = -1;
    
    return res;
}
