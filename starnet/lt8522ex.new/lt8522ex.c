#include "lt8522ex_misc.h"
#include "lt8522ex_main.h"

typedef enum {
    SET_VALUE = 1,
    SAVE_VALUE,
    READ_ADCOUT
} VRTOP_OPT;

FlagStatus R_AdjState;
FlagStatus G_AdjState;
FlagStatus B_AdjState;
char R_VrtopFlag;
char G_VrtopFlag;
char B_VrtopFlag;
unsigned char R_Vrtop = 0x20;
unsigned char G_Vrtop = 0x16;
unsigned char B_Vrtop = 0x20;
unsigned char R_AdjCnt;
unsigned char G_AdjCnt;
unsigned char B_AdjCnt;
volatile unsigned char R_Value = 0x00;
volatile unsigned char G_Value = 0x00;
volatile unsigned char B_Value = 0x00;
volatile unsigned char RxMode;

extern lt8522ex_t* curr_lt8522ex;
extern struct i2c_client* curr_client;

void LT8522EX_ClockEnable(void)
{
    curr_client = curr_lt8522ex->client_addr0;
    HDMI_WriteI2C_Byte(0x02, 0xff);
    HDMI_WriteI2C_Byte(0x03, 0xff);
    HDMI_WriteI2C_Byte(0x04, 0xff);
    HDMI_WriteI2C_Byte(0x05, 0xff);
    HDMI_WriteI2C_Byte(0x06, 0xff);
    HDMI_WriteI2C_Byte(0x07, 0xff);
}

void LT8522EX_ClockEnable_HDMI(void)
{
    curr_client = curr_lt8522ex->client_addr0;
    HDMI_WriteI2C_Byte(0x02, 0x00);
    HDMI_WriteI2C_Byte(0x03, 0x00);
    HDMI_WriteI2C_Byte(0x04, 0x18);
    HDMI_WriteI2C_Byte(0x05, 0x60);
    HDMI_WriteI2C_Byte(0x06, 0xF0);
    HDMI_WriteI2C_Byte(0x07, 0x03);
}

void LT8522EX_ClockEnable_VGA(void)
{
    curr_client = curr_lt8522ex->client_addr0;
    HDMI_WriteI2C_Byte(0x02, 0x3f);
    HDMI_WriteI2C_Byte(0x03, 0x9c);
    HDMI_WriteI2C_Byte(0x04, 0x01);
    HDMI_WriteI2C_Byte(0x05, 0x60);
    HDMI_WriteI2C_Byte(0x06, 0x1f);
    HDMI_WriteI2C_Byte(0x07, 0x00);
}

void LT8522EX_RxTxConfig(unsigned char rx, unsigned char tx)
{
    /**
     0x16 : Tx video&audio source select
     bit[7:4]reserved   bit[3:2] 00:VGA to HDMI   bit[1] 0:VGA to VGA     bit[0] 0(default):HDMI to HDMI de-packet mode
                                 11:HDMI to HDMI         1:HDMI to VGA           1:HDMI to HDMI no de-packet mode
     0x17 : Txpll reference clock source select.
     bit[7]reserved bit[6] 1:HDMI to VGA bit[5]  1:HDMI to HDMI      bit[4:0] 00000
                           0:VGA to VGA          0:VGA to HDMI
     **/
    if (tx == HDMI) {
        if (rx == HDMI) {
            curr_client = curr_lt8522ex->client_addr0;
            HDMI_WriteI2C_Byte(0x16, HDMI_ReadI2C_Byte(0x16) | 0x0D);
            HDMI_WriteI2C_Byte(0x17, HDMI_ReadI2C_Byte(0x17) | 0x20);
        } else if (rx == VGA) {
            curr_client = curr_lt8522ex->client_addr0;
            HDMI_WriteI2C_Byte(0x16, HDMI_ReadI2C_Byte(0x16) & 0xF2);
            HDMI_WriteI2C_Byte(0x17, HDMI_ReadI2C_Byte(0x17) & 0xDF);
        }
    } else if (tx == VGA) {
        if (rx == HDMI) {
            curr_client = curr_lt8522ex->client_addr0;
            HDMI_WriteI2C_Byte(0x16, HDMI_ReadI2C_Byte(0x16) | 0x02);
            HDMI_WriteI2C_Byte(0x17, HDMI_ReadI2C_Byte(0x17) | 0x40);
            curr_client = curr_lt8522ex->client_addr2;
            HDMI_WriteI2C_Byte(0x7c, 0x00);
        } else if (rx == VGA) {
            curr_client = curr_lt8522ex->client_addr0;
            HDMI_WriteI2C_Byte(0x16, HDMI_ReadI2C_Byte(0x16) & 0xFD);
            HDMI_WriteI2C_Byte(0x17, HDMI_ReadI2C_Byte(0x17) & 0xBF);
            curr_client = curr_lt8522ex->client_addr2;
            HDMI_WriteI2C_Byte(0x7c, 0x44);
        }
    }

    curr_client = curr_lt8522ex->client_addr0;
    HDMI_WriteI2C_Byte(0xfa, 0x02);
    HDMI_WriteI2C_Byte(0xfb, 0x02);
}

void LT8522EX_TxDriveConfig(unsigned char NewState)
{
    if (NewState) {
        curr_client = curr_lt8522ex->client_addr0;
        HDMI_WriteI2C_Byte(0x9c, 0x10); //rg_txpll_cpcur_sel
        HDMI_WriteI2C_Byte(0x97, 0x70); //main driver
        HDMI_WriteI2C_Byte(0x98, 0x62); //clock swing
        HDMI_WriteI2C_Byte(0x99, 0x62); //data swing
    } else {
        curr_client = curr_lt8522ex->client_addr0;
        HDMI_WriteI2C_Byte(0x9c, 0x10); //rg_txpll_cpcur_sel
        HDMI_WriteI2C_Byte(0x97, 0x00); //main driver
        HDMI_WriteI2C_Byte(0x98, 0x00); //clock swing
        HDMI_WriteI2C_Byte(0x99, 0x00); //data swing
    }
}

void LT8522EX_TxModeConfig(unsigned char mode)
{
    curr_client = curr_lt8522ex->client_addr2;
    if (mode)
        HDMI_WriteI2C_Byte(0x5C, HDMI_ReadI2C_Byte(0x5C) | 0x80);
    else
        HDMI_WriteI2C_Byte(0x5C, HDMI_ReadI2C_Byte(0x5C) & 0x7F);
}

void LT8522EX_RxModeConfig(unsigned char mode)
{
    curr_client = curr_lt8522ex->client_addr1;
    if (mode) {
        HDMI_WriteI2C_Byte(0xfa, 0x10); //Rx HDMI mode
        RxMode = HDMI;
    } else {
        HDMI_WriteI2C_Byte(0xfa, 0x00); //Rx DVI mode
        RxMode = DVI;
    }
}

void LT8522EX_RxEqConfig(void)
{
    curr_client = curr_lt8522ex->client_addr0;
    HDMI_WriteI2C_Byte(0x5C, 0x01); //eq_adap_en disable
    HDMI_WriteI2C_Byte(0x5E, 0xff); //CH0
    HDMI_WriteI2C_Byte(0x5F, 0xff); //CH1
    HDMI_WriteI2C_Byte(0x60, 0xff); //CH2
}

void LT8522EX_CurrentSet(void)
{
    curr_client = curr_lt8522ex->client_addr0;
    HDMI_WriteI2C_Byte(0x30, 0x12); //internal res enable
    HDMI_WriteI2C_Byte(0x4d, 0x80);
    HDMI_WriteI2C_Byte(0x4e, 0x80);
    HDMI_WriteI2C_Byte(0x4f, 0x20);
    HDMI_WriteI2C_Byte(0x50, 0x08);
}

void LT8522EX_AllModulePowerOn(void)
{
    curr_client = curr_lt8522ex->client_addr0;
    HDMI_WriteI2C_Byte(0x13, 0x00);
    HDMI_WriteI2C_Byte(0x14, 0x00);
    HDMI_WriteI2C_Byte(0x15, 0x03);
}

