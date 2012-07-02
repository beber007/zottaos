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
/* File ZottaOS_Interrupts.c:
** Platform version: All STM32 microcontrollers.
** Version date: March 2012
** Authors: MIS-TIC
*/

#include "ZottaOS.h"
#include "ZottaOS_Timer.h"

/* Table of devices used by the interrupt handler and the peripheral devices to obtain
** the appropriate I/O routine. */

/* Chaque entrées est initialisées NULL ou pointe sur un structure de type TIMERSELECT dans le
** cas ou l'entrée correspond à un vecteur ayant comme source plusieurs timer. TIMERSELECT
** est définit dans ZottaOS_Timer.h */

#if defined(STM32F050XX)
   void *_OSTabDevice[28] = {
      NULL, /* OS_IO_WWDG           0  Window WatchDog */
      NULL, /* OS_IO_PVD            1  PVD through EXTI Line detection */
      NULL, /* OS_IO_RTC            2  RTC through EXTI Line */
      NULL, /* OS_IO_FLASH          3  FLASH */
      NULL, /* OS_IO_RCC            4  RCC */
      NULL, /* OS_IO_EXTI0_1        5  EXTI Line 0 and 1 */
      NULL, /* OS_IO_EXTI2_3        6  EXTI Line 2 and 3 */
      NULL, /* OS_IO_EXTI4_15       7  EXTI Line 4 to 15 */
      NULL, /* OS_IO_TS             8  TS */
      NULL, /* OS_IO_DMA1_Channel1  9  DMA1 Channel 1 */
      NULL, /* OS_IO_DMA1_Channel2_3 10  DMA1 Channel 2 and Channel 3 */
      NULL, /* DMA1_Channel4_5     11  DMA1 Channel 4 and Channel 5 */
      NULL, /* ADC1_COMP           12  ADC1, COMP1 and COMP2 */
      NULL, /* TIM1_BRK_UP_TRG_COM 13  TIM1 Break, Update, Trigger and Commutation */
      NULL, /* TIM1_CC             14  TIM1 Capture Compare */
      NULL, /* TIM2                15  TIM2 */
      NULL, /* TIM3                16  TIM3 */
      NULL,
      NULL,
      NULL, /* OS_IO_TIM14         19  TIM14 */
      NULL,
      NULL, /* OS_IO_TIM16         21  TIM16 */
      NULL, /* OS_IO_TIM17         22  TIM17 */
      NULL, /* OS_IO_I2C1          23  I2C1 */
      NULL,
      NULL, /* OS_IO_SPI1          25  SPI1 */
      NULL,
      NULL  /* OS_IO_USART1        27  USART1 */
   };
#elif defined(STM32F051X4) || defined(STM32F051K6_K8_C6_R6) || defined(STM32F051C8_R8)
   void *_OSTabDevice[31] = {
      NULL, /* OS_IO_WWDG           0  Window WatchDog */
      NULL, /* OS_IO_PVD            1  PVD through EXTI Line detection */
      NULL, /* OS_IO_RTC            2  RTC through EXTI Line */
      NULL, /* OS_IO_FLASH          3  FLASH */
      NULL, /* OS_IO_RCC            4  RCC */
      NULL, /* OS_IO_EXTI0_1        5  EXTI Line 0 and 1 */
      NULL, /* OS_IO_EXTI2_3        6  EXTI Line 2 and 3 */
      NULL, /* OS_IO_EXTI4_15       7  EXTI Line 4 to 15 */
      NULL, /* OS_IO_TS             8  TS */
      NULL, /* OS_IO_DMA1_Channel1  9  DMA1 Channel 1 */
      NULL, /* OS_IO_DMA1_Channel2_3 10  DMA1 Channel 2 and Channel 3 */
      NULL, /* DMA1_Channel4_5     11  DMA1 Channel 4 and Channel 5 */
      NULL, /* ADC1_COMP           12  ADC1, COMP1 and COMP2 */
      NULL, /* TIM1_BRK_UP_TRG_COM 13  TIM1 Break, Update, Trigger and Commutation */
      NULL, /* TIM1_CC             14  TIM1 Capture Compare */
      NULL, /* TIM2                15  TIM2 */
      NULL, /* TIM3                16  TIM3 */
      NULL, /* TIM6_DAC            17  TIM6 and DAC */
      NULL,
      NULL, /* OS_IO_TIM14         19  TIM14 */
      NULL, /* OS_IO_TIM15         20  TIM15 */
      NULL, /* OS_IO_TIM16         21  TIM16 */
      NULL, /* OS_IO_TIM17         22  TIM17 */
      NULL, /* OS_IO_I2C1          23  I2C1 */
      #if defined(STM32F051C8_R8)
         NULL, /* OS_IO_I2C2       24  I2C2 */
      #else
         NULL,
      #endif
      NULL, /* OS_IO_SPI1          25  SPI1 */
      #if defined(STM32F051C8_R8)
         NULL, /* OS_IO_SPI2       26  SPI2 */
      #else
         NULL,
      #endif
      NULL, /* OS_IO_USART1        27  USART1 */
      #if defined(STM32F051K6_K8_C6_R6) || defined(STM32F051C8_R8)
         NULL, /* OS_IO_USART2     28  USART2 */
      #else
         NULL,
      #endif
      NULL,
      NULL  /* OS_IO_CEC           30  CEC */
   };
#elif defined(STM32L151X6_X8_XB) || defined(STM32L152X6_X8_XB)
   void *_OSTabDevice[45] = {
      NULL, /* OS_IO_WWDG           0  Window WatchDog */
      NULL, /* OS_IO_PVD            1  PVD through EXTI Line detection */
      NULL, /* OS_IO_TAMP_STAMP     2  Tamper and TimeStamp through the EXTI line*/
      NULL, /* OS_IO_RTC_WKUP       3  RTC Wakeup through the EXTI line */
      NULL, /* OS_IO_FLASH          4  FLASH global */
      NULL, /* OS_IO_RCC            5  RCC global */
      NULL, /* OS_IO_EXTI0          6  EXTI Line0 */
      NULL, /* OS_IO_EXTI1          7  EXTI Line1 */
      NULL, /* OS_IO_EXTI2          8  EXTI Line2 */
      NULL, /* OS_IO_EXTI3          9  EXTI Line3 */
      NULL, /* OS_IO_EXTI4         10  EXTI Line4 */
      NULL, /* OS_IO_DMA1_Channel1 11  DMA1 Channel 1 global */
      NULL, /* OS_IO_DMA1_Channel2 12  DMA1 Channel 2 global */
      NULL, /* OS_IO_DMA1_Channel3 13  DMA1 Channel 3 global */
      NULL, /* OS_IO_DMA1_Channel4 14  DMA1 Channel 4 global */
      NULL, /* OS_IO_DMA1_Channel5 15  DMA1 Channel 5 global */
      NULL, /* OS_IO_DMA1_Channel6 16  DMA1 Channel 6 global */
      NULL, /* OS_IO_DMA1_Channel7 17  DMA1 Channel 7 global */
      NULL, /* OS_IO_ADC1          18  ADC1 global */
      NULL, /* OS_IO_USB_HP        19  USB High Priority */
      NULL, /* OS_IO_USB_LP        20  USB Low Priority */
      NULL, /* OS_IO_DAC           21  DAC */
      NULL, /* OS_IO_COMP          22  Comparator through EXTI Line  */
      NULL, /* OS_IO_EXTI9_5       23  External Line[9:5] */
      #if defined(STM32L152X6_X8_XB)
         NULL, /* OS_IO_LCD        24  LCD */
      #else
         NULL,
      #endif
      NULL, /* OS_IO_TIM9          25  TIM9 global */
      NULL, /* OS_IO_TIM10         26  TIM10 global */
      NULL, /* OS_IO_TIM11         27  TIM11 global */
      NULL, /* OS_IO_TIM2          28  TIM2 global */
      NULL, /* OS_IO_TIM3          29  TIM3 global */
      NULL, /* OS_IO_TIM4          30  TIM4 global */
      NULL, /* OS_IO_I2C1_EV       31  I2C1 Event */
      NULL, /* OS_IO_I2C1_ER       32  I2C1 Error */
      NULL, /* OS_IO_I2C2_EV       33  I2C2 Event */
      NULL, /* OS_IO_I2C2_ER       34  I2C2 Error */
      NULL, /* OS_IO_SPI1          35  SPI1 global */
      NULL, /* OS_IO_SPI2          36  SPI2 global */
      NULL, /* OS_IO_USART1        37  USART1 global */
      NULL, /* OS_IO_USART2        38  USART2 global */
      NULL, /* OS_IO_USART3        39  USART3 global */
      NULL, /* OS_IO_EXTI15_10     40  External Line[15:10]  */
      NULL, /* OS_IO_RTC_Alarm     41  RTC Alarm through EXTI Line */
      NULL, /* OS_IO_USB_FS_WKUP   42  USB FS WakeUp from suspend through EXTI Line */
      NULL, /* OS_IO_TIM6          43  TIM6 global */
      NULL  /* OS_IO_TIM7          44  TIM7 global */
   };
#elif defined(STM32L151XC) || defined(STM32L152XC) || defined(STM32L151XD) || \
      defined(STM32L152XD) || defined(STM32L162XD)
   void *_OSTabDevice[57] = {
      NULL, /* OS_IO_WWDG           0  Window WatchDog */
      NULL, /* OS_IO_PVD            1  PVD through EXTI Line detection */
      NULL, /* OS_IO_TAMP_STAMP     2  Tamper and TimeStamp through the EXTI line*/
      NULL, /* OS_IO_RTC_WKUP       3  RTC Wakeup through the EXTI line */
      NULL, /* OS_IO_FLASH          4  FLASH global */
      NULL, /* OS_IO_RCC            5  RCC global */
      NULL, /* OS_IO_EXTI0          6  EXTI Line0 */
      NULL, /* OS_IO_EXTI1          7  EXTI Line1 */
      NULL, /* OS_IO_EXTI2          8  EXTI Line2 */
      NULL, /* OS_IO_EXTI3          9  EXTI Line3 */
      NULL, /* OS_IO_EXTI4         10  EXTI Line4 */
      NULL, /* OS_IO_DMA1_Channel1 11  DMA1 Channel 1 global */
      NULL, /* OS_IO_DMA1_Channel2 12  DMA1 Channel 2 global */
      NULL, /* OS_IO_DMA1_Channel3 13  DMA1 Channel 3 global */
      NULL, /* OS_IO_DMA1_Channel4 14  DMA1 Channel 4 global */
      NULL, /* OS_IO_DMA1_Channel5 15  DMA1 Channel 5 global */
      NULL, /* OS_IO_DMA1_Channel6 16  DMA1 Channel 6 global */
      NULL, /* OS_IO_DMA1_Channel7 17  DMA1 Channel 7 global */
      NULL, /* OS_IO_ADC1          18  ADC1 global */
      NULL, /* OS_IO_USB_HP        19  USB High Priority */
      NULL, /* OS_IO_USB_LP        20  USB Low Priority */
      NULL, /* OS_IO_DAC           21  DAC */
      NULL, /* OS_IO_COMP_CA       22  Comparator and Channel acquisition through EXTI Line */
      NULL, /* OS_IO_EXTI9_5       23  External Line[9:5] */
      #if defined(STM32L152XC) || defined(STM32L152XD) || defined(STM32L162XD)
         NULL, /* OS_IO_LCD        24  LCD */
      #else
         NULL,
      #endif
      NULL, /* OS_IO_TIM9          25  TIM9 global */
      NULL, /* OS_IO_TIM10         26  TIM10 global */
      NULL, /* OS_IO_TIM11         27  TIM11 global */
      NULL, /* OS_IO_TIM2          28  TIM2 global */
      NULL, /* OS_IO_TIM3          29  TIM3 global */
      NULL, /* OS_IO_TIM4          30  TIM4 global */
      NULL, /* OS_IO_I2C1_EV       31  I2C1 Event */
      NULL, /* OS_IO_I2C1_ER       32  I2C1 Error */
      NULL, /* OS_IO_I2C2_EV       33  I2C2 Event */
      NULL, /* OS_IO_I2C2_ER       34  I2C2 Error */
      NULL, /* OS_IO_SPI1          35  SPI1 global */
      NULL, /* OS_IO_SPI2          36  SPI2 global */
      NULL, /* OS_IO_USART1        37  USART1 global */
      NULL, /* OS_IO_USART2        38  USART2 global */
      NULL, /* OS_IO_USART3        39  USART3 global */
      NULL, /* OS_IO_EXTI15_10     40  External Line[15:10]  */
      NULL, /* OS_IO_RTC_Alarm     41  RTC Alarm through EXTI Line */
      NULL, /* OS_IO_USB_FS_WKUP   42  USB FS WakeUp from suspend through EXTI Line */
      NULL, /* OS_IO_TIM6          43  TIM6 global */
      NULL, /* OS_IO_TIM7          44  TIM7 global */
      #if defined(STM32L151XD) || defined(STM32L152XD) || defined(STM32L162XD)
         NULL, /* OS_IO_SDIO       45  SDIO global */
      #else
         NULL,
      #endif
      NULL, /* OS_IO_TIM5          46  TIM5 global */
      NULL, /* OS_IO_SPI3          47  SPI3 global */
      #if defined(STM32L151XD) || defined(STM32L152XD) || defined(STM32L162XD)
         NULL, /* OS_IO_UART4      48  UART4 global */
         NULL, /* OS_IO_UART5      49  UART5 global */
      #else
         NULL,
         NULL,
      #endif
      NULL, /* OS_IO_DMA2_Channel1 50  DMA2 Channel 1 global */
      NULL, /* OS_IO_DMA2_Channel2 51  DMA2 Channel 2 global */
      NULL, /* OS_IO_DMA2_Channel3 52  DMA2 Channel 3 global */
      NULL, /* OS_IO_DMA2_Channel4 53  DMA2 Channel 4 global */
      NULL, /* OS_IO_DMA2_Channel5 54  DMA2 Channel 5 global */
      #if defined(STM32L162XD)
         NULL, /* OS_IO_AES        55  AES global */
      #else
         NULL,
      #endif
      NULL , /* OS_IO_COMP_ACQ 56  Comparator Channel Acquisition global */
   };
