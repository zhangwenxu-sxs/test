#ifndef __FPGA_SPI_H
#define __FPGA_SPI_H

#define CS_LOW_DELAY            3
#define CS_HIGH_DELAY           3
#define CLK_HIGH_DELAY          3
#define CLK_LOW_DELAY           3
#define CLK_LOW_MOSI_HIGH_DELAY 3
#define CLK_LOW_MOSI_LOW_DELAY  3

//GPIO12_5
#define GPIO_SPI_CSN_FPGA       101
//SPI_CLK_FPGA   GPIO12_2
#define GPIO_SPI_CLK_FPGA       98
//SPI_MISO_FPGA   GPIO12_6
#define GPIO_SPI_DO_FPGA        102
//SPI_MOSI_FPGA     GPIO12_7
#define GPIO_SPI_DI_FPGA        103

#define SET_CS_LOW()    gpio_direction_output(GPIO_SPI_CSN_FPGA, 0)
#define SET_CS_HIGH()   gpio_direction_output(GPIO_SPI_CSN_FPGA, 1)
#define SET_CLK_LOW()   gpio_direction_output(GPIO_SPI_CLK_FPGA, 0)
#define SET_CLK_HIGH()  gpio_direction_output(GPIO_SPI_CLK_FPGA, 1)
#define SET_MOSI_LOW()  gpio_direction_output(GPIO_SPI_DI_FPGA, 0)
#define SET_MOSI_HIGH() gpio_direction_output(GPIO_SPI_DI_FPGA, 1)

#define SET_CLK_LOW_MOSI_LOW()  do {                    \
                                    SET_CLK_LOW();      \
                                    SET_MOSI_LOW();     \
                                } while(0)

#define SET_CLK_LOW_MOSI_HIGH() do {                    \
                                    SET_CLK_LOW();      \
                                    SET_MOSI_HIGH();    \
                                } while(0)


unsigned int __GetMiso(void);
void __SetCsLow(unsigned char delay);
void __SetCsHigh(unsigned char delay);
void __SetClkLow(unsigned char delay);
void __SetClkHigh(unsigned char delay);
void __SetClkLowMosiHigh(unsigned char delay);
void __SetClkLowMosiLow(unsigned char delay);

void SpiReadBytes(unsigned char *tx_buf, int tx_len, unsigned char *rx_buf, int rx_len);
void SpiWriteBytes(unsigned char *data, int len);
void SpiInit(void);
void SpiDeinit(void);

#endif
