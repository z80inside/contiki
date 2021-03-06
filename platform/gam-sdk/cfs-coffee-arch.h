#ifndef CFS_COFFEE_ARCH_H
#define CFS_COFFEE_ARCH_H

#include "contiki-conf.h"
#include "dev/xmem.h"

/* Coffee configuration parameters. */
#define COFFEE_SECTOR_SIZE		32768UL
#define COFFEE_PAGE_SIZE		256UL
#define COFFEE_START			0
#define COFFEE_SIZE			131072UL
#define COFFEE_NAME_LENGTH		16
#define COFFEE_MAX_OPEN_FILES		6
#define COFFEE_FD_SET_SIZE		8
#define COFFEE_LOG_TABLE_LIMIT		256
#define COFFEE_DYN_SIZE			4*256
#define COFFEE_LOG_SIZE			1024

#define COFFEE_IO_SEMANTICS		1
#define COFFEE_APPEND_ONLY		0
#define COFFEE_MICRO_LOGS		1

/* Flash operations. */
#define COFFEE_WRITE(buf, size, offset)				\
		xmem_pwrite((char *)(buf), (size), COFFEE_START + (offset))

#define COFFEE_READ(buf, size, offset)				\
  		xmem_pread((char *)(buf), (size), COFFEE_START + (offset))

#define COFFEE_ERASE(sector)					\
  		xmem_erase(COFFEE_SECTOR_SIZE, COFFEE_START + (sector) * COFFEE_SECTOR_SIZE)

/* Coffee types. */
typedef int16_t coffee_page_t;

#endif /* !COFFEE_ARCH_H */

