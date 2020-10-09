#ifndef		_I2C_MASTER_H
#define		_I2C_MASTER_H

void Delay_ms(unsigned int mscount);
void delay_us(unsigned int uscount);
unsigned char HDMI_ReadI2C_Byte(unsigned char RegAddr);
int HDMI_WriteI2C_Byte(unsigned char RegAddr, unsigned char data);

#endif
