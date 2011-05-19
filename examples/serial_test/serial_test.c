/*
 * Copyright (c) 2011, Ricardo Cañuelo Navarro
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
 */

/*
 * serial_test:
 * This program is made for testing purposes and meant to run on the 
 * redbee-econotag.
 * Creates and launches three threads. One of them turns the on-board LED on
 * at pseudorandom intervals, the second one turns it off and the third one
 * waits for input at UART1 and then echoes it.
 */

#include "contiki.h"
#include "dev/leds.h"
#include "random.h"
#include "mc1322x.h"
#include "dev/serial-line.h"
#include <stdio.h> /* For printf() */

PROCESS(led_on, "Led-on");
PROCESS(led_off, "Led-off");
PROCESS(serial_echo, "serial_echo");
AUTOSTART_PROCESSES(&led_on, &led_off, &serial_echo);

PROCESS_THREAD(led_on, ev, data)
{
	*GPIO_PAD_DIR0 = 0xffffffff;
	static struct etimer et1;
	PROCESS_BEGIN();

	while(1) {
		printf("Thread: Led-on\n");
		*GPIO_DATA0 = 0xffffffff;
		etimer_set(&et1, 
			CLOCK_SECOND * 1 + random_rand() % (CLOCK_SECOND * 1));
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et1));
	}

	PROCESS_END();
}

PROCESS_THREAD(led_off, ev, data)
{
	static struct etimer et2;
	PROCESS_BEGIN();

	while(1) {
		printf("Thread: Led-off\n");
		*GPIO_DATA0 = 0x00000000;
		etimer_set(&et2, 
			CLOCK_SECOND * 2 + random_rand() % (CLOCK_SECOND * 2));
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et2));
	}

	PROCESS_END();
}

PROCESS_THREAD(serial_echo, ev, data)
{
	PROCESS_BEGIN();
	uart1_set_input(serial_line_input_byte);
	serial_line_init();

	while (1) {
		PROCESS_WAIT_EVENT();
		if (ev == serial_line_event_message)
			printf("Received: %s\n", data);
	}
	PROCESS_END();
}