#elif defined(STM32F101T8_TB) || defined(STM32F101C8_CB_R8_RB_V8_VB) || \
      defined(STM32F101X4_X6)
   void *_OSTabDevice[42] = {
      NULL, /* OS_IO_WWDG           0 Window WatchDog */
      NULL, /* OS_IO_PVD            1 PVD through EXTI Line detection */
      NULL, /* OS_IO_TAMPER         2 Tamper */
      NULL, /* OS_IO_RTC            3 RTC global */
      NULL, /* OS_IO_FLASH          4 FLASH global */
      NULL, /* OS_IO_RCC            5 RCC global */
      NULL, /* OS_IO_EXTI0          6 EXTI Line0 */
      NULL, /* OS_IO_EXTI1          7 EXTI Line1 */
      NULL, /* OS_IO_EXTI2          8 EXTI Line2 */
      NULL, /* OS_IO_EXTI3          9 EXTI Line3 */
      NULL, /* OS_IO_EXTI4         10 EXTI Line4 */
      NULL, /* OS_IO_DMA1_Channel1 11 DMA1 Channel 1 global */
      NULL, /* OS_IO_DMA1_Channel2 12 DMA1 Channel 2 global */
      NULL, /* OS_IO_DMA1_Channel3 13 DMA1 Channel 3 global */
      NULL, /* OS_IO_DMA1_Channel4 14 DMA1 Channel 4 global */
      NULL, /* OS_IO_DMA1_Channel5 15 DMA1 Channel 5 global */
      NULL, /* OS_IO_DMA1_Channel6 16 DMA1 Channel 6 global */
      NULL, /* OS_IO_DMA1_Channel7 17 DMA1 Channel 7 global */
      NULL, /* OS_IO_ADC1          18 ADC1 global */
      NULL,
      NULL,
      NULL,
      NULL,
      NULL, /* OS_IO_EXTI9_5       23 External Line[9:5] */
      NULL,
      NULL,
      NULL,
      NULL,
      NULL, /* OS_IO_TIM2          28  TIM2 global */
      NULL, /* OS_IO_TIM3          29  TIM3 global */
      #if defined(STM32F101T8_TB) || defined(STM32F101C8_CB_R8_RB_V8_VB)
         NULL, /* OS_IO_TIM4       30 TIM4 global */
      #else
         NULL,
      #endif
      NULL, /* OS_IO_I2C1_EV       31 I2C1 Event */
      NULL, /* OS_IO_I2C1_ER       32 I2C1 Error */
      #if defined(STM32F101C8_CB_R8_RB_V8_VB)
         NULL, /* OS_IO_I2C2_EV    33 I2C2 Event */
         NULL, /* OS_IO_I2C2_ER    34 I2C2 Error */
      #else
         NULL,
         NULL,
      #endif
      NULL, /* OS_IO_SPI1          35 SPI1 global */
      #if defined(STM32F101C8_CB_R8_RB_V8_VB)
         NULL, /* OS_IO_SPI2       36 SPI2 global */
      #else
         NULL,
      #endif
      NULL, /* OS_IO_USART1        37 USART1 global */
      NULL, /* OS_IO_USART2        38 USART2 global */
      #if defined(STM32F101C8_CB_R8_RB_V8_VB)
         NULL, /* OS_IO_USART3     39 USART3 global */
      #else
         NULL,
      #endif
      NULL, /* OS_IO_EXTI15_10     40 External Line[15:10] */
      NULL  /* OS_IO_RTCAlarm      41 RTC Alarm through EXTI Line */
   };
#elif defined(STM32F103X4_X6) || defined(STM32F103T8_TB) || \
      defined(STM32F103C8_CB_R8_RB_V8_VB) || defined(STM32F102X4_X6) || \
      defined(STM32F102X8_XB)
   void *_OSTabDevice[43] = {
      NULL, /* OS_IO_WWDG             0 Window WatchDog */
      NULL, /* OS_IO_PVD              1 PVD through EXTI Line detection */
      NULL, /* OS_IO_TAMPER           2 Tamper */
      NULL, /* OS_IO_RTC              3 RTC global */
      NULL, /* OS_IO_FLASH            4 FLASH global */
      NULL, /* OS_IO_RCC              5 RCC global */
      NULL, /* OS_IO_EXTI0            6 EXTI Line0 */
      NULL, /* OS_IO_EXTI1            7 EXTI Line1 */
      NULL, /* OS_IO_EXTI2            8 EXTI Line2 */
      NULL, /* OS_IO_EXTI3            9 EXTI Line3 */
      NULL, /* OS_IO_EXTI4           10 EXTI Line4 */
      NULL, /* OS_IO_DMA1_Channel1   11 DMA1 Channel 1 global */
      NULL, /* OS_IO_DMA1_Channel2   12 DMA1 Channel 2 global */
      NULL, /* OS_IO_DMA1_Channel3   13 DMA1 Channel 3 global */
      NULL, /* OS_IO_DMA1_Channel4   14 DMA1 Channel 4 global */
      NULL, /* OS_IO_DMA1_Channel5   15 DMA1 Channel 5 global */
      NULL, /* OS_IO_DMA1_Channel6   16 DMA1 Channel 6 global */
      NULL, /* OS_IO_DMA1_Channel7   17 DMA1 Channel 7 global */
      NULL, /* OS_IO_ADC1            18 ADC1 global */
      NULL,
      #if defined(STM32F103X4_X6) || defined(STM32F103T8_TB) || \
          defined(STM32F103C8_CB_R8_RB_V8_VB)
         NULL, /* OS_IO_USB_HP_CAN1_TX 19 USB Device High Priority or CAN1 TX */
      #elif defined(STM32F102X4_X6) || defined(STM32F102X8_XB)
         NULL, /* OS_IO_USB_HP         19 USB Device High Priority */
      #endif
      #if defined(STM32F103X4_X6) || defined(STM32F103T8_TB) || \
         NULL, /* OS_IO_USB_LP_CAN1_RX0 20 USB Device Low Priority or CAN1 RX0 */
      #elif defined(STM32F102X4_X6) || defined(STM32F102X8_XB)
         NULL, /* OS_IO_USB_LP          20 USB Device Low Priority */
      #else
         NULL,
      #endif
      #if defined(STM32F103X4_X6) || defined(STM32F103T8_TB) || \
          defined(STM32F103C8_CB_R8_RB_V8_VB)
         NULL, /* OS_IO_CAN1_RX1     21 CAN1 RX1 */
      #else
         NULL,
      #endif
      #if defined(STM32F103X4_X6) || defined(STM32F103T8_TB) || \
          defined(STM32F103C8_CB_R8_RB_V8_VB)
         NULL, /* OS_IO_CAN1_SCE     22 CAN1 SCE */
      #else
         NULL,
      #endif
      NULL, /* OS_IO_EXTI9_5         23 External Line[9:5] */
      #if defined(STM32F103X4_X6) || defined(STM32F103T8_TB) || \
          defined(STM32F103C8_CB_R8_RB_V8_VB)
         NULL, /* OS_IO_TIM1_BRK     24 TIM1 Break */
         NULL, /* OS_IO_TIM1_UP      25 TIM1 Update */
         NULL, /* OS_IO_TIM1_TRG_COM 26 TIM1 Trigger and Commutation */
         NULL, /* OS_IO_TIM1_CC      27 TIM1 Capture Compare */
      #else
         NULL,
         NULL,
         NULL,
         NULL,
      #endif
      NULL, /* OS_IO_TIM2            28 TIM2 global */
      NULL, /* OS_IO_TIM3            29 TIM3 global */
      #if defined(STM32F102X8_XB) || defined(STM32F103T8_TB) || \
          defined(STM32F103C8_CB_R8_RB_V8_VB)
         NULL, /* OS_IO_TIM4         30 TIM4 global */
      #else
         NULL,
      #endif
      NULL, /* OS_IO_I2C1_EV         31 I2C1 Event */
      NULL, /* OS_IO_I2C1_ER         32 I2C1 Error */
      #if defined(STM32F102X8_XB) || defined(STM32F103C8_CB_R8_RB_V8_VB)
         NULL, /* OS_IO_I2C2_EV      33 I2C2 Event */
         NULL, /* OS_IO_I2C2_ER      34 I2C2 Error */
      #else
         NULL,
         NULL,
      #endif
      NULL, /* OS_IO_SPI1            35 SPI1 global */
      #if defined(STM32F102X8_XB) || defined(STM32F103C8_CB_R8_RB_V8_VB) || \
         NULL, /* OS_IO_SPI2         36 SPI2 global */
      #else
        NULL,
      #endif
      NULL, /* OS_IO_USART1          37 USART1 global */
      NULL, /* OS_IO_USART2          38 USART2 global */
      #if defined(STM32F102X8_XB) || defined(STM32F103C8_CB_R8_RB_V8_VB) || \
         NULL, /* OS_IO_USART3       39 USART3 global */
      #else
         NULL,
      #endif
      NULL, /* OS_IO_EXTI15_10       40 External Line[15:10] */
      NULL, /* OS_IO_RTCAlarm        41 RTC Alarm through EXTI Line */
      NULL  /* OS_IO_USBWakeUp       42 USB Device WakeUp from suspend through EXTI Line */
   };
#elif defined(STM32F100X4_X6) || defined(STM32F100X8_XB)
   static TIMERSELECT Timer15ISRSelect = {_OSTimerSelectorHandler,{NULL,NULL},{0x40012C00,0x40014000},0x80};
   static TIMERSELECT Timer16ISRSelect = {_OSTimerSelectorHandler,{NULL,NULL},{0x40012C00,0x40014400},0x01};
   static TIMERSELECT Timer17ISRSelect = {_OSTimerSelectorHandler,{NULL,NULL},{0x40012C00,0x40014800},0x40};
   void *_OSTabDevice[56] = {
      NULL, /* OS_IO_WWDG             0 Window WatchDog */
      NULL, /* OS_IO_PVD              1 PVD through EXTI Line detection */
      NULL, /* OS_IO_TAMPER           2 Tamper */
      NULL, /* OS_IO_RTC              3 RTC global */
      NULL, /* OS_IO_FLASH            4 FLASH global */
      NULL, /* OS_IO_RCC              5 RCC global */
      NULL, /* OS_IO_EXTI0            6 EXTI Line0 */
      NULL, /* OS_IO_EXTI1            7 EXTI Line1 */
      NULL, /* OS_IO_EXTI2            8 EXTI Line2 */
      NULL, /* OS_IO_EXTI3            9 EXTI Line3 */
      NULL, /* OS_IO_EXTI4           10 EXTI Line4 */
      NULL, /* OS_IO_DMA1_Channel1   11 DMA1 Channel 1 global */
      NULL, /* OS_IO_DMA1_Channel2   12 DMA1 Channel 2 global */
      NULL, /* OS_IO_DMA1_Channel3   13 DMA1 Channel 3 global */
      NULL, /* OS_IO_DMA1_Channel4   14 DMA1 Channel 4 global */
      NULL, /* OS_IO_DMA1_Channel5   15 DMA1 Channel 5 global */
      NULL, /* OS_IO_DMA1_Channel6   16 DMA1 Channel 6 global */
      NULL, /* OS_IO_DMA1_Channel7   17 DMA1 Channel 7 global */
      NULL, /* OS_IO_ADC1            18 ADC1 global */
      NULL,
      NULL,
      NULL,
      NULL,
      NULL, /* OS_IO_EXTI9_5         23 External Line[9:5] */
      Timer15ISRSelect, /* OS_IO_TIM1_BRK_TIM15     24 TIM1 Break and TIM15 */
      Timer16ISRSelect, /* OS_IO_TIM1_UP_TIM16      25 TIM1 Update and TIM16 */
      Timer17ISRSelect, /* OS_IO_TIM1_TRG_COM_TIM17 26 TIM1 Trigger and Commutation and TIM17 */
      NULL, /* OS_IO_TIM1_CC         27 TIM1 Capture Compare */
      NULL, /* OS_IO_TIM2            28  TIM2 global */
      NULL, /* OS_IO_TIM3            29  TIM3 global */
      #if defined (STM32F100X8_XB)
         NULL, /* OS_IO_TIM4         30 TIM4 global */
      #else
         NULL,
      #endif
      NULL, /* OS_IO_I2C1_EV         31 I2C1 Event */
      NULL, /* OS_IO_I2C1_ER         32 I2C1 Error */
      #if defined(STM32F100X8_XB)
         NULL, /* OS_IO_I2C2_EV      33 I2C2 Event */
         NULL, /* OS_IO_I2C2_ER      34 I2C2 Error */
      #else
         NULL,
         NULL,
      #endif
         NULL, /* OS_IO_SPI1         35 SPI1 global */
      #if defined(STM32F100X8_XB)
         NULL, /* OS_IO_SPI2         36 SPI2 global */
      #else
         NULL,
      #endif
      NULL, /* OS_IO_USART1          37 USART1 global */
      NULL, /* OS_IO_USART2          38 USART2 global */
      #if defined(STM32F100X8_XB)
          NULL, /* OS_IO_USART3      39 USART3 global */
      #else
          NULL,
      #endif
      NULL, /* OS_IO_EXTI15_10       40 External Line[15:10] */
      NULL, /* OS_IO_RTCAlarm        41 RTC Alarm through EXTI Line */
      NULL, /* OS_IO_CEC             42 HDMI-CEC */
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL, /* OS_IO_TIM6_DAC        54 TIM6 and DAC underrun */
      NULL, /* OS_IO_TIM7            55 TIM7 */
   };
