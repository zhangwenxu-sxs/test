#include "lt8522ex_misc.h"
#include "lt8522ex_main.h"

extern lt8522ex_t* curr_lt8522ex;
extern struct i2c_client* curr_client;

const unsigned char VGAEDID_Default[128] = { 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0x00, 0x32, 0x8D, 0x5F, 0xA0, 0x4C, 0x43, 0x43, 0x33, 0x35, 0x14, 0x01,
    0x03, 0x08, 0x33, 0x1D, 0x78, 0xEE, 0xEE, 0x95, 0xA3, 0x54, 0x4C, 0x99,
    0x26, 0x0F, 0x50, 0x54, 0xA5, 0x4B, 0x00, 0x71, 0x4F, 0x81, 0x80, 0xD1,
    0xC0, 0x81, 0xC0, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02,
    0x3A, 0x80, 0x18, 0x71, 0x38, 0x2D, 0x40, 0x58, 0x2C, 0x45, 0x00, 0xFD,
    0x1E, 0x11, 0x00, 0x00, 0x1E, 0x01, 0x1D, 0x00, 0x72, 0x51, 0xD0, 0x1E,
    0x20, 0x6E, 0x28, 0x55, 0x00, 0xC4, 0x8E, 0x21, 0x00, 0x00, 0x1E, 0x00,
    0x00, 0x00, 0xFC, 0x00, 0x56, 0x47, 0x41, 0x20, 0x54, 0x4F, 0x20, 0x48,
    0x44, 0x4D, 0x49, 0x0A, 0x20, 0x00, 0x00, 0x00, 0xFD, 0x00, 0x38, 0x4C,
    0x1E, 0x53, 0x11, 0x00, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00,
    0x37,    //lontiumifo 2011102
};

const unsigned char EDID_Default[256] = { 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00,
    0x32, 0x8D, 0x59, 0x06, 0x00, 0x00, 0x00, 0x00, 0x2D, 0x13, 0x01, 0x03,
    0x80, 0x58, 0x32, 0x78, 0x2A, 0xEE, 0x91, 0xA3, 0x54, 0x4C, 0x99, 0x26,
    0x0F, 0x50, 0x54, 0xBD, 0xEF, 0x80, 0x71, 0x4F, 0x81, 0x00, 0x81, 0x40,
    0x81, 0x80, 0x95, 0x00, 0x95, 0x0F, 0xB3, 0x00, 0xA9, 0x40, 0x64, 0x19,
    0x00, 0x40, 0x41, 0x00, 0x26, 0x30, 0x18,
    0x88,    //1024X768
    0x36, 0x00, 0x10, 0x09, 0x00, 0x00, 0x00, 0x18, 0x02, 0x3A, 0x80, 0x18,
    0x71, 0x38, 0x2D, 0x40, 0x58, 0x2C, 0x45, 0x00, 0xA0, 0x5A, 0x00, 0x00,
    0x00, 0x1E, 0x00, 0x00, 0x00, 0xFD, 0x00, 0x32, 0x3c, 0x1e, 0x44, 0x0f,
    0x00, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0xFC,
    0x00, 0x4C, 0x4F, 0x4E, 0x54, 0x49, 0x55, 0x4D, 0x0A, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x01,
    0xbe,    //lontiumifo 20111021

    0x02, 0x03, 0x23, 0xF1, 0x4B, 0x10, 0x84, 0x00, 0x00, 0x00, 0x00, 0x02,
    0x03, 0x00, 0x00, 0x00, 0x23, 0x09, 0x07, 0x07, 0x83, 0x01, 0x00, 0x00,
    0xE2, 0x00, 0x0F, 0x67, 0x03, 0x0c, 0x00, 0x20, 0x00, 0x80, 0x2D, 0x01,
    0x1D, 0x00, 0x72, 0x51, 0xD0, 0x1E, 0x20, 0x6E, 0x28, 0x55, 0x00, 0x10,
    0x09, 0x00, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4c
/*
 02  03  25  F1  4D  90  1F  04
 13  05  14  03  12  20  21  22
 07  16  23  09  07  07  83  01
 00  00  E2  00  0F  67  03  0C
 00  10  00  B8  2D  01  1D  80
 D0  72  1C  16  20  10  2C  25
 80  A0  5A  00  00  00  9E  01
 1D  80  18  71  1C  16  20  58
 2C  25  00  A0  5A  00  00  00
 9E  01  1D  00  BC  52  D0  1E
 20  B8  28  55  40  A0  5A  00
 00  00  1E  01  1D  00  72  51
 D0  1E  20  6E  28  55  00  A0
 5A  00  00  00  1E  00  00  00
 00  00  00  00  00  00  00  00
 00  00  00  00  00  00  00  EC  */

};