void LT8522EX_AudioConfig(unsigned char tx)
{
    if (tx == HDMI) {
        if ((curr_lt8522ex->pair1 == (LT8522EX_VGA_INPUT | LT8522EX_HDMI_OUTPUT)) ||
            (curr_lt8522ex->pair2 == (LT8522EX_VGA_INPUT | LT8522EX_HDMI_OUTPUT))) {
            curr_client = curr_lt8522ex->client_addr0;
            HDMI_WriteI2C_Byte(0x7F, 0x80); //audio input enable
            HDMI_WriteI2C_Byte(0x7E, 0x90);
            curr_client = curr_lt8522ex->client_addr3;
            HDMI_WriteI2C_Byte(0x06, 0x08);
            HDMI_WriteI2C_Byte(0x07, 0x10);
            HDMI_WriteI2C_Byte(0x35, 0x00);
            HDMI_WriteI2C_Byte(0x3c, 0x41);
        }
    } else if (tx == VGA) {
        if ((curr_lt8522ex->pair1 == (LT8522EX_HDMI_INPUT | LT8522EX_VGA_OUTPUT)) ||
            (curr_lt8522ex->pair2 == (LT8522EX_HDMI_INPUT | LT8522EX_VGA_OUTPUT))) {
            curr_client = curr_lt8522ex->client_addr1;
            HDMI_WriteI2C_Byte(0xFD, 0x80); //I2S out from hdmi.
            HDMI_WriteI2C_Byte(0xF9, 0xA1); //I2S 0ut0 only.
        } else if ((curr_lt8522ex->pair1 == (LT8522EX_VGA_INPUT | LT8522EX_VGA_OUTPUT)) ||
                   (curr_lt8522ex->pair2 == (LT8522EX_VGA_INPUT | LT8522EX_VGA_OUTPUT))) {
            curr_client = curr_lt8522ex->client_addr0;
            HDMI_WriteI2C_Byte(0x7F, 0x80); //audio input enable
            HDMI_WriteI2C_Byte(0x7E, 0x90);
            curr_client = curr_lt8522ex->client_addr1;
            HDMI_WriteI2C_Byte(0xFD, 0x00); //I2S out from VGA
            HDMI_WriteI2C_Byte(0xF9, 0xA1); //I2S 0ut0 only.
        }
    }
}

void LT8522EX_RxPowerOn(unsigned char rx)
{
    curr_client = curr_lt8522ex->client_addr0;

    if (rx == VGA)
        HDMI_WriteI2C_Byte(0x13, 0x00);
    else if (rx == HDMI)
        HDMI_WriteI2C_Byte(0x14, 0x00);
}

void LT8522EX_RxPowerDown(unsigned char rx)
{
    curr_client = curr_lt8522ex->client_addr0;

    if (rx == VGA)
        HDMI_WriteI2C_Byte(0x13, 0xaf);
    else if (rx == HDMI)
        HDMI_WriteI2C_Byte(0x14, 0xff);
}

void LT8522EX_CONFIG_INPUT_OUTPUT(void)
{
    unsigned char tx = 0xff, rx = 0xff;

    LT8522EX_RxPowerDown(HDMI);
    LT8522EX_RxPowerDown(VGA);

    if ((curr_lt8522ex->pair1 & LT8522EX_INPUT_MASK) == LT8522EX_HDMI_INPUT) {
        rx = HDMI;
    } else if((curr_lt8522ex->pair1 & LT8522EX_INPUT_MASK) == LT8522EX_VGA_INPUT) {
        rx = VGA;
    } else {
        rx = 0xff;
    }

    if ((curr_lt8522ex->pair1 & LT8522EX_OUTPUT_MASK) == LT8522EX_HDMI_OUTPUT) {
        tx = HDMI;
    } else if((curr_lt8522ex->pair1 & LT8522EX_OUTPUT_MASK) == LT8522EX_VGA_OUTPUT) {
        tx = VGA;
    } else {
        tx = 0xff;
    }

    if (tx != 0xff && rx != 0xff) {
        LT8522EX_RxTxConfig(rx, tx);
        LT8522EX_RxTxReset(rx, tx);
    }

    if ((curr_lt8522ex->pair2 & LT8522EX_INPUT_MASK) == LT8522EX_HDMI_INPUT) {
        rx = HDMI;
    } else if((curr_lt8522ex->pair2 & LT8522EX_INPUT_MASK) == LT8522EX_VGA_INPUT) {
        rx = VGA;
    } else {
        rx = 0xff;
    }

    if ((curr_lt8522ex->pair2 & LT8522EX_OUTPUT_MASK) == LT8522EX_HDMI_OUTPUT) {
        tx = HDMI;
    } else if((curr_lt8522ex->pair2 & LT8522EX_OUTPUT_MASK) == LT8522EX_VGA_OUTPUT) {
        tx = VGA;
    } else {
        tx = 0xff;
    }

    if (tx != 0xff && rx != 0xff) {
        LT8522EX_RxTxConfig(rx, tx);
        LT8522EX_RxTxReset(rx, tx);
    }
}

void LT8522EX_Init(void)
{
    LT8522EX_ClockEnable();
    LT8522EX_AllModulePowerOn();
    LT8522EX_RxPowerDown(HDMI);
    LT8522EX_RxPowerDown(VGA);
    LT8522EX_HdcpKeyLoad();
    LT8522EX_HdcpFunction();

    LT8522EX_CONFIG_INPUT_OUTPUT();

    LT8522EX_RxModeConfig(HDMI);
    LT8522EX_TxModeConfig(HDMI);
    LT8522EX_TxDriveConfig(DriveOn);

    LT8522EX_RxEqConfig();
    LT8522EX_CurrentSet();
    SaveEdidToShadow(VGA);
}

void LT8522EX_ColorConvert(unsigned char rxmode)
{
    unsigned char color;
    unsigned char colordepth;

    if (rxmode == HDMI) {
        curr_client = curr_lt8522ex->client_addr1;
        color = HDMI_ReadI2C_Byte(0x8D) & 0x60;
        colordepth = HDMI_ReadI2C_Byte(0xE7) & 0x0f;
        curr_client = curr_lt8522ex->client_addr2;
        switch (color) {
        case 0x00: //input is RGB
            HDMI_WriteI2C_Byte(0x8C, 0x08);
            break;
        case 0x20: //input is Y422
            HDMI_WriteI2C_Byte(0x8C, 0x12);
            break;
        case 0x40: //input is Y444
            HDMI_WriteI2C_Byte(0x8C, 0x0A);
            break;
        default:
            break;
        }
        //HDMI_WriteI2C_Byte(0x7C,HDMI_ReadI2C_Byte(0x7C)|0x08);//rgd_dac_rb_swap

        curr_client = curr_lt8522ex->client_addr0;
        switch (colordepth) {
        case 0x04: //8 bits
            HDMI_WriteI2C_Byte(0x51, 0x03);
            break;
        case 0x05: //10 bits
            if (color == 0x20)
                HDMI_WriteI2C_Byte(0x51, 0x03);
            else
                HDMI_WriteI2C_Byte(0x51, 0x13);
            break;
        case 0x06: //12 bits
            if (color == 0x20)
                HDMI_WriteI2C_Byte(0x51, 0x03);
            else
                HDMI_WriteI2C_Byte(0x51, 0x23);
            break;
        default:
            HDMI_WriteI2C_Byte(0x51, 0x03);
            break;
        }
    } else {
        curr_client = curr_lt8522ex->client_addr2;
        HDMI_WriteI2C_Byte(0x8C, 0x08);
    }
}

void LT8522EX_RxTxReset(unsigned char rx, unsigned char tx)
{
    curr_client = curr_lt8522ex->client_addr0;
    HDMI_WriteI2C_Byte(0x0d, 0x7f);
    Delay_ms(1);
    HDMI_WriteI2C_Byte(0x0d, 0xff); //Rxphy globle soft
    if ((rx == HDMI) && (tx == VGA)) {
        HDMI_WriteI2C_Byte(0x0e, 0xe0);
        Delay_ms(1);
        HDMI_WriteI2C_Byte(0x0e, 0xe1); //Reset RX PLL.
        Delay_ms(1);
        HDMI_WriteI2C_Byte(0x0e, 0xef); //Reset RX PI.
        Delay_ms(1);
        HDMI_WriteI2C_Byte(0x0e, 0xff); //Reset RX CDR.
        HDMI_WriteI2C_Byte(0x0f, 0x78);
        Delay_ms(1);
        HDMI_WriteI2C_Byte(0x0f, 0xff);
        HDMI_WriteI2C_Byte(0x10, 0xf7);
        Delay_ms(1);
        HDMI_WriteI2C_Byte(0x10, 0xff);
        HDMI_WriteI2C_Byte(0x11, 0xbf);
        Delay_ms(1);
        HDMI_WriteI2C_Byte(0x11, 0xff);
        HDMI_WriteI2C_Byte(0x0C, 0xFD);
        Delay_ms(1);
        HDMI_WriteI2C_Byte(0x0C, 0xFF);
    } else if ((rx == VGA) && (tx == VGA)) {
        HDMI_WriteI2C_Byte(0x10, 0xdf);
        Delay_ms(1);
        HDMI_WriteI2C_Byte(0x10, 0xff);
    } else if ((rx == VGA) && (tx == HDMI)) {
        HDMI_WriteI2C_Byte(0x0D, 0xEF);
        Delay_ms(1);
        HDMI_WriteI2C_Byte(0x0D, 0xff); //Rxphy global soft reset.
        HDMI_WriteI2C_Byte(0x0D, 0xBF);
        Delay_ms(1);
        HDMI_WriteI2C_Byte(0x0D, 0xff); //Txphy soft reset.
        HDMI_WriteI2C_Byte(0x0D, 0xDF);
        Delay_ms(1);
        HDMI_WriteI2C_Byte(0x0D, 0xff); //Txpll soft reset.
        HDMI_WriteI2C_Byte(0x10, 0xEF);
        Delay_ms(1);
        HDMI_WriteI2C_Byte(0x10, 0xFF); //Vga2hdmi mode afifo soft reset
    } else if ((rx == HDMI) && (tx == HDMI)) {
        HDMI_WriteI2C_Byte(0x0e, 0xe0);
        Delay_ms(1);
        HDMI_WriteI2C_Byte(0x0e, 0xe1); //Reset RX PLL.
        Delay_ms(1);
        HDMI_WriteI2C_Byte(0x0e, 0xef); //Reset RX PI.
        Delay_ms(1);
        HDMI_WriteI2C_Byte(0x0e, 0xff); //Reset RX CDR.
        HDMI_WriteI2C_Byte(0x10, 0xfb);
        Delay_ms(1);
        HDMI_WriteI2C_Byte(0x10, 0xff);
    }
}

