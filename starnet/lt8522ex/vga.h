#ifndef _VGA_H
#define _VGA_H

typedef enum {
    VESA_640x480_60,
    VESA_640x480_75,
    VESA_800x600_60,
    VESA_800x600_75,
    VESA_1024x768_60,
    VESA_1024x768_75,
    VESA_1152x864_75,
    VESA_1280x720_60,
    VESA_1280x768_60RB,
    VESA_1280x768_60,
    VESA_1280x800_60RB,
    VESA_1280x800_60,
    VESA_1280x800_75,
    VESA_1280x960_60,
    VESA_1280x1024_60,
    VESA_1280x1024_75,
    VESA_1360x768_60,
    VESA_1366x768_60NB,
    VESA_1366x768_60RB,
    VESA_1400x1050_60RB,
    VESA_1400x1050_60,
    VESA_1440x900_60RB,
    VESA_1440x900_60,
    VESA_1600x900_60RB,
    VESA_1600x1200_60,
    VESA_1680x1050_60,
    VESA_1920x1080_60,
    VESA_1920x1200_60RB,
    VESA_1920x1200_60,
    VESA_Nonstandard,
    SignnalOff
} VESA_Resolution;

#define GCM_DE_DLY_LSB      0x00    //0xc9
#define GCM_DE_CNT_LSB      0x01    //0xca
#define GCM_DE_CNT_DLY_MSB  0x02    //0xcb
#define GCM_DE_TOP          0x03    //0xcc
#define GCM_DE_LIN_LSB      0x04    //0xcd
#define GCM_H_TOTAL_LSB     0x05    //0xce
#define GCM_H_TOTAL_LIN_MSB 0x06    //0xcf
#define GCM_V_TOTAL_LSB     0x07    //0xd0
#define GCM_SG_V_TOTAL_MSB  0x08    //0xd1
#define GCM_HWIDTH_LSB      0x09    //0xd2
#define GCM_H_V_TOTAL_MSB   0x0a    //0xd3
#define GCM_CLAMP_POSITION  0x0b    //0xd3
#define GCM_CLAMP_DURATION  0x0c    //0xd3

extern const unsigned char VideoFormatTab[29][13];
void RxVgaChangeDetect(void);
void CheckBestPhase(void);

#endif
