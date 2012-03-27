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
/* File TestTimerEvent.c:
** Version date: March 2012
** Authors: MIS-TIC */

#include "ZottaOS.h"
#include "ZottaOS_TimerEvent.h"

static void InitializeTimer(void);

#define LED_PORT GPIOB
#define LED1_PIN GPIO_Pin_13
#define LED2_PIN GPIO_Pin_14

static void SetLed1Task(void *argument);
static void SetLed2Task(void *argument);
static void ClearLed1Task(void *argument);
static void ClearLed2Task(void *argument);


int main(void)
{
  void *tmp;
  /* Stop timer during debugger connection */
  DBGMCU_Config(DBGMCU_TIM4_STOP,ENABLE);
  DBGMCU_Config(DBGMCU_TIM3_STOP,ENABLE);
  /* Keep debugger connection during sleep mode */
  DBGMCU_Config(DBGMCU_SLEEP,ENABLE);

  OSInitializeSystemClocks();

  OSInitTimerEvent(2,OS_IO_TIM3);

  InitializeTimer();

  #if defined(ZOTTAOS_VERSION_HARD)
     tmp = OSCreateEventDescriptor();
     OSCreateSynchronousTask((void (*)(void *))ClearLed1Task,1000,tmp,NULL);
     OSCreateTask((void (*)(void *))SetLed1Task,0,10000,10000,tmp);

     tmp = OSCreateEventDescriptor();
     OSCreateSynchronousTask((void (*)(void *))ClearLed2Task,2000,tmp,NULL);
     OSCreateTask((void (*)(void *))SetLed2Task,0,20000,20000,tmp);
  #elif defined(ZOTTAOS_VERSION_SOFT)
     tmp = OSCreateEventDescriptor();
     OSCreateSynchronousTask((void (*)(void *))ClearLed1Task,0,1000,0,tmp,NULL);
     OSCreateTask((void (*)(void *))SetLed1Task,0,0,10000,10000,1,1,0,tmp);

     tmp = OSCreateEventDescriptor();
     OSCreateSynchronousTask((void (*)(void *))ClearLed2Task,0,2000,0,tmp,NULL);
     OSCreateTask((void (*)(void *))SetLed2Task,0,0,20000,20000,1,1,0,tmp);
  #endif

  /* Start the OS so that it starts scheduling the user tasks */
  return OSStartMultitasking();
} /* end of main */


/* SetLED: . */
void InitializeTimer(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  /* Enable GPIO_LED clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
 /* Configure GPIO_LED Pin as Output push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_OType_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /* Enable clock for timer */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
  /* Configure timer TIM2 */
  TIM_TimeBaseStructure.TIM_Period = 65535;
  TIM_TimeBaseStructure.TIM_Prescaler = 71;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);
  /* Interrupts configuration */
  TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
  TIM_ClearITPendingBit(TIM3,TIM_IT_CC1);
  TIM_ITConfig(TIM3,TIM_IT_CC1,ENABLE);
  TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);

  /* Enable timer Interrupts */
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  TIM_Cmd(TIM3,ENABLE);   // TIM enable counter
} /* end of SetLED */


/* SetLed1Task: . */
void SetLed1Task(void *argument)
{
  GPIO_SetBits(LED_PORT,LED1_PIN);
  OSScheduleTimerEvent(argument,1000,OS_IO_TIM3);
  OSEndTask();
} /* end of SetLed1Task */


/* SetLed2Task: . */
void SetLed2Task(void *argument)
{
  GPIO_SetBits(LED_PORT,LED2_PIN);
  OSScheduleTimerEvent(argument,2000,OS_IO_TIM3);
  OSEndTask();
} /* end of SetLed2Task */


/* ClearLed1Task: . */
void ClearLed1Task(void *argument)
{
  GPIO_ResetBits(LED_PORT,LED1_PIN);
  OSSuspendSynchronousTask();
} /* end of ClearLed1Task */


/* ClearLed2Task: . */
void ClearLed2Task(void *argument)
{
  GPIO_ResetBits(LED_PORT,LED2_PIN);
  OSSuspendSynchronousTask();
} /* end of ClearLed2Task */