int GetTxEdid(unsigned char sink)
{
    unsigned char Loopx = 0;
    unsigned char Loopy = 0;
    unsigned char OffsetAddress = 0;

    curr_client = curr_lt8522ex->client_addr2;
    HDMI_WriteI2C_Byte(0x06, 0x77);
    HDMI_WriteI2C_Byte(0x00, 0x05);

    //从接在HDMI OUT接口的显示设备读出256字节的EDID
    if (sink == HdmiSink) {
        memset(curr_lt8522ex->HdmiSinkEdid, 0, 256);
        HDMI_WriteI2C_Byte(0x1F, 0xbc);     //HDMI TX DDC IO selected
        HDMI_WriteI2C_Byte(0x1B, 0xA0);     //Device address for downstream DDC
        HDMI_WriteI2C_Byte(0x1E, 0x20);     //Number of bytes for downstream DDC access
        Delay_ms(100);
        for (Loopx = 0; Loopx < 8; Loopx++) {
            HDMI_WriteI2C_Byte(0x1D, OffsetAddress);    //initial address

            HDMI_WriteI2C_Byte(0x1F, 0xa4); //access command read
            HDMI_WriteI2C_Byte(0x1F, 0xbc); //Non opreation
            Delay_ms(100);
            for (Loopy = 0; Loopy < 32; Loopy++) {
                curr_lt8522ex->HdmiSinkEdid[Loopx * 0x20 + Loopy] = HDMI_ReadI2C_Byte(0x7b);
            }
            //HDMI_ReadI2C_ByteN(0x7b,&SinkEdid[Loopx*0x20],0x20);
            OffsetAddress += 0x20;    //next 32 bytes
        }

        if ((curr_lt8522ex->HdmiSinkEdid[0] == 0x00) && (curr_lt8522ex->HdmiSinkEdid[1] == 0xFF)
            && (curr_lt8522ex->HdmiSinkEdid[2] == 0xFF) && (curr_lt8522ex->HdmiSinkEdid[3] == 0xFF)
            && (curr_lt8522ex->HdmiSinkEdid[4] == 0xFF) && (curr_lt8522ex->HdmiSinkEdid[5] == 0xFF)
            && (curr_lt8522ex->HdmiSinkEdid[6] == 0xFF) && (curr_lt8522ex->HdmiSinkEdid[7] == 0x00)) {
            return 0;
        } else {
            memcpy(curr_lt8522ex->HdmiSinkEdid, EDID_Default, 256);
            return -1;
        }
    } else if (sink == VgaSink) {
        memset(curr_lt8522ex->VgaSinkEdid, 0, 128);
        HDMI_WriteI2C_Byte(0x1F, 0x9c);     //HDMI TX DDC IO selected
        HDMI_WriteI2C_Byte(0x1B, 0xA0);     //Device address for downstream DDC
        HDMI_WriteI2C_Byte(0x1E, 0x20);     //Number of bytes for downstream DDC access
        Delay_ms(100);
        for (Loopx = 0; Loopx < 4; Loopx++) {
            HDMI_WriteI2C_Byte(0x1D, OffsetAddress);    //initial address

            HDMI_WriteI2C_Byte(0x1F, 0x84); //access command read
            HDMI_WriteI2C_Byte(0x1F, 0x9c); //Non opreation
            Delay_ms(100);
            for (Loopy = 0; Loopy < 32; Loopy++) {
                curr_lt8522ex->VgaSinkEdid[Loopx * 0x20 + Loopy] = HDMI_ReadI2C_Byte(0x7b);
            }
            //HDMI_ReadI2C_ByteN(0x7b,&SinkEdid[Loopx*0x20],0x20);
            OffsetAddress += 0x20;    //next 32 bytes
        }

        if ((curr_lt8522ex->VgaSinkEdid[0] == 0x00) && (curr_lt8522ex->VgaSinkEdid[1] == 0xFF)
                && (curr_lt8522ex->VgaSinkEdid[2] == 0xFF) && (curr_lt8522ex->VgaSinkEdid[3] == 0xFF)
                && (curr_lt8522ex->VgaSinkEdid[4] == 0xFF) && (curr_lt8522ex->VgaSinkEdid[5] == 0xFF)
                && (curr_lt8522ex->VgaSinkEdid[6] == 0xFF) && (curr_lt8522ex->VgaSinkEdid[7] == 0x00)) {
            return 0;
        } else {
            memcpy(curr_lt8522ex->VgaSinkEdid, EDID_Default, 128);
            return -1;
        }
    }

    return -1;
}

