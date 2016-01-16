//
//  main.c
//  get_msp_telemetry
//
//  Created by Maarten ter Mors on 27-12-15.
//  Copyright © 2015 Morse Digital Media. All rights reserved.
//


#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

// Include types from multiwii
#include "types.h"

// Include Serial io code
#include "serial.h"

#include "msp.h"


float servo2Perc(uint16_t servo, uint16_t minScale, uint16_t maxScale, uint8_t symmetric)
{
    float fServo = servo * 1.0f, fScaleSize = 1.0f * (maxScale-minScale);
    
    if (0 == servo) return 0.0f;
    
    fServo -= minScale;
    
    if (symmetric) // between -100% and 100%
    {
        fServo *= 2;
        fServo -= fScaleSize;
    }

    fServo /= fScaleSize;
    return fServo * 100.0f;
}

int readSingleRecord(MSP_TelemetryRecord *record)
{
    int res, fd = openPort();
    if (fd < 0) return fd;
    
    // Send command to read Attitude status
    res = sendMspCommand(fd, MSP_ATTITUDE);
    if (res < 0) { closePort(fd); return res; }
    // Read servo response straight into struct
    res = getMspResponse(fd, &record->atd_AttitudeData, sizeof(record->atd_AttitudeData));
    if (res < 0) { closePort(fd); return res; }
    
    return 1;
}

int readRecord(MSP_TelemetryRecord *record)
{
    int res, fd = openPort();
    if (fd < 0) return fd;
    
    // Send command to read servo status
    res = sendMspCommand(fd, MSP_SERVO);
    if (res < 0) { closePort(fd); return res; }
    // Read servo response straight into struct
    res = getMspResponse(fd, &record->sd_ServoData, sizeof(record->sd_ServoData));
    if (res < 0) { closePort(fd); return res; }
    
    // Send command to read motor status
    res = sendMspCommand(fd, MSP_MOTOR);
    if (res < 0) { closePort(fd); return res; }
    // Read servo response straight into struct
    res = getMspResponse(fd, &record->md_MotorData, sizeof(record->md_MotorData));
    if (res < 0) { closePort(fd); return res; }
    
    // Send command to read RC status
    res = sendMspCommand(fd, MSP_RC);
    if (res < 0) { closePort(fd); return res; }
    // Read servo response straight into struct
    res = getMspResponse(fd, &record->rc_RCData, sizeof(record->rc_RCData));
    if (res < 0) { closePort(fd); return res; }

    // Send command to read raw GPS status
    res = sendMspCommand(fd, MSP_RAW_GPS);
    if (res < 0) { closePort(fd); return res; }
    // Read servo response straight into struct
    res = getMspResponse(fd, &record->gr_GPSRawData, sizeof(record->gr_GPSRawData));
    if (res < 0) { closePort(fd); return res; }
    
    // Send command to read calculated GPS status
    res = sendMspCommand(fd, MSP_COMP_GPS);
    if (res < 0) { closePort(fd); return res; }
    // Read servo response straight into struct
    res = getMspResponse(fd, &record->gc_GPSCompData, sizeof(record->gc_GPSCompData));
    if (res < 0) { closePort(fd); return res; }
    
    // Send command to read Attitude status
    res = sendMspCommand(fd, MSP_ATTITUDE);
    if (res < 0) { closePort(fd); return res; }
    // Read servo response straight into struct
    res = getMspResponse(fd, &record->atd_AttitudeData, sizeof(record->atd_AttitudeData));
    if (res < 0) { closePort(fd); return res; }
    
    // Send command to read Altitude status
    res = sendMspCommand(fd, MSP_ALTITUDE);
    if (res < 0) { closePort(fd); return res; }
    // Read servo response straight into struct
    res = getMspResponse(fd, &record->ald_AltitudeData, sizeof(record->ald_AltitudeData));
    if (res < 0) { closePort(fd); return res; }
    
    // Send command to read analog status
    res = sendMspCommand(fd, MSP_ANALOG);
    if (res < 0) { closePort(fd); return res; }
    // Read servo response straight into struct
    res = getMspResponse(fd, &record->an_AnalogData, sizeof(record->an_AnalogData));
    if (res < 0) { closePort(fd); return res; }
    
    // Send command to read misc data
    res = sendMspCommand(fd, MSP_MISC);
    // Read response straight into struct
    res = getMspResponse(fd, &record->md_Misc, sizeof(record->md_Misc));
    if (res < 0) { closePort(fd); return res; }

    // Record time stamp
    time(&record->t_TimeStamp);

    return 0; // success
}

