#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "contiki-net.h"
#include "rest.h"
#include "gpio.h"
#include "pwm.h"
#include "rtc.h"
#include "adc.h"

#if defined (CONTIKI_TARGET_SKY) /* Any other targets will be added here (&& defined (OTHER))*/
#include "dev/light-sensor.h"
#include "dev/battery-sensor.h"
#include "dev/sht11-sensor.h"
#include "dev/leds.h"
#endif /*defined (CONTIKI_TARGET_SKY)*/

#define DEBUG 1
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#define PRINT6ADDR(addr) PRINTF(" %02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x ", ((u8_t *)addr)[0], ((u8_t *)addr)[1], ((u8_t *)addr)[2], ((u8_t *)addr)[3], ((u8_t *)addr)[4], ((u8_t *)addr)[5], ((u8_t *)addr)[6], ((u8_t *)addr)[7], ((u8_t *)addr)[8], ((u8_t *)addr)[9], ((u8_t *)addr)[10], ((u8_t *)addr)[11], ((u8_t *)addr)[12], ((u8_t *)addr)[13], ((u8_t *)addr)[14], ((u8_t *)addr)[15])
#define PRINTLLADDR(lladdr) PRINTF(" %02x:%02x:%02x:%02x:%02x:%02x ",(lladdr)->addr[0], (lladdr)->addr[1], (lladdr)->addr[2], (lladdr)->addr[3],(lladdr)->addr[4], (lladdr)->addr[5])
#else
#define PRINTF(...)
#define PRINT6ADDR(addr)
#define PRINTLLADDR(addr)
#endif

char temp[100];
static int duty = 0;
static int new = 0;

/* Resources are defined by RESOURCE macro, signature: resource name, the http methods it handles and its url*/
RESOURCE(helloworld, METHOD_GET, "helloworld");

/* For each resource defined, there corresponds an handler method which should be defined too.
 * Name of the handler method should be [resource name]_handler
 * */
void
helloworld_handler(REQUEST* request, RESPONSE* response)
{
  sprintf(temp,"Medidora de consumo");

  rest_set_header_content_type(response, TEXT_PLAIN);
  rest_set_response_payload(response, (uint8_t*)temp, strlen(temp));
}

RESOURCE(discover, METHOD_GET, ".well-known/core");
void
discover_handler(REQUEST* request, RESPONSE* response)
{
  char temp[100];
  int index = 0;
  index += sprintf(temp + index, "%s,", "</helloworld>;n=\"HelloWorld\"");
  index += sprintf(temp + index, "%s,", "</led>;n=\"LedControl\"");
  index += sprintf(temp + index, "%s", "</light>;n=\"Light\"");

  rest_set_response_payload(response, (uint8_t*)temp, strlen(temp));
  rest_set_header_content_type(response, APPLICATION_LINK_FORMAT);
}

RESOURCE(pwm0, METHOD_POST, "pwm0");
void pwm0_handler(REQUEST* request, RESPONSE* response)
{
	char req_duty[4];
	int rcv_duty;

	if (rest_get_post_variable(request, "duty", req_duty, 4)) {
		rcv_duty = strtol(req_duty, (char **) NULL, 10);
		if (rcv_duty < 0 || rcv_duty > 100) {
			rest_set_response_status(response, BAD_REQUEST_400);
			return;
		}
		PRINTF("duty: %d\n", rcv_duty);
		rest_set_response_status(response, OK_200);
		new = rcv_duty;
	}
}

RESOURCE(adc, METHOD_GET, "adc");
void adc_handler(REQUEST* request, RESPONSE* response)
{
	uint8_t i;
	uint32_t adcread = 0;

	adc_service();
	rtc_delay_ms(10);
	for (i = 0; i < 50; ++i) {
		adc_service();
		adcread += adc_reading[0];
		rtc_delay_ms(10);
	}
	/*
	rtc_delay_ms(100);
	for (i = 0; i < 500; ++i) {
		adc_service();
		adcread[1] += adc_reading[0];
	}
	rtc_delay_ms(100);
	for (i = 0; i < 500; ++i) {
		adc_service();
		adcread[2] += adc_reading[0];
	}

	real_value = ((adcread[0] / 500) + (adcread[1] / 500) + 
			(adcread[2] / 500)) / 3;
	*/
	sprintf(temp,"%d\n", adcread / 50);

	rest_set_header_content_type(response, TEXT_PLAIN);
	rest_set_response_payload(response, (uint8_t*)temp, strlen(temp));
}

#if defined (CONTIKI_TARGET_SKY)
/*A simple actuator example, depending on the color query parameter and post variable mode, corresponding led is activated or deactivated*/
RESOURCE(led, METHOD_POST | METHOD_PUT , "led");

