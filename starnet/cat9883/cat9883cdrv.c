#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/time.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/kthread.h>
#include <linux/platform_device.h>

#include "cat9883cdrv.h"

#define FALSE 0
#define TRUE 1

#define _TV_Mode_External_PLL_
#define _PC_Mode_External_PLL_

typedef struct {
    uint8_t ucAddr;
    uint8_t ucValue;
} reg_pair;

unsigned short CurrentMode = 0;
bool SyncStable = FALSE;
unsigned char FrameRate;

static int thread_run = 1;                      //控制线程的退出
static struct task_struct* poll_task = NULL;    //用于执行轮询任务的线程

static struct i2c_board_info i2c_info;
static struct i2c_adapter* i2c_adapter = NULL;
static struct i2c_client* i2c_client = NULL;

uint8_t PC_576Px50[] = { 0x35, 0xf0, 0xf0, 0x80, 0x20, 0x10, 0xf0, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x80, 0x40, 0x6f, 0xb8, 0x19, 0x00, 0x00 };

uint8_t PC_480Px60[] = { 0x35, 0x90, 0xf0, 0x38, 0x20, 0x10, 0xf0, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x80, 0x40, 0x6f, 0xb8, 0x19, 0x00, 0x00 };

uint8_t PC_720Px60[] = { 0x67, 0x10, 0xf0, 0x38, 0x40, 0x40, 0xf0, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x80, 0x44, 0x6f, 0xb8, 0x19, 0x00, 0x00 };

uint8_t PC_1080ix60[] = { 0x89, 0x70, 0xf0, 0x38, 0x40, 0x40, 0xf0, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x80, 0x44, 0x6f, 0xb8, 0x19, 0x00, 0x00 };

uint8_t PC_640x480x60[] = { 0x31, 0xf0, 0xf0, 0x88, 0x10, 0x10, 0xf0, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x80, 0x42, 0x6e, 0xb8, 0x19, 0x00, 0x00 };

uint8_t PC_640x480x72[] = { 0x33, 0xf0, 0xf0, 0x88, 0x10, 0x10, 0xf0, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x80, 0x42, 0x6e, 0xb8, 0x19, 0x00, 0x00 };

uint8_t PC_640x480x75[] = { 0x34, 0x70, 0xf0, 0x88, 0x10, 0x10, 0xf0, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x80, 0x42, 0x6e, 0xb8, 0x19, 0x00, 0x00 };

uint8_t PC_640x480x85[] = { 0x33, 0xf0, 0xf0, 0x88, 0x10, 0x10, 0xf0, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x80, 0x42, 0x6e, 0xb8, 0x19, 0x00, 0x00 };

uint8_t PC_800x600x56[] = { 0x3f, 0xf0, 0xf0, 0x38, 0x10, 0x20, 0xf0, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x80, 0x42, 0x6e, 0xb8, 0x19, 0x00, 0x00 };

uint8_t PC_800x600x60[] = { 0x41, 0xf0, 0xf0, 0x38, 0x10, 0x20, 0xf0, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x80, 0x42, 0x6e, 0xb8, 0x19, 0x00, 0x00 };

uint8_t PC_800x600x72[] = { 0x40, 0xf0, 0xf0, 0x38, 0x10, 0x20, 0xf0, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x80, 0x42, 0x6e, 0xb8, 0x19, 0x00, 0x00 };

uint8_t PC_800x600x75[] = { 0x41, 0xf0, 0xf0, 0x38, 0x10, 0x20, 0xf0, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x80, 0x42, 0x6e, 0xb8, 0x19, 0x00, 0x00 };

uint8_t PC_800x600x85[] = { 0x41, 0x70, 0xf0, 0x38, 0x10, 0x20, 0xf0, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x80, 0x42, 0x6e, 0xb8, 0x19, 0x00, 0x00 };

uint8_t PC_1024x768x60[] = { 0x53, 0xf0, 0xf0, 0x38, 0x10, 0x40, 0xf0, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x80, 0x42, 0x6e, 0xb8, 0x19, 0x00, 0x00 };

uint8_t PC_1024x768x70[] = { 0x52, 0xf0, 0xf0, 0x38, 0x10, 0x40, 0xf0, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x80, 0x42, 0x6e, 0xb8, 0x19, 0x00, 0x00 };

