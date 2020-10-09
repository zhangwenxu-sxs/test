/***************************************************************************
*
* SIMG PART NUMBER - HDMI Transmitter Driver
*
* Copyright (C) (2011, Silicon Image)
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation version 2.
*
* This program is distributed "as is" WITHOUT ANY WARRANTY of any
* kind, whether express or implied; without even the implied warranty
* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
*****************************************************************************/

#include <linux/i2c.h>
//#include <linux/ipu.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/irq.h>
//#include <mach/hardware.h>


#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/console.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/fb.h>
#include <linux/init.h>
#include <linux/platform_device.h>
//#include <linux/mxcfb.h>
#include <linux/regulator/consumer.h>
//#include <mach/hardware.h>
#include <linux/spinlock_types.h>
#include <linux/gpio.h>

#include "siHdmiTx_902x_TPI.h"

int vmode = HDMI_1080P60;
module_param(vmode, int, S_IRUGO);
MODULE_PARM_DESC(vmode, "vmode (default: 11- HDMI_1024_768_60)");

int vformat = VMD_HDMIFORMAT_CEA_VIC;
module_param(vformat, int, S_IRUGO);
MODULE_PARM_DESC(vformat, "VideoFormat (default: 0 - VMD_HDMIFORMAT_CEA_VIC)");


#define DEVICE_NAME "sii902xA"
#define sii902xA_DEVICE_ID   0xB0

extern struct i2c_client *sii902xA;
extern struct i2c_client *siiEDID;
extern struct i2c_client *siiSegEDID;
extern struct i2c_client *siiHDCP;

typedef struct  {
	struct	i2c_driver	*driver;
	struct	i2c_adapter *pI2cAdapter;
	struct	i2c_client	*pI2cClient;
} hdmiDeviceContext_t, *phdmiDeviceContext;

struct i2c_device_info {
	uint8_t				dev_addr;
	struct i2c_client	*client;
};

#define I2C_DEV_INFO(addr) \
	{.dev_addr = addr, .client = NULL}

static struct i2c_device_info dev_addr[] = {
	I2C_DEV_INFO(0x39),    // 0x72     0111 0010
	I2C_DEV_INFO(0x3d),   // 0x7a      0111 1010
	I2C_DEV_INFO(0x50),   //  0x28      0101 0000
	I2C_DEV_INFO(0x30),   //  0x18       0011 0000
};

hdmiDeviceContext_t g_hdmi_device;

struct work_struct	*sii902xAwork=NULL;
static spinlock_t sii902xA_lock;// = SPIN_LOCK_UNLOCKED;

#define SII9022_INT_PIN_NUM         43  //GPIO5_3
#define SII9022_RESET_PIN_NUM    (132)  // GPIO16_4 , 0复位 16*8+4

static void  *reg_gpio_base=0;
static void  *reg_gpio16_base=0;

void *sii9022_int_data;
void *sii9022_rst_data;

void *gpio_dir;
void *gpio_is;
void *gpio_ibe;
void *gpio_iev;
void *gpio_ris;
void *gpio_mis;
void *gpio_ic;
void *gpio_ie;

#define SII9022_INT_BIT   3    //GPIO5_3
#define SII9022_RESET_BIT 4    // GPIO16_4 , 0复位

#define SII9022_INT_BASE_ADDR               ( (0x12145000-0x12140000)+reg_gpio_base)      // 中断管脚的基地址   GPIO5_3
#define SII9022_RESET_BASE_ADDR          reg_gpio16_base   //    0x12150000     // 复位管脚的基地址  GPIO16_4

#define SII9022_INT_MASK (0x1<<SII9022_INT_BIT)
#define SII9022_RESET_MASK (0x1<<SII9022_RESET_BIT)

#define SII9022_INT_OFFEST (0x1<<(2+SII9022_INT_BIT))    // data offset
#define SII9022_RESET_OFFSET (0x1<<(2+SII9022_RESET_BIT))  // data offset

#define GPIO_DIR_OFFSET 0x400
#define GPIO_IS_OFFSET  0x404
#define GPIO_IBE_OFFSET 0x408
#define GPIO_IEV_OFFSET 0x40C
#define GPIO_IE_OFFSET  0x410
#define GPIO_RIS_OFFSET 0x414
#define GPIO_MIS_OFFSET 0x418
#define GPIO_IC_OFFSET  0x41C

