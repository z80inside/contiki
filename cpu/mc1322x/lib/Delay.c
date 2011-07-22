/************************************************************************************
* Delay.c
* File that contains delay routines for miliseconds and microseconds.
*
* (c) Copyright 2008, Freescale, Inc.  All rights reserved.
*
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale.
*
************************************************************************************/
#include <inttypes.h>

#define SYSTEM_CLOCK (24000000/1000)

/***********************************************************************************/
// this should be compiled in ARM mode, was tested only with ADS compiler
void DelayMs(uint32_t ms)
{
  volatile uint32_t i;
  while(ms--)
  {
    i = (uint32_t)(SYSTEM_CLOCK/7); // 1000 us , 7 CPU cycles / loop
    while(i--);
  }
}

/***********************************************************************************/
// this should be compiled in ARM mode, was tested only with ADS compiler
void DelayUs(uint32_t us)
{
  volatile uint32_t i;
  while(us--)
  {
    i = (uint32_t)(SYSTEM_CLOCK/7000); // 1 us , 7 CPU cycles / loop
    while(i--);
  }
}

/***********************************************************************************/