#elif defined(STM32F101RC_RD_RE) || defined(STM32F101VC_VD_VE_ZC_ZD_ZE) || \
      defined(STM32F101RF_RG) || defined(STM32F101VF_VG_ZF_ZG) || \
      defined(STM32F103RC_RD_RE) || defined(STM32F103VC_VD_VE_ZC_ZD_ZE) || \
      defined(STM32F103RF_RG) || defined(STM32F103VF_VG_ZF_ZG)
   #if defined(STM32F103RF_RG) || defined(STM32F103VF_VG_ZF_ZG)
      static TIMERSELECT Timer9ISRSelect  = {_OSTimerSelectorHandler,{NULL,NULL},{0x40012C00,0x40014C00},0x80};
      static TIMERSELECT Timer10ISRSelect = {_OSTimerSelectorHandler,{NULL,NULL},{0x40012C00,0x40015000},0x01};
      static TIMERSELECT Timer11ISRSelect = {_OSTimerSelectorHandler,{NULL,NULL},{0x40012C00,0x40015400},0x40};
      static TIMERSELECT Timer12ISRSelect = {_OSTimerSelectorHandler,{NULL,NULL},{0x40013400,0x40001800},0x80};
      static TIMERSELECT Timer13ISRSelect = {_OSTimerSelectorHandler,{NULL,NULL},{0x40013400,0x40001C00},0x01};
      static TIMERSELECT Timer14ISRSelect = {_OSTimerSelectorHandler,{NULL,NULL},{0x40013400,0x40002000},0x40};
   #endif
   void *_OSTabDevice[60] = {
      NULL, /* OS_IO_WWDG             0 Window WatchDog */
      NULL, /* OS_IO_PVD              1 PVD through EXTI Line detection */
      NULL, /* OS_IO_TAMPER           2 Tamper */
      NULL, /* OS_IO_RTC              3 RTC global */
      NULL, /* OS_IO_FLASH            4 FLASH global */
      NULL, /* OS_IO_RCC              5 RCC global */
      NULL, /* OS_IO_EXTI0            6 EXTI Line0 */
      NULL, /* OS_IO_EXTI1            7 EXTI Line1 */
      NULL, /* OS_IO_EXTI2            8 EXTI Line2 */
      NULL, /* OS_IO_EXTI3            9 EXTI Line3 */
      NULL, /* OS_IO_EXTI4           10 EXTI Line4 */
      NULL, /* OS_IO_DMA1_Channel1   11 DMA1 Channel 1 global */
      NULL, /* OS_IO_DMA1_Channel2   12 DMA1 Channel 2 global */
      NULL, /* OS_IO_DMA1_Channel3   13 DMA1 Channel 3 global */
      NULL, /* OS_IO_DMA1_Channel4   14 DMA1 Channel 4 global */
      NULL, /* OS_IO_DMA1_Channel5   15 DMA1 Channel 5 global */
      NULL, /* OS_IO_DMA1_Channel6   16 DMA1 Channel 6 global */
      NULL, /* OS_IO_DMA1_Channel7   17 DMA1 Channel 7 global */
      NULL, /* OS_IO_ADC1            18 ADC1 global */
      NULL, /* OS_IO_USB_HP_CAN1_TX  19 USB Device High Priority or CAN1 TX */
      NULL, /* OS_IO_USB_LP_CAN1_RX0 20 USB Device Low Priority or CAN1 RX0 */
      NULL, /* OS_IO_CAN1_RX1        21 CAN1 RX1 */
      NULL, /* OS_IO_CAN1_SCE        22 CAN1 SCE */
      NULL, /* OS_IO_EXTI9_5         23 External Line[9:5] */
      #if defined(STM32F103RC_RD_RE) || defined(STM32F103VC_VD_VE_ZC_ZD_ZE)
         NULL, /* OS_IO_TIM1_BRK     24 TIM1 Break */
         NULL, /* OS_IO_TIM1_UP      25 TIM1 Update */
         NULL, /* OS_IO_TIM1_TRG_COM 26 TIM1 Trigger and Commutation */
         NULL, /* OS_IO_TIM1_CC      27 TIM1 Capture Compare */
      #elif defined (STM32F101RF_RG)|| defined(STM32F101VF_VG_ZF_ZG)
         NULL, /* OS_IO_TIM9         24 TIM9 global */
         NULL, /* OS_IO_TIM10        25 TIM10 global */
         NULL, /* OS_IO_TIM11        26 TIM11 */
         NULL,
      #elif defined(STM32F103RF_RG) || defined(STM32F103VF_VG_ZF_ZG)
         Timer9ISRSelect,  /* OS_IO_TIM1_BRK_TIM9      24 TIM1 Break  and TIM9 global */
         Timer10ISRSelect, /* OS_IO_TIM1_UP_TIM10      25 TIM1 Update and TIM10 global */
         Timer11ISRSelect, /* OS_IO_TIM1_TRG_COM_TIM11 26 TIM1 Trigger and Commutation  and TIM11 */
         NULL, /* OS_IO_TIM1_CC      27 TIM1 Capture Compare */
      #else
         NULL,
         NULL,
         NULL,
         NULL,
      #endif
      NULL, /* OS_IO_TIM2            28  TIM2 global */
      NULL, /* OS_IO_TIM3            29  TIM3 global */
      NULL, /* OS_IO_TIM4            30 TIM4 global */
      NULL, /* OS_IO_I2C1_EV         31 I2C1 Event */
      NULL, /* OS_IO_I2C1_ER         32 I2C1 Error */
      NULL, /* OS_IO_I2C2_EV         33 I2C2 Event */
      NULL, /* OS_IO_I2C2_ER         34 I2C2 Error */
      NULL, /* OS_IO_SPI1            35 SPI1 global */
      NULL, /* OS_IO_SPI2            36 SPI2 global */
      NULL, /* OS_IO_USART1          37 USART1 global */
      NULL, /* OS_IO_USART2          38 USART2 global */
      NULL, /* OS_IO_USART3          39 USART3 global */
      NULL, /* OS_IO_EXTI15_10       40 External Line[15:10] */
      NULL, /* OS_IO_RTCAlarm        41 RTC Alarm through EXTI Line */
      NULL, /* OS_IO_USBWakeUp       42 USB Device WakeUp from suspend through EXTI Line */
      #if defined(STM32F103RC_RD_RE) || defined(STM32F103VC_VD_VE_ZC_ZD_ZE)
         NULL, /* OS_IO_TIM8_BRK     43 TIM8 Break */
         NULL, /* OS_IO_TIM8_UP      44 TIM8 Update */
         NULL, /* OS_IO_TIM8_TRG_COM 45 TIM8 Trigger and Commutation */
         NULL, /* OS_IO_TIM8_CC      46 TIM8 Capture Compare */
      #elif defined(STM32F101RF_RG) || defined(STM32F101VF_VG_ZF_ZG)
         NULL, /* OS_IO_TIM12        43 TIM12 global */
         NULL, /* OS_IO_TIM13        44 TIM13 global */
         NULL, /* OS_IO_TIM14        45 TIM14 global */
         NULL,
      #elif defined(STM32F103RF_RG) || defined(STM32F103VF_VG_ZF_ZG)
         Timer12ISRSelect, /* OS_IO_TIM8_BRK_TIM12     43 TIM8 Break and TIM12 global */
         Timer13ISRSelect, /* OS_IO_TIM8_UP_TIM13      44 TIM8 Update and TIM13 global */
         Timer14ISRSelect, /* OS_IO_TIM8_TRG_COM_TIM14 45 TIM8 Trigger and Commutation  and TIM14 global */
         NULL, /* OS_IO_TIM8_CC      46 TIM8 Capture Compare */
      #else
         NULL,
         NULL,
         NULL,
         NULL,
      #endif
      NULL, /* OS_IO_ADC3            47 ADC3 global */
      NULL, /* OS_IO_FSMC            48 FSMC global */
      NULL, /* OS_IO_SDIO            49 SDIO global */
      NULL, /* OS_IO_TIM5            50 TIM5 global */
      NULL, /* OS_IO_SPI3            51 SPI3 global */
      NULL, /* OS_IO_UART4           52 UART4 global */
      NULL, /* OS_IO_UART5           53 UART5 global */
      NULL, /* OS_IO_TIM6            54 TIM6 global */
      NULL, /* OS_IO_TIM7            55 TIM7 */
      NULL, /* OS_IO_DMA2_Channel1   56 DMA2 Channel 1 global */
      NULL, /* OS_IO_DMA2_Channel2   57 DMA2 Channel 2 global */
      NULL, /* OS_IO_DMA2_Channel3   58 DMA2 Channel 3 global */
      NULL  /* OS_IO_DMA2_Channel4_5 59 DMA2 Channel 4 and Channel 5 global */
   };
#elif defined(STM32F100RC_RD_RE) || defined(STM32F100VC_VD_VE_ZC_ZD_ZE)
   static TIMERSELECT Timer15ISRSelect = {_OSTimerSelectorHandler,{NULL,NULL},{0x40012C00,0x40014000},0x80};
   static TIMERSELECT Timer16ISRSelect = {_OSTimerSelectorHandler,{NULL,NULL},{0x40012C00,0x40014400},0x01};
   static TIMERSELECT Timer17ISRSelect = {_OSTimerSelectorHandler,{NULL,NULL},{0x40012C00,0x40014800},0x40};
   void *_OSTabDevice[61] = {
      NULL, /* OS_IO_WWDG             0  Window WatchDog */
      NULL, /* OS_IO_PVD              1  PVD through EXTI Line detection */
      NULL, /* OS_IO_TAMPER           2  Tamper*/
      NULL, /* OS_IO_RTC              3  RTC global */
      NULL, /* OS_IO_FLASH            4  FLASH global */
      NULL, /* OS_IO_RCC              5  RCC global */
      NULL, /* OS_IO_EXTI0            6  EXTI Line0 */
      NULL, /* OS_IO_EXTI1            7  EXTI Line1 */
      NULL, /* OS_IO_EXTI2            8  EXTI Line2 */
      NULL, /* OS_IO_EXTI3            9  EXTI Line3 */
      NULL, /* OS_IO_EXTI4           10  EXTI Line4 */
      NULL, /* OS_IO_DMA1_Channel1   11  DMA1 Channel 1 global */
      NULL, /* OS_IO_DMA1_Channel2   12  DMA1 Channel 2 global */
      NULL, /* OS_IO_DMA1_Channel3   13  DMA1 Channel 3 global */
      NULL, /* OS_IO_DMA1_Channel4   14  DMA1 Channel 4 global */
      NULL, /* OS_IO_DMA1_Channel5   15  DMA1 Channel 5 global */
      NULL, /* OS_IO_DMA1_Channel6   16  DMA1 Channel 6 global */
      NULL, /* OS_IO_DMA1_Channel7   17  DMA1 Channel 7 global */
      NULL, /* OS_IO_ADC1            18 ADC1 global */
      NULL,
      NULL,
      NULL,
      NULL,
      NULL, /* OS_IO_EXTI9_5        23 External Line[9:5] */
      Timer15ISRSelect, /* OS_IO_TIM1_BRK_TIM15     24 TIM1 Break and TIM15 */
      Timer16ISRSelect, /* OS_IO_TIM1_UP_TIM16      25 TIM1 Update and TIM16 */
      Timer17ISRSelect, /* OS_IO_TIM1_TRG_COM_TIM17 26 TIM1 Trigger and Commutation and TIM17 */
      NULL, /* OS_IO_TIM1_CC         27 TIM1 Capture Compare */
      NULL, /* OS_IO_TIM2            28  TIM2 global */
      NULL, /* OS_IO_TIM3            29  TIM3 global */
      NULL, /* OS_IO_TIM4            30 TIM4 global */
      NULL, /* OS_IO_I2C1_EV         31 I2C1 Event */
      NULL, /* OS_IO_I2C1_ER         32 I2C1 Error */
      NULL, /* OS_IO_I2C2_EV         33 I2C2 Event */
      NULL, /* OS_IO_I2C2_ER         34 I2C2 Error */
      NULL, /* OS_IO_SPI1            35 SPI1 global */
      NULL, /* OS_IO_SPI2            36 SPI2 global */
      NULL, /* OS_IO_USART1          37 USART1 global */
      NULL, /* OS_IO_USART2          38 USART2 global */
      NULL, /* OS_IO_USART3          39 USART3 global */
      NULL, /* OS_IO_EXTI15_10       40 External Line[15:10] */
      NULL, /* OS_IO_RTCAlarm        41 RTC Alarm through EXTI Line */
      NULL, /* OS_IO_CEC             42 HDMI-CEC */
      NULL, /* OS_IO_TIM12           43 TIM12 global */
      NULL, /* OS_IO_TIM13           44 TIM13 global */
      NULL, /* OS_IO_TIM14           45 TIM14 global */
      NULL,
      NULL,
      NULL,
      NULL,
      NULL, /* OS_IO_TIM5            50 TIM5 global */
      NULL, /* OS_IO_SPI3            51 SPI3 global */
      NULL, /* OS_IO_UART4           52 UART4 global */
      NULL, /* OS_IO_UART5           53 UART5 global */
      NULL, /* OS_IO_TIM6_DAC        54 TIM6 and DAC underrun */
      NULL, /* OS_IO_TIM7            55 TIM7 */
      NULL, /* OS_IO_DMA2_Channel1   56 DMA2 Channel 1 global */
      NULL, /* OS_IO_DMA2_Channel2   57 DMA2 Channel 2 global */
      NULL, /* OS_IO_DMA2_Channel3   58 DMA2 Channel 3 global */
      NULL, /* OS_IO_DMA2_Channel4_5 59 DMA2 Channel 4 and Channel 5 global */
      NULL, /* OS_IO_DMA2_Channel5   60 DMA2 Channel 5 global  (DMA2 Channel 5
                                        is mapped at position 60 only if the
                                        MISC_REMAP bit in the AFIO_MAPR2
                                        register is set) */
   };