#define IO_ADDRESS(addr)                 (addr)      // (u32 *)ioremap(addr,4)
#define SYS_WRITEL(Value,Addr ) ((*(volatile unsigned int *)(Addr)) = (Value))
#define SYS_READ(Addr)          (*((volatile int *)(Addr)))

void sii9022_gpio_mux(void)
{
    int *pinmux;

    // 配置中断管脚为GPIO  GPIO5_3
    pinmux = ioremap(0x1f00002C,4);
    if( pinmux != NULL) {
        SYS_WRITEL(0x1400,pinmux);
    }

    //配置复位管脚为GPIO  GPIO16_4
    pinmux = ioremap(0x1f0010d8,4);
    if( pinmux != NULL) {
        SYS_WRITEL(0x400,pinmux);
    }

    return;
}


void sii9022_gpio_reset(void)
{
    gpio_set_value(SII9022_RESET_PIN_NUM, 0 );
    //SYS_WRITEL(0,sii9022_rst_data);
    DelayMS(TX_HW_RESET_PERIOD);
    gpio_set_value(SII9022_RESET_PIN_NUM, 1 );
    //SYS_WRITEL(0xff,sii9022_rst_data);
    DelayMS(TX_HW_RESET_PERIOD);
    printk("%s:reset sii9022!\n",__FUNCTION__);
}


#if 1
void gpio_init(void)
{
    reg_gpio_base = (void*)ioremap(0x12140000, 0x10000);
    if (NULL == reg_gpio_base)
    {
        printk("%s: reg_gpio_base is null\n",__FUNCTION__);
        goto out;
    }

    reg_gpio16_base = (void*)ioremap(0x12150000, 0x10000);
    if (NULL == reg_gpio16_base)
    {
        printk("%s: reg_gpio16_base is null\n",__FUNCTION__);
        goto out;
    }

    sii9022_int_data = IO_ADDRESS(SII9022_INT_BASE_ADDR + SII9022_INT_OFFEST);
    sii9022_rst_data = IO_ADDRESS(SII9022_RESET_BASE_ADDR + SII9022_RESET_OFFSET);

    gpio_dir = IO_ADDRESS(SII9022_INT_BASE_ADDR + GPIO_DIR_OFFSET);
    gpio_is = IO_ADDRESS(SII9022_INT_BASE_ADDR + GPIO_IS_OFFSET);
    gpio_ibe = IO_ADDRESS(SII9022_INT_BASE_ADDR + GPIO_IBE_OFFSET);
    gpio_iev = IO_ADDRESS(SII9022_INT_BASE_ADDR + GPIO_IEV_OFFSET);
    gpio_ris = IO_ADDRESS(SII9022_INT_BASE_ADDR + GPIO_RIS_OFFSET);
    gpio_mis = IO_ADDRESS(SII9022_INT_BASE_ADDR + GPIO_MIS_OFFSET);
    gpio_ic = IO_ADDRESS(SII9022_INT_BASE_ADDR + GPIO_IC_OFFSET);
    gpio_ie = IO_ADDRESS(SII9022_INT_BASE_ADDR + GPIO_IE_OFFSET);

    printk("enter %s:reg_gpio_base=%lx\n",__FUNCTION__,(long int)reg_gpio_base);
    return;
out:
    if (NULL != reg_gpio_base)
    {
        iounmap(reg_gpio_base);
        reg_gpio_base = 0;
    }
    if (NULL != reg_gpio16_base)
    {
        iounmap(reg_gpio16_base);
        reg_gpio16_base = 0;
    }
    printk("exit %s!\n",__FUNCTION__);
    return;
}

u32 gpio_int_check(void)
{
    u32 ris;
    u32 data;

    ris = SYS_READ(gpio_ris);
    data = SYS_READ(sii9022_int_data);

    if (ris&SII9022_INT_MASK) {
        return 1;
    }

    return 0;
}

// 这样写不好吧，把别人的中断给清除掉了？
void gpio_int_clr(void)
{
    u32 ris;

    ris = SYS_READ(gpio_ris);

    //printk("clr.ris = %x\n",ris);

    SYS_WRITEL(SII9022_INT_MASK,gpio_ic);//&GPIO_INT_BIT
}

