#ifndef IR_CODES_H
#define IR_CODES_H

typedef enum
{
    IR_ON = 0x5c,
    IR_OFF = 0x40,
    IR_R1 = 0x58,
    IR_R2 = 0x54,
    IR_R3 = 0x50,
    IR_R4 = 0x1c,
    IR_R5 = 0x18,
    IR_G1 = 0x59,
    IR_G2 = 0x55,
    IR_G3 = 0x51,
    IR_G4 = 0x1d,
    IR_G5 = 0x19,
    IR_B1 = 0x45,
    IR_B2 = 0x49,
    IR_B3 = 0x4d,
    IR_B4 = 0x1e,
    IR_B5 = 0x1a,
    IR_W = 0x44,
    IR_RAINBOW = 0x48,
    IR_METEOR = 0x4c,
    IR_STARS = 0x1f,
    IR_FIRE = 0x1b,
    IR_MODE_UP = 0x14,
    IR_MODE_DOWN = 0xc,
    IR_FAV_UP = 0x15,
    IR_FAV_DOWN = 0xd,
    IR_RAND_SHUF = 0x10,
    IR_FAV_SHUF = 0x11,
    IR_SPEED_UP = 0x17,
    IR_SPEED_DOWN = 0x16,
    IR_BRIGHT_UP = 0x13,
    IR_BRIGHT_DOWN = 0x12,
    IR_LENGTH_UP = 0xf,
    IR_LENGTH_DOWN = 0xe,
    IR_MUSIC_UP = 0x9,
    IR_MUSIC_DOWN = 0x8,
    IR_MUSIC_SHUF = 0xa,
    IR_CHANGE_COLOR = 0xb
} IRCode;

const char *IRCodeToString(IRCode code)
{
    switch (code)
    {
    case IR_ON:
        return "IR_ON";
    case IR_OFF:
        return "IR_OFF";
    case IR_R1:
        return "IR_R1";
    case IR_R2:
        return "IR_R2";
    case IR_R3:
        return "IR_R3";
    case IR_R4:
        return "IR_R4";
    case IR_R5:
        return "IR_R5";
    case IR_G1:
        return "IR_G1";
    case IR_G2:
        return "IR_G2";
    case IR_G3:
        return "IR_G3";
    case IR_G4:
        return "IR_G4";
    case IR_G5:
        return "IR_G5";
    case IR_B1:
        return "IR_B1";
    case IR_B2:
        return "IR_B2";
    case IR_B3:
        return "IR_B3";
    case IR_B4:
        return "IR_B4";
    case IR_B5:
        return "IR_B5";
    case IR_W:
        return "IR_W";
    case IR_RAINBOW:
        return "IR_RAINBOW";
    case IR_METEOR:
        return "IR_METEOR";
    case IR_STARS:
        return "IR_STARS";
    case IR_FIRE:
        return "IR_FIRE";
    case IR_MODE_UP:
        return "IR_MODE_UP";
    case IR_MODE_DOWN:
        return "IR_MODE_DOWN";
    case IR_FAV_UP:
        return "IR_FAV_UP";
    case IR_FAV_DOWN:
        return "IR_FAV_DOWN";
    case IR_RAND_SHUF:
        return "IR_RAND_SHUF";
    case IR_FAV_SHUF:
        return "IR_FAV_SHUF";
    case IR_SPEED_UP:
        return "IR_SPEED_UP";
    case IR_SPEED_DOWN:
        return "IR_SPEED_DOWN";
    case IR_BRIGHT_UP:
        return "IR_BRIGHT_UP";
    case IR_BRIGHT_DOWN:
        return "IR_BRIGHT_DOWN";
    case IR_LENGTH_UP:
        return "IR_LENGTH_UP";
    case IR_LENGTH_DOWN:
        return "IR_LENGTH_DOWN";
    case IR_MUSIC_UP:
        return "IR_MUSIC_UP";
    case IR_MUSIC_DOWN:
        return "IR_MUSIC_DOWN";
    case IR_MUSIC_SHUF:
        return "IR_MUSIC_SHUF";
    case IR_CHANGE_COLOR:
        return "IR_CHANGE_COLOR";
    default:
        return "UNKNOWN";
    }
}

#endif