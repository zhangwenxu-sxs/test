#include <linux/module.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include "linux/pagemap.h"
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/irq.h>
//#include <linux/switch.h>
#include <linux/debugfs.h>
#include <linux/string.h>
#include <linux/miscdevice.h>

#include "ssd1322.h"

//(gpion_m) = n*8+m
#define SCK_GPIO   (3*8 + 4)	//GPIO3_4
#define SDIN_GPIO  (3*8 + 5)	//GPIO3_5
#define RST_GPIO   (3*8 + 6)	//GPIO3_6
#define CS_GPIO    (3*8 + 7)	//GPIO3_7
#define DC_GPIO    (5*8 + 5)	//GPIO5_5
#define VCC_GPIO   (17*8 + 5)	//GPIO17_5
#define LOW		0
#define HIGH	1

#define SYS_WRITEL(Value,Addr ) ((*(volatile unsigned int *)(Addr)) = (Value))

#define COLUMN_MAX 128
#define ROW_MAX 32
static u8 dot_array_buffer[COLUMN_MAX * ROW_MAX / 8];

void reset_ssd1322(int reset_gpio)
{
	gpio_direction_output(reset_gpio,1);
	mdelay(10);
	gpio_direction_output(reset_gpio,0);
	mdelay(10);
	gpio_direction_output(reset_gpio,1);
	mdelay(10);

	return;
}
int gpio_init(void)
{
	int ret;
	int *pinmux;

	if (gpio_is_valid(CS_GPIO)) {
	    //配置CS引脚为GPIO输出  GPIO3_7
	    pinmux = ioremap(0x1f000088,4);
	    if(pinmux != NULL) {
	        SYS_WRITEL(0x0400,pinmux);
	    }
		ret = gpio_request(CS_GPIO, "ssd1322_cs");
		if (ret != 0) {
			pr_err("gpio_request ssd1322 ssd1322_cs fail: %d\n",CS_GPIO);
			goto fail;
		}
	} else {
		printk("CS_GPIO is invalid\n");
		goto fail;
	}


	if (gpio_is_valid(DC_GPIO)) {
	    //配置DC引脚为GPIO输出  GPIO5_5
	    pinmux = ioremap(0x1f00008c,4);
	    if(pinmux != NULL) {
	        SYS_WRITEL(0x0400,pinmux);
	    }
	    ret = gpio_request(DC_GPIO, "ssd1322_dc");
		if (ret != 0) {
			pr_err("gpio_request ssd1322 ssd1322_dc fail: %d\n",DC_GPIO);
			goto fail;
		}
	} else {
		printk("DC_GPIO is invalid\n");
		goto fail;
	}


	if (gpio_is_valid(SDIN_GPIO)) {
	    //配置SDIN引脚为GPIO输出  GPIO3_5
	    pinmux = ioremap(0x1f000080,4);
	    if(pinmux != NULL) {
	        SYS_WRITEL(0x0400,pinmux);
	    }
	    ret = gpio_request(SDIN_GPIO, "ssd1322_sdin");
		if (ret != 0) {
			pr_err("gpio_request ssd1322 ssd1322_dc fail: %d\n",SDIN_GPIO);
			goto fail;
		}
	} else {
		printk("SDIN_GPIO is invalid\n");
		goto fail;
	}


	if (gpio_is_valid(SCK_GPIO)) {
	    //配置SCK引脚为GPIO输出  GPIO3_4
	    pinmux = ioremap(0x1f00007c,4);
	    if(pinmux != NULL) {
	        SYS_WRITEL(0x0400,pinmux);
	    }
	    ret = gpio_request(SCK_GPIO, "ssd1322_sck");
		if (ret != 0) {
			pr_err("gpio_request ssd1322 ssd1322_sck fail: %d\n",SCK_GPIO);
			goto fail;
		}
	} else {
		printk("SCK_GPIO is invalid\n");
		goto fail;
	}


	if (gpio_is_valid(VCC_GPIO)) {
	    //配置VCC电源控制引脚为GPIO输出  GPIO17_5
	    pinmux = ioremap(0x1f000060,4);
	    if(pinmux != NULL) {
	        SYS_WRITEL(0x0400,pinmux);
	    }
	    ret = gpio_request(VCC_GPIO, "ssd1322_vcc");
		if (ret != 0) {
			pr_err("gpio_request ssd1322 ssd1322_vcc fail: %d\n",VCC_GPIO);
			goto fail;
		}
	} else {
		printk("VCC_GPIO is invalid\n");
		goto fail;
	}


	if (gpio_is_valid(RST_GPIO)) {
		//配置RST引脚为GPIO输出  GPIO3_6
	    pinmux = ioremap(0x1f000084,4);
	    if(pinmux != NULL) {
	        SYS_WRITEL(0x0400,pinmux);
	    }
	    ret = gpio_request(RST_GPIO, "ssd1322_rst");
		if (ret != 0) {
			pr_err("gpio_request ssd1322 ssd1322_rst fail: %d\n",RST_GPIO);
			goto fail;
		}
	} else {
		printk("RST_GPIO is invalid\n");
		goto fail;
	}
	return 0;

fail:
	printk("fail\n");
	return -1;
}