#elif defined(STM32F105XX) || defined(STM32F107XX)
   void *_OSTabDevice[68] = {
      NULL, /* OS_IO_WWDG           0  Window WatchDog */
      NULL, /* OS_IO_PVD            1  PVD through EXTI Line detection */
      NULL, /* OS_IO_TAMPER         2  Tamper*/
      NULL, /* OS_IO_RTC            3  RTC global */
      NULL, /* OS_IO_FLASH          4  FLASH global */
      NULL, /* OS_IO_RCC            5  RCC global */
      NULL, /* OS_IO_EXTI0          6  EXTI Line0 */
      NULL, /* OS_IO_EXTI1          7  EXTI Line1 */
      NULL, /* OS_IO_EXTI2          8  EXTI Line2 */
      NULL, /* OS_IO_EXTI3          9  EXTI Line3 */
      NULL, /* OS_IO_EXTI4         10  EXTI Line4 */
      NULL, /* OS_IO_DMA1_Channel1 11  DMA1 Channel 1 global */
      NULL, /* OS_IO_DMA1_Channel2 12  DMA1 Channel 2 global */
      NULL, /* OS_IO_DMA1_Channel3 13  DMA1 Channel 3 global */
      NULL, /* OS_IO_DMA1_Channel4 14  DMA1 Channel 4 global */
      NULL, /* OS_IO_DMA1_Channel5 15  DMA1 Channel 5 global */
      NULL, /* OS_IO_DMA1_Channel6 16  DMA1 Channel 6 global */
      NULL, /* OS_IO_DMA1_Channel7 17  DMA1 Channel 7 global */
      NULL, /* OS_IO_ADC1_2        18 ADC1 and ADC2 global */
      NULL, /* OS_IO_CAN1_TX       19 CAN1 TX */
      NULL, /* OS_IO_CAN1_RX0      20 CAN1 RX0 */
      NULL, /* OS_IO_CAN1_RX1      21 CAN1 RX1 */
      NULL, /* OS_IO_CAN1_SCE      22 CAN1 SCE */
      NULL, /* OS_IO_EXTI9_5       23 External Line[9:5] */
      NULL, /* OS_IO_TIM1_BRK      24 TIM1 Break */
      NULL, /* OS_IO_TIM1_UP       25 TIM1 Update */
      NULL, /* OS_IO_TIM1_TRG_COM  26 TIM1 Trigger and Commutation */
      NULL, /* OS_IO_TIM1_CC       27 TIM1 Capture Compare */
      NULL, /* OS_IO_TIM2          28  TIM2 global */
      NULL, /* OS_IO_TIM3          29  TIM3 global */
      NULL, /* OS_IO_TIM4          30 TIM4 global */
      NULL, /* OS_IO_I2C1_EV       31 I2C1 Event */
      NULL, /* OS_IO_I2C1_ER       32 I2C1 Error */
      NULL, /* OS_IO_I2C2_EV       33 I2C2 Event */
      NULL, /* OS_IO_I2C2_ER       34 I2C2 Error */
      NULL, /* OS_IO_SPI1          35 SPI1 global */
      NULL, /* OS_IO_SPI2          36 SPI2 global */
      NULL, /* OS_IO_USART1        37 USART1 global */
      NULL, /* OS_IO_USART2        38 USART2 global */
      NULL, /* OS_IO_USART3        39 USART3 global */
      NULL, /* OS_IO_EXTI15_10     40 External Line[15:10] */
      NULL, /* OS_IO_RTCAlarm      41 RTC Alarm through EXTI Line */
      NULL, /* OS_IO_OTG_FS_WKUP   42 USB OTG FS WakeUp from suspend through EXTI Line */
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL, /* OS_IO_TIM5          50 TIM5 global */
      NULL, /* OS_IO_SPI3          51 SPI3 global */
      NULL, /* OS_IO_UART4         52 UART4 global */
      NULL, /* OS_IO_UART5         53 UART5 global */
      NULL, /* OS_IO_TIM6          54 TIM6 global */
      NULL, /* OS_IO_TIM7          55 TIM7 */
      NULL, /* OS_IO_DMA2_Channel1 56 DMA2 Channel 1 global */
      NULL, /* OS_IO_DMA2_Channel2 57 DMA2 Channel 2 global */
      NULL, /* OS_IO_DMA2_Channel3 58 DMA2 Channel 3 global */
      NULL, /* OS_IO_DMA2_Channel4 59 DMA2 Channel 4 global */
      NULL, /* OS_IO_DMA2_Channel5 60 DMA2 Channel 5 global */
      NULL, /* OS_IO_ETH           61 Ethernet global */
      NULL, /* OS_IO_ETH_WKUP      62 Ethernet Wakeup through EXTI line */
      NULL, /* OS_IO_CAN2_TX       63 CAN2 TX */
      NULL, /* OS_IO_CAN2_RX0      64 CAN2 RX0 */
      NULL, /* OS_IO_CAN2_RX1      65 CAN2 RX1 */
      NULL, /* OS_IO_CAN2_SCE      66 CAN2 SCE */
      NULL  /* OS_IO_OTG_FS        67 USB OTG FS global */
   };
#elif defined(STM32F2XXXX)
   static TIMERSELECT Timer9ISRSelect  = {_OSTimerSelectorHandler,{NULL,NULL},{0x40010000,0x40014000},0x80};
   static TIMERSELECT Timer10ISRSelect = {_OSTimerSelectorHandler,{NULL,NULL},{0x40010000,0x40014400},0x01};
   static TIMERSELECT Timer11ISRSelect = {_OSTimerSelectorHandler,{NULL,NULL},{0x40010000,0x40014800},0x40};
   static TIMERSELECT Timer12ISRSelect = {_OSTimerSelectorHandler,{NULL,NULL},{0x40010400,0x40001800},0x80};
   static TIMERSELECT Timer13ISRSelect = {_OSTimerSelectorHandler,{NULL,NULL},{0x40010400,0x40001C00},0x01};
   static TIMERSELECT Timer14ISRSelect = {_OSTimerSelectorHandler,{NULL,NULL},{0x40010400,0x40002000},0x40};
   void *_OSTabDevice[81] = {
      NULL, /* OS_IO_WWDG             0  Window WatchDog */
      NULL, /* OS_IO_PVD              1  PVD through EXTI Line detection */
      NULL, /* OS_IO_TAMP_STAMP       2  Tamper and TimeStamp through the EXTI line*/
      NULL, /* OS_IO_RTC_WKUP         3  RTC Wakeup through the EXTI line */
      NULL, /* OS_IO_FLASH            4  FLASH global */
      NULL, /* OS_IO_RCC              5  RCC global */
      NULL, /* OS_IO_EXTI0            6  EXTI Line0 */
      NULL, /* OS_IO_EXTI1            7  EXTI Line1 */
      NULL, /* OS_IO_EXTI2            8  EXTI Line2 */
      NULL, /* OS_IO_EXTI3            9  EXTI Line3 */
      NULL, /* OS_IO_EXTI4           10  EXTI Line4 */
      NULL, /* OS_IO_DMA1_Stream0    11  DMA1 Stream 0 global */
      NULL, /* OS_IO_DMA1_Stream1    12  DMA1 Stream 1 global */
      NULL, /* OS_IO_DMA1_Stream2    13  DMA1 Stream 2 global */
      NULL, /* OS_IO_DMA1_Stream3    14  DMA1 Stream 3 global */
      NULL, /* OS_IO_DMA1_Stream4    15  DMA1 Stream 4 global */
      NULL, /* OS_IO_DMA1_Stream5    16  DMA1 Stream 5 global */
      NULL, /* OS_IO_DMA1_Stream6    17  DMA1 Stream 6 global */
      NULL,  /* OS_IO_ADC            18  ADC1, ADC2 and ADC3 global  */
      NULL,  /* OS_IO_CAN1_TX        19  CAN1 TX */
      NULL,  /* OS_IO_CAN1_RX0       20  CAN1 RX0 */
      NULL,  /* OS_IO_CAN1_RX1       21  CAN1 RX1 */
      NULL,  /* OS_IO_CAN1_SCE       22  CAN1 SCE */
      NULL,  /* OS_IO_EXTI9_5        23  External Line[9:5]  */
      &Timer9ISRSelect,  /* OS_IO_TIM1_BRK_TIM9      24  TIM1 Break and TIM9 global */
      &Timer10ISRSelect, /* OS_IO_TIM1_UP_TIM10      25  TIM1 Update and TIM10 global */
      &Timer11ISRSelect, /* OS_IO_TIM1_TRG_COM_TIM11 26  TIM1 Trigger and Commutation and TIM11 global */
      NULL,  /* OS_IO_TIM1_CC        27  TIM1 Capture Compare */
      NULL,  /* OS_IO_TIM2           28  TIM2 global */
      NULL,  /* OS_IO_TIM3           29  TIM3 global */
      NULL,  /* OS_IO_TIM4           30  TIM4 global */
      NULL,  /* OS_IO_I2C1_EV        31  I2C1 Event */
      NULL,  /* OS_IO_I2C1_ER        32  I2C1 Error */
      NULL,  /* OS_IO_I2C2_EV        33  I2C2 Event */
      NULL,  /* OS_IO_I2C2_ER        34  I2C2 Error */
      NULL,  /* OS_IO_SPI1           35  SPI1 global */
      NULL,  /* OS_IO_SPI2           36  SPI2 global */
      NULL,  /* OS_IO_USART1         37  USART1 global */
      NULL,  /* OS_IO_USART2         38  USART2 global */
      NULL,  /* OS_IO_USART3         39  USART3 global */
      NULL,  /* OS_IO_EXTI15_10      40  External Line[15:10]  */
      NULL,  /* OS_IO_RTC_Alarm      41  RTC Alarm (A and B) through EXTI Line */
      NULL,  /* OS_IO_OTG_FS_WKUP    42  USB OTG FS Wakeup through EXTI line */
      &Timer12ISRSelect, /* OS_IO_TIM8_BRK_TIM12     43  TIM8 Break and TIM12 global */
      &Timer13ISRSelect, /* OS_IO_TIM8_UP_TIM13      44  TIM8 Update and TIM13 global */
      &Timer14ISRSelect, /* OS_IO_TIM8_TRG_COM_TIM14 45  TIM8 Trigger and Commutation and TIM14 global */
      NULL,  /* OS_IO_TIM8_CC        46  TIM8 Capture Compare */
      NULL,  /* OS_IO_DMA1_Stream7   47  DMA1 Stream7 */
      NULL,  /* OS_IO_FSMC           48  FSMC global */
      NULL,  /* OS_IO_SDIO           49  SDIO global */
      NULL,  /* OS_IO_TIM5           50  TIM5 global */
      NULL,  /* OS_IO_SPI3           51  SPI3 global */
      NULL,  /* OS_IO_UART4          52  UART4 global */
      NULL,  /* OS_IO_UART5          53  UART5 global */
      NULL,  /* OS_IO_TIM6_DAC       54  TIM6 global and DAC1&2 underrun error */
      NULL,  /* OS_IO_TIM7           55  TIM7 global */
      NULL,  /* OS_IO_DMA2_Stream0   56  DMA2 Stream 0 global */
      NULL,  /* OS_IO_DMA2_Stream1   57  DMA2 Stream 1 global */
      NULL,  /* OS_IO_DMA2_Stream2   58  DMA2 Stream 2 global */
      NULL,  /* OS_IO_DMA2_Stream3   59  DMA2 Stream 3 global */
      NULL,  /* OS_IO_DMA2_Stream4   60  DMA2 Stream 4 global */
      NULL,  /* OS_IO_ETH            61  Ethernet global */
      NULL,  /* OS_IO_ETH_WKUP       62  Ethernet Wakeup through EXTI line */
      NULL,  /* OS_IO_CAN2_TX        63  CAN2 TX */
      NULL,  /* OS_IO_CAN2_RX0       64  CAN2 RX0 */
      NULL,  /* OS_IO_CAN2_RX1       65  CAN2 RX1 */
      NULL,  /* OS_IO_CAN2_SCE       66  CAN2 SCE */
      NULL,  /* OS_IO_OTG_FS         67  USB OTG FS global */
      NULL,  /* OS_IO_DMA2_Stream5   68  DMA2 Stream 5 global */
      NULL,  /* OS_IO_DMA2_Stream6   69  DMA2 Stream 6 global */
      NULL,  /* OS_IO_DMA2_Stream7   70  DMA2 Stream 7 global */
      NULL,  /* OS_IO_USART6         71  USART6 global */
      NULL,  /* OS_IO_I2C3_EV        72  I2C3 event */
      NULL,  /* OS_IO_I2C3_ER        73  I2C3 error */
      NULL,  /* OS_IO_OTG_HS_EP1_OUT 74  USB OTG HS End Point 1 Out global */
      NULL,  /* OS_IO_OTG_HS_EP1_IN  75  USB OTG HS End Point 1 In global */
      NULL,  /* OS_IO_OTG_HS_WKUP    76  USB OTG HS Wakeup through EXTI */
      NULL,  /* OS_IO_OTG_HS         77  USB OTG HS global */
      NULL,  /* OS_IO_DCMI           78  DCMI global */
      NULL,  /* OS_IO_CRYP           79  CRYP crypto global */
      NULL   /* OS_IO_HASH_RNG       80  Hash and Rng global */
   };
