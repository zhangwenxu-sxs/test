#include "lt8522ex_misc.h"
#include "lt8522ex_main.h"

extern lt8522ex_t* curr_lt8522ex;
extern struct i2c_client* curr_client;

const unsigned short VESA_Vtotal[24] = { 0x1F4, 0x20D, 0x271, 0x274, 0x2EE, 0x316, 0x31B,
        0x31E, 0x320, 0x326, 0x337, 0x33F, 0x346, 0x384, 0x39E, 0x3A6, 0x3E8,
        0x42A, 0x438, 0x441, 0x465, 0x4D3, 0x4DD, 0x4E2 };
const unsigned char VideoFormatTab[29][13] = { { 0x90, 0x80, 0x20, 0x23, 0xe0, 0x20, 0x31,
        0x0d, 0x02, 0x60, 0x02, 0x0a, 0x15 }, //640*480_60
        { 0xB8, 0x80, 0x20, 0x13, 0xe0, 0x48, 0x31, 0xF4, 0x01, 0x40, 0x03,
                0x0a, 0x15 }, //640*480_75
        { 0xD8, 0x20, 0x30, 0x1B, 0x58, 0x20, 0x42, 0x74, 0x02, 0x80, 0x04,
                0x10, 0x30 }, //800*600_60
        { 0xF0, 0x20, 0x30, 0x18, 0x58, 0x20, 0x42, 0x71, 0x02, 0x50, 0x03,
                0x10, 0x30 }, //800*600_75
        { 0x28, 0x00, 0x41, 0x23, 0x00, 0x40, 0x53, 0x26, 0x03, 0x88, 0x06,
                0x10, 0x30 }, //1024*768_60
        { 0x10, 0x00, 0x41, 0x1f, 0x00, 0x20, 0x53, 0x20, 0x03, 0x60, 0x03,
                0x10, 0x30 }, //1024*768_75
        { 0x80, 0x80, 0x41, 0x23, 0x60, 0x40, 0x63, 0x84, 0x03, 0x80, 0x03,
                0x10, 0x30 }, //1152*864_75
        { 0x04, 0x00, 0x51, 0x19, 0xD0, 0x72, 0x62, 0xEE, 0x02, 0x28, 0x05,
                0x10, 0x30 }, //1280*720_60
        { 0x70, 0x00, 0x50, 0x13, 0x00, 0xA0, 0x53, 0x16, 0x03, 0x20, 0x07,
                0x10, 0x30 }, //1280*768_60RB
        { 0x40, 0x00, 0x51, 0x1B, 0x00, 0x80, 0x63, 0x1E, 0x03, 0x80, 0x07,
                0x10, 0x30 }, //1280*768_60
        { 0x70, 0x00, 0x50, 0x14, 0x20, 0xA0, 0x53, 0x37, 0x03, 0x20, 0x06,
                0x10, 0x30 }, //1280*800_60RB
        { 0x48, 0x00, 0x51, 0x1C, 0x20, 0x90, 0x63, 0x3F, 0x03, 0x80, 0x06,
                0x10, 0x30 }, //1280*800_60
        { 0x50, 0x00, 0x51, 0x23, 0x20, 0xA0, 0x63, 0x46, 0x03, 0x80, 0x06,
                0x10, 0x30 }, //1280*800_75
        { 0xa8, 0x00, 0x51, 0x27, 0xc0, 0x08, 0x73, 0x3F, 0x03, 0x80, 0x06,
                0x10, 0x30 }, //1280*960_60
        { 0x68, 0x00, 0x51, 0x2A, 0x00, 0x98, 0x64, 0x2A, 0x04, 0x70, 0x03,
                0x10, 0x30 }, //1280*1024_60
        { 0x88, 0x00, 0x51, 0x29, 0x00, 0x98, 0x64, 0x2A, 0x04, 0x90, 0x03,
                0x10, 0x30 }, //1280*1024_75
        { 0x70, 0x50, 0x51, 0x18, 0x00, 0x00, 0x73, 0x1B, 0x03, 0x70, 0x06,
                0x10, 0x30 }, //1360*768_60
        { 0x64, 0x56, 0x51, 0x1B, 0x00, 0x00, 0x73, 0x1E, 0x03, 0x8F, 0x03,
                0x10, 0x30 }, //1366*768_60NB
        { 0x78, 0x56, 0x50, 0x1f, 0x00, 0xdc, 0x53, 0x20, 0x03, 0x38, 0x03,
                0x10, 0x30 }, //1366*768_60RB
        { 0x70, 0x78, 0x50, 0x1B, 0x1A, 0x18, 0x64, 0x38, 0x04, 0x20, 0x04,
                0x10, 0x30 }, //1400*1050_60RB
        { 0x77, 0x78, 0x51, 0x25, 0x1A, 0x48, 0x74, 0x41, 0x04, 0x90, 0x04,
                0x10, 0x30 }, //1400*1050_60
        { 0x70, 0xA0, 0x50, 0x17, 0x84, 0x40, 0x63, 0x9E, 0x03, 0x20, 0x06,
                0x10, 0x30 }, //1440*900_60RB
        { 0x80, 0xA0, 0x51, 0x1F, 0x84, 0x70, 0x73, 0xA6, 0x03, 0x98, 0x06,
                0x10, 0x30 }, //1440*900_60
        { 0xB0, 0x40, 0x60, 0x63, 0x84, 0x08, 0x73, 0xE8, 0x03, 0x50, 0x03,
                0x10, 0x30 }, //1600*900_60RB
        { 0xEF, 0x40, 0x61, 0x32, 0xB0, 0x70, 0x84, 0xE2, 0x04, 0xC0, 0x03,
                0x10, 0x30 }, //1600*1200_60
        { 0xC8, 0x90, 0x61, 0x24, 0x1A, 0xC0, 0x84, 0x41, 0x04, 0xB0, 0x06,
                0x10, 0x30 }, //1680*1050_60
        { 0xC0, 0x80, 0x70, 0x29, 0x38, 0x98, 0x84, 0x65, 0x04, 0x2C, 0x05,
                0x10, 0x30 }, //1920*1080_60
        { 0x70, 0x80, 0x70, 0x20, 0xB0, 0x20, 0x84, 0xD3, 0x04, 0x20, 0x06,
                0x10, 0x30 }, //1920*1200_60RB
        { 0x18, 0x80, 0x72, 0x2A, 0xB0, 0x20, 0xA4, 0xDD, 0x04, 0xC8, 0x06,
                0x10, 0x30 }, //1920*1200_60
        };