uint8_t PC_1024x768x75[] = { 0x51, 0xf0, 0xf0, 0x38, 0x10, 0x40, 0xf0, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x80, 0x42, 0x6e, 0xb8, 0x19, 0x00, 0x00 };

uint8_t PC_1024x768x80[] = { 0x53, 0x70, 0xf0, 0x38, 0x10, 0x40, 0xf0, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x80, 0x42, 0x6e, 0xb8, 0x19, 0x00, 0x00 };

uint8_t PC_1024x768x85[] = { 0x55, 0xf0, 0xf0, 0x38, 0x10, 0x40, 0xf0, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x80, 0x42, 0x6e, 0xb8, 0x19, 0x00, 0x00 };

uint8_t PC_1280x1024x60[] = { 0x69, 0x70, 0xf0, 0x38, 0x10, 0x40, 0xf0, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x80, 0x42, 0x6e, 0xb8, 0x19, 0x00, 0x00 };

uint8_t PC_1280x1024x75[] = { 0x69, 0x70, 0xf0, 0x38, 0x10, 0x40, 0xf0, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x80, 0x42, 0x6e, 0xb8, 0x19, 0x00, 0x00 };

uint8_t PC_1280x1024x85[] = { 0x6b, 0xf0, 0xf0, 0x38, 0x10, 0x40, 0xf0, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x80, 0x42, 0x6e, 0xb8, 0x19, 0x00, 0x00 };

uint8_t PC_1600x1200x60[] = { 0x86, 0xf0, 0xf0, 0x40, 0x10, 0x80, 0xf0, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x80, 0x42, 0x6e, 0xb8, 0x19, 0x00, 0x00 };

uint8_t PC_1360x768x60[] = { 0x6f, 0xf0, 0xf0, 0x10, 0x10, 0x40, 0xf0, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x80, 0x40, 0x6f, 0xb8, 0x19, 0x00, 0x00 };

uint8_t PC_1680x1050x60[] = { 0x8b, 0xf0, 0xf0, 0xa8, 0x40, 0x40, 0xf0, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x80, 0x40, 0x6f, 0xb8, 0x19, 0x00, 0x00 };

uint8_t PC_1440x900x60[] = { 0x76, 0xf0, 0xf0, 0x00, 0x20, 0x20, 0xf0, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x80, 0x40, 0x6f, 0xb8, 0x19, 0x00, 0x00 };

uint8_t PC_1280x800x60[] = { 0x68, 0xf0, 0xf0, 0x00, 0x10, 0x40, 0xf0, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x80, 0x40, 0x6f, 0xb8, 0x19, 0x00, 0x00 };

uint8_t PC_1280x960x60[] = { 0x70, 0x70, 0xf0, 0x00, 0x10, 0x40, 0xf0, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x80, 0x40, 0x6f, 0xb8, 0x19, 0x00, 0x00 };

uint8_t PC_1920x1080x60[] = { 0x89, 0x70, 0xf0, 0x80, 0x30, 0x20, 0xf0, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x80, 0x40, 0x6f, 0xb8, 0x19, 0x00, 0x00 };

uint8_t PC_1920x1200xReduce[] = { 0x81, 0xf0, 0xf0, 0x80, 0x06, 0x10, 0xf0,
        0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x40, 0x6f, 0xb8, 0x19, 0x00, 0x00 };

uint8_t TV_480ix60[] = { 0x35, 0x90, 0xf0, 0xa8, 0x08, 0x10, 0xf0, 0x80, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x60, 0x6e, 0x8d, 0x19, 0x08, 0x00 };

uint8_t TV_480Px60[] = { 0x35, 0x90, 0xf0, 0x38, 0x08, 0x10, 0xf0, 0x80, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x60, 0x6e, 0x8d, 0x19, 0x08, 0x00 };

uint8_t TV_576ix50[] = { 0x35, 0xf0, 0xf0, 0x80, 0x08, 0x10, 0xf0, 0x80, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x60, 0x6e, 0x8d, 0x19, 0x08, 0x00 };

uint8_t TV_576Px50[] = { 0x35, 0xf0, 0xf0, 0x80, 0x08, 0x10, 0xf0, 0x80, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x60, 0x6e, 0x8d, 0x19, 0x08, 0x00 };

uint8_t TV_720Px60[] = { 0x67, 0x10, 0xf0, 0x38, 0x40, 0x20, 0xf0, 0x80, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x44, 0x6e, 0x8d, 0x19, 0x08, 0x00 };

