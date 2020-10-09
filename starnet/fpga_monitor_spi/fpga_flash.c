#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/string.h>
#include <linux/delay.h>
#include <linux/vmalloc.h>
#include <linux/compat.h>
#include <asm/unistd.h>
#include <asm/uaccess.h>
#include <asm/io.h>

#include "fpga_spi.h"
#include "fpga_flash.h"
#include "fpga_monitor.h"

//#define FLASH_UPGRADE_CHECK
#define HEAD_LENGTH     (4)

static int flash_wait_ready(int sleep_ms);

/******************************************************************************
 * SPI FLASH elementray definition and function
 ******************************************************************************/

#define FLASH_PAGESIZE  256

/* Flash opcodes. */
#define OPCODE_WREN	    6       /* Write enable */
#define OPCODE_WRDI	    4	    /* Write disable */
#define OPCODE_RDSR	    5	    /* Read status register */
#define OPCODE_WRSR	    1	    /* Write status register */
#define OPCODE_CLSR	    0x30
#define OPCODE_READ	    3	    /* Read data bytes */
#define OPCODE_PP	    2	    /* Page program */
#define OPCODE_SE       0xD8    /* Sector erase  块擦除*/
#define OPCODE_BE       0x60    /* Bulk Erase 整片擦除 */
#define OPCODE_BE1      0xC7    /* Bulk Erase 整片擦除*/

#define OPCODE_RES	    0xAB    /* Read Electronic Signature 读一个字节的设备id*/
#define OPCODE_RDID	    0x9F    /* Read JEDEC ID */
#define OPCODE_READ_ID	0x90    /* Read Manufacturer and Device ID */

// 命令标记
#define SPIC_READ_BYTES     (1<<0)
#define SPIC_WRITE_BYTES    (1<<1)
#define SPIC_ADDR_VALID     (1<<2)
#define SPIC_4B_ADDR        (1<<3)

/* Status Register bits. */
#define SR_WIP          1   /* Write in progress */
#define SR_WEL          2   /* Write enable latch */
#define SR_BP0          4   /* Block protect 0 */
#define SR_BP1          8   /* Block protect 1 */
#define SR_BP2          0x10    /* Block protect 2 */
#define SR_EPE          0x20    /* Erase/Program error */
#define SR_SRWD         0x80    /* SR write protect */

struct chip_info {
    char* name;
    unsigned char id;
    unsigned int jedec_id;
    unsigned int sector_size;
    unsigned int n_sectors;
    char addr4b;
};

struct chip_info *spi_chip_info = NULL;