void RxVgaChangeDetect(void)
{
    unsigned char loop;
    unsigned short HsyncCnt = 0x0000;
    unsigned short VsyncCnt = 0x0000;

    curr_client = curr_lt8522ex->client_addr0;
    HsyncCnt = HDMI_ReadI2C_Byte(0xDD) * 256 + HDMI_ReadI2C_Byte(0xDC);
    VsyncCnt = (HDMI_ReadI2C_Byte(0xE0) & 0x0F) * 256 + HDMI_ReadI2C_Byte(0xDE);

    if (HsyncCnt >= 0xFF00) {
        Delay_ms(100);
        HsyncCnt = HDMI_ReadI2C_Byte(0xDD) * 256 + HDMI_ReadI2C_Byte(0xDC);
        if (HsyncCnt >= 0xFF00) {
            curr_lt8522ex->videoformat = SignnalOff;
        }
    } else {
        for (loop = 1; loop < 24; loop++) {
            if ((VsyncCnt >= VESA_Vtotal[loop - 1]) && (VsyncCnt < VESA_Vtotal[loop])) {
                if ((VsyncCnt - VESA_Vtotal[loop - 1]) <= (VESA_Vtotal[loop] - VsyncCnt))
                    VsyncCnt = VESA_Vtotal[loop - 1];
                else
                    VsyncCnt = VESA_Vtotal[loop];
                break;
            } else if (VsyncCnt >= VESA_Vtotal[23]) {
                VsyncCnt = VESA_Vtotal[23];
                break;
            } else if (VsyncCnt < VESA_Vtotal[0]) {
                VsyncCnt = VESA_Vtotal[0];
                break;
            }
        }
        switch (VsyncCnt) {
        case 0x20D:
            curr_lt8522ex->videoformat = VESA_640x480_60;
            break;
        case 0x1F4:
            curr_lt8522ex->videoformat = VESA_640x480_75;
            break;
        case 0x274:
            curr_lt8522ex->videoformat = VESA_800x600_60;
            break;
        case 0x271:
            curr_lt8522ex->videoformat = VESA_800x600_75;
            break;
        case 0x326:
            curr_lt8522ex->videoformat = VESA_1024x768_60;
            break;
        case 0x320:
            if (HsyncCnt <= 0x3500)
                curr_lt8522ex->videoformat = VESA_1024x768_75;
            else
                curr_lt8522ex->videoformat = VESA_1366x768_60RB;
            break;
        case 0x384:
            curr_lt8522ex->videoformat = VESA_1152x864_75;
            break;
        case 0x2EE:
            curr_lt8522ex->videoformat = VESA_1280x720_60;
            break;
        case 0x316:
            curr_lt8522ex->videoformat = VESA_1280x768_60RB;
            break;
        case 0x31E:
            if (HsyncCnt <= 0x4055)
                curr_lt8522ex->videoformat = VESA_1280x768_60;
            else if (HsyncCnt > 0x4055)
                curr_lt8522ex->videoformat = VESA_1366x768_60NB;
            break;
        case 0x337:
            curr_lt8522ex->videoformat = VESA_1280x800_60RB;
            break;
        case 0x33F:
            curr_lt8522ex->videoformat = VESA_1280x800_60;
            break;
        case 0x346:
            curr_lt8522ex->videoformat = VESA_1280x800_75;
            break;
        case 0x3E8:
            curr_lt8522ex->videoformat = VESA_1600x900_60RB;
            break;
        case 0x42A:
            if (HsyncCnt >= 0x3000)
                curr_lt8522ex->videoformat = VESA_1280x1024_60;
            else
                curr_lt8522ex->videoformat = VESA_1280x1024_75;
            break;
        case 0x31B:
            curr_lt8522ex->videoformat = VESA_1360x768_60;
            break;
        case 0x438:
            curr_lt8522ex->videoformat = VESA_1400x1050_60RB;
            break;
        case 0x441:
            if (HsyncCnt >= 0x2F10)
                curr_lt8522ex->videoformat = VESA_1400x1050_60;
            else
                curr_lt8522ex->videoformat = VESA_1680x1050_60;
            break;
        case 0x39E:
            curr_lt8522ex->videoformat = VESA_1440x900_60RB;
            break;
        case 0x3A6:
            curr_lt8522ex->videoformat = VESA_1440x900_60;
            break;
        case 0x4E2:
            curr_lt8522ex->videoformat = VESA_1600x1200_60;
            break;
        case 0x465:
            curr_lt8522ex->videoformat = VESA_1920x1080_60;
            break;
        case 0x4D3:
            curr_lt8522ex->videoformat = VESA_1920x1200_60RB;
            break;
        case 0x4DD:
            curr_lt8522ex->videoformat = VESA_1920x1200_60;
            break;
        default:
            curr_lt8522ex->videoformat = VESA_Nonstandard;
            break;
        }

        if (curr_lt8522ex->LastVideoFormat != curr_lt8522ex->videoformat) {
            curr_lt8522ex->FlagVgaTimingChange = 1;
        }
    }

    if ((curr_lt8522ex->videoformat == SignnalOff) || (curr_lt8522ex->videoformat == VESA_Nonstandard)) {
        LT8522EX_RxPowerDown(VGA);
    } else {
        LT8522EX_RxPowerOn(VGA);
    }

    if (curr_lt8522ex->LastVideoFormat == SignnalOff && curr_lt8522ex->videoformat != SignnalOff) {
        curr_lt8522ex->event |= EVENT_VGA_IN_PLUG_IN;
    } else if(curr_lt8522ex->LastVideoFormat != SignnalOff && curr_lt8522ex->videoformat == SignnalOff) {
        curr_lt8522ex->event |= EVENT_VGA_IN_PLUG_OUT;
    }

    curr_lt8522ex->LastVideoFormat = curr_lt8522ex->videoformat;
}

