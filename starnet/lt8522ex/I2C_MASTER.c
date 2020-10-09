#include "lt8522ex_main.h"
#include "lt8522ex_misc.h"

extern lt8522ex_t* curr_lt8522ex;
extern struct i2c_client* curr_client;

void Delay_ms(unsigned int mscount)
{
    usleep_range(mscount * 1000, mscount * 1000 + 100);
}

void delay_us(unsigned int uscount)
{
    usleep_range(uscount, uscount + 100);
}

unsigned char HDMI_ReadI2C_Byte(unsigned char RegAddr)
{
    int ret;
    unsigned char p_data;

    ret = i2c_smbus_read_byte_data(curr_client, RegAddr);
    if (ret < 0) {
        lt8522ex_log("%s: do i2c read failed. addr:0x%.2x \n", curr_lt8522ex->name, RegAddr);
        return 0xff;
    }

    p_data = (unsigned char)(ret & 0xff);

    return p_data;
}

int HDMI_WriteI2C_Byte(unsigned char RegAddr, unsigned char data)
{
    int ret;

    ret = i2c_smbus_write_byte_data(curr_client, RegAddr, data);
    if (ret < 0) {
        lt8522ex_log("%s: do i2c write failed. addr:0x%.2x  val:0x%.2x \n", curr_lt8522ex->name, RegAddr, data);
    }

    return ret;
}