static struct chip_info chips_data[] = {
    { "AT25DF321", 0x1f, 0x47000000, 64 * 1024, 64, 0 },
    { "AT26DF161", 0x1f, 0x46000000, 64 * 1024, 32, 0 },
    { "FL016AIF", 0x01, 0x02140000, 64 * 1024, 32, 0 },
    { "FL064AIF", 0x01, 0x02160000, 64 * 1024, 128, 0 },
    {"MX25L1605D", 0xc2, 0x2015c220, 64 * 1024, 32, 0 },
    { "MX25L3205D", 0xc2, 0x2016c220, 64 * 1024, 64, 0 },
    { "MX25L6405D", 0xc2, 0x2017c220, 64 * 1024, 128, 0 },
    { "MX25L12805D", 0xc2, 0x2018c220, 64 * 1024, 256, 0 },
#ifndef NO_4B_ADDRESS_SUPPORT
    { "MX25L25635E", 0xc2, 0x2019c220, 64 * 1024, 512, 1 },
    { "S25FL256S", 0x01, 0x02194D01, 64 * 1024, 512, 1 },
    { "N25Q256A", 0x20, 0xba191000, 64 * 1024, 512, 1 },
    { "MT25QL512AB", 0x20, 0xba201044, 64 * 1024, 1024, 1 },
#endif
    { "S25FL128P", 0x01, 0x20180301, 64 * 1024, 256, 0 },
    { "S25FL129P", 0x01, 0x20184D01, 64 * 1024, 256, 0 },
    { "S25FL164K", 0x01, 0x40170140, 64 * 1024, 128, 0 },
    { "S25FL132K", 0x01, 0x40160140, 64 * 1024, 64, 0 },
    { "S25FL032P", 0x01, 0x02154D00, 64 * 1024, 64, 0 },
    { "S25FL064P", 0x01, 0x02164D00, 64 * 1024, 128, 0 },
    { "S25FL116K", 0x01, 0x40150140, 64 * 1024, 32, 0 },
    {"F25L64QA", 0x8c, 0x41170000, 64 * 1024, 128, 0 },  //ESMT
    { "F25L32QA", 0x8c, 0x41168c41, 64 * 1024, 64, 0 },  //ESMT
    { "EN25F16", 0x1c, 0x31151c31, 64 * 1024, 32, 0 },
    { "EN25Q32B", 0x1c, 0x30161c30, 64 * 1024, 64, 0 },
    { "EN25F32", 0x1c, 0x31161c31, 64 * 1024, 64, 0 },
    { "EN25F64", 0x1c, 0x20171c20, 64 * 1024, 128, 0 },     //EN25P64
    { "EN25Q64", 0x1c, 0x30171c30, 64 * 1024, 128, 0 },
    { "W25Q32BV", 0xef, 0x40160000, 64 * 1024, 64, 0 },      //S25FL032K //W25Q32FV
    { "W25Q64BV", 0xef, 0x40170000, 64 * 1024, 128, 0 }, //S25FL064K //W25Q64FV
    { "W25Q128BV", 0xef, 0x40180000, 64 * 1024, 256, 0 },  //W25Q128FV
    { "W25Q256FV", 0xef, 0x40190000, 64 * 1024, 512, 1 },
    { "N25Q032A13ESE40F", 0x20, 0xba161000, 64 * 1024, 64, 0 },
    { "N25Q064A13ESE40F", 0x20, 0xba171000, 64 * 1024, 128, 0 },
    { "N25Q128A13ESE40F", 0x20, 0xba181000, 64 * 1024, 256, 0 },
    { "GD25Q32B", 0xC8, 0x40160000, 64 * 1024, 64, 0 },
    { "GD25Q64B", 0xC8, 0x40170000, 64 * 1024, 128, 0 },
    { "GD25Q128C", 0xC8, 0x40180000, 64 * 1024, 256, 0 },
};

static int flash_cmd(const unsigned char cmd, const unsigned int addr, const unsigned char mode,
        unsigned char *buf, const size_t n_buf, const unsigned int user, const int flag)
{
    unsigned char quick_buf[64];
    unsigned char* tx_buf = NULL;

    switch (cmd) {
    case OPCODE_WREN:
        SpiWriteBytes(&cmd, 1);
        break;
    case OPCODE_WRDI:
        SpiWriteBytes(&cmd, 1);
        break;
    case OPCODE_RDSR:
        //写一个字节读一个字节
        SpiReadBytes(&cmd, 1, buf, n_buf);
        break;
    case OPCODE_WRSR:
        //写两个字节
        quick_buf[0] = cmd;
        quick_buf[1] = buf[0];
        SpiWriteBytes(quick_buf, 2);
        break;
    case OPCODE_CLSR:
        break;
    case OPCODE_READ:
        //命令、地址、读取n字节
        quick_buf[0] = cmd;
        quick_buf[1] = (addr >> 16) & 0xff;
        quick_buf[2] = (addr >> 8) & 0xff;
        quick_buf[3] = addr & 0xff;
        SpiReadBytes(quick_buf, 4, buf, n_buf);
        break;
    case OPCODE_PP:
        //命令、地址、写n字节
        tx_buf = kzalloc(1 + 3 + n_buf, GFP_KERNEL);
        tx_buf[0] = cmd;
        tx_buf[1] = (addr >> 16) & 0xff;
        tx_buf[2] = (addr >> 8) & 0xff;
        tx_buf[3] = addr & 0xff;
        memcpy(tx_buf + 4, buf, n_buf);
        SpiWriteBytes(tx_buf, 1 + 3 + n_buf);
        break;
    case OPCODE_SE:
        //命令、地址
        quick_buf[0] = cmd;
        quick_buf[1] = (addr >> 16) & 0xff;
        quick_buf[2] = (addr >> 8) & 0xff;
        quick_buf[3] = addr & 0xff;
        SpiWriteBytes(quick_buf, 4);
        break;
    case OPCODE_BE:
        break;
    case OPCODE_BE1:
        break;
    case OPCODE_RES:
        break;
    case OPCODE_RDID:
        //命令、读n字节
        SpiReadBytes(&cmd, 1, buf, n_buf);
        break;
    case OPCODE_READ_ID:
        break;
    }

    if (tx_buf) {
        kfree(tx_buf);
    }

    return n_buf;
}