void LT8522EX_AdcReset(void)
{
    curr_client = curr_lt8522ex->client_addr0;
    HDMI_WriteI2C_Byte(0x0c, 0xc7);
    Delay_ms(1);
    HDMI_WriteI2C_Byte(0x0c, 0xff);
}

void LT8522EX_AdcPllInit(void)
{
    curr_client = curr_lt8522ex->client_addr0;

    switch (curr_lt8522ex->videoformat) {
    case VESA_640x480_60: /*25.175 */
        HDMI_WriteI2C_Byte(0x24, 0x28);
        break;
    case VESA_640x480_75: /*25.175 */
        HDMI_WriteI2C_Byte(0x24, 0x28);
        break;
    case VESA_800x600_60: /*40 */
        //HDMI_WriteI2C_Byte( 0x24, 0x18 );
        HDMI_WriteI2C_Byte(0x24, 0x40);       //190702
        break;
    case VESA_800x600_75: /*49.5 */
        HDMI_WriteI2C_Byte(0x24, 0x44);
        break;
    case VESA_1024x768_60: /*65 */
        HDMI_WriteI2C_Byte(0x24, 0x84);
        break;
    case VESA_1024x768_75: /*78.750 */
        HDMI_WriteI2C_Byte(0x24, 0x94);       //0x54 );
        break;
    case VESA_1152x864_75: /*108 */
        HDMI_WriteI2C_Byte(0x24, 0x52);       //52
        break;
    case VESA_1280x720_60: /*74.25 */
        HDMI_WriteI2C_Byte(0x24, 0x64);   //d4
        break;
    case VESA_1280x768_60RB: /*68.25 */
        HDMI_WriteI2C_Byte(0x24, 0x93);
        break;
    case VESA_1280x768_60: /*79.5 */
        HDMI_WriteI2C_Byte(0x24, 0x93);
        break;
    case VESA_1280x800_60RB: /*71 */
        HDMI_WriteI2C_Byte(0x24, 0xd4);
        break;
    case VESA_1280x800_60: /*83.5 */
        HDMI_WriteI2C_Byte(0x24, 0xd4);
        break;
    case VESA_1280x800_75: /*106.5 */
        HDMI_WriteI2C_Byte(0x24, 0xd2);
        break;
    case VESA_1280x960_60: /*108 */
        HDMI_WriteI2C_Byte(0x24, 0xd2);
        break;
    case VESA_1280x1024_60: /*108 */
        HDMI_WriteI2C_Byte(0x24, 0x74);   //0x52 );
        break;
    case VESA_1280x1024_75: /*135 */
        HDMI_WriteI2C_Byte(0x24, 0x93);       //d2
        break;
    case VESA_1360x768_60: /*85.5 */
        HDMI_WriteI2C_Byte(0x24, 0x93);       //d2
        break;
    case VESA_1366x768_60RB: /*72 */
        HDMI_WriteI2C_Byte(0x24, 0x94);
        break;
    case VESA_1366x768_60NB: /*85 */
        HDMI_WriteI2C_Byte(0x24, 0x94);
        break;
    case VESA_1400x1050_60RB: /*101 */
        HDMI_WriteI2C_Byte(0x24, 0x42);
        break;
    case VESA_1400x1050_60: /*121.75 */
        HDMI_WriteI2C_Byte(0x24, 0x42);
        break;
    case VESA_1440x900_60RB: /*88.75 */
        HDMI_WriteI2C_Byte(0x24, 0xd2);
        break;
    case VESA_1440x900_60: /*106.5 */
        HDMI_WriteI2C_Byte(0x24, 0xd2);
        break;
    case VESA_1600x900_60RB: /*108 */
        HDMI_WriteI2C_Byte(0x24, 0xd2);
        break;
    case VESA_1600x1200_60: /*162 */
        HDMI_WriteI2C_Byte(0x24, 0xd3);
        break;
    case VESA_1680x1050_60: /*146.25 */
        HDMI_WriteI2C_Byte(0x24, 0x94);
        break;
    case VESA_1920x1080_60: /*148.5 */
        HDMI_WriteI2C_Byte(0x24, 0xd3);
        break;
    case VESA_1920x1200_60RB: /*154 */
        HDMI_WriteI2C_Byte(0x24, 0xd2);
        break;
    case VESA_1920x1200_60: /*193.25 */
        HDMI_WriteI2C_Byte(0x24, 0xd2);
        break;
    default:
        break;
    }

    HDMI_WriteI2C_Byte(0x25, 0x30); //third-order R.......
    HDMI_WriteI2C_Byte(0x26, 0x18);
}
EXPORT_SYMBOL_GPL(LT8522EX_AdcPllInit);

void LT8522EX_AdcPllDiv(void)
{
    curr_client = curr_lt8522ex->client_addr0;
    HDMI_WriteI2C_Byte(0x20, 0x01);

    switch (curr_lt8522ex->videoformat) {
    case VESA_640x480_60: //25.175
        HDMI_WriteI2C_Byte(0x21, 0x20);
        HDMI_WriteI2C_Byte(0x22, 0x03);
        HDMI_WriteI2C_Byte(0x23, 0x08);
        break;
    case VESA_640x480_75: //31.5
        HDMI_WriteI2C_Byte(0x21, 0x48);
        HDMI_WriteI2C_Byte(0x22, 0x03);
        HDMI_WriteI2C_Byte(0x23, 0x08);
        break;
    case VESA_800x600_60: //40
        HDMI_WriteI2C_Byte(0x21, 0x20);
        HDMI_WriteI2C_Byte(0x22, 0x04);
        HDMI_WriteI2C_Byte(0x23, 0x08);
        break;
    case VESA_800x600_75: //49.5
        HDMI_WriteI2C_Byte(0x21, 0x20);
        HDMI_WriteI2C_Byte(0x22, 0x04);
        HDMI_WriteI2C_Byte(0x23, 0x04);
        break;
    case VESA_1024x768_60: //65
        HDMI_WriteI2C_Byte(0x21, 0x40);
        HDMI_WriteI2C_Byte(0x22, 0x05);
        HDMI_WriteI2C_Byte(0x23, 0x04);
        break;
    case VESA_1024x768_75: //78.750
        HDMI_WriteI2C_Byte(0x21, 0x20);
        HDMI_WriteI2C_Byte(0x22, 0x05);
        HDMI_WriteI2C_Byte(0x23, 0x04);
        break;
    case VESA_1152x864_75: //108
        HDMI_WriteI2C_Byte(0x21, 0x40);
        HDMI_WriteI2C_Byte(0x22, 0x06);
        HDMI_WriteI2C_Byte(0x23, 0x02);
        break;
    case VESA_1280x720_60: //74.25
        HDMI_WriteI2C_Byte(0x21, 0x72);
        HDMI_WriteI2C_Byte(0x22, 0x06);
        HDMI_WriteI2C_Byte(0x23, 0x04);
        break;
    case VESA_1280x768_60RB: //68.25
        HDMI_WriteI2C_Byte(0x21, 0xA0);
        HDMI_WriteI2C_Byte(0x22, 0x05);
        HDMI_WriteI2C_Byte(0x23, 0x04);
        break;
    case VESA_1280x768_60: //79.5
        HDMI_WriteI2C_Byte(0x21, 0x80);
        HDMI_WriteI2C_Byte(0x22, 0x06);
        HDMI_WriteI2C_Byte(0x23, 0x04);
        break;
    case VESA_1280x800_60RB: //71
        HDMI_WriteI2C_Byte(0x21, 0xA0);
        HDMI_WriteI2C_Byte(0x22, 0x05);
        HDMI_WriteI2C_Byte(0x23, 0x04);
        break;
    case VESA_1280x800_60: //83.5
        HDMI_WriteI2C_Byte(0x21, 0x90);
        HDMI_WriteI2C_Byte(0x22, 0x06);
        HDMI_WriteI2C_Byte(0x23, 0x04);
        break;
    case VESA_1280x800_75: //106.5
        HDMI_WriteI2C_Byte(0x21, 0xA0);
        HDMI_WriteI2C_Byte(0x22, 0x06);
        HDMI_WriteI2C_Byte(0x23, 0x02);
        break;
    case VESA_1280x960_60: //108
        HDMI_WriteI2C_Byte(0x21, 0x08);
        HDMI_WriteI2C_Byte(0x22, 0x07);
        HDMI_WriteI2C_Byte(0x23, 0x02);
        break;
    case VESA_1280x1024_60: //108
        HDMI_WriteI2C_Byte(0x21, 0x98);
        HDMI_WriteI2C_Byte(0x22, 0x06);
        HDMI_WriteI2C_Byte(0x23, 0x02);
        break;
    case VESA_1280x1024_75: //135
        HDMI_WriteI2C_Byte(0x21, 0x98);
        HDMI_WriteI2C_Byte(0x22, 0x06);
        HDMI_WriteI2C_Byte(0x23, 0x02);
        break;
    case VESA_1360x768_60: //85.5
        HDMI_WriteI2C_Byte(0x21, 0x00); //0x00
        HDMI_WriteI2C_Byte(0x22, 0x07);
        HDMI_WriteI2C_Byte(0x23, 0x04);
        break;
    case VESA_1366x768_60RB: //72
        HDMI_WriteI2C_Byte(0x21, 0xdc);
        HDMI_WriteI2C_Byte(0x22, 0x05);
        HDMI_WriteI2C_Byte(0x23, 0x04);
        break;
    case VESA_1366x768_60NB: //85.5
        HDMI_WriteI2C_Byte(0x21, 0x00);
        HDMI_WriteI2C_Byte(0x22, 0x07);
        HDMI_WriteI2C_Byte(0x23, 0x04);
        break;
    case VESA_1400x1050_60RB: //101
        HDMI_WriteI2C_Byte(0x21, 0x18);
        HDMI_WriteI2C_Byte(0x22, 0x06);
        HDMI_WriteI2C_Byte(0x23, 0x02);
        break;
    case VESA_1400x1050_60: //121.75
        HDMI_WriteI2C_Byte(0x21, 0x48);
        HDMI_WriteI2C_Byte(0x22, 0x07);
        HDMI_WriteI2C_Byte(0x23, 0x02);
        break;
    case VESA_1440x900_60RB: //88.75
        HDMI_WriteI2C_Byte(0x21, 0x40);
        HDMI_WriteI2C_Byte(0x22, 0x06);
        HDMI_WriteI2C_Byte(0x23, 0x02);
        break;
    case VESA_1440x900_60: //106.5
        HDMI_WriteI2C_Byte(0x21, 0x70);
        HDMI_WriteI2C_Byte(0x22, 0x07);
        HDMI_WriteI2C_Byte(0x23, 0x02);
        break;
    case VESA_1600x900_60RB: //108
        HDMI_WriteI2C_Byte(0x21, 0x08);
        HDMI_WriteI2C_Byte(0x22, 0x07);
        HDMI_WriteI2C_Byte(0x23, 0x02);
        break;
    case VESA_1600x1200_60: //162
        HDMI_WriteI2C_Byte(0x21, 0x70);
        HDMI_WriteI2C_Byte(0x22, 0x08);
        HDMI_WriteI2C_Byte(0x23, 0x02);
        break;
    case VESA_1680x1050_60: //146.25
        HDMI_WriteI2C_Byte(0x21, 0xc0);
        HDMI_WriteI2C_Byte(0x22, 0x08);
        HDMI_WriteI2C_Byte(0x23, 0x02);
        break;
    case VESA_1920x1080_60: //148.5
        HDMI_WriteI2C_Byte(0x21, 0x98);
        HDMI_WriteI2C_Byte(0x22, 0x08);
        HDMI_WriteI2C_Byte(0x23, 0x02); //170-340:01,85-170:02,42.5-85:04,<42.5:08
        break;
    case VESA_1920x1200_60RB: //154
        HDMI_WriteI2C_Byte(0x21, 0x20);
        HDMI_WriteI2C_Byte(0x22, 0x08);
        HDMI_WriteI2C_Byte(0x23, 0x02); //170-340:01,85-170:02,42.5-85:04,<42.5:08
        break;
    case VESA_1920x1200_60: //193.25
        HDMI_WriteI2C_Byte(0x21, 0x20);
        HDMI_WriteI2C_Byte(0x22, 0x0a);
        HDMI_WriteI2C_Byte(0x23, 0x02); //170-340:01,85-170:02,42.5-85:04,<42.5:08
        break;
    default:
        break;

    }

    Delay_ms(30);
    HDMI_WriteI2C_Byte(0x0c, 0xef);
    Delay_ms(1);
    HDMI_WriteI2C_Byte(0x0c, 0xff);
}

