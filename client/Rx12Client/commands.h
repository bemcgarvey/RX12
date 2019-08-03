#ifndef COMMANDS_H
#define COMMANDS_H

enum {
    ENABLE_LOGGING = 0x20, DISABLE_LOGGING = 0x21, GET_SETTINGS = 0x22,
    SET_FRAME_22 = 0x23, SET_FRAME_11 = 0x24, GET_LOG = 0x25, CLEAR_LOG = 0x26,
    SET_VOLTAGE_CALIBRATION = 0x27, GET_VOLTAGE = 0x28
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