//
//  msp.c
//  get_msp_telemetry
//
//  Created by Maarten ter Mors on 27-12-15.
//  Copyright © 2015 Morse Digital Media. All rights reserved.
//

#include <stdint.h>
#include <string.h>

#include "msp.h"
#include "serial.h"


int sendMspCommand(int fd, uint8_t opcode)
{
    uint8_t command[6] = "$M<";
    uint8_t checksum = 0;
    int i;
    
    // The 0 terminator of the message doubles as the number of
    // data bytes, which is also 0
    command[3] = 0;
    
    // Opcode is byte 4
    command[4] = opcode;
    
    // The checksum is the XOR of all the bytes
    for (i=3; i<5; i++) checksum ^= command[i];
    command[5] = checksum;
    
    //fprintf(stderr, "Send command %d\n", opcode);
    
    return writePort(fd, command, sizeof(command));
}

int getMspResponse(int fd, void *response, size_t expectedNumBytes)
{
    uint8_t buffer[255]; // 255 bytes should be enough for everybody
    int i, res, totalBytes;
    uint8_t checksum;
    
    // Read any bytes from the buffer until we find a $ character
    do
    {
        res = readPort(fd, buffer, 1);
        if (res != 1) return -1;
    } while (buffer[0] != '$');
    
    // Total number of bytes is expected data plus the remaining five of the header
    totalBytes = (int) expectedNumBytes + 5;
    if (totalBytes > 255) return -2; // buffer too small, should not occur as MSP messages are all much smaller

    // Wait for remaining bytes
    res = readPort(fd, buffer, totalBytes);
    if (res != totalBytes)
    {
        fprintf(stderr, "expectedNumBytes=%zu, totalBytes=%d, res=%d\n", expectedNumBytes, totalBytes, res);
        return -3; // wrong number of bytes
    }
    
    // Check that the rest of the header is correct
    if (! (buffer[0] == 'M' && buffer[1] == '>' && ((size_t) buffer[2]) == expectedNumBytes) )
    {
        fprintf(stderr, "buffer[0]=%c, buffer[1]=%c, buffer[1]=%d, expectedNumBytes=%zu\n", buffer[0], buffer[1], buffer[2], expectedNumBytes);
        return -4; // message invalid
    }
    
    // Calculate checksum
    checksum = 0;
    for (i=2; i<totalBytes-1; i++) checksum ^= buffer[i];
    if (checksum != buffer[totalBytes-1]) return -5; // invalid checksum
    
    // Copy response message to buffer
    memcpy(response, &buffer[4], expectedNumBytes);
    return (int) expectedNumBytes;
}