void LT8522EX_SetTimingReg(void)
{
    curr_client = curr_lt8522ex->client_addr0;
    HDMI_WriteI2C_Byte(0xc9, VideoFormatTab[curr_lt8522ex->videoformat][GCM_DE_DLY_LSB]);
    HDMI_WriteI2C_Byte(0xca, VideoFormatTab[curr_lt8522ex->videoformat][GCM_DE_CNT_LSB]);
    HDMI_WriteI2C_Byte(0xcb, VideoFormatTab[curr_lt8522ex->videoformat][GCM_DE_CNT_DLY_MSB]);
    HDMI_WriteI2C_Byte(0xcc, VideoFormatTab[curr_lt8522ex->videoformat][GCM_DE_TOP]);
    HDMI_WriteI2C_Byte(0xcd, VideoFormatTab[curr_lt8522ex->videoformat][GCM_DE_LIN_LSB]);
    HDMI_WriteI2C_Byte(0xce, VideoFormatTab[curr_lt8522ex->videoformat][GCM_H_TOTAL_LSB]);
    HDMI_WriteI2C_Byte(0xcf, VideoFormatTab[curr_lt8522ex->videoformat][GCM_H_TOTAL_LIN_MSB]);

    HDMI_WriteI2C_Byte(0xd0, VideoFormatTab[curr_lt8522ex->videoformat][GCM_V_TOTAL_LSB]);
    HDMI_WriteI2C_Byte(0xd1, VideoFormatTab[curr_lt8522ex->videoformat][GCM_SG_V_TOTAL_MSB]);
    HDMI_WriteI2C_Byte(0xd2, VideoFormatTab[curr_lt8522ex->videoformat][GCM_HWIDTH_LSB]);
    HDMI_WriteI2C_Byte(0xd3, VideoFormatTab[curr_lt8522ex->videoformat][GCM_H_V_TOTAL_MSB]);
    HDMI_WriteI2C_Byte(0x45, VideoFormatTab[curr_lt8522ex->videoformat][GCM_CLAMP_POSITION]);
    HDMI_WriteI2C_Byte(0x46, VideoFormatTab[curr_lt8522ex->videoformat][GCM_CLAMP_DURATION]);
}

void LT8522EX_SetTimingWindow(void) //we need to give a window to let 8522x calculate the phase.
{
    unsigned short D_CNT_Dly = 0x0000;
    unsigned short D_line_top = 0x0000;
    curr_client = curr_lt8522ex->client_addr0;

    D_CNT_Dly = (VideoFormatTab[curr_lt8522ex->videoformat][0x02] & 0x0f) * 256
            + ((VideoFormatTab[curr_lt8522ex->videoformat][0x02] & 0xf0) >> 4) * 256
            + VideoFormatTab[curr_lt8522ex->videoformat][0x00]
            + VideoFormatTab[curr_lt8522ex->videoformat][0x01];

    HDMI_WriteI2C_Byte(0xf0, VideoFormatTab[curr_lt8522ex->videoformat][0x00]);
    HDMI_WriteI2C_Byte(0xf1, (unsigned char) D_CNT_Dly);
    HDMI_WriteI2C_Byte(0xf2, (((unsigned char) (D_CNT_Dly >> 4)) & 0xf0) | (VideoFormatTab[curr_lt8522ex->videoformat][0x02] & 0x0f));
    D_line_top = (VideoFormatTab[curr_lt8522ex->videoformat][0x06] & 0x0f) * 256
            + VideoFormatTab[curr_lt8522ex->videoformat][0x03]
            + VideoFormatTab[curr_lt8522ex->videoformat][0x04];
    HDMI_WriteI2C_Byte(0xf3, VideoFormatTab[curr_lt8522ex->videoformat][0x03]);
    HDMI_WriteI2C_Byte(0xf4, (unsigned char) D_line_top);
    HDMI_WriteI2C_Byte(0xf5, ((unsigned char) (D_line_top >> 4)) & 0xF0);
}

void LT8522EX_SetSyncPol(void)
{
    unsigned char Pol_Detect;
    unsigned char Pol_Config;

    curr_client = curr_lt8522ex->client_addr2;
    Pol_Config = HDMI_ReadI2C_Byte(0x5c) & 0x80;

    curr_client = curr_lt8522ex->client_addr0;
    Pol_Detect = HDMI_ReadI2C_Byte(0xD8);

    switch (Pol_Detect) {
    case 0x1c:
        curr_client = curr_lt8522ex->client_addr0;
        HDMI_WriteI2C_Byte(0x28, 0x90);
        HDMI_WriteI2C_Byte(0xb0, 0x98);
        curr_client = curr_lt8522ex->client_addr2;
        HDMI_WriteI2C_Byte(0x5c, Pol_Config | 0x04);
        break;
    case 0x1d:
        curr_client = curr_lt8522ex->client_addr0;
        HDMI_WriteI2C_Byte(0x28, 0x90);
        HDMI_WriteI2C_Byte(0xb0, 0x9c);
        curr_client = curr_lt8522ex->client_addr2;
        HDMI_WriteI2C_Byte(0x5c, Pol_Config | 0x0c);
        break;
    case 0x1e:
        curr_client = curr_lt8522ex->client_addr0;
        HDMI_WriteI2C_Byte(0x28, 0x92);
        HDMI_WriteI2C_Byte(0xb0, 0x98);
        curr_client = curr_lt8522ex->client_addr2;
        HDMI_WriteI2C_Byte(0x5c, Pol_Config | 0x04);
        break;
    case 0x1f:
        curr_client = curr_lt8522ex->client_addr0;
        HDMI_WriteI2C_Byte(0x28, 0x92);
        HDMI_WriteI2C_Byte(0xb0, 0x9c);
        curr_client = curr_lt8522ex->client_addr2;
        HDMI_WriteI2C_Byte(0x5c, Pol_Config | 0x0c);
        break;
    default:
        break;
    }

    //0917 add
    if (curr_lt8522ex->videoformat == VESA_1024x768_60) {
        //rgd_dac_vsync_pol_adj+rgd_dac_hsync_pol_adj
        curr_client = curr_lt8522ex->client_addr2;
        HDMI_WriteI2C_Byte(0x5b, 0x13);
    } else {
        curr_client = curr_lt8522ex->client_addr2;
        HDMI_WriteI2C_Byte(0x5b, 0x10);
    }
}