#elif defined(STM32F4XXXX)
   static TIMERSELECT Timer9ISRSelect  = {_OSTimerSelectorHandler,{NULL,NULL},{0x40010000,0x40014000},0x80};
   static TIMERSELECT Timer10ISRSelect = {_OSTimerSelectorHandler,{NULL,NULL},{0x40010000,0x40014400},0x01};
   static TIMERSELECT Timer11ISRSelect = {_OSTimerSelectorHandler,{NULL,NULL},{0x40010000,0x40014800},0x40};
   static TIMERSELECT Timer12ISRSelect = {_OSTimerSelectorHandler,{NULL,NULL},{0x40010400,0x40001800},0x80};
   static TIMERSELECT Timer13ISRSelect = {_OSTimerSelectorHandler,{NULL,NULL},{0x40010400,0x40001C00},0x01};
   static TIMERSELECT Timer14ISRSelect = {_OSTimerSelectorHandler,{NULL,NULL},{0x40010400,0x40002000},0x40};
   void *_OSTabDevice[82] = {
      NULL, /* OS_IO_WWDG             0  Window WatchDog */
      NULL, /* OS_IO_PVD              1  PVD through EXTI Line detection */
      NULL, /* OS_IO_TAMP_STAMP       2  Tamper and TimeStamp through the EXTI line*/
      NULL, /* OS_IO_RTC_WKUP         3  RTC Wakeup through the EXTI line */
      NULL, /* OS_IO_FLASH            4  FLASH global */
      NULL, /* OS_IO_RCC              5  RCC global */
      NULL, /* OS_IO_EXTI0            6  EXTI Line0 */
      NULL, /* OS_IO_EXTI1            7  EXTI Line1 */
      NULL, /* OS_IO_EXTI2            8  EXTI Line2 */
      NULL, /* OS_IO_EXTI3            9  EXTI Line3 */
      NULL, /* OS_IO_EXTI4           10  EXTI Line4 */
      NULL, /* OS_IO_DMA1_Stream0    11  DMA1 Stream 0 global */
      NULL, /* OS_IO_DMA1_Stream1    12  DMA1 Stream 1 global */
      NULL, /* OS_IO_DMA1_Stream2    13  DMA1 Stream 2 global */
      NULL, /* OS_IO_DMA1_Stream3    14  DMA1 Stream 3 global */
      NULL, /* OS_IO_DMA1_Stream4    15  DMA1 Stream 4 global */
      NULL, /* OS_IO_DMA1_Stream5    16  DMA1 Stream 5 global */
      NULL, /* OS_IO_DMA1_Stream6    17  DMA1 Stream 6 global */
      NULL,  /* OS_IO_ADC            18  ADC1, ADC2 and ADC3 global  */
      NULL,  /* OS_IO_CAN1_TX        19  CAN1 TX */
      NULL,  /* OS_IO_CAN1_RX0       20  CAN1 RX0 */
      NULL,  /* OS_IO_CAN1_RX1       21  CAN1 RX1 */
      NULL,  /* OS_IO_CAN1_SCE       22  CAN1 SCE */
      NULL,  /* OS_IO_EXTI9_5        23  External Line[9:5]  */
      &Timer9ISRSelect,  /* OS_IO_TIM1_BRK_TIM9      24  TIM1 Break and TIM9 global */
      &Timer10ISRSelect, /* OS_IO_TIM1_UP_TIM10      25  TIM1 Update and TIM10 global */
      &Timer11ISRSelect, /* OS_IO_TIM1_TRG_COM_TIM11 26  TIM1 Trigger and Commutation and TIM11 global */
      NULL,  /* OS_IO_TIM1_CC        27  TIM1 Capture Compare */
      NULL,  /* OS_IO_TIM2           28  TIM2 global */
      NULL,  /* OS_IO_TIM3           29  TIM3 global */
      NULL,  /* OS_IO_TIM4           30  TIM4 global */
      NULL,  /* OS_IO_I2C1_EV        31  I2C1 Event */
      NULL,  /* OS_IO_I2C1_ER        32  I2C1 Error */
      NULL,  /* OS_IO_I2C2_EV        33  I2C2 Event */
      NULL,  /* OS_IO_I2C2_ER        34  I2C2 Error */
      NULL,  /* OS_IO_SPI1           35  SPI1 global */
      NULL,  /* OS_IO_SPI2           36  SPI2 global */
      NULL,  /* OS_IO_USART1         37  USART1 global */
      NULL,  /* OS_IO_USART2         38  USART2 global */
      NULL,  /* OS_IO_USART3         39  USART3 global */
      NULL,  /* OS_IO_EXTI15_10      40  External Line[15:10]  */
      NULL,  /* OS_IO_RTC_Alarm      41  RTC Alarm (A and B) through EXTI Line */
      NULL,  /* OS_IO_OTG_FS_WKUP    42  USB OTG FS Wakeup through EXTI line */
      &Timer12ISRSelect, /* OS_IO_TIM8_BRK_TIM12     43  TIM8 Break and TIM12 global */
      &Timer13ISRSelect, /* OS_IO_TIM8_UP_TIM13      44  TIM8 Update and TIM13 global */
      &Timer14ISRSelect, /* OS_IO_TIM8_TRG_COM_TIM14 45  TIM8 Trigger and Commutation and TIM14 global */
      NULL,  /* OS_IO_TIM8_CC        46  TIM8 Capture Compare */
      NULL,  /* OS_IO_DMA1_Stream7   47  DMA1 Stream7 */
      NULL,  /* OS_IO_FSMC           48  FSMC global */
      NULL,  /* OS_IO_SDIO           49  SDIO global */
      NULL,  /* OS_IO_TIM5           50  TIM5 global */
      NULL,  /* OS_IO_SPI3           51  SPI3 global */
      NULL,  /* OS_IO_UART4          52  UART4 global */
      NULL,  /* OS_IO_UART5          53  UART5 global */
      NULL,  /* OS_IO_TIM6_DAC       54  TIM6 global and DAC1&2 underrun error */
      NULL,  /* OS_IO_TIM7           55  TIM7 global */
      NULL,  /* OS_IO_DMA2_Stream0   56  DMA2 Stream 0 global */
      NULL,  /* OS_IO_DMA2_Stream1   57  DMA2 Stream 1 global */
      NULL,  /* OS_IO_DMA2_Stream2   58  DMA2 Stream 2 global */
      NULL,  /* OS_IO_DMA2_Stream3   59  DMA2 Stream 3 global */
      NULL,  /* OS_IO_DMA2_Stream4   60  DMA2 Stream 4 global */
      NULL,  /* OS_IO_ETH            61  Ethernet global */
      NULL,  /* OS_IO_ETH_WKUP       62  Ethernet Wakeup through EXTI line */
      NULL,  /* OS_IO_CAN2_TX        63  CAN2 TX */
      NULL,  /* OS_IO_CAN2_RX0       64  CAN2 RX0 */
      NULL,  /* OS_IO_CAN2_RX1       65  CAN2 RX1 */
      NULL,  /* OS_IO_CAN2_SCE       66  CAN2 SCE */
      NULL,  /* OS_IO_OTG_FS         67  USB OTG FS global */
      NULL,  /* OS_IO_DMA2_Stream5   68  DMA2 Stream 5 global */
      NULL,  /* OS_IO_DMA2_Stream6   69  DMA2 Stream 6 global */
      NULL,  /* OS_IO_DMA2_Stream7   70  DMA2 Stream 7 global */
      NULL,  /* OS_IO_USART6         71  USART6 global */
      NULL,  /* OS_IO_I2C3_EV        72  I2C3 event */
      NULL,  /* OS_IO_I2C3_ER        73  I2C3 error */
      NULL,  /* OS_IO_OTG_HS_EP1_OUT 74  USB OTG HS End Point 1 Out global */
      NULL,  /* OS_IO_OTG_HS_EP1_IN  75  USB OTG HS End Point 1 In global */
      NULL,  /* OS_IO_OTG_HS_WKUP    76  USB OTG HS Wakeup through EXTI */
      NULL,  /* OS_IO_OTG_HS         77  USB OTG HS global */
      NULL,  /* OS_IO_DCMI           78  DCMI global */
      NULL,  /* OS_IO_CRYP           79  CRYP crypto global */
      NULL,  /* OS_IO_HASH_RNG       80  Hash and Rng global */
      NULL   /* OS_IO_FPU            81  FPU global */
   };
#else
  #error STM32 version undefined
#endif


#ifdef DEBUG_MODE
/* UndefinedInterrupt: Dummy ISR to fall into an infinite loop when referring to an in-
** existent entry of the interrupt vector table. */
static void UndefinedInterrupt(void)
{
  UINT32 intNum;
  _OSDisableInterrupts();
  intNum  = (*((UINT32 *)0xE000ED04) & 0x1FF);
  intNum  -= 16;
  while (TRUE); // referring to an inexistent entry of the interrupt vector table.
} /* end of undefinedInterrupt */
#endif


