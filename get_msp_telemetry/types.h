//
//  types.h
//  get_msp_telemetry
//
//  Created by Maarten ter Mors on 27-12-15.
//  Copyright © 2015 Morse Digital Media. All rights reserved.
//

#ifndef types_h
#define types_h


// Commands we wil use
typedef enum
{
    MSP_SERVO = 103,
    MSP_MOTOR = 104,
    MSP_RC = 105,
    MSP_RAW_GPS = 106,
    MSP_COMP_GPS = 107,
    MSP_ATTITUDE = 108,
    MSP_ALTITUDE = 109,
    MSP_ANALOG = 110,
    MSP_MISC = 114
    
} MSP_COMMANDS;

typedef struct
{
    uint16_t u16_ServoValue[8];
} MSP_ServoData;

typedef struct
{
    uint16_t u16_MotorValue[8];
} MSP_MotorData;

typedef struct
{
    uint16_t u16_Roll;
    uint16_t u16_Pitch;
    uint16_t u16_Yaw;
    uint16_t u16_Throttle;
    uint16_t u16_Aux[4];
} MSP_RcData;

#pragma pack(1)

typedef struct
{
    uint8_t u8_GpsFix;
    uint8_t u8_NumSat;
    uint32_t u32_GpsLat;
    uint32_t u32_GpsLon;
    uint16_t u16_GpsAlt;
    uint16_t u16_GpsSpeed;
    uint16_t u16_GpsHeading;
} MSP_GpsRawData;

typedef struct
{
    uint16_t u16_GpsDistHome;
    uint16_t u16_GpsDirHome;
    uint8_t u8_GpsUpdate;
} MSP_GpsCompData;

typedef struct
{
    int16_t i16_AngleX;
    int16_t i16_AngleY;
    int16_t i16_Heading;
} MSP_AttitudeData;

typedef struct
{
    int32_t i32_EstAltitude;
    int16_t i16_AltVariation;
} MSP_AltitudeData;

typedef struct
{
    uint8_t u8_VBat;
    uint16_t u16_PowerMeterSum;
    uint16_t u16_Rssi;
    uint16_t u16_Amperage;
} MSP_AnalogData;


typedef struct
{
    uint16_t u16_PowerTrigger;
    uint16_t u16_MinThrottle;
    uint16_t u16_MaxThrottle;
    uint16_t u16_MinCommand;
    uint16_t u16_FailSafeThrottle;
    uint16_t u16_ArmCounter;
    uint32_t u32_LifeTime;
    uint16_t u16_MagDeclination;
    uint8_t u8_VBatScale;
    uint8_t u8_VBatLevelWarn1;
    uint8_t u8_VBatLevelWarn2;
    uint8_t u8_VBatLevelCrit;
    
} MSP_MiscData;

typedef struct
{
    time_t t_TimeStamp;
    MSP_ServoData sd_ServoData;
    MSP_MotorData md_MotorData;
    MSP_RcData rc_RCData;
    MSP_GpsRawData gr_GPSRawData;
    MSP_GpsCompData gc_GPSCompData;
    MSP_AttitudeData atd_AttitudeData;
    MSP_AltitudeData ald_AltitudeData;
    MSP_AnalogData an_AnalogData;
    MSP_MiscData md_Misc;
} MSP_TelemetryRecord;

#endif /* types_h */