void LT8522EX_HdcpKeyLoad(void)
{
    /*
     ReadStep = 0x00;
     KeyDataCnt = 0x00;
     FlagStopI2cSlave = 0;*/
    curr_client = curr_lt8522ex->client_addr2;
    HDMI_WriteI2C_Byte(0x02, 0x85);
    HDMI_WriteI2C_Byte(0x08, 0x04);
    HDMI_WriteI2C_Byte(0x06, 0xfd);
    Delay_ms(500);
    HDMI_WriteI2C_Byte(0x06, 0xf5);
}

void LT8522EX_HdcpFunction(void)
{
    curr_client = curr_lt8522ex->client_addr2;
    HDMI_WriteI2C_Byte(0x11, 0x3e);
    HDMI_WriteI2C_Byte(0x12, 0x2c);
    HDMI_WriteI2C_Byte(0x24, 0x33); //rx only
}

#if 0
void LT8522EX_RGBClamp(void)
{
    R_AdjState = FLASH_ReadByte(0x4000);
    B_AdjState = FLASH_ReadByte(0x4020);
    curr_client = curr_lt8522ex->client_addr0;
    HDMI_WriteI2C_Byte(0x30, 0x0c);
    HDMI_WriteI2C_Byte(0x31, 0x80);
    HDMI_WriteI2C_Byte(0x32, 0x04);
    HDMI_WriteI2C_Byte(0x33, 0x80);
    HDMI_WriteI2C_Byte(0x35, 0x80);
    HDMI_WriteI2C_Byte(0x37, 0x10); //0x07
    HDMI_WriteI2C_Byte(0x38, 0x50); //
    HDMI_WriteI2C_Byte(0x3e, 0x40);
    if (R_AdjState) {
        HDMI_WriteI2C_Byte(0x34, FLASH_ReadByte(0x4001));
        HDMI_WriteI2C_Byte(0x36, FLASH_ReadByte(0x4002));
    } else {
        HDMI_WriteI2C_Byte(0x34, 0x0c);
        HDMI_WriteI2C_Byte(0x36, 0x00);
    }

    if (B_AdjState) {
        HDMI_WriteI2C_Byte(0x3d, FLASH_ReadByte(0x4021));
        HDMI_WriteI2C_Byte(0x3f, FLASH_ReadByte(0x4022));
    } else {
        HDMI_WriteI2C_Byte(0x3d, 0x80);
        HDMI_WriteI2C_Byte(0x3f, 0x18);
    }
}

void LT8522EX_VrtopSelect(void)
{
    R_AdjState = FLASH_ReadByte( 0x4000 );
    B_AdjState = FLASH_ReadByte( 0x4020 );
    G_AdjState = FLASH_ReadByte( 0x4010 );
    curr_client = curr_lt8522ex->client_addr0;

    if (R_AdjState) {
        HDMI_WriteI2C_Byte(0x40, FLASH_ReadByte(0x4003));
        HDMI_WriteI2C_Byte(0x41, FLASH_ReadByte(0x4004));
    } else {
        HDMI_WriteI2C_Byte(0x40, 0x08);
        HDMI_WriteI2C_Byte(0x41, 0x06);
    }
    if (G_AdjState) {
        HDMI_WriteI2C_Byte(0x42, FLASH_ReadByte(0x4011));
    } else {
        HDMI_WriteI2C_Byte(0x42, 0x2d); //rg_vrtop_sel<6:0>
    }
    HDMI_WriteI2C_Byte(0x43, 0xAE); //Clamp start
    HDMI_WriteI2C_Byte(0x43, 0x2E); //.....
    HDMI_WriteI2C_Byte(0x44, 0x9f);
    HDMI_WriteI2C_Byte(0x45, 0x10);
    HDMI_WriteI2C_Byte(0x46, 0x30);
}