/*
 @interrupt void Interrupt_BestPhase(void)
 {
 unsigned char Int_phase;
 unsigned char Phase_Best;
 unsigned short SumValue;
 FlagInterrupt = TRUE;
 I2CADR = LT8522EX_ADR_2;
 Int_phase = HDMI_ReadI2C_Byte(0xf3);
 if(Int_phase&0x02)
 {
 Int_phase = HDMI_ReadI2C_Byte(0xfa);
 if(Int_phase&0x01)
 {
 I2CADR = LT8522EX_ADR_1;
 Phase_Best = HDMI_ReadI2C_Byte(0x1e);
 I2CADR = LT8522EX_ADR;
 HDMI_WriteI2C_Byte(0x27,Phase_Best);

 HDMI_WriteI2C_Byte(0xf9,0x00);
 Delay_ms(300);
 HDMI_WriteI2C_Byte(0xf9,0x63);
 Delay_ms(300);

 I2CADR = LT8522EX_ADR_1;
 SumValue = HDMI_ReadI2C_Byte(0x1d)*256+HDMI_ReadI2C_Byte(0x1c);
 if(SumValue > 0x22)
 {
 AutoPhaseDone = TRUE;
 }

 LT8522X_InterruptClear();
 I2CADR = LT8522EX_ADR;
 HDMI_WriteI2C_Byte(0xf9,0x00);
 I2CADR = LT8522EX_ADR_2;
 HDMI_WriteI2C_Byte(0xd3,0xff);//Low active for interrupt pin;
 HDMI_WriteI2C_Byte(0xda,0xff);// int mask
 }
 }
 }
 */
