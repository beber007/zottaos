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

#include "stm32f4xx_dbgmcu.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "misc.h"


#define FLAG_PORT GPIOB
#define FLAG1_PIN GPIO_Pin_13
#define FLAG2_PIN GPIO_Pin_14

#define EVENT_TIMER_INDEX OS_IO_TIM1_CC

static void SetLed1Task(void *argument);
static void SetLed2Task(void *argument);
static void ClearLed1Task(void *argument);
static void ClearLed2Task(void *argument);
static void InitializeFlags(UINT16 GPIO_Pin);


int main(void)
{
  void *tmp;
  #if ZOTTAOS_TIMER == OS_IO_TIM14
     DBGMCU_Config(DBGMCU_TIM14_STOP,ENABLE);
     DBGMCU_APB1PeriphConfig(DBGMCU_TIM14_STOP,ENABLE);
  #elif ZOTTAOS_TIMER == OS_IO_TIM13
     DBGMCU_Config(DBGMCU_TIM13_STOP,ENABLE);
     DBGMCU_APB1PeriphConfig(DBGMCU_TIM13_STOP,ENABLE);
  #elif ZOTTAOS_TIMER == OS_IO_TIM12
     DBGMCU_Config(DBGMCU_TIM12_STOP,ENABLE);
     DBGMCU_APB1PeriphConfig(DBGMCU_TIM12_STOP,ENABLE);
  #elif ZOTTAOS_TIMER == OS_IO_TIM11
     DBGMCU_Config(DBGMCU_TIM11_STOP,ENABLE);
     DBGMCU_APB2PeriphConfig(DBGMCU_TIM11_STOP,ENABLE);
  #elif ZOTTAOS_TIMER == OS_IO_TIM10
     DBGMCU_Config(DBGMCU_TIM10_STOP,ENABLE);
     DBGMCU_APB2PeriphConfig(DBGMCU_TIM10_STOP,ENABLE);
  #elif ZOTTAOS_TIMER == OS_IO_TIM9
     DBGMCU_Config(DBGMCU_TIM9_STOP,ENABLE);
     DBGMCU_APB2PeriphConfig(DBGMCU_TIM9_STOP,ENABLE);
  #elif ZOTTAOS_TIMER == OS_IO_TIM8
     DBGMCU_Config(DBGMCU_TIM8_STOP,ENABLE);
     DBGMCU_APB2PeriphConfig(DBGMCU_TIM8_STOP,ENABLE);
  #elif ZOTTAOS_TIMER == OS_IO_TIM5
     DBGMCU_Config(DBGMCU_TIM5_STOP,ENABLE);
     DBGMCU_APB1PeriphConfig(DBGMCU_TIM5_STOP,ENABLE);
  #elif ZOTTAOS_TIMER == OS_IO_TIM4
     DBGMCU_Config(DBGMCU_TIM4_STOP,ENABLE);
     DBGMCU_APB1PeriphConfig(DBGMCU_TIM4_STOP,ENABLE);
  #elif ZOTTAOS_TIMER == OS_IO_TIM3
     DBGMCU_Config(DBGMCU_TIM3_STOP,ENABLE);
     DBGMCU_APB1PeriphConfig(DBGMCU_TIM3_STOP,ENABLE);
  #elif ZOTTAOS_TIMER == OS_IO_TIM2
     DBGMCU_Config(DBGMCU_TIM2_STOP,ENABLE);
     DBGMCU_APB1PeriphConfig(DBGMCU_TIM2_STOP,ENABLE);
  #elif ZOTTAOS_TIMER == OS_IO_TIM1
     DBGMCU_Config(DBGMCU_TIM1_STOP,ENABLE);
     DBGMCU_APB2PeriphConfig(DBGMCU_TIM1_STOP,ENABLE);
  #endif
  /* Keep debugger connection during sleep mode */
  DBGMCU_Config(DBGMCU_SLEEP,ENABLE);


  DBGMCU_Config(DBGMCU_TIM1_STOP,ENABLE);
  DBGMCU_APB2PeriphConfig(DBGMCU_TIM1_STOP,ENABLE);

/* Initialize Hardware */
  SystemInit();
  InitializeFlags(FLAG1_PIN | FLAG2_PIN);

  OSInitTimerEvent(2,83,1,0,EVENT_TIMER_INDEX);

  #if defined(ZOTTAOS_VERSION_HARD)
     tmp = OSCreateEventDescriptor();
     OSCreateSynchronousTask(ClearLed1Task,100,tmp,NULL);
     OSCreateTask(SetLed1Task,0,200,200,tmp);

     tmp = OSCreateEventDescriptor();
     OSCreateSynchronousTask(ClearLed2Task,200,tmp,NULL);
     OSCreateTask(SetLed2Task,0,400,400,tmp);
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


/* SetLed1Task: . */
void SetLed1Task(void *argument)
{
  GPIO_SetBits(FLAG_PORT,FLAG1_PIN);
  OSScheduleTimerEvent(argument,40,EVENT_TIMER_INDEX);
  OSEndTask();
} /* end of SetLed1Task */


/* SetLed2Task: . */
void SetLed2Task(void *argument)
{
  GPIO_SetBits(FLAG_PORT,FLAG2_PIN);
  OSScheduleTimerEvent(argument,80,EVENT_TIMER_INDEX);
  OSEndTask();
} /* end of SetLed2Task */


/* ClearLed1Task: . */
void ClearLed1Task(void *argument)
{
  GPIO_ResetBits(FLAG_PORT,FLAG1_PIN);
  OSSuspendSynchronousTask();
} /* end of ClearLed1Task */


/* ClearLed2Task: . */
void ClearLed2Task(void *argument)
{
  GPIO_ResetBits(FLAG_PORT,FLAG2_PIN);
  OSSuspendSynchronousTask();
} /* end of ClearLed2Task */


/* InitializeFlags: Initialize input/output pin for flags.*/
void InitializeFlags(UINT16 GPIO_Pin)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  /* Enable GPIO_LED clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  /* Configure GPIO_LED Pin as Output push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
} /* end of InitializeFlags */