void record2XML(FILE *file, MSP_TelemetryRecord *record)
{
    char buf[255];
    struct tm tmUtc;
    int i;
    
    // Format time stamp in XML format
    gmtime_r(&record->t_TimeStamp, &tmUtc);
    strftime(buf, sizeof(buf)-1, "%Y-%m-%dT%H:%M:%SZ", &tmUtc);
    
    fprintf(file, "<TelemetryRecord><Time>%s</Time>", buf);
    fprintf(file, "<BatteryV>%.1f</BatteryV>", record->an_AnalogData.u8_VBat / 10.0f);
    fprintf(file, "<ArmCount>%d</ArmCount>", record->md_Misc.u16_ArmCounter);
    
    // Attitude
    fprintf(file, "<AngleX>%.1f</AngleX><AngleY>%.1f</AngleY><Heading>%d</Heading>",
            record->atd_AttitudeData.i16_AngleX / 10.0f,
            record->atd_AttitudeData.i16_AngleY / 10.0f,
            record->atd_AttitudeData.i16_Heading
            );
    
    // Altitude
    fprintf(file, "<AltitudeM>%.2f</AltitudeM><AscentMpS>%.2f</AscentMpS>",
            record->ald_AltitudeData.i32_EstAltitude / 100.0f,
            record->ald_AltitudeData.i16_AltVariation / 100.0f
            );
    
    // RC inputs
    fprintf(file,"<RcRollPct>%.3f</RcRollPct>",  servo2Perc(record->rc_RCData.u16_Roll, 1000, 2000, 1));
    fprintf(file,"<RcPitchPct>%.3f</RcPitchPct>", servo2Perc(record->rc_RCData.u16_Pitch, 1000, 2000, 1));
    fprintf(file,"<RcYawPct>%.3f</RcYawPct>", servo2Perc(record->rc_RCData.u16_Yaw, 1000, 2000, 1));
    fprintf(file,"<RcThrottlePct>%.3f</RcThrottlePct>", servo2Perc(record->rc_RCData.u16_Throttle, 1000, 2000, 0));
    for (i=0; i<4; i++)
        fprintf(file,"<RcAux%dPct>%.3f</RcAux%dPct>", i+1, servo2Perc(record->rc_RCData.u16_Aux[i], 1000, 2000, 1), i+1);
    
    // GPS data, raw first
    fprintf(file, "<GpsFix>%s</GpsFix><GpsSatCount>%d</GpsSatCount>", record->gr_GPSRawData.u8_GpsFix ? "true" : "false", record->gr_GPSRawData.u8_NumSat);
    fprintf(file, "<GpsLatitude>%.8f</GpsLatitude>", record->gr_GPSRawData.u32_GpsLat / 1.0e7f);
    fprintf(file, "<GpsLongitude>%.8f</GpsLongitude>", record->gr_GPSRawData.u32_GpsLon / 1.0e7f);
    fprintf(file, "<GpsAltitudeM>%d</GpsAltitudeM>", record->gr_GPSRawData.u16_GpsAlt);
    fprintf(file, "<GpsSpeedMpS>%.2f</GpsSpeedMpS>", record->gr_GPSRawData.u16_GpsSpeed / 100.0f);
    fprintf(file, "<GpsHeading>%.1f</GpsHeading>", record->gr_GPSRawData.u16_GpsHeading / 10.0f);
    
    // GPS computed
    fprintf(file, "<GpsDistanceHomeM>%d</GpsDistanceHomeM><GpsDirectionHome>%d</GpsDirectionHome>",
            record->gc_GPSCompData.u16_GpsDistHome, record->gc_GPSCompData.u16_GpsDirHome);
    

    // Servo and motor values
    for (i=0; i<8; i++) fprintf(file, "<Motor%dPct>%.3f</Motor%dPct>",
        i+1, servo2Perc(record->md_MotorData.u16_MotorValue[i], 1000, 2000, 0), i+1);
    for (i=0; i<8; i++) fprintf(file, "<Servo%dPct>%.3f</Servo%dPct>",
                                i+1, servo2Perc(record->sd_ServoData.u16_ServoValue[i], 1000, 2000, 1), i+1);
    
    // Close record
    fputs("</TelemetryRecord>", file);
    
}

void generateTestRecord(MSP_TelemetryRecord *record)
{
    
    time(&record->t_TimeStamp);
    
    record->an_AnalogData.u8_VBat = 114;
    
    record->ald_AltitudeData.i32_EstAltitude = 1100;
    record->ald_AltitudeData.i16_AltVariation = -50;
    
    record->atd_AttitudeData.i16_AngleX = 430;
    record->atd_AttitudeData.i16_AngleY = -800;
    record->atd_AttitudeData.i16_Heading = 136;
    
    record->gc_GPSCompData.u16_GpsDirHome = 117;
    record->gc_GPSCompData.u16_GpsDistHome = 137;
    
    record->rc_RCData.u16_Throttle = 1600;
    record->rc_RCData.u16_Pitch = 1545;
    record->rc_RCData.u16_Roll = 1440;
    record->rc_RCData.u16_Yaw = 1501;
    
    record->gr_GPSRawData.u8_GpsFix = 1;
    record->gr_GPSRawData.u8_NumSat = 5;
    record->gr_GPSRawData.u16_GpsAlt = 10;
    record->gr_GPSRawData.u16_GpsHeading = 1353;
    record->gr_GPSRawData.u16_GpsSpeed = 1050;
    record->gr_GPSRawData.u32_GpsLat = (uint32_t) (52.377490 * 1.0e7f);
    record->gr_GPSRawData.u32_GpsLon = (uint32_t) (4.813117  * 1.0e7f); // Amsterdam
    
    record->md_MotorData.u16_MotorValue[0] = 1633;
    record->md_MotorData.u16_MotorValue[1] = 1925;
    record->md_MotorData.u16_MotorValue[2] = 1755;
    record->md_MotorData.u16_MotorValue[3] = 1801;
    
}

int main(int argc, const char * argv[])
{
    MSP_TelemetryRecord record;
    int rc = 0;

    memset(&record, 0, sizeof(MSP_TelemetryRecord));

    rc = readRecord(&record);

    //    generateTestRecord(&record);
    //    rc = readSingleRecord(&record);
    
    if (rc < 0)
    {
        fprintf(stderr, "Can't read full telemetry record, rc=%d\n", rc);
        return rc;
    }
    
    record2XML(stdout, &record);
    return 0;
}
