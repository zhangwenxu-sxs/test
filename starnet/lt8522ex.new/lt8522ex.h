#ifndef  _LT8522EX_H
#define  _LT8522EX_H

#define HDMI 0x01
#define VGA 0x00
#define DVI 0x00

#define DriveOn 0x01
#define DriveOff 0x00

#define LT8522EX_ADR 0x50
#define LT8522EX_ADR_1 0x52
#define LT8522EX_ADR_2 0x54
#define LT8522EX_ADR_3 0x56

#define FLASH_ReadByte(addr) 0
#define FLASH_ProgramByte(addr, val)

typedef enum {
  RESET = 0,
  SET = !RESET
} FlagStatus, ITStatus, BitStatus;

extern volatile unsigned char RxMode;
extern unsigned char R_Vrtop;
extern unsigned char G_Vrtop;
extern unsigned char B_Vrtop;

void LT8522EX_Init(void);
void LT8522EX_ClockEnable(void);
void LT8522EX_ClockEnable_HDMI(void);
void LT8522EX_ClockEnable_VGA(void);
void LT8522EX_ColorConvert(unsigned char rxmode);
void LT8522EX_RxTxConfig(unsigned char rx, unsigned char tx);
void LT8522EX_SetSyncPol(void);
void LT8522EX_AdcReset(void);
void LT8522EX_AdcPllInit(void);
void LT8522EX_AdcPllDiv(void);
void LT8522EX_RGBClamp(void);
void LT8522EX_VrtopSelect(void);
void LT8522EX_InterruptClear(void);
void LT8522EX_SetTimingReg(void);
void LT8522EX_SetTimingWindow(void);
void LT8522EX_AutoPiPhase(void);
void LT8522EX_AdcPowerOn(void);
void LT8522EX_AdcPowerDown(void);
void LT8522EX_TxDriveConfig(unsigned char NewState);
void LT8522EX_AudioConfig(unsigned char tx);
void LT8522EX_HdcpKeyLoad(void);
void LT8522EX_HdcpFunction(void);
void LT8522EX_RxPowerOn(unsigned char rx);
void LT8522EX_RxPowerDown(unsigned char rx);
void LT8522EX_RxTxReset(unsigned char rx, unsigned char tx);
void LT8522EX_AdjustColor(void);
void LT8522EX_CONFIG_INPUT_OUTPUT(void);

#endif
