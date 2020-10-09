#ifndef __FPGA_FLASH_H
#define __FPGA_FLASH_H

int fpga_flash_write(char* buffer, size_t len);
int fpga_flash_read(char* buffer, size_t len);

#endif
