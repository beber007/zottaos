/* Copyright (c) 2006-2012 MIS Institute of the HEIG-VD affiliated to the University of
** Applied Sciences of Western Switzerland. All rights reserved.
** Permission to use, copy, modify, and distribute this software and its documentation
** for any purpose, without fee, and without written agreement is hereby granted, pro-
** vided that the above copyright notice, the following three sentences and the authors
** appear in all copies of this software and in the software where it is used.
** IN NO EVENT SHALL THE MIS INSTITUTE NOR THE HEIG-VD NOR THE UNIVERSITY OF APPLIED
** SCIENCES OF WESTERN SWITZERLAND BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL,
** INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS
** DOCUMENTATION, EVEN IF THE MIS INSTITUTE OR THE HEIG-VD OR THE UNIVERSITY OF APPLIED
** SCIENCES OF WESTERN SWITZERLAND HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
** THE MIS INSTITUTE, THE HEIG-VD AND THE UNIVERSITY OF APPLIED SCIENCES OF WESTERN SWIT-
** ZERLAND SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFT-
** WARE PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE MIS INSTITUTE NOR THE HEIG-VD
** AND NOR THE UNIVERSITY OF APPLIED SCIENCES OF WESTERN SWITZERLAND HAVE NO OBLIGATION
** TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
*/
/* File TestTimerEvent.c: Shows how to use API ZottaOS_TimerEvent. This simple program
** periodically turns LEDS on and then schedules a event to turn them off.
** Prior to using this API, you should run ZottaOSconf.exe to define a timer with two
** interrupt sources (one for the overflow and its corresponding capture/compare regis-
** ter 1, e.g. OS_IO_TIMER1_A1_TA and OS_IO_TIMER1_A1_CC1 for Timer1 A), and also a port
** pin interrupt to act as a software interrupt (e.g. port 1 pin 6 OS_IO_PORT1_6).
** Version identifier: May 2012
** Authors: MIS-TIC */

#include "ZottaOS.h"
#include "ZottaOS_TimerEvent.h"

static void SetLed1Task(void *argument);
static void ClearLed1Task(void *argument);
static void SetLed2Task(void *argument);
static void ClearLed2Task(void *argument);

#define SetFlag(GPIO_Pin) P1OUT |= GPIO_Pin;
#define ClearFlag(GPIO_Pin) P1OUT &= ~GPIO_Pin;

int main(void)
{
  void *tmp;

  WDTCTL = WDTPW + WDTHOLD;  // Disable watchdog timer

  /* Initialize output I/O ports */
  P1SEL = 0x00;    // Set for GPIO
  P1DIR = 0x03;    // Set to output
  P1OUT = 0x00;    // Initially start at low

  /* Define and start the event handlers */
  if (!OSInitTimerEvent(2,OS_IO_PORT1_6,OS_IO_TIMER1_A1_TA))
     while (TRUE); // Initialization problem

  #if defined(ZOTTAOS_VERSION_HARD)
     tmp = OSCreateEventDescriptor();
     OSCreateSynchronousTask(ClearLed1Task,100,tmp,NULL);
     OSCreateTask(SetLed1Task,0,500,500,tmp);
     tmp = OSCreateEventDescriptor();
     OSCreateSynchronousTask(ClearLed2Task,10,tmp,NULL);
     OSCreateTask(SetLed2Task,0,1000,1000,tmp);
  #elif defined(ZOTTAOS_VERSION_SOFT)
     tmp = OSCreateEventDescriptor();
     OSCreateSynchronousTask(ClearLed1Task,0,1000,0,tmp,NULL);
     OSCreateTask(SetLed1Task,0,0,10000,10000,1,1,0,tmp);
     tmp = OSCreateEventDescriptor();
     OSCreateSynchronousTask(ClearLed2Task,0,2000,0,tmp,NULL);
     OSCreateTask(SetLed2Task,0,0,20000,20000,1,1,0,tmp);
  #endif

  /* Start the OS so that it starts scheduling the user tasks */
  return OSStartMultitasking();
} /* end of main */

/* SetLed1Task: Sets a LED and triggers its clear after 100 clock ticks. */
void SetLed1Task(void *argument)
{
  SetFlag(1);
  OSScheduleTimerEvent(argument,100,OS_IO_PORT1_6);
  OSEndTask();
} /* end of SetLed1Task */


/* ClearLed1Task: Clears the LED toggled by SetLed1Task(). */
void ClearLed1Task(void *argument)
{
  ClearFlag(1);
  OSSuspendSynchronousTask();
} /* end of ClearLed1Task */


/* SetLed2Task: Sets a LED and triggers its clear after a variable delay comprised between
** 10 and 900 clock ticks. */
void SetLed2Task(void *argument)
{
  static UINT16 delay = 10;
  SetFlag(2);
  OSScheduleTimerEvent(argument,delay,OS_IO_PORT1_6);
  delay += 1;
  if (delay > 900)
     delay = 10;
  OSEndTask();
} /* end of SetLed2Task */


/* ClearLed2Task: Clears the LED toggled by SetLed2Task(). */
void ClearLed2Task(void *argument)
{
  ClearFlag(2);
  P1IFG |= 0x40;
  OSSuspendSynchronousTask();
} /* end of ClearLed2Task */