void
led_handler(REQUEST* request, RESPONSE* response)
{
  char color[10];
  char mode[10];
  uint32_t led = 0;
  int success = 1;

  if (rest_get_query_variable(request, "color", color, 10)) {
    PRINTF("color %s\n", color);

    if (!strcmp(color,"red")) {
      led = (1 << 11);
    } else if(!strcmp(color,"green")) {
      led = LEDS_GREEN;
    } else if ( !strcmp(color,"blue") ) {
      led = LEDS_BLUE;
    } else {
      success = 0;
    }
  } else {
    success = 0;
  }

  if (success && rest_get_post_variable(request, "mode", mode, 10)) {
    PRINTF("mode %s\n", mode);

    if (!strcmp(mode, "on")) {
	gpio_pad_dir_set((uint64_t) (1 << 11));
	gpio_data_set((uint64_t) (1 << 11));
      //leds_on(led);
    } else if (!strcmp(mode, "off")) {
	gpio_pad_dir_set((uint64_t) (1 << 11));
	gpio_data_reset((uint64_t) (1 << 11));
      //leds_off(led);
    } else {
      success = 0;
    }
  } else {
    success = 0;
  }

  if (!success) {
    rest_set_response_status(response, BAD_REQUEST_400);
  }
}

uint16_t light_photosynthetic;
uint16_t light_solar;

void
read_light_sensor(uint16_t* light_1, uint16_t* light_2)
{
  *light_1 = light_sensor.value(LIGHT_SENSOR_PHOTOSYNTHETIC);
  *light_2 = light_sensor.value(LIGHT_SENSOR_TOTAL_SOLAR);
}
#endif

/*A simple getter example. Returns the reading from light sensor with a simple etag*/
RESOURCE(light, METHOD_GET, "light");
void
light_handler(REQUEST* request, RESPONSE* response)
{
#ifdef CONTIKI_TARGET_SKY
  read_light_sensor(&light_photosynthetic, &light_solar);
  sprintf(temp,"%u;%u", light_photosynthetic, light_solar);
#else /*CONTIKI_TARGET_SKY*/
  sprintf(temp,"%d.%d", 0, 0);
#endif /*CONTIKI_TARGET_SKY*/

  char etag[4] = "ABCD";
  rest_set_header_content_type(response, TEXT_PLAIN);
  rest_set_header_etag(response, etag, sizeof(etag));
  rest_set_response_payload(response, temp, strlen(temp));
}

/*A simple actuator example. Toggles the red led*/
RESOURCE(toggle, METHOD_GET | METHOD_PUT | METHOD_POST, "toggle");
void
toggle_handler(REQUEST* request, RESPONSE* response)
{
//  leds_toggle(LEDS_RED);
}
//#endif /*defined (CONTIKI_TARGET_SKY)*/


PROCESS(rest_server_example, "Rest Server Example");
PROCESS(pwm_control, "pwm-control");
//PROCESS(adc_control, "adc-control");
AUTOSTART_PROCESSES(&rest_server_example, &pwm_control);

PROCESS_THREAD(pwm_control, ev, data)
{
	static struct etimer et1;
	int i;

	PROCESS_BEGIN();
	while (1) {
		etimer_set(&et1, CLOCK_SECOND / 2);
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et1));
		if (duty < new) {
			for (i = duty; i <= new; ++i) {
				pwm_duty_ex(0, (int)((65536 * (100 - i)) / 100));
				rtc_delay_ms((i - duty)/2);
			}
		}
		else if (new < duty) {
			for (i = duty; i >= new ; --i) {
				pwm_duty_ex(0, (int)((65536 * (100 - i)) / 100));
				rtc_delay_ms((duty - i)/2);
			}
		}
		duty = new;
	}
	PROCESS_END();
}

/*
PROCESS_THREAD(adc_control, ev, data)
{
	static struct etimer et1;
	PROCESS_BEGIN();
	while (1) {
		etimer_set(&et1, CLOCK_SECOND);
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et1));
		adcread = ADC_READ();
		ADC_flush();
		adc_service();
	}
	PROCESS_END();
}
*/

PROCESS_THREAD(rest_server_example, ev, data)
{
  PROCESS_BEGIN();

#ifdef WITH_COAP
  PRINTF("COAP Server\n");
#else
  PRINTF("HTTP Server\n");
#endif

  rest_init();

	/* Timer0 PWM setup */
	gpio_pad_dir_set((uint64_t) (1 << 11));
	pwm_init_ex(0, 500, 65535, 1);
	pwm_duty_ex(0, 65535);

	/* ADC setup (Channel 0) */
	adc_init();
	adc_select_channels(1);
	adc_enable();
	adc_service();

	/* RTC setup */
	rtc_init_osc(0);
	rtc_calibrate();

#if defined (CONTIKI_TARGET_SKY)
  SENSORS_ACTIVATE(light_sensor);
  rest_activate_resource(&resource_led);
  rest_activate_resource(&resource_light);
  rest_activate_resource(&resource_toggle);
#endif /*defined (CONTIKI_TARGET_SKY)*/

  rest_activate_resource(&resource_helloworld);
  rest_activate_resource(&resource_pwm0);
  rest_activate_resource(&resource_adc);
  rest_activate_resource(&resource_discover);

  PROCESS_END();
}
