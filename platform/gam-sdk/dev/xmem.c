/*
 * Device driver for the ST M25P40 external memory
 */

#include "contiki.h"
#include "dev/xmem.h"
#include "mc1322x.h"
#include <stdio.h>

nvmType_t type = 0;

void xmem_init(void)
{
	nvmErr_t err;
	gpio_select_function(4, 1);
	gpio_select_function(5, 1);
	gpio_select_function(6, 1);
	gpio_select_function(7, 1);
	err = nvm_detect(gNvmExternalInterface_c, &type);
}

int xmem_pread(void *buf, int nbytes, unsigned long offset)
{
	nvmErr_t err;
	err = nvm_read(gNvmExternalInterface_c, type, buf, offset, nbytes);
	if (err == gNvmErrNoError)
		return nbytes;
	else
		return 0;
}

int xmem_pwrite(const void *buf, int nbytes, unsigned long offset)
{
	nvmErr_t err;
	err = nvm_write(gNvmExternalInterface_c, type, buf, offset, nbytes);
	if (err == gNvmErrNoError)
		return nbytes;
	else
		return 0;
}

int xmem_erase(long nbytes, unsigned long offset)
{
	nvmErr_t err;
	unsigned long end = nbytes + offset;
	if (nbytes % 65536 != 0) {
		printf("xmem_erase: bad size\n");
		return -1;
	}
	if (offset % 65536 != 0) {
		printf("xmem_erase: bad offset\n");
		return -1;
	}
	while (offset < end) {
		err = nvm_erase(gNvmExternalInterface_c, type, offset / 65536);
		offset += 65536;
	}
}