void gpio_isr_init(void)
{
    u32 dir;
    u32 is;
    u32 ibe;
    u32 iev;
    //u32 ris;
    //u32 mis;
    //u32 ic;
    u32 ie;
    //int res;
    int *gpio_reset_dir;
    u32 val;

    //设置为边沿触发中断
    is = SYS_READ(gpio_is);
    is &= ~SII9022_INT_MASK;
    SYS_WRITEL(is, gpio_is);

    //设置为单边沿触发中断，不是双边沿触发
    ibe = SYS_READ(gpio_ibe);
    ibe &= ~SII9022_INT_MASK;
    SYS_WRITEL(ibe, gpio_ibe);

    //设置为下降沿触发或低电平触发
    iev = SYS_READ(gpio_iev);
    iev &= ~SII9022_INT_MASK;
    SYS_WRITEL(is, gpio_iev);

    //设置中断管脚为输入
    dir = SYS_READ(gpio_dir);
    dir &= ~SII9022_INT_MASK;
    SYS_WRITEL(dir, gpio_dir);

    // 使能中断
    ie = SYS_READ(gpio_ie);
    ie |= SII9022_INT_MASK;
    SYS_WRITEL(ie,gpio_ie);

    // 清除中断
    SYS_WRITEL(SII9022_INT_MASK,gpio_ic);

    // 设置复位管脚为输出
    gpio_reset_dir = IO_ADDRESS(SII9022_RESET_BASE_ADDR + GPIO_DIR_OFFSET);
    val = SYS_READ(gpio_reset_dir);
    val |= SII9022_RESET_MASK ;
    SYS_WRITEL(val,gpio_reset_dir);

    printk("exit  %s\n",__FUNCTION__);

    return ;
}
#endif

static int match_id(const struct i2c_device_id *id, const struct i2c_client *client)
{
	if (strcmp(client->name, id->name) == 0)
		return true;

	return false;
}

static void work_queue(struct work_struct *work)
{
	siHdmiTx_TPI_Poll();
	enable_irq(sii902xA->irq);
}

static irqreturn_t sii902xA_interrupt(int irq, void *dev_id)
{
	unsigned long lock_flags = 0;	 

    printk("enter The sii902xA interrupt............\n");
#if 1
    disable_irq_nosync(irq);
	spin_lock_irqsave(&sii902xA_lock, lock_flags);
#endif
	//printk("The sii902xA interrupt handeler is working..\n");  
	//printk("The most of sii902xA interrupt work will be done by following tasklet..\n");  

	schedule_work(sii902xAwork);
#if 1
	spin_unlock_irqrestore(&sii902xA_lock, lock_flags);
#endif
    //gpio_int_clr();
	//enable_irq(irq);

	return IRQ_HANDLED;
}


static const struct i2c_device_id hmdi_sii_id[] = {
	{ "sii902xA", 0 },
	{ "siiEDID", 0 },
	{ "siiSegEDID", 0 },
	{ "siiHDCP", 0 },
};

static int hdmi_sii_probe(struct i2c_client *client,
			const struct i2c_device_id *id)
{
	int ret=0;
#if 0
	//static struct mxc_lcd_platform_data *plat_data;
	if (!i2c_check_functionality(client->adapter,
				     I2C_FUNC_SMBUS_BYTE | I2C_FUNC_I2C))
		return -ENODEV;
	
	
	if(match_id(&hmdi_sii_id[1], client))
		{
			siiEDID = client;
			dev_info(&client->adapter->dev, "attached hmdi_EDID: %s "
				"into i2c adapter successfully \n", id->name);
		}
	else if(match_id(&hmdi_sii_id[2], client))
		{
			siiSegEDID = client;
			dev_info(&client->adapter->dev, "attached hmdi_Seg_EDID: %s "
				"into i2c adapter successfully \n", id->name);
		}
	else if(match_id(&hmdi_sii_id[3], client))
		{
			siiHDCP = client;
			dev_info(&client->adapter->dev, "attached hmdi_HDCP: %s "
				"into i2c adapter successfully \n", id->name);
		}
	else if(match_id(&hmdi_sii_id[0], client))
		{	
			/*
			plat_data = client->dev.platform_data;
			if (plat_data->reset)
				plat_data->reset();
			*/
			sii902xA = client;
			dev_info(&client->adapter->dev, "attached hmdi_sii_id[0] %s "
				"into i2c adapter successfully\n", id->name);
			
			if(sii902xA != NULL )
			{				
				printk("\n============================================\n");
				printk("SiI-902xA Driver Version 1.4 \n");
				printk("============================================\n");
				
				sii902xAwork = kmalloc(sizeof(*sii902xAwork), GFP_ATOMIC);
				INIT_WORK(sii902xAwork, work_queue); 
				//
				// Initialize the registers as required. Setup firmware vars.
				//
				
				siHdmiTx_VideoSel( HDMI_1024_768_60,VMD_HDMIFORMAT_CEA_VIC);
				siHdmiTx_AudioSel( AFS_48K);
				siHdmiTx_TPI_Init();
				siHdmiTx_PowerStateD3();
				
				ret = request_irq(client->irq, sii902xA_interrupt, IRQ_TYPE_LEVEL_LOW,
					  client->name, client);
			
				if (ret)
					printk(KERN_INFO "%s:%d:902xA interrupt failed\n", __func__,__LINE__);	
					//free_irq(irq, iface);
				else
					enable_irq_wake(client->irq);	
			}			
		}
	else
		{
			dev_err(&client->adapter->dev, "invalid i2c adapter: can not found dev_id matched\n");
			return -EIO;
		}
#endif
	return ret;

}