void LT8522EX_AdjustColor(void)
{
    volatile unsigned char DataChar;
    volatile unsigned char DataChar0;
    volatile unsigned char DataChar1;
    G_AdjState = FLASH_ReadByte(0x4010);
    R_AdjState = FLASH_ReadByte(0x4000);
    B_AdjState = FLASH_ReadByte(0x4020);

    if (!G_AdjState) {
        for (G_AdjCnt = 0; G_Vrtop != 0; G_AdjCnt++) {
            curr_client = curr_lt8522ex->client_addr0;
            DataChar = (G_Vrtop << 1) | 0x01;
            HDMI_WriteI2C_Byte(0X42, (G_Vrtop << 1) | 0x01);
            Delay_ms(5);
            HDMI_WriteI2C_Byte(0x44, 0x9f); //ECO
            Delay_ms(10);
            HDMI_WriteI2C_Byte(0x44, 0x8f);
            Delay_ms(5);
            DataChar0 = (HDMI_ReadI2C_Byte(0x70) << 5); //adc_dout_g[7:5]:
            DataChar1 = ((HDMI_ReadI2C_Byte(0x73) & 0x3e) >> 1); //adc_dout_g[4:0]:
            G_Value = DataChar0 | DataChar1;
            if (G_Value >= 0xf0) {
                if ((G_Value < 0xff) && G_VrtopFlag) {
                    curr_client = curr_lt8522ex->client_addr0;
                    HDMI_WriteI2C_Byte(0X42, (G_Vrtop << 1) | 0x01);
                    FLASH_ProgramByte(0x4011, (G_Vrtop << 1) | 0x01);
                    FLASH_ProgramByte(0x4010, 0x01);
                    FLASH_ProgramByte(0x4012, G_Vrtop);
                    break;
                }
            }
            if (G_Value == 0xff) {
                G_Vrtop++;
                G_VrtopFlag = 1;
                curr_client = curr_lt8522ex->client_addr0;
                DataChar = (G_Vrtop << 1) | 0x01;
                HDMI_WriteI2C_Byte(0X42, (G_Vrtop << 1) | 0x01);
                Delay_ms(5);
                HDMI_WriteI2C_Byte(0x44, 0x9f);
                Delay_ms(10);
                HDMI_WriteI2C_Byte(0x44, 0x8f);
                Delay_ms(5);
                DataChar0 = (HDMI_ReadI2C_Byte(0x70) << 5);             //adc_dout_g[7:5]:
                DataChar1 = ((HDMI_ReadI2C_Byte(0x73) & 0x3e) >> 1);    //adc_dout_g[4:0]:
                G_Value = DataChar0 | DataChar1;
                if ((G_Value >= 0xfa) && (G_Value < 0xff)) {
                    curr_client = curr_lt8522ex->client_addr0;
                    HDMI_WriteI2C_Byte(0X42, (G_Vrtop << 1) | 0x01);
                    FLASH_ProgramByte(0x4011, (G_Vrtop << 1) | 0x01);
                    FLASH_ProgramByte(0x4010, 0x01);
                    FLASH_ProgramByte(0x4012, G_Vrtop);
                    break;
                }
            } else {
                if (G_Value >= 0x80) {
                    G_Vrtop -= 1;
                }
            }
        }
    }
    if (!R_AdjState) {
        for (R_AdjCnt = 0; R_Vrtop != 0; R_AdjCnt++) {
            curr_client = curr_lt8522ex->client_addr0;
            HDMI_WriteI2C_Byte(0x44, 0x9f);
            Delay_ms(10);
            HDMI_WriteI2C_Byte(0x44, 0x8f);
            HDMI_WriteI2C_Byte(0x34, ((R_Vrtop >> 2) & 0x08) | 0x04);
            HDMI_WriteI2C_Byte(0x36, (R_Vrtop >> 3) & 0x03);
            HDMI_WriteI2C_Byte(0x40, ((R_Vrtop << 5) & 0x80) | ((R_Vrtop << 3) & 0x10) | 0x08);
            HDMI_WriteI2C_Byte(0x41, ((R_Vrtop << 4) & 0x10) | 0x06);
            R_Value = HDMI_ReadI2C_Byte(0x69); //adc_dout_r[7:0]:
            if (R_Value >= 0xf0) {
                if ((R_Value < 0xff) && R_VrtopFlag) {
                    curr_client = curr_lt8522ex->client_addr0;
                    HDMI_WriteI2C_Byte(0x34, ((R_Vrtop >> 2) & 0x08) | 0x04);
                    HDMI_WriteI2C_Byte(0x36, (R_Vrtop >> 3) & 0x03);
                    HDMI_WriteI2C_Byte(0x40, ((R_Vrtop << 5) & 0x80) | ((R_Vrtop << 3) & 0x10) | 0x08);
                    HDMI_WriteI2C_Byte(0x41, ((R_Vrtop << 4) & 0x10) | 0x06);
                    FLASH_ProgramByte(0x4001, ((R_Vrtop >> 2) & 0x08) | 0x04);
                    FLASH_ProgramByte(0x4002, (R_Vrtop >> 3) & 0x03);
                    FLASH_ProgramByte(0x4003, ((R_Vrtop << 5) & 0x80) | ((R_Vrtop << 3) & 0x10) | 0x08);
                    FLASH_ProgramByte(0x4004, ((R_Vrtop << 4) & 0x10) | 0x06);
                    FLASH_ProgramByte(0x4000, 0x01);
                    FLASH_ProgramByte(0x4005, R_Vrtop);
                    break;
                }
            }
            if (R_Value == 0xff) {
                R_Vrtop++;
                R_VrtopFlag = 1;
                curr_client = curr_lt8522ex->client_addr0;
                HDMI_WriteI2C_Byte(0x44, 0x9f);
                Delay_ms(10);
                HDMI_WriteI2C_Byte(0x44, 0x8f);
                HDMI_WriteI2C_Byte(0x34, ((R_Vrtop >> 2) & 0x08) | 0x04);
                HDMI_WriteI2C_Byte(0x36, (R_Vrtop >> 3) & 0x03);
                HDMI_WriteI2C_Byte(0x40, ((R_Vrtop << 5) & 0x80) | ((R_Vrtop << 3) & 0x10) | 0x08);
                HDMI_WriteI2C_Byte(0x41, ((R_Vrtop << 4) & 0x10) | 0x06);
                R_Value = HDMI_ReadI2C_Byte(0x69); //adc_dout_r[7:0]:
                if ((R_Value >= 0xfa) && (R_Value < 0xff)) {
                    curr_client = curr_lt8522ex->client_addr0;
                    HDMI_WriteI2C_Byte(0x34, ((R_Vrtop >> 2) & 0x08) | 0x04);
                    HDMI_WriteI2C_Byte(0x36, (R_Vrtop >> 3) & 0x03);
                    HDMI_WriteI2C_Byte(0x40, ((R_Vrtop << 5) & 0x80) | ((R_Vrtop << 3) & 0x10) | 0x08);
                    HDMI_WriteI2C_Byte(0x41, ((R_Vrtop << 4) & 0x10) | 0x06);
                    FLASH_ProgramByte(0x4001, ((R_Vrtop >> 2) & 0x08) | 0x04);
                    FLASH_ProgramByte(0x4002, (R_Vrtop >> 3) & 0x03);
                    FLASH_ProgramByte(0x4003, ((R_Vrtop << 5) & 0x80) | ((R_Vrtop << 3) & 0x10) | 0x08);
                    FLASH_ProgramByte(0x4004, ((R_Vrtop << 4) & 0x10) | 0x06);
                    FLASH_ProgramByte(0x4000, 0x01);
                    FLASH_ProgramByte(0x4005, R_Vrtop);
                    break;
                }
            } else {
                if (R_Value >= 0x80) {
                    R_Vrtop -= 1;
                }
            }
        }
    }
    if (!B_AdjState) {
        for (B_AdjCnt = 0; B_Vrtop != 0; B_AdjCnt++) {
            curr_client = curr_lt8522ex->client_addr0;
            HDMI_WriteI2C_Byte(0x44, 0x9f);
            Delay_ms(10);
            HDMI_WriteI2C_Byte(0x44, 0x8f);
            HDMI_WriteI2C_Byte(0x3d, (B_Vrtop & 0x03) | 0x80);
            HDMI_WriteI2C_Byte(0x3f, ((B_Vrtop >> 2) & 0x0f) | 0x10);
            DataChar0 = (HDMI_ReadI2C_Byte(0x76) << 3);
            DataChar1 = ((HDMI_ReadI2C_Byte(0x70) & 0x38) >> 3);
            B_Value = DataChar0 | DataChar1;
            if (B_Value >= 0xf0) {
                if ((B_Value < 0xff) && B_VrtopFlag) {
                    curr_client = curr_lt8522ex->client_addr0;
                    HDMI_WriteI2C_Byte(0x3d, (B_Vrtop & 0x03) | 0x80);
                    HDMI_WriteI2C_Byte(0x3f, ((B_Vrtop >> 2) & 0x0f) | 0x10);
                    FLASH_ProgramByte(0x4021, (B_Vrtop & 0x03) | 0x80);
                    FLASH_ProgramByte(0x4022, ((B_Vrtop >> 2) & 0x0f) | 0x10);
                    FLASH_ProgramByte(0x4020, 0x01);
                    FLASH_ProgramByte(0x4023, B_Vrtop);
                    B_Value = (((HDMI_ReadI2C_Byte(0x6f) & 0x3f) << 2) | ((HDMI_ReadI2C_Byte(0x72) & 0x30) >> 4));
                    FLASH_ProgramByte(0x4024, B_Value);
                    break;
                }
            }
            if (B_Value == 0xff) {
                B_Vrtop++;
                B_VrtopFlag = 1;
                curr_client = curr_lt8522ex->client_addr0;
                HDMI_WriteI2C_Byte(0x44, 0x9f);
                Delay_ms(10);
                HDMI_WriteI2C_Byte(0x44, 0x8f);
                HDMI_WriteI2C_Byte(0x3d, (B_Vrtop & 0x03) | 0x80);
                HDMI_WriteI2C_Byte(0x3f, ((B_Vrtop >> 2) & 0x0f) | 0x10);
                DataChar0 = (HDMI_ReadI2C_Byte(0x76) << 3);
                DataChar1 = ((HDMI_ReadI2C_Byte(0x70) & 0x38) >> 3);
                B_Value = DataChar0 | DataChar1;
                if ((B_Value >= 0xf0) && (B_Value < 0xff)) {
                    curr_client = curr_lt8522ex->client_addr0;
                    HDMI_WriteI2C_Byte(0x3d, (B_Vrtop & 0x03) | 0x80);
                    HDMI_WriteI2C_Byte(0x3f, ((B_Vrtop >> 2) & 0x0f) | 0x10);
                    FLASH_ProgramByte(0x4021, (B_Vrtop & 0x03) | 0x80);
                    FLASH_ProgramByte(0x4022, ((B_Vrtop >> 2) & 0x0f) | 0x10);
                    FLASH_ProgramByte(0x4020, 0x01);
                    FLASH_ProgramByte(0x4023, B_Vrtop);
                    DataChar = (((HDMI_ReadI2C_Byte(0x6f) & 0x3f) << 2) | ((HDMI_ReadI2C_Byte(0x72) & 0x30) >> 4));
                    FLASH_ProgramByte(0x4024, B_Value);
                    break;
                }
            } else {
                if (B_Value >= 0x80) {
                    B_Vrtop -= 1;
                }
            }
        }
    }
}
#endif

