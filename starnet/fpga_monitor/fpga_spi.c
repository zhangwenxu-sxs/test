#include <linux/delay.h>
#include <linux/gpio.h>
#include "fpga_spi.h"

unsigned int __GetMiso(void)
{
    gpio_direction_input(GPIO_SPI_DO_FPGA);
    return gpio_get_value(GPIO_SPI_DO_FPGA);
}

void __SetCsLow(unsigned char delay)
{
    SET_CS_LOW();
    usleep_range(delay, delay + 10);
}

void __SetCsHigh(unsigned char delay)
{
    SET_CS_HIGH();
    usleep_range(delay, delay + 10);
}

void __SetClkLow(unsigned char delay)
{
    SET_CLK_LOW();
    ndelay(delay);
}

void __SetClkHigh(unsigned char delay)
{
    SET_CLK_HIGH();
    ndelay(delay);
}

void __SetClkLowMosiHigh(unsigned char delay)
{
    SET_CLK_LOW_MOSI_HIGH();
    ndelay(delay);
}

void __SetClkLowMosiLow(unsigned char delay)
{
    SET_CLK_LOW_MOSI_LOW();
    ndelay(delay);
}

void SpiReadByte(unsigned char *data)
{
    register int i = 8;
    register unsigned char data_tmp = 0;

    do {
        //设备在时钟的下降沿开始输出数据  output data is available on the falling edge of SCLK
        __SetClkLow(CLK_LOW_DELAY);
        data_tmp <<= 1;
        __SetClkHigh(1);
        //host 在时钟的上升沿开始读出数据
        if (__GetMiso()) {
            data_tmp |= 1;
        }
        __SetClkHigh(CLK_HIGH_DELAY - 1);
    } while (--i);

    *data = data_tmp;
}

void SpiWriteByte(unsigned char data)
{
    register int i = 8;

    do {
        //host在时钟的下降沿开始输出数据
        if (data & 0x80) {
            __SetClkLowMosiHigh(CLK_LOW_MOSI_HIGH_DELAY);
        } else {
            __SetClkLowMosiLow(CLK_LOW_MOSI_LOW_DELAY);
        }
        //device在时钟的上升沿锁定数据  Input data is latched in on the rising edge of SCLK
        __SetClkHigh(CLK_HIGH_DELAY);
        data <<= 1;
    } while (--i);
}

void SpiInit(void)
{
    gpio_request(GPIO_SPI_CSN_FPGA, NULL);
    gpio_request(GPIO_SPI_CLK_FPGA, NULL);
    gpio_request(GPIO_SPI_DO_FPGA, NULL);
    gpio_request(GPIO_SPI_DI_FPGA, NULL);

    // CS初始化为无效
    SET_CS_HIGH();
    // CLK初始化为低电平
    SET_CLK_HIGH();
    // 数据初始化为低电平
    SET_MOSI_HIGH();

    gpio_direction_input(GPIO_SPI_DO_FPGA);

    return;
}

void SpiDeinit(void)
{
    gpio_direction_input(GPIO_SPI_CSN_FPGA);
    gpio_direction_input(GPIO_SPI_CLK_FPGA);
    gpio_direction_input(GPIO_SPI_DI_FPGA);
    gpio_direction_input(GPIO_SPI_DO_FPGA);

    gpio_free(GPIO_SPI_CSN_FPGA);
    gpio_free(GPIO_SPI_CLK_FPGA);
    gpio_free(GPIO_SPI_DI_FPGA);
    gpio_free(GPIO_SPI_DO_FPGA);
}