uint8_t TV_720Px50[] = { 0x7b, 0xb0, 0xf0, 0x80, 0x40, 0x20, 0xf0, 0x80, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x44, 0x6e, 0x8d, 0x19, 0x08, 0x00 };

uint8_t TV_1080ix50[] = { 0xa4, 0xf0, 0xf0, 0x38, 0x40, 0x20, 0xf0, 0x80, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x44, 0x6e, 0x8d, 0x19, 0x08, 0x00 };

uint8_t TV_1080ix60[] = { 0x89, 0x70, 0xf0, 0x38, 0x40, 0x20, 0xf0, 0x80, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x44, 0x6e, 0x8d, 0x19, 0x08, 0x00 };

uint8_t TV_1080Px50[] = { 0xa4, 0xf0, 0xf0, 0x38, 0x40, 0x20, 0xf0, 0x80, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x40, 0x6e, 0xbd, 0x19, 0x08, 0x00 };

uint8_t TV_1080Px60[] = { 0x89, 0x70, 0xf0, 0x38, 0x40, 0x20, 0xf0, 0x80, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x40, 0x6e, 0xbd, 0x19, 0x08, 0x00 };

uint8_t Default_ini[] = { 0x69, 0xd0, 0xf0, 0x38, 0x10, 0x40, 0xf0, 0x80, 0x80,
        0x80, 0x80, 0x80, 0x80, 0x40, 0x6e, 0xbd, 0x19, 0x00, 0x00 };

reg_pair cat9883c_ini_value[] = {
    { 0x01, 0x69 },
    { 0x02, 0xd0 },
    { 0x03, 0x88 },
    { 0x07, 0xf0 },
    { 0x8f, 0x68 },
    { 0x86, 0x29 },
    { 0x8d, 0x80 },
    { 0x84, 0x00 },
    { 0x87, 0x69 },
    { 0x91, 0x30 },
#if	defined(_TV_Mode_External_PLL_)
    { 0x96, 0x22 },
#elif 	defined(_PC_Mode_External_PLL_)
    { 0x96, 0x22 },
#else
    { 0x96, 0x25 },
#endif
    { 0x98, 0x19 },
    { 0x84, 0x0C },
    { 0x99, 0x08 }
};

reg_pair TV480iini[] = {
    { 0x91, 0x70 },
#ifdef 	_TV_Mode_External_PLL_
    { 0x94, 0x81 },
    { 0x96, 0x2a },
#else
	{ 0x94, 0xF1 },
	{ 0x96, 0x29 },
#endif
};

reg_pair TV576iini[] = {
    { 0x91, 0x70 },
#ifdef 	_TV_Mode_External_PLL_
    { 0x94, 0x81 },
    { 0x96, 0x2a },
#else
    { 0x94, 0xF1 },
    { 0x96, 0x29 },
#endif
};

reg_pair TV480pini[] = {
    { 0x91, 0x70 },
#ifdef 	_TV_Mode_External_PLL_
    { 0x03, 0xa8 },
    { 0x04, 0x38 },
    { 0x94, 0x41 },
    { 0x96, 0x22 },
#else
    { 0x94, 0x31 },
    { 0x96, 0x29 },
#endif
};

reg_pair TV576pini[] = {
    { 0x91, 0x70 },
#ifdef 	_TV_Mode_External_PLL_
    { 0x03, 0xa8 },
    { 0x04, 0x80 },
    { 0x94, 0x41 },
    { 0x96, 0x22 },
#else
    { 0x94, 0x31 },
    { 0x96, 0x29 },
#endif
};

reg_pair TV720pini[] = {
    { 0x91, 0x70 },
#ifdef 	_TV_Mode_External_PLL_
    { 0x03, 0xb8 },
    { 0x04, 0x38 },
    { 0x94, 0x31 },
    { 0x96, 0x22 },
#else
    { 0x94, 0x31 },
    { 0x96, 0x29 },
#endif
};

reg_pair TV1080iini[] = {
    { 0x91, 0x70 },
#ifdef 	_TV_Mode_External_PLL_
    { 0x03, 0xa8 },
    { 0x04, 0x38 },
    { 0x94, 0x81 },
    { 0x96, 0x22 },
#else
    { 0x94, 0x51 },
    { 0x96, 0x29 },
#endif
};