/*
 * read SPI flash device ID
 */
static int flash_read_devid(unsigned char *rxbuf, int n_rx)
{
    int retval;
    unsigned char code = OPCODE_RDID;

    retval = flash_cmd(code, 0, 0, rxbuf, n_rx, 0, SPIC_READ_BYTES);
    if (retval != n_rx) {
        fpga_log("%s: ret: %x\n", __func__, retval);
        return retval;
    }

    return retval;
}

/*
 * read status register
 */
static int flash_read_sr(unsigned char *val)
{
    ssize_t retval;
    unsigned char code = OPCODE_RDSR;

    retval = flash_cmd(code, 0, 0, val, 1, 0, SPIC_READ_BYTES);
    if (retval != 1) {
        fpga_log("%s: ret: %x\n", __func__, retval);
        return -1;
    }

    return 0;
}

/*
 * write status register
 */
static int flash_write_sr(unsigned char *val)
{
    ssize_t retval;
    unsigned char code = OPCODE_WRSR;

    retval = flash_cmd(code, 0, 0, val, 1, 0, SPIC_WRITE_BYTES);
    if (retval != 1) {
        fpga_log("%s: ret: %x\n", __func__, retval);
        return -1;
    }

    return 0;
}

/*
 * 写使能，在写和擦除flash的时候，都必须先调用此函数.
 *
 */
static inline int flash_write_enable(void)
{
    unsigned char code = OPCODE_WREN;

    flash_cmd(code, 0, 0, 0, 0, 0, 0);

    return 0;
}

/*
 * 写关闭，在写和擦除flash完成的时候，都必须调用此函数.
 *
 */
static inline int flash_write_disable(void)
{
    unsigned char code = OPCODE_WRDI;

    flash_cmd(code, 0, 0, 0, 0, 0, 0);

    return 0;
}

/*
 * 去掉写保护
 * 在写和擦除flash的时候，都必须先调用此函数.
 */
static inline int flash_unprotect(void)
{
    unsigned char sr = 0;

    if (flash_read_sr(&sr) < 0) {
        fpga_log("%s: read_sr fail: sr=0x%x\n", __func__, sr);
        return -1;
    }

    //fpga_dbg("%s: read_sr success: sr=0x%x\n", __func__, sr);
    if ((sr & (SR_BP0 | SR_BP1 | SR_BP2 | SR_EPE)) != 0) {
        sr = 0;
        flash_write_sr(&sr);
    } else {
        sr = 0;
        flash_write_sr(&sr);
    }

    return 0;
}

/*
 * 在写和擦除flash的时候，都必须调用此函数.
 * 检测写和擦是否完成.
 */
static int flash_wait_ready(int sleep_ms)
{
    int count;
    int sr = 0;

    /* one chip guarantees max 5 msec wait here after page writes,
     * but potentially three seconds (!) after page erase.
     */
    for (count = 0; count < ((sleep_ms + 1) * 1000); count++) {
        if ((flash_read_sr((unsigned char*) &sr)) < 0) {
            fpga_log("%s:flash_read_sr fail,sr=0x%x!\n", __FUNCTION__, sr);
            break;
        } else if (!(sr & SR_WIP)) {
            return 0;
        }

//        fpga_dbg("%s:flash_read_sr ,sr=0x%x!\n",__FUNCTION__,sr);
        usleep_range(500, 510);
        /* REVISIT sometimes sleeping would be best */
    }

    fpga_log("%s: read_sr fail: sr=0x%x\n", __func__, sr);

    return -1;
}

/*
 * 扇区擦除功能
 * 此函数完成一个扇区的擦除
 *
 * 0表示成功；由于是gpio模拟，所以，无法识别擦除失败
 */
