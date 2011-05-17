/*
 * Copyright (c) 2006, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 * $Id: hello-world.c,v 1.1 2006/10/02 21:46:46 adamdunkels Exp $
 */

/**
 * \file
 *         A very simple Contiki application showing how Contiki programs look
 * \author
 *         Adam Dunkels <adam@sics.se>
 */

#include "contiki.h"
#include "nvm.h"

#include <stdio.h> /* For printf() */

#define READ_ADDR 0x1f000

nvmType_t type = 0;
nvmErr_t err;
uint32_t buf[256];
uint32_t i;
uint32_t j;
uint32_t size;
uint32_t block = 0;

/*---------------------------------------------------------------------------*/
PROCESS(nvm_test_process, "Non-volatile memory process");
AUTOSTART_PROCESSES(&nvm_test_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(nvm_test_process, ev, data)
{
	PROCESS_BEGIN();

	printf("Hello, world\n");
	printf("Detecting internal nvm\n");
	err = nvm_detect(gNvmInternalInterface_c, &type);
	printf("nvm_detect returned: %#010x\n", err);
	printf("Type is: %#010x\n\n", type);
	printf("Reading flash . . . \n\n");
	err = nvm_read(gNvmInternalInterface_c, type, (uint8_t *)buf, 
		0, 32);
	size = buf[1];
	printf("Image size: %i bytes\n", size);
	for (i = 0; i < size/256; ++i) {
		err = nvm_read(gNvmInternalInterface_c, type, (uint8_t *)buf, 
			256 * block, 256);
		for (j = 0; j < 256; ++j) {
			printf("%i: %#010x\n", (256 * block) + j, buf[j]);
		}
		block++;
	}
	err = nvm_read(gNvmInternalInterface_c, type, (uint8_t *)buf, 
		256 * block, size - (256*block));
	for (i = 0; i < size - (256*block); ++i)
		printf("%i: %#010x\n", (256 * block) + i, buf[i]);

	PROCESS_END();
}
/*---------------------------------------------------------------------------*/
