#include "contiki.h"
#include "contiki-net.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>

/* Only one protosocket, so we won't be able to handle more than one connection
 * at a time
 */
static struct psock psocket;

/* Buffer for incoming data */
static uint8_t buffer[50];

/* Protothread required by the protosocket. Handles a connection */
static PT_THREAD(handle_connection(struct psock *p))
{
	/* Since this is a protosocket's protothread it must start with this*/
	PSOCK_BEGIN(p);

	/* Send welcome message */
	PSOCK_SEND_STR(p, "Echo server: please type something and "
				"press Enter.\n");
	/* Read incoming data */
	PSOCK_READTO(p, '\n');
	/* Send back the contents of the buffer */
	PSOCK_SEND_STR(p, "Thank you. Here's your input back: ");
	PSOCK_SEND(p, buffer, PSOCK_DATALEN(p));
	PSOCK_SEND_STR(p, "Bye!\r\n");

	/* Close the protosocket */
	PSOCK_CLOSE(p);

	/* End the protosocket's protothread */
	PSOCK_END(p);
}

/* Process declaration */
PROCESS(tcp_echo_server, "TCP echo server");
AUTOSTART_PROCESSES(&tcp_echo_server);

/* Process definition */
PROCESS_THREAD(tcp_echo_server, ev, data)
{
	PROCESS_BEGIN();

	/* Start listening on a TCP port */
	tcp_listen(UIP_HTONS(5005));

	while (1) {
		/* Wait until we receive a TCP/IP event (connection) */
		PROCESS_WAIT_EVENT_UNTIL(ev == tcpip_event);
		/* If someone connected with us*/
		if (uip_connected()) {
			/* Initialize the protosocket and binds the input buffer
			 * to it */
			PSOCK_INIT(&psocket, buffer, sizeof(buffer));
			/* Until connection is aborted, closed or times out */
			while(!(uip_aborted() || uip_closed()
				|| uip_timedout())) {
				/* Wait for the next tcpip event */
				PROCESS_WAIT_EVENT_UNTIL(ev == tcpip_event);
				handle_connection(&psocket);
			}
		}
	}
	PROCESS_END();
}