reg_pair TV1080pini[] = {
    { 0x91, 0x70 },
#ifdef 	_TV_Mode_External_PLL_
    { 0x03, 0xe8 },
    { 0x04, 0x38 },
    { 0x94, 0x81 },
    { 0x96, 0x22 },
#else
    { 0x94, 0x51 },
    { 0x96, 0x29 },
#endif
};

int cat9883_i2c_write_byte(unsigned char addr, unsigned char value)
{
    int ret;

    ret = i2c_smbus_write_byte_data(i2c_client, addr, value);
    if (ret < 0) {
        CAT9883_LOG("do i2c write failed. addr:0x%.2x  val:0x%.2x \n",addr, value);
    }

    return ret;
}

int cat9883_i2c_write_bytes(unsigned char addr, unsigned char* values, unsigned int length)
{
#if 1
    int ret;
    unsigned char* buf;
    buf = (unsigned char*)kmalloc(length + 1, GFP_KERNEL);

    if (buf == NULL) {
        return -1;
    }

    buf[0] = addr;
    memcpy(buf + 1, values, length);

    ret = i2c_master_send(i2c_client, buf, length + 1);
    kfree(buf);

    return ret;
#else
    int i;

    for (i = 0; i < length; i++) {
        cat9883_i2c_write_byte(addr, values[i]);
    }

    return length;
#endif
}

int cat9883_i2c_read_byte(unsigned char addr, unsigned char* val)
{
    int ret;

    ret = i2c_smbus_read_byte_data(i2c_client, addr);
    if (ret < 0) {
        CAT9883_LOG("do i2c read failed. addr:0x%.2x \n", addr);
        *val = -1;
        return ret;
    }

    *val = (unsigned char)(ret & 0xff);

    return 0;
}

void __load_reg(reg_pair *p_reg_pair, uint32_t num)
{
    uint32_t i;

    for (i = 0; i < num; i++) {
        cat9883_i2c_write_byte(p_reg_pair->ucAddr, p_reg_pair->ucValue);
        p_reg_pair += 1;
    }
}

void __cat9883c_initial(void)
{
    __load_reg(cat9883c_ini_value, sizeof(cat9883c_ini_value) / 2);
    return;
}

unsigned short __Hsync_counter(void)
{
    uint8_t i, j;
    unsigned short hsync_counter;
    uint8_t ucValue = 0x68;

    cat9883_i2c_write_byte(0x8F, ucValue);

    cat9883_i2c_read_byte(0xAB, &i);
    cat9883_i2c_read_byte(0xAC, &j);

    hsync_counter = ((j << 4) & 0x0F00) + i;

    return hsync_counter;
}

unsigned short __Vsync_timer(void)
{
    return 0x3bb0;
}

unsigned short __Frame_rate(void)
{
    unsigned short FrameRate_mode;

    // >20480 unknow
    if (__Vsync_timer() > 0x5000) {
        FrameRate = 30;
        FrameRate_mode = 0x0030;
    } else if (__Vsync_timer() > 0x4300) {
        // >13600 50Hz---0x47f1
        FrameRate = 50;
        FrameRate_mode = 0x0050;
    } else if (__Vsync_timer() > 0x3d00) {
        // >13600 56Hz---0x4008
        FrameRate = 56;
        FrameRate_mode = 0x0056;
    } else if (__Vsync_timer() > 0x3700) {
        // >15204 60Hz ---0x3bb0
        FrameRate = 60;
        FrameRate_mode = 0x0060;
    } else if (__Vsync_timer() > 0x32ab) {
        // >14096 70Hz ---0x3377
        FrameRate = 70;
        FrameRate_mode = 0x0070;
    } else if (__Vsync_timer() > 0x30eb) {
        // >13600 72Hz ---0x31df
        FrameRate = 72;
        FrameRate_mode = 0x0072;
    } else if (__Vsync_timer() > 0x2d27) {
        // >12500 75Hz ---0x2ff7
        FrameRate = 75;
        FrameRate_mode = 0x0075;
    } else if (__Vsync_timer() > 0x2800) {
        // >15000 85Hz ---0x2a57
        FrameRate = 85;
        FrameRate_mode = 0x0085;
    } else {
        // unknow
        FrameRate = 100;
        FrameRate_mode = 0x00ff;
    }

    return FrameRate_mode;
}