void CheckBestPhase(void)
{
    unsigned char Int_phase;
    unsigned char Phase_Best;
    unsigned short SumValue;

    curr_client = curr_lt8522ex->client_addr2;
    Int_phase = HDMI_ReadI2C_Byte(0xf3);
    if (Int_phase & 0x02) {
        Int_phase = HDMI_ReadI2C_Byte(0xfa);
        if (Int_phase & 0x01) {
            curr_client = curr_lt8522ex->client_addr1;
            Phase_Best = HDMI_ReadI2C_Byte(0x1e);
            curr_client = curr_lt8522ex->client_addr0;
            HDMI_WriteI2C_Byte(0x27, Phase_Best);
            HDMI_WriteI2C_Byte(0xf9, 0x00);
            Delay_ms(300);
            HDMI_WriteI2C_Byte(0xf9, 0x63);
            Delay_ms(100);

            curr_client = curr_lt8522ex->client_addr1;
            SumValue = HDMI_ReadI2C_Byte(0x1d) * 256 + HDMI_ReadI2C_Byte(0x1c);
            if (SumValue > 0x22) {
                curr_lt8522ex->AutoPhaseDone = 1;
            }

            LT8522EX_InterruptClear();
            curr_client = curr_lt8522ex->client_addr0;
            HDMI_WriteI2C_Byte(0xf9, 0x00);
            curr_client = curr_lt8522ex->client_addr2;
            HDMI_WriteI2C_Byte(0xd3, 0xff); //Low active for interrupt pin;
            HDMI_WriteI2C_Byte(0xda, 0xff); // int mask
        }
    }
}