/* STM32 specific interrupt vector table, which is added by the linker at the end of table
** CortextM3VectorTable which is defined in ZottaOS_CortexM3.c. */
__attribute__ ((section(".isr_vector_specific")))
void (* const STM32VectorTable[])(void) =
{
  _OSIOHandler, /* OS_IO_WWDG                   0  Window WatchDog */
  _OSIOHandler, /* OS_IO_PVD                    1  PVD through EXTI Line detection */
#if defined(STM32F05XXX)
  _OSIOHandler, /* OS_IO_RTC                    2  RTC through EXTI Line */
  _OSIOHandler, /* OS_IO_FLASH                  3  FLASH */
#elif defined(STM32L1XXXX) || defined(STM32F2XXXX) || defined(STM32F4XXXX)
  _OSIOHandler, /* OS_IO_TAMP_STAMP             2  Tamper and TimeStamp through the EXTI
                                                   line*/
  _OSIOHandler, /* OS_IO_RTC_WKUP               3  RTC Wakeup through the EXTI line */
#elif defined(STM32F1XXXX)
  _OSIOHandler, /* OS_IO_TAMPER                 2  Tamper*/
  _OSIOHandler, /* OS_IO_RTC                    3  RTC global */
#else
   #error STM32 version undefined
#endif
#if defined(STM32F05XXX)
  _OSIOHandler, /* OS_IO_RCC                    4  RCC */
  _OSIOHandler, /* OS_IO_EXTI0_1                5  EXTI Line 0 and 1 */
  _OSIOHandler, /* OS_IO_EXTI2_3                6  EXTI Line 2 and 3 */
  _OSIOHandler, /* OS_IO_EXTI4_15               7  EXTI Line 4 to 15 */
  _OSIOHandler, /* OS_IO_TS                     8  TS */
  _OSIOHandler, /* OS_IO_DMA1_Channel1          9  DMA1 Channel 1 */
  _OSIOHandler, /* OS_IO_DMA1_Channel2_3       10  DMA1 Channel 2 and Channel 3 */
#elif defined(STM32L1XXXX) || defined(STM32F1XXXX) || defined(STM32F2XXXX) || defined(STM32F4XXXX)
  _OSIOHandler, /* OS_IO_FLASH                  4  FLASH global */
  _OSIOHandler, /* OS_IO_RCC                    5  RCC global */
  _OSIOHandler, /* OS_IO_EXTI0                  6  EXTI Line0 */
  _OSIOHandler, /* OS_IO_EXTI1                  7  EXTI Line1 */
  _OSIOHandler, /* OS_IO_EXTI2                  8  EXTI Line2 */
  _OSIOHandler, /* OS_IO_EXTI3                  9  EXTI Line3 */
  _OSIOHandler, /* OS_IO_EXTI4                 10  EXTI Line4 */
#else
   #error STM32 version undefined
#endif
#if defined(STM32F05XXX)
  _OSIOHandler, /* DMA1_Channel4_5             11  DMA1 Channel 4 and Channel 5 */
  _OSIOHandler, /* ADC1_COMP                   12  ADC1, COMP1 and COMP2 */
  _OSIOHandler, /* TIM1_BRK_UP_TRG_COM         13  TIM1 Break, Update, Trigger and Commutation */
  _OSIOHandler, /* TIM1_CC                     14  TIM1 Capture Compare */
  _OSIOHandler, /* TIM2                        15  TIM2 */
  _OSIOHandler, /* TIM3                        16  TIM3 */
  #if defined(STM32F051X4) || defined(STM32F051K6_K8_C6_R6) || defined(STM32F051C8_R8)
     _OSIOHandler, /* TIM6_DAC                 17  TIM6 and DAC */
  #elif defined(DEBUG_MODE)
     UndefinedInterrupt,
  #else
     NULL,
  #endif
#elif defined(STM32F2XXXX) || defined(STM32F4XXXX)
  _OSIOHandler, /* OS_IO_DMA1_Stream0          11  DMA1 Stream 0 global */
  _OSIOHandler, /* OS_IO_DMA1_Stream1          12  DMA1 Stream 1 global */
  _OSIOHandler, /* OS_IO_DMA1_Stream2          13  DMA1 Stream 2 global */
  _OSIOHandler, /* OS_IO_DMA1_Stream3          14  DMA1 Stream 3 global */
  _OSIOHandler, /* OS_IO_DMA1_Stream4          15  DMA1 Stream 4 global */
  _OSIOHandler, /* OS_IO_DMA1_Stream5          16  DMA1 Stream 5 global */
  _OSIOHandler, /* OS_IO_DMA1_Stream6          17  DMA1 Stream 6 global */
#elif defined(STM32L1XXXX) || defined(STM32F1XXXX)
  _OSIOHandler, /* OS_IO_DMA1_Channel1         11  DMA1 Channel 1 global */
  _OSIOHandler, /* OS_IO_DMA1_Channel2         12  DMA1 Channel 2 global */
  _OSIOHandler, /* OS_IO_DMA1_Channel3         13  DMA1 Channel 3 global */
  _OSIOHandler, /* OS_IO_DMA1_Channel4         14  DMA1 Channel 4 global */
  _OSIOHandler, /* OS_IO_DMA1_Channel5         15  DMA1 Channel 5 global */
  _OSIOHandler, /* OS_IO_DMA1_Channel6         16  DMA1 Channel 6 global */
  _OSIOHandler, /* OS_IO_DMA1_Channel7         17  DMA1 Channel 7 global */
#else
   #error STM32 version undefined
#endif
/* ------------------------------ STM32F0 --------------------------------------------- */
#if defined(STM32F05XXX)
  #if defined(DEBUG_MODE)
     UndefinedInterrupt,
  #else
     NULL,
  #endif
  _OSIOHandler, /* OS_IO_TIM14                 19 TIM14 */
  #if defined(STM32F051X4) || defined(STM32F051K6_K8_C6_R6) || defined(STM32F051C8_R8)
     _OSIOHandler, /* OS_IO_TIM15              20 TIM15 */
  #elif defined(DEBUG_MODE)
     UndefinedInterrupt,
  #else
     NULL,
  #endif
  _OSIOHandler, /* OS_IO_TIM16                 21 TIM16 */
  _OSIOHandler, /* OS_IO_TIM17                 22 TIM17 */
  _OSIOHandler, /* OS_IO_I2C1                  23 I2C1 */
  #if defined(STM32F051C8_R8)
     _OSIOHandler, /* OS_IO_I2C2               24 I2C2 */
  #elif defined(DEBUG_MODE)
     UndefinedInterrupt,
  #else
     NULL,
  #endif
  _OSIOHandler, /* OS_IO_SPI1                  25 SPI1 */
  #if defined(STM32F051C8_R8)
     _OSIOHandler, /* OS_IO_SPI2               26 SPI2 */
  #elif defined(DEBUG_MODE)
     UndefinedInterrupt,
  #else
     NULL,
  #endif
  _OSIOHandler, /* OS_IO_USART1                27 USART1 */
  #if defined(STM32F051K6_K8_C6_R6) || defined(STM32F051C8_R8)
     _OSIOHandler, /* OS_IO_USART2             28 USART2 */
  #elif defined(DEBUG_MODE)
     UndefinedInterrupt,
  #else
     NULL,
  #endif
  #if defined(DEBUG_MODE)
     UndefinedInterrupt,
  #else
     NULL,
  #endif
  #if defined(STM32F051X4) || defined(STM32F051K6_K8_C6_R6) || defined(STM32F051C8_R8)
     _OSIOHandler, /* OS_IO_CEC                30 CEC */
  #elif defined(DEBUG_MODE)
     UndefinedInterrupt,
  #else
     NULL,
  #endif
/* ------------------------------ STM32L1 --------------------------------------------- */
#elif defined(STM32L1XXXX)
  _OSIOHandler, /* OS_IO_ADC1                  18  ADC1 global */
  _OSIOHandler, /* OS_IO_USB_HP                19  USB High Priority */
  _OSIOHandler, /* OS_IO_USB_LP                20  USB Low Priority */
  _OSIOHandler, /* OS_IO_DAC                   21  DAC */
  #if defined(STM32L151XC) || defined(STM32L152XC) || defined(STM32L151XD) || \
      defined(STM32L152XD) || defined(STM32L162XD)
     _OSIOHandler, /* OS_IO_COMP_CA            22  Comparator and Channel acquisition
                                                   through EXTI Line */
  #else
     _OSIOHandler, /* OS_IO_COMP               22  Comparator through EXTI Line  */
  #endif
  _OSIOHandler, /* OS_IO_EXTI9_5               23  External Line[9:5] */
  #if defined(STM32L152X6_X8_XB) || defined(STM32L152XC) || defined(STM32L152XD) || \
      defined(STM32L162XD)
     _OSIOHandler, /* OS_IO_LCD                24  LCD */
  #elif defined(DEBUG_MODE)
     UndefinedInterrupt,
  #else
     NULL,
  #endif
  _OSIOHandler, /* OS_IO_TIM9                  25  TIM9 global */
  _OSIOHandler, /* OS_IO_TIM10                 26  TIM10 global */
  _OSIOHandler, /* OS_IO_TIM11                 27  TIM11 global */
  _OSIOHandler, /* OS_IO_TIM2                  28  TIM2 global */
  _OSIOHandler, /* OS_IO_TIM3                  29  TIM3 global */
  _OSIOHandler, /* OS_IO_TIM4                  30  TIM4 global */
  _OSIOHandler, /* OS_IO_I2C1_EV               31  I2C1 Event */
  _OSIOHandler, /* OS_IO_I2C1_ER               32  I2C1 Error */
  _OSIOHandler, /* OS_IO_I2C2_EV               33  I2C2 Event */
  _OSIOHandler, /* OS_IO_I2C2_ER               34  I2C2 Error */
  _OSIOHandler, /* OS_IO_SPI1                  35  SPI1 global */
  _OSIOHandler, /* OS_IO_SPI2                  36  SPI2 global */
  _OSIOHandler, /* OS_IO_USART1                37  USART1 global */
  _OSIOHandler, /* OS_IO_USART2                38  USART2 global */
  _OSIOHandler, /* OS_IO_USART3                39  USART3 global */
  _OSIOHandler, /* OS_IO_EXTI15_10             40  External Line[15:10]  */
  _OSIOHandler, /* OS_IO_RTC_Alarm             41  RTC Alarm through EXTI Line */
  _OSIOHandler, /* OS_IO_USB_FS_WKUP           42  USB FS WakeUp from suspend through EXTI
                                                   Line */
  _OSIOHandler, /* OS_IO_TIM6                  43  TIM6 global */
  _OSIOHandler, /* OS_IO_TIM7                  44  TIM7 global */
  #if defined(STM32L151XD) || defined(STM32L152XD) || defined(STM32L162XD)
     _OSIOHandler, /* OS_IO_SDIO               45  SDIO global */
  #elif defined(DEBUG_MODE)
     UndefinedInterrupt,
  #else
     NULL,
  #endif
  #if defined(STM32L151XC) || defined(STM32L152XC) || defined(STM32L151XD) || \
      defined(STM32L152XD) || defined(STM32L162XD)
     _OSIOHandler, /* OS_IO_TIM5               46  TIM5 global */
     _OSIOHandler, /* OS_IO_SPI3               47  SPI3 global */
  #elif defined(DEBUG_MODE)
     UndefinedInterrupt,
     UndefinedInterrupt,
  #else
     NULL,
     NULL,
  #endif
  #if defined(STM32L151XD) || defined(STM32L152XD) || defined(STM32L162XD)
     _OSIOHandler, /* OS_IO_UART4              48  UART4 global */
     _OSIOHandler, /* OS_IO_UART5              49  UART5 global */
  #elif defined(DEBUG_MODE)
     UndefinedInterrupt,
     UndefinedInterrupt,
  #else
     NULL,
     NULL,
  #endif
  #if defined(STM32L151XC) || defined(STM32L152XC) || defined(STM32L151XD) || \
      defined(STM32L152XD) || defined(STM32L162XD)
     _OSIOHandler, /* OS_IO_DMA2_Channel1      50  DMA2 Channel 1 global */
     _OSIOHandler, /* OS_IO_DMA2_Channel2      51  DMA2 Channel 2 global */
     _OSIOHandler, /* OS_IO_DMA2_Channel3      52  DMA2 Channel 3 global */
     _OSIOHandler, /* OS_IO_DMA2_Channel4      53  DMA2 Channel 4 global */
     _OSIOHandler, /* OS_IO_DMA2_Channel5      54  DMA2 Channel 5 global */
  #elif defined(DEBUG_MODE)
     UndefinedInterrupt,
     UndefinedInterrupt,
     UndefinedInterrupt,
     UndefinedInterrupt,
     UndefinedInterrupt,
  #else
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
  #endif
  #if defined(STM32L162XD)
     _OSIOHandler, /* OS_IO_AES                55  AES global */
  #elif defined(DEBUG_MODE)
     UndefinedInterrupt,
  #else
     NULL,
  #endif
  #if defined(STM32L151XC) || defined(STM32L152XC) || defined(STM32L151XD) || \
      defined(STM32L152XD) || defined(STM32L162XD)
     _OSIOHandler, /* OS_IO_COMP_ACQ           56  Comparator Channel Acquisition global */
  #elif defined(DEBUG_MODE)
     UndefinedInterrupt,
  #else
     NULL,
  #endif
/* ------------------------------ STM32F1 --------------------------------------------- */
#elif defined(STM32F1XXXX)
  #if defined(STM32F100X4_X6) || defined(STM32F100X8_XB) || \
      defined(STM32F100RC_RD_RE) || defined(STM32F100VC_VD_VE_ZC_ZD_ZE) || \
      defined(STM32F101X4_X6) || defined(STM32F101T8_TB) || \
      defined(STM32F101C8_CB_R8_RB_V8_VB) || defined(STM32F101RC_RD_RE) || \
      defined(STM32F101VC_VD_VE_ZC_ZD_ZE) || defined(STM32F102X4_X6) || \
      defined(STM32F102X8_XB)
     _OSIOHandler, /* OS_IO_ADC1               18 ADC1 global */
  #elif defined(STM32F101RF_RG) || defined(STM32F101VF_VG_ZF_ZG) || \
        defined(STM32F103X4_X6) || defined(STM32F103T8_TB) || \
        defined(STM32F103C8_CB_R8_RB_V8_VB) || defined(STM32F105XX) || \
        defined(STM32F107XX)
     _OSIOHandler, /* OS_IO_ADC1_2             18 ADC1 and ADC2 global */
  #elif defined(DEBUG_MODE)
     UndefinedInterrupt,
  #else
     NULL,
  #endif
  #if defined(STM32F103X4_X6) || defined(STM32F103T8_TB) || \
      defined(STM32F103C8_CB_R8_RB_V8_VB) || defined(STM32F103RC_RD_RE) || \
      defined(STM32F103VC_VD_VE_ZC_ZD_ZE) || defined(STM32F103RF_RG) || \
      defined(STM32F103VF_VG_ZF_ZG)
     _OSIOHandler, /* OS_IO_USB_HP_CAN1_TX     19 USB Device High Priority or CAN1 TX */
  #elif defined(STM32F102X4_X6) || defined(STM32F102X8_XB)
     _OSIOHandler, /* OS_IO_USB_HP             19 USB Device High Priority */
  #elif defined(STM32F105XX) || defined(STM32F107XX)
     _OSIOHandler, /* OS_IO_CAN1_TX            19 CAN1 TX */
  #elif defined(DEBUG_MODE)
     UndefinedInterrupt,
  #else
     NULL,
  #endif
  #if defined(STM32F103X4_X6) || defined(STM32F103T8_TB) || \
      defined(STM32F103C8_CB_R8_RB_V8_VB) || defined(STM32F103RC_RD_RE) || \
      defined(STM32F103VC_VD_VE_ZC_ZD_ZE) || defined(STM32F103RF_RG) || \
      defined(STM32F103VF_VG_ZF_ZG)
     _OSIOHandler, /* OS_IO_USB_LP_CAN1_RX0    20 USB Device Low Priority or CAN1 RX0 */
  #elif defined(STM32F102X4_X6) || defined(STM32F102X8_XB)
     _OSIOHandler, /* OS_IO_USB_LP             20 USB Device Low Priority */
  #elif defined(STM32F105XX) || defined(STM32F107XX)
     _OSIOHandler, /* OS_IO_CAN1_RX0           20 CAN1 RX0 */
  #elif defined(DEBUG_MODE)
     UndefinedInterrupt,
  #else
     NULL,
  #endif
  #if defined(STM32F103X4_X6) || defined(STM32F103T8_TB) || \
      defined(STM32F103C8_CB_R8_RB_V8_VB) || defined(STM32F103RC_RD_RE) || \
      defined(STM32F103VC_VD_VE_ZC_ZD_ZE) || defined(STM32F103RF_RG) || \
      defined(STM32F103VF_VG_ZF_ZG) || defined(STM32F105XX) || defined(STM32F107XX)
     _OSIOHandler, /* OS_IO_CAN1_RX1           21 CAN1 RX1 */
  #elif defined(DEBUG_MODE)
     UndefinedInterrupt,
  #else
     NULL,
  #endif
  #if defined(STM32F103X4_X6) || defined(STM32F103T8_TB) || \
      defined(STM32F103C8_CB_R8_RB_V8_VB) || defined(STM32F103RC_RD_RE) || \
      defined(STM32F103VC_VD_VE_ZC_ZD_ZE) || defined(STM32F103RF_RG) || \
      defined(STM32F103VF_VG_ZF_ZG) || defined(STM32F105XX) || defined(STM32F107XX)
     _OSIOHandler, /* OS_IO_CAN1_SCE           22 CAN1 SCE */
  #elif defined(DEBUG_MODE)
     UndefinedInterrupt,
  #else
     NULL,
  #endif
  _OSIOHandler, /* OS_IO_EXTI9_5               23 External Line[9:5] */
  #if defined(STM32F103X4_X6) || defined(STM32F103T8_TB) || \
      defined(STM32F103C8_CB_R8_RB_V8_VB) || defined(STM32F103RC_RD_RE) || \
      defined(STM32F103VC_VD_VE_ZC_ZD_ZE) || defined(STM32F105XX) || defined(STM32F107XX)
     _OSIOHandler, /* OS_IO_TIM1_BRK     24 TIM1 Break */
     _OSIOHandler, /* OS_IO_TIM1_UP      25 TIM1 Update */
     _OSIOHandler, /* OS_IO_TIM1_TRG_COM 26 TIM1 Trigger and Commutation */
     _OSIOHandler, /* OS_IO_TIM1_CC      27 TIM1 Capture Compare */
  #elif defined(STM32F100X4_X6) || defined(STM32F100X8_XB) || \
        defined(STM32F100RC_RD_RE) || defined(STM32F100VC_VD_VE_ZC_ZD_ZE)
        _OSIOHandler, /* OS_IO_TIM1_BRK_TIM15     24 TIM1 Break and TIM15 */
        _OSIOHandler, /* OS_IO_TIM1_UP_TIM16      25 TIM1 Update and TIM16 */
        _OSIOHandler, /* OS_IO_TIM1_TRG_COM_TIM17 26 TIM1 Trigger and Commutation and TIM17 */
        _OSIOHandler, /* OS_IO_TIM1_CC            27 TIM1 Capture Compare */
  #elif defined (STM32F101RF_RG)|| defined(STM32F101VF_VG_ZF_ZG)
     _OSIOHandler, /* OS_IO_TIM9       24 TIM9 global */
     _OSIOHandler, /* OS_IO__TIM10     25 TIM10 global */
     _OSIOHandler, /* OS_IO_TIM11      26 TIM11 */
     #if defined(DEBUG_MODE)
        UndefinedInterrupt,
     #else
        NULL,
     #endif
  #elif defined(STM32F103RF_RG) || defined(STM32F103VF_VG_ZF_ZG)
     _OSIOHandler, /* OS_IO_TIM1_BRK_TIM9      24 TIM1 Break  and TIM9 global */
     _OSIOHandler, /* OS_IO_TIM1_UP_TIM10      25 TIM1 Update and TIM10 global */
     _OSIOHandler, /* OS_IO_TIM1_TRG_COM_TIM11 26 TIM1 Trigger and Commutation  and TIM11 */
     _OSIOHandler, /* OS_IO_TIM1_CC            27 TIM1 Capture Compare */
  #elif defined(DEBUG_MODE)
     UndefinedInterrupt,
     UndefinedInterrupt,
     UndefinedInterrupt,
     UndefinedInterrupt,
  #else
     NULL,
     NULL,
     NULL,
     NULL,
  #endif
  _OSIOHandler, /* OS_IO_TIM2                  28  TIM2 global */
  _OSIOHandler, /* OS_IO_TIM3                  29  TIM3 global */
  #if defined (STM32F100X8_XB) || defined (STM32F100RC_RD_RE) || \
      defined (STM32F100VC_VD_VE_ZC_ZD_ZE) || defined(STM32F101T8_TB) || \
      defined(STM32F101C8_CB_R8_RB_V8_VB) || defined(STM32F101RC_RD_RE) || \
      defined(STM32F101VC_VD_VE_ZC_ZD_ZE) || defined(STM32F102X8_XB) || \
      defined(STM32F103T8_TB) || defined(STM32F103C8_CB_R8_RB_V8_VB) || \
      defined(STM32F103RC_RD_RE) || defined(STM32F103VC_VD_VE_ZC_ZD_ZE) || \
      defined (STM32F101RF_RG)|| defined(STM32F101VF_VG_ZF_ZG) || \
      defined(STM32F103RF_RG) || defined(STM32F103VF_VG_ZF_ZG) || \
      defined(STM32F105XX) || defined(STM32F107XX)
     _OSIOHandler, /* OS_IO_TIM4               30 TIM4 global */
  #elif defined(DEBUG_MODE)
     UndefinedInterrupt,
  #else
     NULL,
  #endif
  _OSIOHandler, /* OS_IO_I2C1_EV               31 I2C1 Event */
  _OSIOHandler, /* OS_IO_I2C1_ER               32 I2C1 Error */
  #if defined(STM32F100X8_XB) || defined(STM32F100RC_RD_RE) || \
      defined(STM32F100VC_VD_VE_ZC_ZD_ZE) || defined(STM32F101C8_CB_R8_RB_V8_VB) || \
      defined(STM32F101RC_RD_RE) || defined(STM32F101VC_VD_VE_ZC_ZD_ZE) || \
      defined(STM32F101RF_RG) || defined(STM32F101VF_VG_ZF_ZG) || \
      defined(STM32F102X8_XB) || defined(STM32F103C8_CB_R8_RB_V8_VB) || \
      defined(STM32F103RC_RD_RE) || defined(STM32F103VC_VD_VE_ZC_ZD_ZE) || \
      defined(STM32F103RF_RG) || defined(STM32F103VF_VG_ZF_ZG) || \
      defined(STM32F105XX) || defined(STM32F107XX)
     _OSIOHandler, /* OS_IO_I2C2_EV            33 I2C2 Event */
     _OSIOHandler, /* OS_IO_I2C2_ER            34 I2C2 Error */
  #elif defined(DEBUG_MODE)
     UndefinedInterrupt,
     UndefinedInterrupt,
  #else
     NULL,
     NULL,
  #endif
  _OSIOHandler, /* OS_IO_SPI1                  35 SPI1 global */
  #if defined(STM32F100X8_XB) || defined(STM32F100RC_RD_RE) || \
      defined(STM32F100VC_VD_VE_ZC_ZD_ZE) || defined(STM32F101C8_CB_R8_RB_V8_VB) || \
      defined(STM32F101RC_RD_RE) || defined(STM32F101VC_VD_VE_ZC_ZD_ZE) || \
      defined(STM32F101RF_RG) || defined(STM32F101VF_VG_ZF_ZG) || \
      defined(STM32F102X8_XB) || defined(STM32F103C8_CB_R8_RB_V8_VB) || \
      defined(STM32F103RC_RD_RE) || defined(STM32F103VC_VD_VE_ZC_ZD_ZE) || \
      defined(STM32F103RF_RG) || defined(STM32F103VF_VG_ZF_ZG) || \
      defined(STM32F105XX) || defined(STM32F107XX)
     _OSIOHandler, /* OS_IO_SPI2               36 SPI2 global */
  #elif defined(DEBUG_MODE)
     UndefinedInterrupt,
  #else
     NULL,
  #endif
  _OSIOHandler, /* OS_IO_USART1                37 USART1 global */
  _OSIOHandler, /* OS_IO_USART2                38 USART2 global */
  #if defined(STM32F100X8_XB) || defined(STM32F100RC_RD_RE) || \
      defined(STM32F100VC_VD_VE_ZC_ZD_ZE) || defined(STM32F101C8_CB_R8_RB_V8_VB) || \
      defined(STM32F101RC_RD_RE) || defined(STM32F101VC_VD_VE_ZC_ZD_ZE) || \
      defined(STM32F101RF_RG) || defined(STM32F101VF_VG_ZF_ZG) || \
      defined(STM32F102X8_XB) || defined(STM32F103C8_CB_R8_RB_V8_VB) || \
      defined(STM32F103RC_RD_RE) || defined(STM32F103VC_VD_VE_ZC_ZD_ZE) || \
      defined(STM32F103RF_RG) || defined(STM32F103VF_VG_ZF_ZG) || \
      defined(STM32F105XX) || defined(STM32F107XX)
     _OSIOHandler, /* OS_IO_USART3             39 USART3 global */
  #elif defined(DEBUG_MODE)
     UndefinedInterrupt,
  #else
     NULL,
  #endif
  _OSIOHandler, /* OS_IO_EXTI15_10             40 External Line[15:10] */
  _OSIOHandler, /* OS_IO_RTCAlarm              41 RTC Alarm through EXTI Line */
  #if defined(STM32F103X4_X6) || defined(STM32F103T8_TB) || \
      defined(STM32F103C8_CB_R8_RB_V8_VB) || defined(STM32F103RC_RD_RE) || \
      defined(STM32F103VC_VD_VE_ZC_ZD_ZE) || defined(STM32F103RF_RG) || \
      defined(STM32F103VF_VG_ZF_ZG) || defined(STM32F102X4_X6) || defined(STM32F102X8_XB)
     _OSIOHandler, /* OS_IO_USBWakeUp          42 USB Device WakeUp from suspend through
                                                  EXTI Line */
  #elif defined(STM32F100X4_X6) || defined(STM32F100X8_XB) || \
        defined(STM32F100RC_RD_RE) || defined(STM32F100VC_VD_VE_ZC_ZD_ZE)
     _OSIOHandler, /* OS_IO_CEC                42 HDMI-CEC */
  #elif defined(STM32F105XX) || defined(STM32F107XX)
     _OSIOHandler, /* OS_IO_OTG_FS_WKUP        42 USB OTG FS WakeUp from suspend through
                                                  EXTI Line */
  #elif defined(DEBUG_MODE)
     UndefinedInterrupt,
  #else
     NULL,
  #endif
  #if defined(STM32F103RC_RD_RE) || defined(STM32F103VC_VD_VE_ZC_ZD_ZE)
     _OSIOHandler, /* OS_IO_TIM8_BRK           43 TIM8 Break */
     _OSIOHandler, /* OS_IO_TIM8_UP            44 TIM8 Update */
     _OSIOHandler, /* OS_IO_TIM8_TRG_COM       45 TIM8 Trigger and Commutation */
     _OSIOHandler, /* OS_IO_TIM8_CC            46 TIM8 Capture Compare */
  #elif defined(STM32F101RF_RG) || defined(STM32F101VF_VG_ZF_ZG) || \
        defined(STM32F100RC_RD_RE) || defined(STM32F100VC_VD_VE_ZC_ZD_ZE )
     _OSIOHandler, /* OS_IO_TIM12              43 TIM12 global */
     _OSIOHandler, /* OS_IO_TIM13              44 TIM13 global */
     _OSIOHandler, /* OS_IO_TIM14              45 TIM14 global */
     #if defined(DEBUG_MODE)
        UndefinedInterrupt,
     #else
        NULL,
     #endif
  #elif defined(STM32F103RF_RG) || defined(STM32F103VF_VG_ZF_ZG)
     _OSIOHandler, /* OS_IO_TIM8_BRK_TIM12        43 TIM8 Break and TIM12 global */
     _OSIOHandler, /* OS_IO_TIM8_UP_TIM13         44 TIM8 Update and TIM13 global */
     _OSIOHandler, /* OS_IO_TIM8_TRG_COM_TIM14    45 TIM8 Trigger and Commutation  and TIM14 global */
     _OSIOHandler, /* OS_IO_TIM8_CC               46 TIM8 Capture Compare */
  #elif defined(DEBUG_MODE)
     UndefinedInterrupt,
     UndefinedInterrupt,
     UndefinedInterrupt,
     UndefinedInterrupt,
  #else
     NULL,
     NULL,
     NULL,
     NULL,
  #endif
  #if defined(STM32F103RC_RD_RE) || defined(STM32F103VC_VD_VE_ZC_ZD_ZE) || \
      defined(STM32F103RF_RG) || defined(STM32F103VF_VG_ZF_ZG)
     _OSIOHandler, /* OS_IO_ADC3               47 ADC3 global */
  #elif defined(DEBUG_MODE)
     UndefinedInterrupt,
  #else
     NULL,
  #endif
  #if defined(STM32F100VC_VD_VE_ZC_ZD_ZE) || defined(STM32F101VC_VD_VE_ZC_ZD_ZE) || \
      defined(STM32F101VF_VG_ZF_ZG) || defined(STM32F103VC_VD_VE_ZC_ZD_ZE) || \
      defined(STM32F103VF_VG_ZF_ZG)
     _OSIOHandler, /* OS_IO_FSMC               48 FSMC global */
  #elif defined(DEBUG_MODE)
     UndefinedInterrupt,
  #else
     NULL,
  #endif
  #if defined(STM32F103RC_RD_RE) || defined(STM32F103VC_VD_VE_ZC_ZD_ZE) || \
      defined(STM32F103RF_RG) || defined(STM32F103VF_VG_ZF_ZG)
     _OSIOHandler, /* OS_IO_SDIO               49 SDIO global */
  #elif defined(DEBUG_MODE)
     UndefinedInterrupt,
  #else
     NULL,
  #endif
  #if defined(STM32F100RC_RD_RE) || defined(STM32F100VC_VD_VE_ZC_ZD_ZE) || \
      defined(STM32F101RC_RD_RE) || defined(STM32F101VC_VD_VE_ZC_ZD_ZE) || \
      defined(STM32F101RF_RG) || defined(STM32F101VF_VG_ZF_ZG) || \
      defined(STM32F103RC_RD_RE) || defined(STM32F103VC_VD_VE_ZC_ZD_ZE) || \
      defined(STM32F103RF_RG) || defined(STM32F103VF_VG_ZF_ZG) || \
      defined(STM32F105XX) || defined(STM32F107XX)
     _OSIOHandler, /* OS_IO_TIM5               50 TIM5 global */
     _OSIOHandler, /* OS_IO_SPI3               51 SPI3 global */
  #elif defined(DEBUG_MODE)
     UndefinedInterrupt,
     UndefinedInterrupt,
  #else
     NULL,
     NULL,
  #endif
  #if defined(STM32F100RC_RD_RE) || defined(STM32F100VC_VD_VE_ZC_ZD_ZE) || \
      defined(STM32F101RC_RD_RE) || defined(STM32F101VC_VD_VE_ZC_ZD_ZE) || \
      defined(STM32F101RF_RG) || defined(STM32F101VF_VG_ZF_ZG) || \
      defined(STM32F103RC_RD_RE) || defined(STM32F103VC_VD_VE_ZC_ZD_ZE) || \
      defined(STM32F103RF_RG) || defined(STM32F103VF_VG_ZF_ZG) || \
      defined(STM32F105XX) || defined(STM32F107XX)
     _OSIOHandler, /* OS_IO_UART4              52 UART4 global */
     _OSIOHandler, /* OS_IO_UART5              53 UART5 global */
  #elif defined(DEBUG_MODE)
     UndefinedInterrupt,
     UndefinedInterrupt,
  #else
     NULL,
     NULL,
  #endif
  #if defined(STM32F100X4_X6) || defined(STM32F100X8_XB) || \
      defined(STM32F100RC_RD_RE) || defined(STM32F100VC_VD_VE_ZC_ZD_ZE)
     _OSIOHandler, /* OS_IO_TIM6_DAC           54 TIM6 and DAC underrun */
  #elif defined(STM32F101RC_RD_RE) || defined(STM32F101VC_VD_VE_ZC_ZD_ZE) || \
        defined(STM32F101RF_RG) || defined(STM32F101VF_VG_ZF_ZG) || \
        defined(STM32F103RC_RD_RE) || defined(STM32F103VC_VD_VE_ZC_ZD_ZE) || \
        defined(STM32F103RF_RG) || defined(STM32F103VF_VG_ZF_ZG) || \
        defined(STM32F105XX) || defined(STM32F107XX)
     _OSIOHandler, /* OS_IO_TIM6               54 TIM6 global */
  #elif defined(DEBUG_MODE)
     UndefinedInterrupt,
  #else
     NULL,
  #endif
  #if defined(STM32F100X4_X6) || defined(STM32F100X8_XB) || \
      defined(STM32F100RC_RD_RE) || defined(STM32F100VC_VD_VE_ZC_ZD_ZE) || \
      defined(STM32F101RC_RD_RE) || defined(STM32F101VC_VD_VE_ZC_ZD_ZE) || \
      defined(STM32F101RF_RG) || defined(STM32F101VF_VG_ZF_ZG) || \
      defined(STM32F103RC_RD_RE) || defined(STM32F103VC_VD_VE_ZC_ZD_ZE) || \
      defined(STM32F103RF_RG) || defined(STM32F103VF_VG_ZF_ZG) || \
      defined(STM32F105XX) || defined(STM32F107XX)
     _OSIOHandler, /* OS_IO_TIM7               55 TIM7 */
  #elif defined(DEBUG_MODE)
     UndefinedInterrupt,
  #else
     NULL,
  #endif
  #if defined(STM32F100RC_RD_RE) || defined(STM32F100VC_VD_VE_ZC_ZD_ZE) || \
      defined(STM32F101RC_RD_RE) || defined(STM32F101VC_VD_VE_ZC_ZD_ZE) || \
      defined(STM32F101RF_RG) || defined(STM32F101VF_VG_ZF_ZG) || \
      defined(STM32F103RC_RD_RE) || defined(STM32F103VC_VD_VE_ZC_ZD_ZE) || \
      defined(STM32F103RF_RG) || defined(STM32F103VF_VG_ZF_ZG) || \
      defined(STM32F105XX) || defined(STM32F107XX)
     _OSIOHandler, /* OS_IO_DMA2_Channel1      56 DMA2 Channel 1 global */
     _OSIOHandler, /* OS_IO_DMA2_Channel2      57 DMA2 Channel 2 global */
     _OSIOHandler, /* OS_IO_DMA2_Channel3      58 DMA2 Channel 3 global */
  #elif defined(DEBUG_MODE)
     UndefinedInterrupt,
     UndefinedInterrupt,
     UndefinedInterrupt,
  #else
     NULL,
     NULL,
     NULL,
  #endif
  #if defined(STM32F100RC_RD_RE) || defined(STM32F100VC_VD_VE_ZC_ZD_ZE) || \
      defined(STM32F101RC_RD_RE) || defined(STM32F101VC_VD_VE_ZC_ZD_ZE) || \
      defined(STM32F101RF_RG) || defined(STM32F101VF_VG_ZF_ZG) || \
      defined(STM32F103RC_RD_RE) || defined(STM32F103VC_VD_VE_ZC_ZD_ZE) || \
      defined(STM32F103RF_RG) || defined(STM32F103VF_VG_ZF_ZG)
     _OSIOHandler, /* OS_IO_DMA2_Channel4_5    59 DMA2 Channel 4 and Channel 5 global */
  #elif defined(STM32F105XX) || defined(STM32F107XX)
     _OSIOHandler, /* OS_IO_DMA2_Channel4      59 DMA2 Channel 4 global */
  #elif defined(DEBUG_MODE)
     UndefinedInterrupt,
  #else
     NULL,
  #endif
  #if defined(STM32F100RC_RD_RE) || defined(STM32F100VC_VD_VE_ZC_ZD_ZE)
     _OSIOHandler, /* OS_IO_DMA2_Channel5      60 DMA2 Channel 5 global  (DMA2 Channel 5
                                                  is mapped at position 60 only if the
                                                  MISC_REMAP bit in the AFIO_MAPR2
                                                  register is set) */
  #elif defined(STM32F105XX) || defined(STM32F107XX)
     _OSIOHandler, /* OS_IO_DMA2_Channel5      60 DMA2 Channel 5 global */
  #elif defined(DEBUG_MODE)
     UndefinedInterrupt,
  #else
     NULL,
  #endif
  #if defined(STM32F107XX)
     _OSIOHandler, /* OS_IO_ETH                61 Ethernet global */
     _OSIOHandler, /* OS_IO_ETH_WKUP           62 Ethernet Wakeup through EXTI line */
  #elif defined(DEBUG_MODE)
     UndefinedInterrupt,
     UndefinedInterrupt,
  #else
     NULL,
     NULL,
  #endif
  #if defined(STM32F105XX) || defined(STM32F107XX)
     _OSIOHandler, /* OS_IO_CAN2_TX            63 CAN2 TX */
     _OSIOHandler, /* OS_IO_CAN2_RX0           64 CAN2 RX0 */
     _OSIOHandler, /* OS_IO_CAN2_RX1           65 CAN2 RX1 */
     _OSIOHandler, /* OS_IO_CAN2_SCE           66 CAN2 SCE */
     _OSIOHandler, /* OS_IO_OTG_FS             67 USB OTG FS global */
  #elif defined(DEBUG_MODE)
     UndefinedInterrupt,
     UndefinedInterrupt,
     UndefinedInterrupt,
     UndefinedInterrupt,
     UndefinedInterrupt,
  #else
     NULL,
     NULL,
     NULL,
     NULL,
     NULL,
  #endif
/* ------------------------------ STM32F2 and STM32F4 --------------------------------- */
#elif defined(STM32F2XXXX) || defined(STM32F4XXXX)
  _OSIOHandler, /* OS_IO_ADC                   18  ADC1, ADC2 and ADC3 global  */
  _OSIOHandler, /* OS_IO_CAN1_TX               19  CAN1 TX */
  _OSIOHandler, /* OS_IO_CAN1_RX0              20  CAN1 RX0 */
  _OSIOHandler, /* OS_IO_CAN1_RX1              21  CAN1 RX1 */
  _OSIOHandler, /* OS_IO_CAN1_SCE              22  CAN1 SCE */
  _OSIOHandler, /* OS_IO_EXTI9_5               23  External Line[9:5]  */
  _OSIOHandler, /* OS_IO_TIM1_BRK_TIM9         24  TIM1 Break and TIM9 global */
  _OSIOHandler, /* OS_IO_TIM1_UP_TIM10         25  TIM1 Update and TIM10 global */
  _OSIOHandler, /* OS_IO_TIM1_TRG_COM_TIM11    26  TIM1 Trigger and Commutation and TIM11 global */
  _OSIOHandler, /* OS_IO_TIM1_CC               27  TIM1 Capture Compare */
  _OSIOHandler, /* OS_IO_TIM2                  28  TIM2 global */
  _OSIOHandler, /* OS_IO_TIM3                  29  TIM3 global */
  _OSIOHandler, /* OS_IO_TIM4                  30  TIM4 global */
  _OSIOHandler, /* OS_IO_I2C1_EV               31  I2C1 Event */
  _OSIOHandler, /* OS_IO_I2C1_ER               32  I2C1 Error */
  _OSIOHandler, /* OS_IO_I2C2_EV               33  I2C2 Event */
  _OSIOHandler, /* OS_IO_I2C2_ER               34  I2C2 Error */
  _OSIOHandler, /* OS_IO_SPI1                  35  SPI1 global */
  _OSIOHandler, /* OS_IO_SPI2                  36  SPI2 global */
  _OSIOHandler, /* OS_IO_USART1                37  USART1 global */
  _OSIOHandler, /* OS_IO_USART2                38  USART2 global */
  _OSIOHandler, /* OS_IO_USART3                39  USART3 global */
  _OSIOHandler, /* OS_IO_EXTI15_10             40  External Line[15:10]  */
  _OSIOHandler, /* OS_IO_RTC_Alarm             41  RTC Alarm (A and B) through EXTI Line */
  _OSIOHandler, /* OS_IO_OTG_FS_WKUP           42  USB OTG FS Wakeup through EXTI line */
  _OSIOHandler, /* OS_IO_TIM8_BRK_TIM12        43  TIM8 Break and TIM12 global */
  _OSIOHandler, /* OS_IO_TIM8_UP_TIM13         44  TIM8 Update and TIM13 global */
  _OSIOHandler, /* OS_IO_TIM8_TRG_COM_TIM14    45  TIM8 Trigger and Commutation and TIM14 global */
  _OSIOHandler, /* OS_IO_TIM8_CC               46  TIM8 Capture Compare */
  _OSIOHandler, /* OS_IO_DMA1_Stream7          47  DMA1 Stream7 */
  #if defined(STM32F205VX_ZX) || defined(STM32F215VX_ZX) || defined(STM32F207XX) || \
      defined(STM32F217XX) || defined(STM32F405VX_ZX) || defined(STM32F415VX_ZX) || \
      defined(STM32F407XX) || defined(STM32F417XX)
     _OSIOHandler, /* OS_IO_FSMC               48  FSMC global */
  #elif defined(DEBUG_MODE)
     UndefinedInterrupt,
  #else
     NULL,
  #endif
  _OSIOHandler, /* OS_IO_SDIO                  49  SDIO global */
  _OSIOHandler, /* OS_IO_TIM5               50  TIM5 global */
  _OSIOHandler, /* OS_IO_SPI3                  51  SPI3 global */
  _OSIOHandler, /* OS_IO_UART4                 52  UART4 global */
  _OSIOHandler, /* OS_IO_UART5                 53  UART5 global */
  _OSIOHandler, /* OS_IO_TIM6_DAC              54  TIM6 global and DAC1&2 underrun error */
  _OSIOHandler, /* OS_IO_TIM7                  55  TIM7 global */
  _OSIOHandler, /* OS_IO_DMA2_Stream0          56  DMA2 Stream 0 global */
  _OSIOHandler, /* OS_IO_DMA2_Stream1          57  DMA2 Stream 1 global */
  _OSIOHandler, /* OS_IO_DMA2_Stream2          58  DMA2 Stream 2 global */
  _OSIOHandler, /* OS_IO_DMA2_Stream3          59  DMA2 Stream 3 global */
  _OSIOHandler, /* OS_IO_DMA2_Stream4          60  DMA2 Stream 4 global */
  #if defined(STM32F207XX) || defined(STM32F217XX) || defined(STM32F407XX) || \
      defined(STM32F417XX)
     _OSIOHandler, /* OS_IO_ETH                61  Ethernet global */
     _OSIOHandler, /* OS_IO_ETH_WKUP           62  Ethernet Wakeup through EXTI line */
  #elif defined(DEBUG_MODE)
     UndefinedInterrupt,
     UndefinedInterrupt,
  #else
     NULL,
     NULL,
  #endif
  _OSIOHandler, /* OS_IO_CAN2_TX               63  CAN2 TX */
  _OSIOHandler, /* OS_IO_CAN2_RX0              64  CAN2 RX0 */
  _OSIOHandler, /* OS_IO_CAN2_RX1              65  CAN2 RX1 */
  _OSIOHandler, /* OS_IO_CAN2_SCE              66  CAN2 SCE */
  #if defined(STM32F207XX) || defined(STM32F217XX) || defined(STM32F407XX) || \
      defined(STM32F417XX)
     _OSIOHandler, /* OS_IO_OTG_FS             67  USB OTG FS global */
  #elif defined(DEBUG_MODE)
     UndefinedInterrupt,
  #else
     NULL,
  #endif
  _OSIOHandler, /* OS_IO_DMA2_Stream5          68  DMA2 Stream 5 global */
  _OSIOHandler, /* OS_IO_DMA2_Stream6          69  DMA2 Stream 6 global */
  _OSIOHandler, /* OS_IO_DMA2_Stream7          70  DMA2 Stream 7 global */
  _OSIOHandler, /* OS_IO_USART6                71  USART6 global */
  _OSIOHandler, /* OS_IO_I2C3_EV               72  I2C3 event */
  _OSIOHandler, /* OS_IO_I2C3_ER               73  I2C3 error */
  _OSIOHandler, /* OS_IO_OTG_HS_EP1_OUT        74  USB OTG HS End Point 1 Out global */
  _OSIOHandler, /* OS_IO_OTG_HS_EP1_IN         75  USB OTG HS End Point 1 In global */
  _OSIOHandler, /* OS_IO_OTG_HS_WKUP           76  USB OTG HS Wakeup through EXTI */
  _OSIOHandler, /* OS_IO_OTG_HS                77  USB OTG HS global */
  #if defined(STM32F207XX) || defined(STM32F217XX) || defined(STM32F407XX) || \
      defined(STM32F417XX)
     _OSIOHandler, /* OS_IO_DCMI               78  DCMI global */
  #elif defined(DEBUG_MODE)
     UndefinedInterrupt,
  #else
     NULL,
  #endif
  #if defined(STM32F215RX) || defined(STM32F215VX_ZX) || defined(STM32F217XX) || \
      defined(STM32F415RG) || defined(STM32F415VG_ZG) || defined(STM32F417XX)
     _OSIOHandler, /* OS_IO_CRYP               79  CRYP crypto global */
  #elif defined(DEBUG_MODE)
     UndefinedInterrupt,
  #else
     NULL,
  #endif
  _OSIOHandler, /* OS_IO_HASH_RNG              80  Hash and Rng global */
  #if defined(STM32F4XXXX)
     _OSIOHandler, /* OS_IO_FPU                81  FPU global */
  #elif defined(DEBUG_MODE)
     UndefinedInterrupt,
  #else
     NULL,
  #endif
#else
  #error STM32 version undefined
#endif
};