unsigned short __TV_mode(void)
{
    unsigned short Resolution_mode;

    if (__Hsync_counter() < 0x0070) {
        // <xxx unknow
        Resolution_mode = 0xff00;
    } else if (__Hsync_counter() < 0x0120) {
        // 480i = 0x107
        Resolution_mode = 0xa400;
    } else if (__Hsync_counter() < 0x0180) {
        // 576i = 0x138
        Resolution_mode = 0xa500;
    } else if (__Hsync_counter() < 0x0220) {
        // 480P = 0x20d
        Resolution_mode = 0xb400;
    } else if (__Hsync_counter() < 0x0250) {
        // 1080i= 0x233
        Resolution_mode = 0xa100;
    } else if (__Hsync_counter() < 0x02a0) {
        // 576P = 0x271
        Resolution_mode = 0xb500;
    } else if (__Hsync_counter() < 0x03a0) {
        // 720P = 0x2ee
        Resolution_mode = 0xb700;
    } else if (__Hsync_counter() < 0x04a0) {
        // 1080P= 0x465
        Resolution_mode = 0xb100;
    } else {
        // unknow
        Resolution_mode = 0xff00;
    }

    return (Resolution_mode + __Frame_rate());
}

unsigned short __PC_mode(void)
{
    unsigned short Resolution_mode;

    if (__Hsync_counter() < 0x0170) {
        // <xxx unknow
        Resolution_mode = 0xff00;
    } else if (__Hsync_counter() >= 0x01a0 && __Hsync_counter() < 0x0240) {
        // 640x480 = 0x20d
        Resolution_mode = 0x6400;
    } else if (__Hsync_counter() >= 0x0240 && __Hsync_counter() < 0x02d0) {
        // 800x600 = 0x0274
        Resolution_mode = 0x8000;
    } else if (__Hsync_counter() >= 0x02d0 && __Hsync_counter() < 0x0310) {
        // 1280x720 = 0x02EE//Clive
        Resolution_mode = 0x0300;
    } else if (__Hsync_counter() >= 0x0310 && __Hsync_counter() < 0x031d) {
        // 1360x768 = 0x031b
        Resolution_mode = 0x1300;
    } else if (__Hsync_counter() >= 0x031e && __Hsync_counter() < 0x0331) {
        // 1024x768 = 0x0326/(320@75)
        Resolution_mode = 0x1000;
    } else if (__Hsync_counter() >= 0x0332 && __Hsync_counter() < 0x0352) {
        // 1280x800  V_total=828=0x33c    Clive 0407
        Resolution_mode = 0xC800;
    } else if (__Hsync_counter() >= 0x0360 && __Hsync_counter() < 0x039f) {
        // 1440x900 reducedblanking
        Resolution_mode = 0x1480;
    } else if (__Hsync_counter() >= 0x03a0 && __Hsync_counter() <= 0x03a7) {
        // 1440x900
        Resolution_mode = 0x1400;
    } else if (__Hsync_counter() >= 0x03c0 && __Hsync_counter() < 0x0400) {
        // 1280x960 = 0x03e8
        Resolution_mode = 0xC900;
    } else if (__Hsync_counter() >= 0x0420 && __Hsync_counter() < 0x0437) {
        // 1280x1024 = 0x042a
        Resolution_mode = 0x1200;
    } else if (__Hsync_counter() >= 0x0437 && __Hsync_counter() <= 0x043a) {
        // 1680x1050 reducedblanking =
        Resolution_mode = 0x1680;
    } else if (__Hsync_counter() >= 0x0440 && __Hsync_counter() < 0x0459) {
        // 1680x1050
        Resolution_mode = 0x1600;
    } else if (__Hsync_counter() >= 0x0459 && __Hsync_counter() < 0x0490) {
        // 1920x1080 = 0x465 //Clive 041108 0x460 => 0x459
        Resolution_mode = 0x1900;
    } else if (__Hsync_counter() >= 0x04d0 && __Hsync_counter() <= 0x04d6) {
        // 1920x1200 reducedblanking = 0x04d3
        Resolution_mode = 0x1980;
    } else if (__Hsync_counter() >= 0x04da && __Hsync_counter() < 0x0510) {
        // 1600x1200 = 0x04e2
        Resolution_mode = 0x1C00;
    } else {
        // >=1300 unknow
        Resolution_mode = 0xff00;
    }

    return (Resolution_mode + __Frame_rate());
}

bool __Is_TV_mode(void)
{
    uint8_t ucValue = 0;

    cat9883_i2c_read_byte(0x14, &ucValue);

    return (ucValue & 0x40) ? TRUE : FALSE;

}