void gpio_spi_init(void)
{
	gpio_direction_output(CS_GPIO, 1);
	gpio_direction_output(DC_GPIO, 1);
	gpio_direction_output(SDIN_GPIO, 1);
	gpio_direction_output(SCK_GPIO, 1);
	return ;
}

void gpio_spi_test(void)
{
	int i=0;

	while (i<40000) {
		gpio_direction_output(RST_GPIO,1);
		gpio_direction_output(CS_GPIO, 1);
		gpio_direction_output(DC_GPIO, 1);
		gpio_direction_output(SDIN_GPIO, 1);
		gpio_direction_output(SCK_GPIO, 1);
		udelay(20);
		gpio_direction_output(RST_GPIO,0);
		gpio_direction_output(CS_GPIO, 0);
		gpio_direction_output(DC_GPIO, 0);
		gpio_direction_output(SDIN_GPIO, 0);
		gpio_direction_output(SCK_GPIO, 0);
		udelay(20);
		i++;
	}
	printk("ssd1322 gpio test success!\n");
	return ;
}

/* spi write command */
void write_command_spi(u8 command)
{
	u8 i,value;

	value = command;

	gpio_set_value(CS_GPIO ,LOW);
	gpio_set_value(DC_GPIO ,LOW);
	for(i=0;i<8;i++)
	{
		gpio_set_value(SCK_GPIO ,LOW);
		//Delay_1ms(1);
		if(value & 0x80)
			gpio_set_value(SDIN_GPIO ,HIGH);
		else
			gpio_set_value(SDIN_GPIO ,LOW);
//		mdelay(2);
		udelay(20);
		gpio_set_value(SCK_GPIO ,HIGH);
//		mdelay(2);
		udelay(20);
		value =(value << 1);
	}
	gpio_set_value(CS_GPIO,HIGH);
	//Delay_1ms(1);
	return;
}

void write_data_spi(u8 date)
{
	u8 i,value;

	value = date;
	gpio_set_value(CS_GPIO ,LOW);
	gpio_set_value(DC_GPIO ,HIGH);
	for(i=0;i<8;i++)
	{
		gpio_set_value(SCK_GPIO ,LOW);
		//Delay_1ms(1);
		if(value & 0x80)
			gpio_set_value(SDIN_GPIO ,HIGH);
		else
			gpio_set_value(SDIN_GPIO ,LOW);
//		mdelay(2);
		udelay(20);
		gpio_set_value(SCK_GPIO ,HIGH);
//		mdelay(2);
		udelay(20);
		value =(value << 1);
	}
	gpio_set_value(CS_GPIO,HIGH);
	//Delay_1ms(1);
	return;
}

void power_on(void)
{
	gpio_direction_output(VCC_GPIO, 0);
	mdelay(50);
//	reset_ssd1322(RST_GPIO);
	gpio_direction_output(RST_GPIO,1);
	mdelay(10);
	gpio_direction_output(RST_GPIO,0);
	mdelay(10);
	gpio_direction_output(RST_GPIO,1);
	mdelay(10);
	gpio_direction_output(VCC_GPIO,1);
	mdelay(500);
	printk("set vcc gpio 1\n");
}

