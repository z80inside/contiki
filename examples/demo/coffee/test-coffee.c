#include "contiki.h"
#include "cfs/cfs.h"
#include "cfs/cfs-coffee.h"
#include "lib/crc16.h"
#include "lib/random.h"
#include "Delay.h"

#include <stdio.h>
#include <string.h>

PROCESS(testcoffee_process, "Test CFS/Coffee process");
AUTOSTART_PROCESSES(&testcoffee_process);

static int coffee_file_test(void)
{
	int fd;
	int ret;
	int i;

	unsigned char buf[256];
	unsigned char buf2[256];

	/*
	for (i = 0; i < 256; i++)
		buf[i] = 255 - i;

	cfs_remove("testfile");

	fd = cfs_open("testfile", CFS_WRITE);
	if (fd < 0)
		return -1;

	ret = cfs_write(fd, buf, sizeof(buf));
	if (ret < 0)
		return -1;

	cfs_close(fd);
	*/

	fd = cfs_open("testfile", CFS_READ);
	if (fd < 0)
		return -1;

	ret = cfs_read(fd, buf2, sizeof(buf2));
	
	for (i = 0; i < 256; ++i) {
		printf(" %x ", buf2[i]);
		DelayMs(100);
	}

	cfs_close(fd);
	return 0;
}

PROCESS_THREAD(testcoffee_process, ev, data)
{
  int start;

  PROCESS_BEGIN();

  start = clock_seconds();
//  printf("Coffee format: %d\n", cfs_coffee_format());
  printf("Coffee file test: %d\n", coffee_file_test());
  printf("Test time: %d seconds\n", (int)(clock_seconds() - start));

  PROCESS_END();
}