void LT8522EX_RGBClamp(void)
{
    R_AdjState = FLASH_ReadByte(0x4000);
    B_AdjState = FLASH_ReadByte(0x4020);
    G_AdjState = FLASH_ReadByte(0x4010);

    curr_client = curr_lt8522ex->client_addr0;
    //B channel
    HDMI_WriteI2C_Byte(0x30, 0x0c); //bit3: b_aim_vcm_sel[4]   bit2:rg_b_aip_clmp_en
    HDMI_WriteI2C_Byte(0x31, 0x80);       //b_clmp_aip_vref_sel[7:0]
    //G channel
    HDMI_WriteI2C_Byte(0x32, 0x04);       //bit2:rg_g_aip_clmp_en
    HDMI_WriteI2C_Byte(0x33, 0x80);       //rg_g_clmp_aip_vref_sel[7:0]

    //R channel
    HDMI_WriteI2C_Byte(0x35, 0x80);       //rg_r_clmp_aip_vref_sel[7:0]
    HDMI_WriteI2C_Byte(0x37, 0x10); /*0x07 */  //bit4-0:rg_g_aim_vcm_sel[4:0] G
    HDMI_WriteI2C_Byte(0x38, 0x50); /* */ //bit5:rg_aip_vcm_sel[2]  bit4: rg_aip_vcm_sel[0]
    HDMI_WriteI2C_Byte(0x3e, 0x40);       //

    if (R_AdjState == 1) {
        HDMI_WriteI2C_Byte(0x34, FLASH_ReadByte(0x4001));
        HDMI_WriteI2C_Byte(0x36, FLASH_ReadByte(0x4002));
        HDMI_WriteI2C_Byte(0x40, FLASH_ReadByte(0x4003));
        HDMI_WriteI2C_Byte(0x41, FLASH_ReadByte(0x4004));
    } else {
        HDMI_WriteI2C_Byte(0x34, 0x0c); // bit3:rg_r_vrtop _sel[6]  bit2:rg_r_aip_clmp_en
        HDMI_WriteI2C_Byte(0x36, 0x00); // bit5-4:rg_b_aim_vcm_sel[1:0] bit1-0:rg_r_vrtop _sel[5:4]
        HDMI_WriteI2C_Byte(0x40, 0x08); //bit7 :rg_r_vrtop _sel[3]  bit4 :rg_r_vrtop _sel[2] bit3-0:rg_r_vrbot_sel[6:3]
        HDMI_WriteI2C_Byte(0x41, 0x06);       //bit4:rg_r_vrtop _sel[1]
    }

    if (B_AdjState == 1) {
        HDMI_WriteI2C_Byte(0x3d, FLASH_ReadByte(0x4021));
        HDMI_WriteI2C_Byte(0x3f, FLASH_ReadByte(0x4022));
    } else {
        HDMI_WriteI2C_Byte(0x3d, 0x80); //bit7-2:rg_b_vrbot _sel[6:1]   bit1-0: rg_b_vrtop _sel[2:1]
        HDMI_WriteI2C_Byte(0x3f, 0x18); //bit4: rg_vrbot_i_sel[0] bit3:0 rg_b_vrtop_sel[3:0]
    }

    if (G_AdjState == 1) {
        HDMI_WriteI2C_Byte(0x42, FLASH_ReadByte(0x4011));
    } else {
        HDMI_WriteI2C_Byte(0x42, 0x41);  //0x2d );       /*rg_vrtop_sel<6:0> */
                                         //bit6-1: rg_g_vrtop_sel[6:1]  bit0:rg_r_aim_vcm_sel[2]
    }

    HDMI_WriteI2C_Byte(0x44, 0x9f); //bit7-4 :rg_clmp_aip_cycle[3:0]  bit3-0 :rg_clmp_hsync_period[3:0]
    HDMI_WriteI2C_Byte(0x45, 0x10);           //rg_clmp_plac[7:0]
    HDMI_WriteI2C_Byte(0x46, 0x30);           //rg_clmp_dura[7:0]

    HDMI_WriteI2C_Byte(0x43, 0xAE); /*Clamp start */ //bit7:rg_clmp_start  bit5:rg_adc_clmp_en
    Delay_ms(200);
    HDMI_WriteI2C_Byte(0x43, 0x2E);       //0222 modify
}

void LT8522EX_VrtopSelect(void)
{
}

void Set_G_Vrtop(unsigned char option)
{
    unsigned char DataChar;
    unsigned char DataChar0;
    unsigned char DataChar1;

    if (option == READ_ADCOUT) {
        curr_client = curr_lt8522ex->client_addr0;
        HDMI_WriteI2C_Byte(0x44, 0x9f); /*ECO */
        Delay_ms(10);
        HDMI_WriteI2C_Byte(0x44, 0x8f);
        Delay_ms(10);
        DataChar0 = (HDMI_ReadI2C_Byte(0x70) << 5); /*bit2-0:adc_dout_g[7:5]: */
        DataChar1 = ((HDMI_ReadI2C_Byte(0x73) & 0x3e) >> 1); /*bit5-1:adc_dout_g[4:0]: */
        G_Value = DataChar0 | DataChar1;
    } else if (option == SET_VALUE) {
        curr_client = curr_lt8522ex->client_addr0;
        DataChar = (G_Vrtop << 1) | 0x01;
        HDMI_WriteI2C_Byte(0X42, (G_Vrtop << 1) | 0x01);
    } else if (option == SAVE_VALUE) {
        curr_client = curr_lt8522ex->client_addr0;
        HDMI_WriteI2C_Byte(0X42, (G_Vrtop << 1) | 0x01);
        FLASH_ProgramByte( 0x4011, (G_Vrtop << 1) | 0x01 );
        FLASH_ProgramByte( 0x4010, 0x01 );
        FLASH_ProgramByte( 0x4012, G_Vrtop );
    }
}

void Set_R_Vrtop(unsigned char option)
{
    unsigned char tmp_data;

    if (option == READ_ADCOUT) {
        curr_client = curr_lt8522ex->client_addr0;
        HDMI_WriteI2C_Byte(0x44, 0x9f); /*ECO */
        Delay_ms(10);
        HDMI_WriteI2C_Byte(0x44, 0x8f);
        Delay_ms(10);
        R_Value = HDMI_ReadI2C_Byte(0x69);/*adc_dout_r[7:0]: */
    } else if (option == SET_VALUE) {
        curr_client = curr_lt8522ex->client_addr0;
        HDMI_WriteI2C_Byte(0x34, ((R_Vrtop >> 2) & 0x08) | 0x04);
        HDMI_WriteI2C_Byte(0x36, (R_Vrtop >> 3) & 0x03);
        HDMI_WriteI2C_Byte(0x40, ((R_Vrtop << 5) & 0x80) | ((R_Vrtop << 3) & 0x10) | 0x08);
        HDMI_WriteI2C_Byte(0x41, ((R_Vrtop << 4) & 0x10) | 0x06);
    } else if (option == SAVE_VALUE) {
        curr_client = curr_lt8522ex->client_addr0;
        HDMI_WriteI2C_Byte(0x34, ((R_Vrtop >> 2) & 0x08) | 0x04);
        HDMI_WriteI2C_Byte(0x36, (R_Vrtop >> 3) & 0x03);
        HDMI_WriteI2C_Byte(0x40, ((R_Vrtop << 5) & 0x80) | ((R_Vrtop << 3) & 0x10) | 0x08);
        HDMI_WriteI2C_Byte(0x41, ((R_Vrtop << 4) & 0x10) | 0x06);

        FLASH_ProgramByte( 0x4001, ( (R_Vrtop >> 2) & 0x08) | 0x04 );
        FLASH_ProgramByte( 0x4002, (R_Vrtop >> 3) & 0x03 );
        FLASH_ProgramByte( 0x4003, ( (R_Vrtop << 5) & 0x80) | ( (R_Vrtop << 3) & 0x10) | 0x08 );
        FLASH_ProgramByte( 0x4004, ( (R_Vrtop << 4) & 0x10) | 0x06 );
        FLASH_ProgramByte( 0x4000, 0x01 );
        FLASH_ProgramByte( 0x4005, R_Vrtop );
    }
}

void Set_B_Vrtop(unsigned char option)
{
    unsigned char DataChar0;
    unsigned char DataChar1;

    if (option == READ_ADCOUT) {
        curr_client = curr_lt8522ex->client_addr0;
        HDMI_WriteI2C_Byte(0x44, 0x9f); /*ECO */
        Delay_ms(10);
        HDMI_WriteI2C_Byte(0x44, 0x8f);
        Delay_ms(10);
        DataChar0 = (HDMI_ReadI2C_Byte(0x76) << 3);    //bit4-0: adc_dout_b[7:3]
        DataChar1 = ((HDMI_ReadI2C_Byte(0x70) & 0x38) >> 3); //bit5-3: adc_dout_b[2:0]
        B_Value = DataChar0 | DataChar1;
    } else if (option == SET_VALUE) {
        curr_client = curr_lt8522ex->client_addr0;
        HDMI_WriteI2C_Byte(0x3d, (B_Vrtop & 0x03) | 0x80);
        HDMI_WriteI2C_Byte(0x3f, ((B_Vrtop >> 2) & 0x0f) | 0x10);
    }

    if (option == SAVE_VALUE) {
        curr_client = curr_lt8522ex->client_addr0;
        HDMI_WriteI2C_Byte(0x3d, (B_Vrtop & 0x03) | 0x80);
        HDMI_WriteI2C_Byte(0x3f, ((B_Vrtop >> 2) & 0x0f) | 0x10);

        FLASH_ProgramByte( 0x4021, (B_Vrtop & 0x03) | 0x80 );
        FLASH_ProgramByte( 0x4022, ( (B_Vrtop >> 2) & 0x0f) | 0x10 );
        FLASH_ProgramByte( 0x4020, 0x01 );
        FLASH_ProgramByte( 0x4023, B_Vrtop );
        B_Value = (((HDMI_ReadI2C_Byte(0x6f) & 0x3f) << 2) | ((HDMI_ReadI2C_Byte(0x72) & 0x30) >> 4));
        FLASH_ProgramByte( 0x4024, B_Value );
    }
}