void init_ssd1322(void)
{
	write_command_spi(0xfd);	//Set Command Lock
	write_data_spi(0x12);

	write_command_spi(0xae);	//Set Sleep mode OFF

	write_command_spi(0x15);	//Set Column Address
	write_data_spi(0x1c);
	write_data_spi(0x5b);

	write_command_spi(0x75);	//Set Row Address
	write_data_spi(0x00);
	write_data_spi(0x3f);

	write_command_spi(0xa0);	//Set Re-map and Dual COM Line mode
	write_data_spi(0x14);
	write_data_spi(0x11);

	write_command_spi(0xa1);	//Set Display Start Line
	write_data_spi(0x00);

	write_command_spi(0xa2);	//Set Display Offset
	write_data_spi(0x00);

	write_command_spi(0xab);	//Function Selection
	write_data_spi(0x01);

	write_command_spi(0xb1);	//Set Phase Length
	write_data_spi(0xe2);

	write_command_spi(0xb3);	//Set Front Clock Divider/Oscillator
	write_data_spi(0x91);

	write_command_spi(0xb4);	//Display Enhancement A
	write_data_spi(0xa0);
	write_data_spi(0xfd);

	write_command_spi(0xb5);	//Set GPIO
	write_data_spi(0x00);

	write_command_spi(0xb6);	//Set Second Precharge Period
	write_data_spi(0x08);

	write_command_spi(0xb9);	//Select Default Linear Gray Scale table

	write_command_spi(0xbb);	//Set Pre-charge
	write_data_spi(0x1f);

	write_command_spi(0xbe);	//Set VCOMH
	write_data_spi(0x07);

	write_command_spi(0xc1);	//Set Contrast
	write_data_spi(0x9f);

	write_command_spi(0xc7);	//Master Contrast Current Control
	write_data_spi(0x0f);

	write_command_spi(0xca);	//Set MUX Ratio
	write_data_spi(0x3f);

	write_command_spi(0xd1);	//Display Enhancement B
	write_data_spi(0x82);
	write_data_spi(0x20);

//	write_command_spi(0xa6);	//Set Display normal Mode
	write_command_spi(0xa5);	//Set Entire Display ON

	write_command_spi(0xaf);	//Set Display ON

	mdelay(15);
	printk("init ssd1322 end\n");
	return;
}

void ram_address(void)
{
	write_command_spi(0x15);
	write_data_spi(0x1c);
	write_data_spi(0x5b);

	write_command_spi(0x75);
	write_data_spi(0x00);
	write_data_spi(0x3f);

	write_command_spi(0x5c);
}

void ssd1322_light_on_all_screen(void)
{
	unsigned int i,j;
	ram_address();
	for(i = 0;i < 64;i++) {
		for(j = 0;j < 64;j++) {
			write_data_spi(0xff);
			write_data_spi(0xff);
		}
	}
}

void string_to_buff(uint8_t *ptr)
{
	uint8_t string_ascii;
	uint16_t offset = 0;
	u8 *tmp;
	int i;
	int char_num = strlen(ptr);

	memset( (void *)dot_array_buffer,0,sizeof(dot_array_buffer) );

	while(*ptr != 0) {
		string_ascii = *ptr;
		string_ascii -= 32;		//对应.h中的字符
		if(string_ascii >= CHAR_EN_NUM) {
			continue;
		}
		tmp = dot_array_buffer + offset;
		for(i = 0;i < FONT_HEIGHT_EN;i++) {
			tmp[i*char_num] = &font_en[string_ascii * FONT_HEIGHT_EN];
		}
		offset++;
		ptr++;
	}
}

void ssd1322_display_string(uint8_t *pstring)
{

}

static int ssd1322_init(void)
{
	int ret;

	printk("ssd1322 start init\n");

	ret = gpio_init();
	if(ret != 0) {
		printk("gpio init fail\n");
	}

	power_on();
	gpio_spi_init();

	printk("gpio spi init end\n");
//	gpio_spi_test();
	init_ssd1322();
//	msleep(300);
//	ssd1322_light_on_all_screen();
//	printk("ssd1322 light all screen\n");
	return 0;

}

static void ssd1322_exit(void)
{
	printk("ssd1322 exit\n");
	gpio_free(SCK_GPIO);
	gpio_free(SDIN_GPIO);
	gpio_free(RST_GPIO);
	gpio_free(CS_GPIO);
	gpio_free(DC_GPIO);
	gpio_free(VCC_GPIO);
}

module_init(ssd1322_init);
module_exit(ssd1322_exit);

MODULE_AUTHOR("LINDONGLIANG");
MODULE_DESCRIPTION("OLED SSD1322 DRIVER");
MODULE_LICENSE("GPL");