/* OSSetTimerISRDescriptor: Associates an timer ISR descriptor with an _OSTabDevice
** sub-entry. */
void OSSetTimerISRDescriptor(UINT8 entry, UINT8 subentry, void *descriptor)
{
  if (_OSTabDevice[entry] != NULL &&
      ((TIMERSELECT *)_OSTabDevice[entry])->TimerSelector == _OSTimerSelectorHandler &&
      subentry < TIMER_ISR_TAB_SIZE)
     /* Vecteur ayant deux timer comme source. */
     ((TIMERSELECT *)_OSTabDevice[entry])->TimerISRTab[subentry] = descriptor;
  else
     /* Vecteur ayant un seul timer comme source */
     _OSTabDevice[entry] = descriptor;
} /* end of OSSetTimerISRDescriptor */


/* OSGetTimerISRDescriptor: Returns the timer ISR descriptor associated with an
** _OSTabDevice sub-entry. */
void *OSGetTimerISRDescriptor(UINT8 entry, UINT8 subentry)
{
  if (_OSTabDevice[entry] != NULL &&
      ((TIMERSELECT *)_OSTabDevice[entry])->TimerSelector == _OSTimerSelectorHandler &&
      subentry < TIMER_ISR_TAB_SIZE)
     /* Vecteur ayant deux timer comme source. */
     return ((TIMERSELECT *)_OSTabDevice[entry])->TimerISRTab[subentry];
  else
     /* Vecteur ayant un seul timer comme source */
     return _OSTabDevice[entry];
} /* end of OSGetTimerISRDescriptor */