unsigned short __get_mode_number(void)
{
    if (__Is_TV_mode()) {
        return __TV_mode();
    } else {
        return __PC_mode();
    }
}

bool __mode_stable(void)
{
    //VGA do not connect
    if (__Hsync_counter() == 0xFFF)
        return TRUE;

    if ((CurrentMode == 0) || ((CurrentMode & 0x00ff) == 0x00ff)
            || ((CurrentMode & 0xff00) == 0xff00))
        return FALSE;

    //Set_counter0_Timer1();
    //Start_Stop_counter();

    if ((__get_mode_number() == CurrentMode))
        return TRUE;

    return FALSE;
}

#define Reg_Pair_initial(x) __load_reg(x, (sizeof(x)/2))

void __set_mode(unsigned short Mode_number)
{
    uint8_t ucValue;

    printk("------ Mode parameter ------\n");
    printk("------ Hsync_counter = %x------\n", __Hsync_counter());
    printk("------ Mode_number = %x------\n", Mode_number);

    if (!(__Is_TV_mode())) {
        printk(
                "------ FGM _PC_Mode_External_PLL_ Mode Support 2018/1/8 ------\n");
        ucValue = 0x81;
        cat9883_i2c_write_byte(0x94, ucValue);

        ucValue = 0x22;
        cat9883_i2c_write_byte(0x96, ucValue);	//set to external PLL on PC mode FGM 2014012
    }

    switch (Mode_number) {
    case 0x0150:
        cat9883_i2c_write_bytes(0x01, PC_576Px50, sizeof(PC_576Px50));
        break;
    case 0x0260:
        cat9883_i2c_write_bytes(0x01, PC_480Px60, sizeof(PC_480Px60));
        break;
    case 0x0360:
        cat9883_i2c_write_bytes(0x01, PC_720Px60, sizeof(PC_720Px60));
        break;
    case 0x0460:
        cat9883_i2c_write_bytes(0x01, PC_1080ix60, sizeof(PC_1080ix60));
        break;
//---------------640x480----------------//
    case 0x6460:
        cat9883_i2c_write_bytes(0x01, PC_640x480x60, sizeof(PC_640x480x60));
        break;
    case 0x6472:
        cat9883_i2c_write_bytes(0x01, PC_640x480x72, sizeof(PC_640x480x72));
        break;
    case 0x6475:
        cat9883_i2c_write_bytes(0x01, PC_640x480x75, sizeof(PC_640x480x75));
        break;
    case 0x6485:
        cat9883_i2c_write_bytes(0x01, PC_640x480x85, sizeof(PC_640x480x85));
        break;
//---------------800x600----------------//
    case 0x8056:
        cat9883_i2c_write_bytes(0x01, PC_800x600x56, sizeof(PC_800x600x56));
        break;
    case 0x8060:
        cat9883_i2c_write_bytes(0x01, PC_800x600x60, sizeof(PC_800x600x60));
        break;
    case 0x8072:
        cat9883_i2c_write_bytes(0x01, PC_800x600x72, sizeof(PC_800x600x72));
        break;
    case 0x8075:
        cat9883_i2c_write_bytes(0x01, PC_800x600x75, sizeof(PC_800x600x75));
        break;
    case 0x8085:
        cat9883_i2c_write_bytes(0x01, PC_800x600x85, sizeof(PC_800x600x85));
        break;
//---------------1024x768---------------//
    case 0x1060:
        cat9883_i2c_write_bytes(0x01, PC_1024x768x60, sizeof(PC_1024x768x60));
        break;
    case 0x1070:
        cat9883_i2c_write_bytes(0x01, PC_1024x768x70, sizeof(PC_1024x768x70));
        break;
    case 0x1075:
        cat9883_i2c_write_bytes(0x01, PC_1024x768x75, sizeof(PC_1024x768x75));
        break;
    case 0x1085:
        cat9883_i2c_write_bytes(0x01, PC_1024x768x85, sizeof(PC_1024x768x85));
        break;
//---------------1280x800---------------//        Clive 0407
    case 0xC860:
        cat9883_i2c_write_bytes(0x01, PC_1280x800x60, sizeof(PC_1280x800x60));
        break;
//---------------1280x960---------------//
    case 0xC960:
        cat9883_i2c_write_bytes(0x01, PC_1280x960x60, sizeof(PC_1280x960x60));
        break;
//---------------1280x1024---------------//
    case 0x1260:
        cat9883_i2c_write_bytes(0x01, PC_1280x1024x60, sizeof(PC_1280x1024x60));
        break;
    case 0x1275:
        cat9883_i2c_write_bytes(0x01, PC_1280x1024x75, sizeof(PC_1280x1024x75));
        break;
    case 0x1285:
        cat9883_i2c_write_bytes(0x01, PC_1280x1024x85, sizeof(PC_1280x1024x85));
        break;
//---------------1360x768---------------//
    case 0x1360:
        cat9883_i2c_write_bytes(0x01, PC_1360x768x60, sizeof(PC_1360x768x60));
        break;
//---------------1440x900---------------//
    case 0x1460:
        cat9883_i2c_write_bytes(0x01, PC_1440x900x60, sizeof(PC_1440x900x60));
//---------------1680x1050---------------//
        break;
    case 0x1660:
        cat9883_i2c_write_bytes(0x01, PC_1680x1050x60, sizeof(PC_1680x1050x60));
        break;
//---------------1600x1200---------------//
    case 0x1C60:
        cat9883_i2c_write_bytes(0x01, PC_1600x1200x60, sizeof(PC_1600x1200x60));
        break;
//---------------1920x1200---------------//
    case 0x19e0:
        cat9883_i2c_write_bytes(0x01, PC_1920x1200xReduce,
                sizeof(PC_1920x1200xReduce));
        break;
//---------------1920x1080---------------//
    case 0x1960:
        cat9883_i2c_write_bytes(0x01, PC_1920x1080x60, sizeof(PC_1920x1080x60));
        break;

// TV Frame Rate    50Hz    56Hz    60Hz
//  480i                            a450
//  576i            a550
//  480P                            b460
// 1080i            a150            a160
//  576P            b550
//  720P            b750            b760
// 1080P                            b160
    case 0xa460:
        cat9883_i2c_write_bytes(0x01, TV_480ix60, sizeof(TV_480ix60));
        break;

    case 0xa550:
        cat9883_i2c_write_bytes(0x01, TV_576ix50, sizeof(TV_576ix50));
        break;

    case 0xb460:
        cat9883_i2c_write_bytes(0x01, TV_480Px60, sizeof(TV_480Px60));
        break;

    case 0xa150:
        cat9883_i2c_write_bytes(0x01, TV_1080ix50, sizeof(TV_1080ix50));
        break;
    case 0xa160:
        cat9883_i2c_write_bytes(0x01, TV_1080ix60, sizeof(TV_1080ix60));
        break;

    case 0xb550:
        cat9883_i2c_write_bytes(0x01, TV_576Px50, sizeof(TV_576Px50));
        break;

    case 0xb750:
        cat9883_i2c_write_bytes(0x01, TV_720Px50, sizeof(TV_720Px50));
        break;
    case 0xb760:
        cat9883_i2c_write_bytes(0x01, TV_720Px60, sizeof(TV_720Px60));
        break;

    case 0xb150:
        cat9883_i2c_write_bytes(0x01, TV_1080Px50, sizeof(TV_1080Px50)); // CC 0511
        break;
    case 0xb160:
        cat9883_i2c_write_bytes(0x01, TV_1080Px60, sizeof(TV_1080Px60));
        break;

    default:
        cat9883_i2c_write_bytes(0x01, Default_ini, sizeof(Default_ini));
    }

    if (__Is_TV_mode()) {
        switch (Mode_number) {

        case 0xb160:
            Reg_Pair_initial(TV1080pini);		  // Add 1080P with internal PLL
            break;
        case 0xb150:
            Reg_Pair_initial(TV1080pini);		  // Add 1080P with internal PLL
            break;
        case 0xa150:
            Reg_Pair_initial(TV1080iini);
            break;
        case 0xa160:
            Reg_Pair_initial(TV1080iini);
            break;
        case 0xb750:
            Reg_Pair_initial(TV720pini);
            break;
        case 0xb760:
            Reg_Pair_initial(TV720pini);
            break;
        case 0xa550:
            Reg_Pair_initial(TV576iini);
            break;
        case 0xb550:
            Reg_Pair_initial(TV576pini);
            break;
        case 0xa460:
            Reg_Pair_initial(TV480iini);
            break;
        case 0xb460:
            Reg_Pair_initial(TV480pini);
            break;
        default:
            ucValue = 0x55;
            cat9883_i2c_write_byte(0x94, ucValue);
            ucValue = 0x22;
            cat9883_i2c_write_byte(0x96, ucValue);
            break;
        }

        mdelay(10);
    }

    CurrentMode = Mode_number;

    return;
}