void LT8522EX_AdjustColor(void)
{
    unsigned char adjust_success_num = 0;

    R_AdjState = FLASH_ReadByte(0x4000);
    B_AdjState = FLASH_ReadByte(0x4020);
    G_AdjState = FLASH_ReadByte(0x4010);

    G_VrtopFlag = 0;
    R_VrtopFlag = 0;
    B_VrtopFlag = 0;

    LT8522EX_RGBClamp();

    if (!G_AdjState) {
        for (G_AdjCnt = 0; G_Vrtop != 0; G_AdjCnt++) {
            Set_G_Vrtop(READ_ADCOUT);
            if ((G_Value > 0xfa) && (G_Value < 0xff)) {
                if (G_VrtopFlag) {
                    if (++adjust_success_num > 2) {
                        adjust_success_num = 0;
                        Set_G_Vrtop(SAVE_VALUE);
                        break;
                    }
                }
            } else {
                adjust_success_num = 0;
            }

            if (G_Value == 0xff) {
                G_Vrtop++;
                if (G_Vrtop > 0x3f) {
                    G_Vrtop = 0x16;
                    Set_G_Vrtop(SAVE_VALUE);
                    break;
                }
                Set_G_Vrtop(SET_VALUE);
                G_VrtopFlag = 1;
            } else if ((G_Value > 0x80) && (G_Value < 0xfa)) {
                G_Vrtop--;
                Set_G_Vrtop(SET_VALUE);
            }
        }
    }

    if (!R_AdjState) {
        for (R_AdjCnt = 0; R_Vrtop != 0; R_AdjCnt++) {
            Set_R_Vrtop(READ_ADCOUT);

            if ((R_Value > 0xfa) && (R_Value < 0xff)) {
                if (R_VrtopFlag) {
                    if (++adjust_success_num > 2) {
                        adjust_success_num = 0;
                        Set_R_Vrtop(SAVE_VALUE);
                        break;
                    }
                }
            } else {
                adjust_success_num = 0;
            }

            if (R_Value == 0xff) {
                R_Vrtop++;
                if (R_Vrtop > 0x3f) {
                    R_Vrtop = 0x20;
                    Set_R_Vrtop(SAVE_VALUE);
                    break;
                }
                Set_R_Vrtop(SET_VALUE);
                R_VrtopFlag = 1;
            } else if ((R_Value > 0x80) && (R_Value < 0xfa)) {
                R_Vrtop--;
                Set_R_Vrtop(SET_VALUE);
            }
        }
    }

    if (!B_AdjState) {
        for (B_AdjCnt = 0; B_Vrtop != 0; B_AdjCnt++) {
            Set_B_Vrtop(READ_ADCOUT);
            if ((B_Value > 0xfa) && (B_Value < 0xff)) {
                if (B_VrtopFlag) {
                    if (++adjust_success_num > 2) {
                        adjust_success_num = 0;

                        B_Vrtop = B_Vrtop + 2;
                        Set_B_Vrtop(SAVE_VALUE);
                        break;
                    }
                }
            } else {
                adjust_success_num = 0;
            }
            if (B_Value == 0xff) {
                B_Vrtop++;
                if (B_Vrtop > 0x3f) {
                    B_Vrtop = 0x20;
                    Set_B_Vrtop(SAVE_VALUE);
                    break;
                }
                Set_B_Vrtop(SET_VALUE);
                B_VrtopFlag = 1;
            } else if ((B_Value > 0x80) && (B_Value < 0xfa)) {
                B_Vrtop--;
                Set_B_Vrtop(SET_VALUE);
            }
        }
    }
}


void LT8522EX_InterruptClear(void)
{
    curr_client = curr_lt8522ex->client_addr2;
    HDMI_WriteI2C_Byte(0xe3, 0xff);
    HDMI_WriteI2C_Byte(0xe3, 0xfd);
    HDMI_WriteI2C_Byte(0xea, 0xff);
    HDMI_WriteI2C_Byte(0xea, 0xfe);
}

void LT8522EX_AutoPiPhase(void)
{
    curr_lt8522ex->AutoPhaseDone = 0;
    LT8522EX_TxDriveConfig(DriveOff);

    //this for some customer do not use 8522x interrupt pin.
    do {
        curr_client = curr_lt8522ex->client_addr0;
        HDMI_WriteI2C_Byte(0x27, 0x80);

        curr_client = curr_lt8522ex->client_addr2;
        HDMI_WriteI2C_Byte(0xd3, 0xfd); //Low active for interrupt pin;
        HDMI_WriteI2C_Byte(0xda, 0xfe); // int mask

        curr_client = curr_lt8522ex->client_addr0;
        HDMI_WriteI2C_Byte(0xf9, 0x63); //start phase
        Delay_ms(300);
        CheckBestPhase();
    } while (!curr_lt8522ex->AutoPhaseDone);

    curr_client = curr_lt8522ex->client_addr0;
    HDMI_WriteI2C_Byte(0xf9, 0x00);
    LT8522EX_TxDriveConfig(DriveOn);
}

unsigned char Phase_SumValue(void)
{
    unsigned char i;
    unsigned short SumValue;
    unsigned short value;
    unsigned short c = 0xffff;
    unsigned char b;

    for (i = 0; i < 0x40; i += 4) {
        curr_client = curr_lt8522ex->client_addr0;
        HDMI_WriteI2C_Byte(0x27, i);
        HDMI_WriteI2C_Byte(0xf9, 0x63);
        Delay_ms(10);

        curr_client = curr_lt8522ex->client_addr1;
        SumValue = HDMI_ReadI2C_Byte(0x1d) * 256 + HDMI_ReadI2C_Byte(0x1c);
        value = 0xffff - SumValue;
        if (value < c) {
            c = value;
            b = i;
        }

        curr_client = curr_lt8522ex->client_addr0;
        HDMI_WriteI2C_Byte(0xf9, 0x00);
    }

    curr_client = curr_lt8522ex->client_addr0;
    HDMI_WriteI2C_Byte(0xf9, 0x00);

    return b;
}

void Bubble_Sort(unsigned char a[], unsigned char n)
{
    unsigned char i, j, temp;

    for (i = 0; i < n - 1; i++) {
        for (j = 0; j < n - 1 - i; j++) {
            if (a[j] > a[j + 1]) {
                temp = a[j];
                a[j] = a[j + 1];
                a[j + 1] = temp;
            }
        }
    }
}

unsigned char Diff_Func(unsigned char a[], unsigned char b[], unsigned char n)
{
    unsigned char i, j;
    unsigned char rtn;
    unsigned char min_diff;

    j = 0;
    min_diff = 0;

    for (i = 0; i < n - 1; i++) {
        if ((a[i + 1] - a[i]) < 1) {
            if (a[i - 1] != a[i]) {
                b[j++] = a[i];
                min_diff = 1;
            }
        }
    }

    if (min_diff) {
        if (j == 1)
            rtn = b[0];
        else if (j == 2)
            rtn = b[0] > b[1] ? b[0] : b[1];
        else
            rtn = b[2];        //20190918 add
    } else
        rtn = a[3];

    return rtn;
}

void AutoPhaseCalibration(void)
{
    unsigned char i, j;
    unsigned short SumValue;
    unsigned short value;        //,tmp_value;
    unsigned short c = 0xffff;
    unsigned char b;
    unsigned char adcpi_phase;

    unsigned char temp_code[6], temp_adcpi[6];

    LT8522EX_TxDriveConfig(DriveOff);
    curr_client = curr_lt8522ex->client_addr0;
    HDMI_WriteI2C_Byte(0x27, 0x00); /*//rg_adcpi_phase_sel=1; */
    HDMI_WriteI2C_Byte(0xf9, 0x63); /*start phase */

    temp_code[0] = Phase_SumValue();
    Delay_ms(5);
    temp_code[1] = Phase_SumValue();
    Delay_ms(5);
    temp_code[2] = Phase_SumValue();
    Delay_ms(5);
    temp_code[3] = Phase_SumValue();
    Delay_ms(5);
    temp_code[4] = Phase_SumValue();
    Delay_ms(5);
    temp_code[5] = Phase_SumValue();

    Bubble_Sort(temp_code, 6);                   //code value??D\A8\B0

    //debug   0920 add
    curr_client = curr_lt8522ex->client_addr3;
    HDMI_WriteI2C_Byte(0x44, temp_code[0]);
    HDMI_WriteI2C_Byte(0x45, temp_code[1]);
    HDMI_WriteI2C_Byte(0x46, temp_code[2]);
    HDMI_WriteI2C_Byte(0x47, temp_code[3]);
    HDMI_WriteI2C_Byte(0x48, temp_code[4]);
    HDMI_WriteI2C_Byte(0x49, temp_code[5]);
    //debug   end

    b = Diff_Func(temp_code, temp_adcpi, 6);      //?\A8\B0?\A8\A4\A8\AA?

    c = 0xffff;
    if (b == 0)
        b = 3;
    if (b > 0x3c)
        b = 0x3c;
    for (i = (b - 3); i < (b + 3); i++) {
        curr_client = curr_lt8522ex->client_addr0;
        HDMI_WriteI2C_Byte(0x27, i);
        HDMI_WriteI2C_Byte(0xf9, 0x63);

        Delay_ms(30);
        curr_client = curr_lt8522ex->client_addr1;

        SumValue = HDMI_ReadI2C_Byte(0x1d) * 256 + HDMI_ReadI2C_Byte(0x1c);
        value = 0xffff - SumValue;

        //HDMI_WriteI2C_Byte( 0xf9, 0x00 );

        if (value < c) {
            c = value;
            adcpi_phase = i;
        }
    }

    curr_client = curr_lt8522ex->client_addr0;
    HDMI_WriteI2C_Byte(0x27, adcpi_phase);
    Delay_ms(50);
    HDMI_WriteI2C_Byte(0xf9, 0x00);
    LT8522EX_TxDriveConfig(DriveOn);
}

void RxHdmiDviModeDetect(void)
{
    curr_client = curr_lt8522ex->client_addr2;
    HDMI_WriteI2C_Byte(0xe2, 0xff);
    HDMI_WriteI2C_Byte(0xe2, 0x00);
}