void RebuildSinkEdid(unsigned char rx, unsigned char tx)
{
    unsigned char checksum = 0;
    unsigned char ClkFre1 = 0;
    unsigned char ClkFre2 = 0;
    unsigned short loopx = 0;

    if ((rx == HDMI) && (tx == VGA)) {
        memcpy(curr_lt8522ex->EdidBuff, curr_lt8522ex->VgaSinkEdid, 128);
        memcpy(&curr_lt8522ex->EdidBuff[0x80], &EDID_Default[0x80], 128);
        curr_lt8522ex->EdidBuff[0x14] = 0xa0;
        curr_lt8522ex->EdidBuff[0x7e] = 0x01;
    } else if ((rx == VGA) && (tx == HDMI)) {
        memcpy(curr_lt8522ex->EdidBuff, curr_lt8522ex->HdmiSinkEdid, 128);

        ClkFre1 = curr_lt8522ex->EdidBuff[55];
        ClkFre1 <<= 8;
        ClkFre1 &= 0xf0;
        ClkFre1 |= curr_lt8522ex->EdidBuff[54];

        ClkFre2 = EDID_Default[55];
        ClkFre2 <<= 8;
        ClkFre2 &= 0xf0;
        ClkFre2 |= EDID_Default[54];

        if (ClkFre1 > ClkFre2) {
            for (loopx = 0; loopx < 18; loopx++) {
                curr_lt8522ex->EdidBuff[loopx + 54] = EDID_Default[loopx + 54];
            }
        }

        curr_lt8522ex->EdidBuff[0x14] = 0x08;
        curr_lt8522ex->EdidBuff[0x7e] = 0x00;
    } else if ((rx == HDMI) && (tx == HDMI)) {
        memcpy(curr_lt8522ex->EdidBuff, curr_lt8522ex->HdmiSinkEdid, 256);
        for (loopx = 0; loopx < 256; loopx++) {
            if ((curr_lt8522ex->EdidBuff[loopx] == 0x03) && (curr_lt8522ex->EdidBuff[loopx + 1] == 0x0c)
                    && (curr_lt8522ex->EdidBuff[loopx + 2] == 0x00)) {
                curr_lt8522ex->EdidBuff[loopx + 5] &= 0x8F;
                break;
            }
        }
        curr_lt8522ex->EdidBuff[0x14] = 0xa0;
        curr_lt8522ex->EdidBuff[0x7e] = 0x01;
    } else if ((rx == VGA) && (tx == VGA)) {
        memcpy(curr_lt8522ex->EdidBuff, curr_lt8522ex->VgaSinkEdid, 128);
        curr_lt8522ex->EdidBuff[0x14] = 0x08;
        curr_lt8522ex->EdidBuff[0x7e] = 0x00;
    }

    checksum = 0;
    for (loopx = 0; loopx < 127; loopx++)
        checksum += curr_lt8522ex->EdidBuff[loopx];
    checksum = 0xFF - checksum + 1;
    curr_lt8522ex->EdidBuff[0x7f] = checksum;

    checksum = 0;
    for (loopx = 128; loopx < 255; loopx++)
        checksum += curr_lt8522ex->EdidBuff[loopx];
    checksum = 0xFF - checksum + 1;
    curr_lt8522ex->EdidBuff[0xff] = checksum;
}

void BuildSourceEdid(unsigned char rx)
{
    if (rx == HDMI) {
        //make VGA edid to hdmi edid HDMI2VGA
        if ((curr_lt8522ex->pair1 == (LT8522EX_HDMI_INPUT | LT8522EX_VGA_OUTPUT) ||
             curr_lt8522ex->pair2 == (LT8522EX_HDMI_INPUT | LT8522EX_VGA_OUTPUT)) &&
            (!curr_lt8522ex->FlagTxVgaOff)) {
            RebuildSinkEdid(HDMI, VGA);
        } else {
            //if((!HDMI2VGA)&&HDMI2HDMI)//choose HDMI edid
            RebuildSinkEdid(HDMI, HDMI);
        }
    } else if(rx == VGA) {
        //make Hdmi edid to vga edid VGA2VGA
        if ((curr_lt8522ex->pair1 == (LT8522EX_VGA_INPUT | LT8522EX_VGA_OUTPUT) ||
             curr_lt8522ex->pair2 == (LT8522EX_VGA_INPUT | LT8522EX_VGA_OUTPUT)) &&
            (!curr_lt8522ex->FlagTxVgaOff)) {
            RebuildSinkEdid(VGA, VGA);
        } else {
            //if((!VGA2VGA)&&VGA2HDMI)
            RebuildSinkEdid(VGA, HDMI);
        }
    }
}

void SaveEdidToShadow(unsigned char rx)
{
    unsigned short loopx = 0;

    if (rx == HDMI) {
        curr_client = curr_lt8522ex->client_addr2;
        HDMI_WriteI2C_Byte(0xFD, 0x01);  //Hdmi rx edid shadow ddc slave disable
        HDMI_WriteI2C_Byte(0xFE, 0x00);
        for (loopx = 0; loopx < 256; loopx++) {
            HDMI_WriteI2C_Byte(0xFF, curr_lt8522ex->EdidBuff[loopx]);   //edid_buff  edid_default
        }

        HDMI_WriteI2C_Byte(0xFD, 0x00); //Hdmi rx edid shadow ddc slave enable
    } else if (rx == VGA) {
        curr_client = curr_lt8522ex->client_addr0;
        HDMI_WriteI2C_Byte(0xFD, 0x01); //Hdmi rx edid shadow ddc slave disable
        for (loopx = 0; loopx < 128; loopx++) {
//            HDMI_WriteI2C_Byte(0xFF, VGAEDID_Default[loopx]); //edid_buff  edid_default(可能这里会有问题)
            HDMI_WriteI2C_Byte(0xFF, curr_lt8522ex->EdidBuff[loopx]);   //edid_buff  edid_default
        }

        HDMI_WriteI2C_Byte(0xFD, 0x00); //Hdmi rx edid shadow ddc slave enable
    }
}