static int hdmi_sii_remove(struct i2c_client *client)
{
#if 0
	//int irq = (int)(sii902xA->irq);

	free_irq(sii902xA->irq,&sii902xA); 
	hdmi_sii_remove(sii902xA);
	dev_info(&client->adapter->dev, "detached from i2c adapter successfully\n");
#endif
	return 0;
}


static struct i2c_driver hdmi_sii_i2c_driver = {
	.driver = {
		.name = DEVICE_NAME,
		.owner = THIS_MODULE,
	},
	.probe = hdmi_sii_probe,
	.remove =  __exit_p(hdmi_sii_remove),
	.id_table = hmdi_sii_id,
};


static struct i2c_board_info __initdata si_9022_i2c_boardinfo[] = {
    {
        I2C_BOARD_INFO(DEVICE_NAME, (0x39)),
        .flags = I2C_CLIENT_WAKE,
        .irq = 197,
	}
};

void hdmi_work_init(void)
{
    int ret;
    unsigned long  irqFlags;

    printk("hdmi_work_init :in\n");

    sii902xAwork = kmalloc(sizeof(*sii902xAwork), GFP_ATOMIC);
    INIT_WORK(sii902xAwork, work_queue);

    //sii9022_gpio_reset();

    //siHdmiTx_VideoSel( HDMI_1024_768_60,VMD_HDMIFORMAT_CEA_VIC);
    siHdmiTx_VideoSel(vmode,vformat);

    siHdmiTx_AudioSel( AFS_48K);
    siHdmiTx_TPI_Init();
    siHdmiTx_PowerStateD3();

    //irqFlags = IRQF_TRIGGER_FALLING|IRQF_SHARED|IRQF_ONESHOT|IRQF_NO_SUSPEND;
    irqFlags = IRQF_TRIGGER_FALLING|IRQF_SHARED;
    ret = request_irq(sii902xA->irq, sii902xA_interrupt, irqFlags,
        sii902xA->name, sii902xA);

    if (ret)
        printk(KERN_INFO "%s:%d:902xA interrupt failed\n", __func__,__LINE__);	
					//free_irq(irq, iface);
    else
        enable_irq_wake(sii902xA->irq);	

    printk("%s:%d:902xA interrupt=%d success!\n", __FUNCTION__,__LINE__,sii902xA->irq);
}

