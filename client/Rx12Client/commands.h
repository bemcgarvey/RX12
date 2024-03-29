#ifndef COMMANDS_H
#define COMMANDS_H

enum {
    ENABLE_LOGGING = 0x20, DISABLE_LOGGING, GET_SETTINGS,
    SET_FRAME_22, SET_FRAME_11, GET_LOG, CLEAR_LOG,
    SET_VOLTAGE_CALIBRATION, GET_VOLTAGE, GET_VOLTAGE_CALIBRATION,
    GET_LOG_COUNT, GET_VERSION, SET_OUTPUT_PWM, SET_OUTPUT_PPM, SET_OUTPUT_SBUS
};

enum {
    FRAME_22MS = 0x22, FRAME_11MS = 0x11
};

enum {
    HOLD_FAILSAFE = 0, PRESET_FAILSAFE = 1
};

enum {
    SYSTEM_TYPE_DSM2_1024 = 0x01, SYSTEM_TYPE_DSM2_2048 = 0x12,
    SYSTEM_TYPE_DSMX_22 = 0xa2, SYSTEM_TYPE_DSMX_11 = 0xb2,
    SYSTEM_TYPE_NONE = 0x00
};

#define LOGGING_MAGIC_NUMBER    0x12398745

#endif // COMMANDS_H