static int flash_erase_sector(unsigned int offset)
{
    /* Wait until finished previous write command. */
    if (flash_wait_ready(3))
        return -1;

    /* Send write enable, then erase commands. */
    flash_write_enable();
    flash_unprotect();
    flash_write_enable();

    flash_cmd(OPCODE_SE, offset, 0, 0, 0, 0, SPIC_ADDR_VALID);
    flash_wait_ready(950);

    flash_write_disable();

#ifdef FLASH_CONTENT_PRINT
    {
        // for debug, should while
        unsigned char sr = 0;

        if (flash_read_sr(&sr) < 0) {
            fpga_log("%s: read_sr fail: sr=0x%x\n", __func__, sr);
            return -1;
        }
        fpga_log("%s: read_sr success: sr=0x%x\n", __func__, sr);
    }
    #endif

    return 0;
}

/*
 * 读flash的设备id和厂家id，以便确定flash的型号和确定flash是否存在.
 * 如果不存在，使用默认的flash型号.
 */
struct chip_info* chip_prob(void)
{
    int i;
    unsigned char buf[5];
    unsigned int jedec, weight;
    struct chip_info *info, *match;

    flash_read_devid(buf, 5);
    jedec = (unsigned int) ((unsigned int) (buf[1] << 24) | ((unsigned int) buf[2] << 16)
            | ((unsigned int) buf[3] << 8) | (unsigned int) buf[4]);

    fpga_log("spi device id: %x %x %x %x %x (%x)\n", buf[0], buf[1], buf[2], buf[3], buf[4], jedec);

    //  assign default as AT25D
    weight = 0xffffffff;
    match = &chips_data[0];
    for (i = 0; i < sizeof(chips_data) / sizeof(chips_data[0]); i++) {
        info = &chips_data[i];
        if (info->id == buf[0]) {
            if (info->jedec_id == jedec) {
                fpga_log("find flash: %s\n", info->name);
                return info;
            }

            if (weight > (info->jedec_id ^ jedec)) {
                weight = info->jedec_id ^ jedec;
                match = info;
            }
        }
    }
    fpga_log("Warning: un-recognized chip ID, please update flash driver!\n");

    return match;
}

/*
 * 需要将offs修改为sector_size的整数倍才好.
 *
 */
int flash_erase(unsigned int offs, int len)
{
    fpga_dbg("offs:0x%x len:0x%x\n", offs, len);

    /* sanity checks */
    if (len == 0)
        return 0;

    /* now erase those sectors */
    while (len > 0) {
        if (flash_erase_sector(offs)) {
            fpga_log("%s:flash_erase_sector fail!\n", __FUNCTION__);
            return -1;
        }

        offs += spi_chip_info->sector_size;
        len -= spi_chip_info->sector_size;
        printk(".");
    }

    return 0;
}

// flash的读命令可以从任意地址开始读数据
int flash_read(char *buf, unsigned int from, int len)
{
    unsigned char code;
    int rdlen;

    fpga_dbg(" from:%x len:%x \n", from, len);

    /* sanity checks */
    if (len == 0)
        return 0;

    /* Wait till previous write/erase is done. */
    if (flash_wait_ready(1)) {
        /* REVISIT status return?? */
        fpga_log("%s:flash_wait_ready fail!\n", __FUNCTION__);
        return -1;
    }

    code = OPCODE_READ;
    rdlen = flash_cmd(code, from, 0, buf, len, 0,
            SPIC_READ_BYTES | SPIC_ADDR_VALID);

    return rdlen;
}