void __ModeDetectingprocess(void)
{
    __set_mode(__get_mode_number());
    return;
}

void __AutoColorCalibration(void)
{
    uint8_t ucValue = 0x0c;

    cat9883_i2c_write_byte(0x84, ucValue);		  // Calibration once
    mdelay(20);

    return;
}

void __ADC_Mode_Change(void)
{
    __ModeDetectingprocess();
    __AutoColorCalibration();
    return;
}

static int cat9883_poll_thread(void* data)
{
    while (thread_run) {
        usleep_range(1000000, 1100000);
        if (!__mode_stable()) {
            printk("mode change \n");
            __ADC_Mode_Change();
        }
    }

    return 0;
}

static int cat9883_probe(struct platform_device* pdev)
{
    int ret;
    unsigned int i2c_bus;   //cat9883所在的i2c总线
    unsigned int dev_addr;  //cat9883的设备地址(八位读写地址)

    CAT9883_LOG("start probe cat9883 in board \n");
    if (pdev->dev.of_node == NULL) {
        CAT9883_LOG("not device tree associated with this device \n");
        return -1;
    }

    CAT9883_LOG("parse device node[%s] \n", pdev->dev.of_node->full_name);

    //获取cat9883的总线地址
    ret = of_property_read_u32_index(pdev->dev.of_node, "i2c_info", 0, &i2c_bus);
    if (ret != 0) {
        CAT9883_LOG("get number of i2c bus failed \n");
        return -1;
    }

    //获取cat9883设备地址
    ret = of_property_read_u32_index(pdev->dev.of_node, "i2c_info", 1, &dev_addr);
    if (ret != 0) {
        CAT9883_LOG("get device address of cat9883 failed \n");
        return -1;
    }

    strcpy(i2c_info.type, "cat9883");
    i2c_info.addr = dev_addr >> 1;      //转换成7位设备地址

    i2c_adapter = i2c_get_adapter(i2c_bus);
    if (i2c_adapter == NULL) {
        CAT9883_LOG("get adapter failed for cat9883 \n");
        return -1;
    }

    i2c_client = i2c_new_device(i2c_adapter, &i2c_info);
    if (i2c_client == NULL) {
        CAT9883_LOG("create i2c client for cat9883 failed \n");
        i2c_put_adapter(i2c_adapter);
        return -1;
    }

    //创建轮询线程
    poll_task = kthread_create(cat9883_poll_thread, NULL, "cat9883 polling thread");
    if (poll_task == NULL) {
        i2c_put_adapter(i2c_adapter);
        i2c_unregister_device(i2c_client);
        return -1;
    }

    //初始化cat9883
    __cat9883c_initial();
    //开启轮询线程
    wake_up_process(poll_task);
    CAT9883_LOG("probe one cat9883@i2c-%d with device address: 0x%x \n", i2c_bus, dev_addr);

    return 0;
}

static int cat9883_remove(struct platform_device* pdev)
{
    thread_run = 0;
    kthread_stop(poll_task);

    if (i2c_adapter != NULL) {
        i2c_put_adapter(i2c_adapter);
    }

    if (i2c_client != NULL) {
        i2c_unregister_device(i2c_client);
    }

    return 0;
}

static struct of_device_id cat9883_ids[] = {
    {.compatible = "cat9883"},
    {}
};

static struct platform_driver cat9883_drv = {
    .driver = {
        .name = "cat9883",
        .of_match_table = of_match_ptr(cat9883_ids)
    },
    .probe = cat9883_probe,
    .remove = cat9883_remove
};

static int __init cat9883c_drv_init(void)
{
    return platform_driver_register(&cat9883_drv);
}

static void __exit cat9883c_drv_exit(void)
{
    platform_driver_unregister(&cat9883_drv);
}

module_init(cat9883c_drv_init);
module_exit(cat9883c_drv_exit);

MODULE_AUTHOR("zhang gaowang (starnet)");
MODULE_DESCRIPTION("cat9883c_drvr");
MODULE_LICENSE("GPL");
MODULE_ALIAS("cat9883c_drv");