//static int __init hdmi_sii_init(void)
static int  hdmi_sii_init(void)
{
	int ret,idx;

    spin_lock_init(&sii902xA_lock);

    sii9022_gpio_mux();
    // 下面两个函数不能调用，如果调用，会导致中断无法产生
#if 0
    gpio_init();
    gpio_isr_init();
#endif

    ret = gpio_request(SII9022_RESET_PIN_NUM, "sii9022-gpio-reset");
    if (ret != 0) {
        printk("gpio_request sii9022_gpio-reset faile: %d\n",SII9022_RESET_PIN_NUM);
        goto failout;
    }
    printk("%s: sii9022  reset_gpio = %d set as output!\n", __FUNCTION__,SII9022_RESET_PIN_NUM);
    gpio_direction_output(SII9022_RESET_PIN_NUM, 0);
    sii9022_gpio_reset();
    // for /sys/class/gpio debug
   // gpio_free(SII9022_RESET_PIN_NUM);


    ret = gpio_request(SII9022_INT_PIN_NUM, "sii9022_int");
    if (ret) {
        printk("gpio_request sii9022_gpio int  faile: %d\n",SII9022_INT_PIN_NUM);
        goto failout1;
    }
    gpio_direction_input(SII9022_INT_PIN_NUM);
    si_9022_i2c_boardinfo[0].irq = gpio_to_irq(SII9022_INT_PIN_NUM);
    printk("%s: sii9022  int_gpio = %d set as input,irq=%d!\n", __FUNCTION__,SII9022_INT_PIN_NUM,si_9022_i2c_boardinfo[0].irq);


    g_hdmi_device.pI2cAdapter = i2c_get_adapter(10);
    if (g_hdmi_device.pI2cAdapter == NULL) {
		pr_err ("%s() failed to get i2c adapter\n", __func__);
        ret = -1;
		goto err_exit;
	}

	for (idx = 0; idx < ARRAY_SIZE(dev_addr); idx++) {
		if (idx ==0 ) {
			g_hdmi_device.pI2cClient = i2c_new_device(g_hdmi_device.pI2cAdapter, &si_9022_i2c_boardinfo[idx]);
			dev_addr[idx].client = g_hdmi_device.pI2cClient;
		} else {
			dev_addr[idx].client = i2c_new_dummy(g_hdmi_device.pI2cAdapter,
											dev_addr[idx].dev_addr);
		}
		if (dev_addr[idx].client == NULL){
		    printk("%s:i2c_new_device fail!\n",__FUNCTION__);
		    if(idx == 0 ) {
		        goto err_exit;
		    }
		} else {
            printk("%s:i2c_new_device success! dev_addr=0x%02x,idx=%d!\n",__FUNCTION__,dev_addr[idx].dev_addr,idx);
		}
	}
    g_hdmi_device.driver = &hdmi_sii_i2c_driver;
	ret = i2c_add_driver(g_hdmi_device.driver);
	if (ret < 0) {
		pr_err("[ERROR] %s():%d failed !\n", __func__, __LINE__);
        ret = -1;
		goto err_exit;
	}

    sii902xA = dev_addr[0].client;
    siiEDID = dev_addr[2].client;
    siiSegEDID = dev_addr[3].client;
    siiHDCP = dev_addr[1].client;

    hdmi_work_init();

    return ret;

    /*
	ret = i2c_add_driver(&hdmi_sii_i2c_driver);
	if (ret)
		printk(KERN_ERR "%s: failed to add sii902xA i2c driver\n", __func__);
    */
err_exit:
    i2c_put_adapter(g_hdmi_device.pI2cAdapter);
    //free_irq(sii902xA->irq,&sii902xA);
    gpio_free(SII9022_INT_PIN_NUM);
failout1:
    gpio_free(SII9022_RESET_PIN_NUM);
failout:
	return ret;	
}

static void __exit hdmi_sii_exit(void)
{
    int idx;

    if( sii902xAwork != NULL ) {
        kfree(sii902xAwork);
        sii902xAwork = NULL;
    }

    i2c_put_adapter(g_hdmi_device.pI2cAdapter);
    //free_irq(sii902xA->irq,&sii902xA);
    gpio_free(SII9022_INT_PIN_NUM);
    gpio_free(SII9022_RESET_PIN_NUM);

    if(g_hdmi_device.driver ) {
        i2c_del_driver(g_hdmi_device.driver);
    }

    for (idx = 0; idx < ARRAY_SIZE(dev_addr); idx++) {
        if (dev_addr[idx].client != NULL){
                i2c_unregister_device(dev_addr[idx].client);
        }
    }

	printk("Module is leaving..\n");  

}

late_initcall(hdmi_sii_init);
module_exit(hdmi_sii_exit);
MODULE_VERSION("1.4");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Silicon image SZ office, Inc.");
MODULE_DESCRIPTION("sii902xA HDMI driver");
MODULE_ALIAS("platform:hdmi-sii902xA");