int flash_write(char *buf, unsigned int to, int len)
{
    unsigned int page_offset, page_size;
    int rc = 0, retlen = 0;

    fpga_dbg("%s: to:0x%x len:0x%x \n", __func__, to, len);

    /* sanity checks */
    if (len == 0)
        return 0;
    if (to + len > spi_chip_info->sector_size * spi_chip_info->n_sectors) {
        fpga_log("addr is exceed chip_size!\n");
        return -1;
    }

    /* Wait until finished previous write command. */
    if (flash_wait_ready(2)) {
        fpga_log("%s:error, write in progress!\n", __FUNCTION__);
        return -1;
    }

    /* what page do we start with? */
    page_offset = to % FLASH_PAGESIZE;

    /* write everything in PAGESIZE chunks */
    while (len > 0) {
        page_size = (len < (FLASH_PAGESIZE - page_offset) ? len : (FLASH_PAGESIZE - page_offset));
        page_offset = 0;

        flash_wait_ready(3);
        flash_write_enable();
        flash_unprotect();
        flash_write_enable();

        if (spi_chip_info->addr4b)
            rc = flash_cmd(OPCODE_PP, to, 0, buf, page_size, 0, SPIC_WRITE_BYTES | SPIC_4B_ADDR | SPIC_ADDR_VALID);
        else
            rc = flash_cmd(OPCODE_PP, to, 0, buf, page_size, 0, SPIC_WRITE_BYTES | SPIC_ADDR_VALID);

        if ((retlen & 0xffff) == 0)
            printk(".");

        if (rc > 0) {
            retlen += rc;
            if (rc < page_size) {
                fpga_log("%s: rc:0x%x page_size:0x%x\n", __func__, rc, page_size);
                flash_write_disable();
                return retlen;
            }
        }

        len -= page_size;
        to += page_size;
        buf += page_size;
    }

    printk("\n");

    flash_write_disable();

#ifdef  FLASH_CONTENT_PRINT
    {
        // for debug
        unsigned char sr = 0;

        if (flash_read_sr(&sr) < 0) {
            fpga_log("%s: read_sr fail: sr=0x%x\n", __func__, sr);
            return -1;
        }
        fpga_log("%s: read_sr success: sr=0x%x\n", __func__, sr);
    }
#endif

    return retlen;
}

//#define BLOCK_ALIGNE(a) (((a) & blockmask))
int flash_erase_write(char *buf, unsigned int offs, int count)
{
    int blocksize = spi_chip_info->sector_size;
    int blockmask = blocksize - 1;

    fpga_dbg(" offs:0x%x, count:0x%x \n", offs, count);

    if ((count + offs) > (spi_chip_info->sector_size * spi_chip_info->n_sectors)) {
        fpga_log("Abort: image size larger than %d! \n\n", (spi_chip_info->sector_size * spi_chip_info->n_sectors) - offs);
        mdelay(10 * 1000);  // 不能使用udelay
        return -1;
    }

    while (count > 0) {
        if (((offs & blockmask) != 0) || (count < blocksize)) {
            char *block;
            unsigned int piece, blockaddr;
            int piece_size;
            char *temp;

            block = kmalloc(blocksize, GFP_KERNEL);
            if (!block)
                return -1;
            temp = kmalloc(blocksize, GFP_KERNEL);
            if (!temp)
                return -1;

            blockaddr = offs & ~blockmask;  //flash的块地址
            memset((void*) block, 0xff, blocksize);

            piece = offs & blockmask;       //flash地址的页偏移
            if (count < blocksize - piece)
                piece_size = count;
            else
                piece_size = blocksize - piece;

            memcpy(block + piece, buf, piece_size);

            if (flash_erase(blockaddr, blocksize) != 0) {
                kfree(block);
                kfree(temp);
                return -3;
            }
            if (flash_write(block, blockaddr, blocksize) != blocksize) {
                kfree(block);
                kfree(temp);
                return -4;
            }

#ifdef FLASH_UPGRADE_CHECK
            if (flash_read(temp, blockaddr, blocksize) != blocksize) {
                kfree(block);
                kfree(temp);
                return -2;
            }

            if (memcmp(block, temp, blocksize) == 0) {
                fpga_log("block write ok \n");
            } else {
                fpga_log("block write incorrect \n");
                kfree(block);
                kfree(temp);
                return -2;
            }
#endif
            kfree(temp);
            kfree(block);

            buf += piece_size;
            offs += piece_size;
            count -= piece_size;
        } else {
            unsigned int aligned_size = count & ~blockmask; //页对齐
            char *temp;
            int i;

            temp = kmalloc(blocksize, GFP_KERNEL);
            if (!temp) {
                fpga_log("%s:kmalloc fail!\n", __FUNCTION__);
                return -1;
            }
            memset((void*) temp, 0, blocksize);

            if (flash_erase(offs, aligned_size) != 0) {
                kfree(temp);
                return -1;
            }

            if (flash_write(buf, offs, aligned_size) != aligned_size) {
                kfree(temp);
                return -1;
            }

#ifdef FLASH_UPGRADE_CHECK
            for (i = 0; i < (aligned_size / blocksize); i++) {
                if (flash_read(temp, offs + (i * blocksize), blocksize)
                        != blocksize) {
                    fpga_log("%s:flash_read fail!\n", __FUNCTION__);
                    kfree(temp);
                    return -2;
                }
                if (memcmp(buf + (i * blocksize), temp, blocksize) == 0) {
                    fpga_log("block=%d write ok! \n", i);
                } else {
                    fpga_log("block=%d  write incorrect!\n", i);
                    kfree(temp);
                    return -2;
                }
            }
#endif
            kfree(temp);

            buf += aligned_size;
            offs += aligned_size;
            count -= aligned_size;
        }
    }

    fpga_log("Done!\n");

    return 0;
}

