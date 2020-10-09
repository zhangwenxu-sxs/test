#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/spi/spi.h>
#include <linux/spi/spidev.h>

#include "fpga_spi.h"
#include "fpga_monitor.h"

extern fpga_monitor_t* fpga_monitor;

unsigned int __GetMiso(void)
{
    gpio_direction_input(GPIO_SPI_DO_FPGA);
    return gpio_get_value(GPIO_SPI_DO_FPGA);
}

void __SetCsLow(unsigned char delay)
{
//    SET_CS_LOW();
//    usleep_range(delay, delay + 10);
}

void __SetCsHigh(unsigned char delay)
{
//    SET_CS_HIGH();
//    usleep_range(delay, delay + 10);
}

void __SetClkLow(unsigned char delay)
{
    SET_CLK_LOW();
    udelay(delay);
}

void __SetClkHigh(unsigned char delay)
{
    SET_CLK_HIGH();
    udelay(delay);
}

void __SetClkLowMosiHigh(unsigned char delay)
{
    SET_CLK_LOW_MOSI_HIGH();
    udelay(delay);
}

void __SetClkLowMosiLow(unsigned char delay)
{
    SET_CLK_LOW_MOSI_LOW();
    udelay(delay);
}

void SpiReadBytes(unsigned char *tx_buf, int tx_len, unsigned char *rx_buf, int rx_len)
{
#if 0
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
#else
//    int ret;
//    struct spi_message msg = {};
//    struct spi_transfer rx = {
//        .rx_buf = data,
//        .len = len,
//        .speed_hz = 500000
////        .speed_hz = fpga_monitor->spi_dev->max_speed_hz
//    };
//
//    spi_message_init(&msg);
//    spi_message_add_tail(&rx, &msg);
//
//    ret = spi_sync(fpga_monitor->spi_dev, &msg);

    spi_write_then_read(fpga_monitor->spi_dev, tx_buf, tx_len, rx_buf, rx_len);
#endif
}

void SpiWriteBytes(unsigned char* data, int len)
{
#if 0
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
#else
//    int ret;
//    struct spi_message msg = {};
//    struct spi_transfer tx = {
//        .tx_buf = data,
//        .len = len,
//        .speed_hz = 500000
////        .speed_hz = fpga_monitor->spi_dev->max_speed_hz
//    };
//
//    spi_message_init(&msg);
//    spi_message_add_tail(&tx, &msg);
//
//    ret = spi_sync(fpga_monitor->spi_dev, &msg);

    spi_write(fpga_monitor->spi_dev, data, len);

#endif
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