int __fpga_flash_read(char* buf, size_t count)
{
    int offs = 0;
    int blocksize = spi_chip_info->sector_size;
    int blockmask = blocksize - 1;

    fpga_dbg(" offs:0x%x, count:0x%x \n", offs, count);

    if ((count + offs) > (spi_chip_info->sector_size * spi_chip_info->n_sectors)) {
        fpga_log("Abort: image size larger than %d! \n\n", (spi_chip_info->sector_size * spi_chip_info->n_sectors) - offs);
        mdelay(10 * 1000);  // 不能使用udelay
        return -1;
    }

    while (count > 0) {
        if (((offs & blockmask) != 0) || (count < blocksize)) {
            int piece_size;
            unsigned int piece, blockaddr;

            blockaddr = offs & ~blockmask;  //flash的块地址

            piece = offs & blockmask;       //flash地址的页偏移
            if (count < blocksize - piece)
                piece_size = count;
            else
                piece_size = blocksize - piece;

            if (flash_read(buf, blockaddr, blocksize) != blocksize) {
                fpga_log("%s:flash_read fail!\n", __FUNCTION__);
                return -1;
            }

            buf += piece_size;
            offs += piece_size;
            count -= piece_size;
        } else {
            int i;
            unsigned int aligned_size = count & ~blockmask; //页对齐

            for (i = 0; i < (aligned_size / blocksize); i++) {
                if (flash_read(buf, offs + (i * blocksize), blocksize) != blocksize) {
                    fpga_log("%s:flash_read fail!\n", __FUNCTION__);
                    return -1;
                }

                buf += blocksize;
            }

            offs += aligned_size;
            count -= aligned_size;
        }
    }

    return 0;
}

int fpga_flash_write(char* buffer, size_t len)
{
    size_t ret = 0;
    int write_pos = 0;

    //初始化相关资源
    SpiInit();
//    flash_cmd(0xff, 0, 0, 0, 0, 0, 0);
    spi_chip_info = chip_prob();

    //开始执行升级相关操作
    write_pos = *(int*)buffer;
    fpga_log("write_pos=%d, file_size=%d \n", write_pos, len - 4);

    ret = flash_erase_write(buffer + HEAD_LENGTH, write_pos, len - 4);

    //解初始化
    spi_chip_info = NULL;
    SpiDeinit();

    return ret;
}

int fpga_flash_read(char* buffer, size_t len)
{
    size_t ret = 0;
    char* tmp_buffer;

    //初始化相关资源
    SpiInit();
//    flash_cmd(0xff, 0, 0, 0, 0, 0, 0);
    spi_chip_info = chip_prob();

    //buffer页对齐
    tmp_buffer = kzalloc((len / spi_chip_info->sector_size + 1) * spi_chip_info->sector_size, GFP_KERNEL);
    if (tmp_buffer == NULL) {
        ret = -1;
        goto end;
    }

    ret = __fpga_flash_read(tmp_buffer, len);
    if (ret == 0) {
        ret = copy_to_user(buffer, tmp_buffer, len);
    }

    kfree(tmp_buffer);
end:
    //解初始化
    spi_chip_info = NULL;
    SpiDeinit();

    return ret;
}
