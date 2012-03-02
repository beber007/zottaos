/* Copyright (c) 2006-2012 MIS Institute of the HEIG affiliated to the University of
** Applied Sciences of Western Switzerland. All rights reserved.
** IN NO EVENT SHALL THE MIS INSTITUTE NOR THE HEIG NOR THE UNIVERSITY OF APPLIED
** SCIENCES OF WESTERN SWITZERLAND BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL,
** INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS
** DOCUMENTATION, EVEN IF THE MIS INSTITUTE OR THE HEIG OR THE UNIVERSITY OF APPLIED
** SCIENCES OF WESTERN SWITZERLAND HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
** THE MIS INSTITUTE, THE HEIG AND THE UNIVERSITY OF APPLIED SCIENCES OF WESTERN SWITZER-
** LAND SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
** WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE PRO-
** VIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE MIS INSTITUTE NOR THE HEIG AND NOR THE
** UNIVERSITY OF APPLIED SCIENCES OF WESTERN SWITZERLAND HAVE NO OBLIGATION TO PROVIDE
** MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
**
** File ZottaOS_Interrupts.h:
** Version date: January 2012
** Authors: MIS-TIC
*/

#ifndef _ZOTTAOS_INTERRUPTS_
#define _ZOTTAOS_INTERRUPTS_


/* The nested vector interrupt controller (NVIC) under Cortex-M3 allows dynamic prioriti-
** zation of interrupts with up to 256 levels that can be arranged into priority level
** groups where each group can be preempted. The NVIC is configurable by the pair (A,B),
** where A denotes the number of bits used to set the number of groups (number of priori-
** ties), and B is the number of bits used for the number of subpriorities within a
** group. A + B = 8 and A = [2..7]. Note that NTRTOS requires 3 priority groups for it-
** self. From highest to lowest priority, these are: one for the peripheral hardware tim-
** er, one for the software timer interrupt and a final one for the PendSV used to finali-
** ze a context switch.
** In the following, PRIGROUP determines the setting of the (A,B) pair, where PRIGROUP is
** a value in the range [0..5] yielding A = 7 - PRIGROUP and B = 8 - A.
** Note that some microcontrollers may have fewer than 8 bits to define the priority le-
** vels, for example STM-32 uses only 4 bits and PRIGROUP must be in the range [3..5]
** which then gives A = 7 - PRIGROUP and B = 4 - A. */
#define PRIGROUP (UINT32)3


/* Defines the priority group and level of the interval-timer. The timer's priority must
** be higher than that of SysTick. See OSStartMultitasking(). */
#define TIMER_PRIORITY      (UINT8)0
#define TIMER_SUB_PRIORITY  (UINT8)0


/* Uncomment the line below that corresponds to your target STM32 device while leaving
** the others commented. */
 #if !defined (STM32F10X_LD) && !defined (STM32F10X_LD_VL) && !defined (STM32F10X_MD) && !defined (STM32F10X_MD_VL) && !defined (STM32F10X_HD) && !defined (STM32F10X_HD_VL) && !defined (STM32F10X_XL) && !defined (STM32F10X_CL)
  /* #define STM32F10X_LD */     /*!< STM32F10X_LD: STM32 Low density devices */
  /* #define STM32F10X_LD_VL */  /*!< STM32F10X_LD_VL: STM32 Low density Value Line devices */
  /* #define STM32F10X_MD */     /*!< STM32F10X_MD: STM32 Medium density devices */
  /* #define STM32F10X_MD_VL */  /*!< STM32F10X_MD_VL: STM32 Medium density Value Line devices */
  /* #define STM32F10X_HD */     /*!< STM32F10X_HD: STM32 High density devices */
  #define STM32F10X_HD_VL     /*!< STM32F10X_HD_VL: STM32 High density value line devices */
  /* #define STM32F10X_XL */     /*!< STM32F10X_XL: STM32 XL-density devices */
  /* #define STM32F10X_CL */     /*!< STM32F10X_CL: STM32 Connectivity line devices */
#endif


#define OS_IO_WWDG                 0  /* Window WatchDog Interrupt */
#define OS_IO_PVD                  1  /* PVD through EXTI Line detection Interrupt */
#define OS_IO_TAMPER               2  /* Tamper Interrupt */
#define OS_IO_RTC                  3  /* RTC global Interrupt */
#define OS_IO_FLASH                4  /* FLASH global Interrupt */
#define OS_IO_RCC                  5  /* RCC global Interrupt */
#define OS_IO_EXTI0                6  /* EXTI Line0 Interrupt */
#define OS_IO_EXTI1                7  /* EXTI Line1 Interrupt */
#define OS_IO_EXTI2                8  /* EXTI Line2 Interrupt */
#define OS_IO_EXTI3                9  /* EXTI Line3 Interrupt */
#define OS_IO_EXTI4                10 /* EXTI Line4 Interrupt */
#define OS_IO_DMA1_Channel1        11 /* DMA1 Channel 1 global Interrupt */
#define OS_IO_DMA1_Channel2        12 /* DMA1 Channel 2 global Interrupt */
#define OS_IO_DMA1_Channel3        13 /* DMA1 Channel 3 global Interrupt */
#define OS_IO_DMA1_Channel4        14 /* DMA1 Channel 4 global Interrupt */
#define OS_IO_DMA1_Channel5        15 /* DMA1 Channel 5 global Interrupt */
#define OS_IO_DMA1_Channel6        16 /* DMA1 Channel 6 global Interrupt */
#define OS_IO_DMA1_Channel7        17 /* DMA1 Channel 7 global Interrupt */
#ifdef STM32F10X_LD
  #define OS_IO_ADC1_2             18 /* ADC1 and ADC2 global Interrupt */
  #define OS_IO_USB_HP_CAN1_TX     19 /* USB Device High Priority or CAN1 TX Interrupts */
  #define OS_IO_USB_LP_CAN1_RX0    20 /* USB Device Low Priority or CAN1 RX0 Interrupts */
  #define OS_IO_CAN1_RX1           21 /* CAN1 RX1 Interrupt */
  #define OS_IO_CAN1_SCE           22 /* CAN1 SCE Interrupt */
  #define OS_IO_EXTI9_5            23 /* External Line[9:5] Interrupts */
  #define OS_IO_TIM1_BRK           24 /* TIM1 Break Interrupt */
  #define OS_IO_TIM1_UP            25 /* TIM1 Update Interrupt */
  #define OS_IO_TIM1_TRG_COM       26 /* TIM1 Trigger and Commutation Interrupt */
  #define OS_IO_TIM1_CC            27 /* TIM1 Capture Compare Interrupt */
  #define OS_IO_TIM2               28 /* TIM2 global Interrupt */
  #define OS_IO_TIM3               29 /* TIM3 global Interrupt */
  #define OS_IO_I2C1_EV            31 /* I2C1 Event Interrupt */
  #define OS_IO_I2C1_ER            32 /* I2C1 Error Interrupt */
  #define OS_IO_SPI1               35 /* SPI1 global Interrupt */
  #define OS_IO_USART1             37 /* USART1 global Interrupt */
  #define OS_IO_USART2             38 /* USART2 global Interrupt */
  #define OS_IO_EXTI15_10          40 /* External Line[15:10] Interrupts */
  #define OS_IO_RTCAlarm           41 /* RTC Alarm through EXTI Line Interrupt */
  #define OS_IO_USBWakeUp          42 /* USB Device WakeUp from suspend through EXTI Line
                                         Interrupt */
#elif defined (STM32F10X_LD_VL)
  #define OS_IO_ADC1               18 /* ADC1 global Interrupt */
  #define OS_IO_EXTI9_5            23 /* External Line[9:5] Interrupts */
  #define OS_IO_TIM1_BRK_TIM15     24 /* TIM1 Break and TIM15 Interrupts */
  #define OS_IO_TIM1_UP_TIM16      25 /* TIM1 Update and TIM16 Interrupts */
  #define OS_IO_TIM1_TRG_COM_TIM17 26 /* TIM1 Trigger and Commutation and TIM17
                                         Interrupt */
  #define OS_IO_TIM1_CC            27 /* TIM1 Capture Compare Interrupt */
  #define OS_IO_TIM2               28 /* TIM2 global Interrupt */
  #define OS_IO_TIM3               29 /* TIM3 global Interrupt */
  #define OS_IO_I2C1_EV            31 /* I2C1 Event Interrupt */
  #define OS_IO_I2C1_ER            32 /* I2C1 Error Interrupt */
  #define OS_IO_SPI1               35 /* SPI1 global Interrupt */
  #define OS_IO_USART1             37 /* USART1 global Interrupt */
  #define OS_IO_USART2             38 /* USART2 global Interrupt */
  #define OS_IO_EXTI15_10          40 /* External Line[15:10] Interrupts */
  #define OS_IO_RTCAlarm           41 /* RTC Alarm through EXTI Line Interrupt */
  #define OS_IO_CEC                42 /* HDMI-CEC Interrupt */
  #define OS_IO_TIM6_DAC           54 /* TIM6 and DAC underrun Interrupt */
  #define OS_IO_TIM7               55 /* TIM7 Interrupt */
#elif defined (STM32F10X_MD)
  #define OS_IO_ADC1_2             18 /* ADC1 and ADC2 global Interrupt */
  #define OS_IO_USB_HP_CAN1_TX     19 /* USB Device High Priority or CAN1 TX Interrupts */
  #define OS_IO_USB_LP_CAN1_RX0    20 /* USB Device Low Priority or CAN1 RX0 Interrupts */
  #define OS_IO_CAN1_RX1           21 /* CAN1 RX1 Interrupt */
  #define OS_IO_CAN1_SCE           22 /* CAN1 SCE Interrupt */
  #define OS_IO_EXTI9_5            23 /* External Line[9:5] Interrupts */
  #define OS_IO_TIM1_BRK           24 /* TIM1 Break Interrupt */
  #define OS_IO_TIM1_UP            25 /* TIM1 Update Interrupt */
  #define OS_IO_TIM1_TRG_COM       26 /* TIM1 Trigger and Commutation Interrupt */
  #define OS_IO_TIM1_CC            27 /* TIM1 Capture Compare Interrupt */
  #define OS_IO_TIM2               28 /* TIM2 global Interrupt */
  #define OS_IO_TIM3               29 /* TIM3 global Interrupt */
  #define OS_IO_TIM4               30 /* TIM4 global Interrupt */
  #define OS_IO_I2C1_EV            31 /* I2C1 Event Interrupt */
  #define OS_IO_I2C1_ER            32 /* I2C1 Error Interrupt */
  #define OS_IO_I2C2_EV            33 /* I2C2 Event Interrupt */
  #define OS_IO_I2C2_ER            34 /* I2C2 Error Interrupt */
  #define OS_IO_SPI1               35 /* SPI1 global Interrupt */
  #define OS_IO_SPI2               36 /* SPI2 global Interrupt */
  #define OS_IO_USART1             37 /* USART1 global Interrupt */
  #define OS_IO_USART2             38 /* USART2 global Interrupt */
  #define OS_IO_USART3             39 /* USART3 global Interrupt */
  #define OS_IO_EXTI15_10          40 /* External Line[15:10] Interrupts */
  #define OS_IO_RTCAlarm           41 /* RTC Alarm through EXTI Line Interrupt */
  #define OS_IO_USBWakeUp          42 /* USB Device WakeUp from suspend through EXTI Line
                                         Interrupt */
#elif defined (STM32F10X_MD_VL)
  #define OS_IO_ADC1               18 /* ADC1 global Interrupt */
  #define OS_IO_EXTI9_5            23 /* External Line[9:5] Interrupts */
  #define OS_IO_TIM1_BRK_TIM15     24 /* TIM1 Break and TIM15 Interrupts */
  #define OS_IO_TIM1_UP_TIM16      25 /* TIM1 Update and TIM16 Interrupts */
  #define OS_IO_TIM1_TRG_COM_TIM17 26 /* TIM1 Trigger and Commutation and TIM17
                                         Interrupt */
  #define OS_IO_TIM1_CC            27 /* TIM1 Capture Compare Interrupt */
  #define OS_IO_TIM2               28 /* TIM2 global Interrupt */
  #define OS_IO_TIM3               29 /* TIM3 global Interrupt */
  #define OS_IO_TIM4               30 /* TIM4 global Interrupt */
  #define OS_IO_I2C1_EV            31 /* I2C1 Event Interrupt */
  #define OS_IO_I2C1_ER            32 /* I2C1 Error Interrupt */
  #define OS_IO_I2C2_EV            33 /* I2C2 Event Interrupt */
  #define OS_IO_I2C2_ER            34 /* I2C2 Error Interrupt */
  #define OS_IO_SPI1               35 /* SPI1 global Interrupt */
  #define OS_IO_SPI2               36 /* SPI2 global Interrupt */
  #define OS_IO_USART1             37 /* USART1 global Interrupt */
  #define OS_IO_USART2             38 /* USART2 global Interrupt */
  #define OS_IO_USART3             39 /* USART3 global Interrupt */
  #define OS_IO_EXTI15_10          40 /* External Line[15:10] Interrupts */
  #define OS_IO_RTCAlarm           41 /* RTC Alarm through EXTI Line Interrupt */
  #define OS_IO_CEC                42 /* HDMI-CEC Interrupt */
  #define OS_IO_TIM6_DAC           54 /* TIM6 and DAC underrun Interrupt */
  #define OS_IO_TIM7               55 /* TIM7 Interrupt */
#elif defined (STM32F10X_HD)
  #define OS_IO_ADC1_2             18 /* ADC1 and ADC2 global Interrupt */
  #define OS_IO_USB_HP_CAN1_TX     19 /* USB Device High Priority or CAN1 TX Interrupts */
  #define OS_IO_USB_LP_CAN1_RX0    20 /* USB Device Low Priority or CAN1 RX0 Interrupts */
  #define OS_IO_CAN1_RX1           21 /* CAN1 RX1 Interrupt */
  #define OS_IO_CAN1_SCE           22 /* CAN1 SCE Interrupt */
  #define OS_IO_EXTI9_5            23 /* External Line[9:5] Interrupts */
  #define OS_IO_TIM1_BRK           24 /* TIM1 Break Interrupt */
  #define OS_IO_TIM1_UP            25 /* TIM1 Update Interrupt */
  #define OS_IO_TIM1_TRG_COM       26 /* TIM1 Trigger and Commutation Interrupt */
  #define OS_IO_TIM1_CC            27 /* TIM1 Capture Compare Interrupt */
  #define OS_IO_TIM2               28 /* TIM2 global Interrupt */
  #define OS_IO_TIM3               29 /* TIM3 global Interrupt */
  #define OS_IO_TIM4               30 /* TIM4 global Interrupt */
  #define OS_IO_I2C1_EV            31 /* I2C1 Event Interrupt */
  #define OS_IO_I2C1_ER            32 /* I2C1 Error Interrupt */
  #define OS_IO_I2C2_EV            33 /* I2C2 Event Interrupt */
  #define OS_IO_I2C2_ER            34 /* I2C2 Error Interrupt */
  #define OS_IO_SPI1               35 /* SPI1 global Interrupt */
  #define OS_IO_SPI2               36 /* SPI2 global Interrupt */
  #define OS_IO_USART1             37 /* USART1 global Interrupt */
  #define OS_IO_USART2             38 /* USART2 global Interrupt */
  #define OS_IO_USART3             39 /* USART3 global Interrupt */
  #define OS_IO_EXTI15_10          40 /* External Line[15:10] Interrupts */
  #define OS_IO_RTCAlarm           41 /* RTC Alarm through EXTI Line Interrupt */
  #define OS_IO_USBWakeUp          42 /* USB Device WakeUp from suspend through EXTI Line
                                         Interrupt */
  #define OS_IO_TIM8_BRK           43 /* TIM8 Break Interrupt */
  #define OS_IO_TIM8_UP            44 /* TIM8 Update Interrupt */
  #define OS_IO_TIM8_TRG_COM       45 /* TIM8 Trigger and Commutation Interrupt */
  #define OS_IO_TIM8_CC            46 /* TIM8 Capture Compare Interrupt */
  #define OS_IO_ADC3               47 /* ADC3 global Interrupt */
  #define OS_IO_FSMC               48 /* FSMC global Interrupt */
  #define OS_IO_SDIO               49 /* SDIO global Interrupt */
  #define OS_IO_TIM5               50 /* TIM5 global Interrupt */
  #define OS_IO_SPI3               51 /* SPI3 global Interrupt */
  #define OS_IO_UART4              52 /* UART4 global Interrupt */
  #define OS_IO_UART5              53 /* UART5 global Interrupt */
  #define OS_IO_TIM6               54 /* TIM6 global Interrupt */
  #define OS_IO_TIM7               55 /* TIM7 global Interrupt */
  #define OS_IO_DMA2_Channel1      56 /* DMA2 Channel 1 global Interrupt */
  #define OS_IO_DMA2_Channel2      57 /* DMA2 Channel 2 global Interrupt */
  #define OS_IO_DMA2_Channel3      58 /* DMA2 Channel 3 global Interrupt */
  #define OS_IO_DMA2_Channel4_5    59 /* DMA2 Channel 4 and Channel 5 global Interrupt */
#elif defined (STM32F10X_HD_VL)
  #define OS_IO_ADC1               18 /* ADC1 global Interrupt */
  #define OS_IO_EXTI9_5            23 /* External Line[9:5] Interrupts */
  #define OS_IO_TIM1_BRK_TIM15     24 /* TIM1 Break and TIM15 Interrupts */
  #define OS_IO_TIM1_UP_TIM16      25 /* TIM1 Update and TIM16 Interrupts */
  #define OS_IO_TIM1_TRG_COM_TIM17 26 /* TIM1 Trigger and Commutation and TIM17
                                         Interrupt */
  #define OS_IO_TIM1_CC            27 /* TIM1 Capture Compare Interrupt */
  #define OS_IO_TIM2               28 /* TIM2 global Interrupt */
  #define OS_IO_TIM3               29 /* TIM3 global Interrupt */
  #define OS_IO_TIM4               30 /* TIM4 global Interrupt */
  #define OS_IO_I2C1_EV            31 /* I2C1 Event Interrupt */
  #define OS_IO_I2C1_ER            32 /* I2C1 Error Interrupt */
  #define OS_IO_I2C2_EV            33 /* I2C2 Event Interrupt */
  #define OS_IO_I2C2_ER            34 /* I2C2 Error Interrupt */
  #define OS_IO_SPI1               35 /* SPI1 global Interrupt */
  #define OS_IO_SPI2               36 /* SPI2 global Interrupt */
  #define OS_IO_USART1             37 /* USART1 global Interrupt */
  #define OS_IO_USART2             38 /* USART2 global Interrupt */
  #define OS_IO_USART3             39 /* USART3 global Interrupt */
  #define OS_IO_EXTI15_10          40 /* External Line[15:10] Interrupts */
  #define OS_IO_RTCAlarm           41 /* RTC Alarm through EXTI Line Interrupt */
  #define OS_IO_CEC                42 /* HDMI-CEC Interrupt */
  #define OS_IO_TIM12              43 /* TIM12 global Interrupt */
  #define OS_IO_TIM13              44 /* TIM13 global Interrupt */
  #define OS_IO_TIM14              45 /* TIM14 global Interrupt */
  #define OS_IO_TIM5               50 /* TIM5 global Interrupt */
  #define OS_IO_SPI3               51 /* SPI3 global Interrupt */
  #define OS_IO_UART4              52 /* UART4 global Interrupt */
  #define OS_IO_UART5              53 /* UART5 global Interrupt */
  #define OS_IO_TIM6_DAC           54 /* TIM6 and DAC underrun Interrupt */
  #define OS_IO_TIM7               55 /* TIM7 Interrupt */
  #define OS_IO_DMA2_Channel1      56 /* DMA2 Channel 1 global Interrupt */
  #define OS_IO_DMA2_Channel2      57 /* DMA2 Channel 2 global Interrupt */
  #define OS_IO_DMA2_Channel3      58 /* DMA2 Channel 3 global Interrupt */
  #define OS_IO_DMA2_Channel4_5    59 /* DMA2 Channel 4 and Channel 5 global Interrupt */
  #define OS_IO_DMA2_Channel5      60 /* DMA2 Channel 5 global Interrupt (DMA2 Channel 5
                                         is mapped at position 60 only if the MISC_REMAP
                                         bit in the AFIO_MAPR2 register is set) */
#elif defined (STM32F10X_XL)
  #define OS_IO_ADC1_2             18 /* ADC1 and ADC2 global Interrupt */
  #define OS_IO_USB_HP_CAN1_TX     19 /* USB Device High Priority or CAN1 TX Interrupts */
  #define OS_IO_USB_LP_CAN1_RX0    20 /* USB Device Low Priority or CAN1 RX0 Interrupts */
  #define OS_IO_CAN1_RX1           21 /* CAN1 RX1 Interrupt */
  #define OS_IO_CAN1_SCE           22 /* CAN1 SCE Interrupt */
  #define OS_IO_EXTI9_5            23 /* External Line[9:5] Interrupts */
  #define OS_IO_TIM1_BRK_TIM9      24 /* TIM1 Break Interrupt and TIM9 global Interrupt */
  #define OS_IO_TIM1_UP_TIM10      25 /* TIM1 Update Interrupt and TIM10 global
                                         Interrupt */
  #define OS_IO_TIM1_TRG_COM_TIM11 26 /* TIM1 Trigger and Commutation Interrupt and TIM11
                                         global interrupt */
  #define OS_IO_TIM1_CC            27 /* TIM1 Capture Compare Interrupt */
  #define OS_IO_TIM2               28 /* TIM2 global Interrupt */
  #define OS_IO_TIM3               29 /* TIM3 global Interrupt */
  #define OS_IO_TIM4               30 /* TIM4 global Interrupt */
  #define OS_IO_I2C1_EV            31 /* I2C1 Event Interrupt */
  #define OS_IO_I2C1_ER            32 /* I2C1 Error Interrupt */
  #define OS_IO_I2C2_EV            33 /* I2C2 Event Interrupt */
  #define OS_IO_I2C2_ER            34 /* I2C2 Error Interrupt */
  #define OS_IO_SPI1               35 /* SPI1 global Interrupt */
  #define OS_IO_SPI2               36 /* SPI2 global Interrupt */
  #define OS_IO_USART1             37 /* USART1 global Interrupt */
  #define OS_IO_USART2             38 /* USART2 global Interrupt */
  #define OS_IO_USART3             39 /* USART3 global Interrupt */
  #define OS_IO_EXTI15_10          40 /* External Line[15:10] Interrupts */
  #define OS_IO_RTCAlarm           41 /* RTC Alarm through EXTI Line Interrupt */
  #define OS_IO_USBWakeUp          42 /* USB Device WakeUp from suspend through EXTI Line
                                         Interrupt */
  #define OS_IO_TIM8_BRK_TIM12     43 /* TIM8 Break Interrupt and TIM12 global
                                         Interrupt */
  #define OS_IO_TIM8_UP_TIM13      44 /* TIM8 Update Interrupt and TIM13 global
                                         Interrupt */
  #define OS_IO_TIM8_TRG_COM_TIM14 45 /* TIM8 Trigger and Commutation Interrupt and TIM14
                                         global interrupt */
  #define OS_IO_TIM8_CC            46 /* TIM8 Capture Compare Interrupt */
  #define OS_IO_ADC3               47 /* ADC3 global Interrupt */
  #define OS_IO_FSMC               48 /* FSMC global Interrupt */
  #define OS_IO_SDIO               49 /* SDIO global Interrupt */
  #define OS_IO_TIM5               50 /* TIM5 global Interrupt */
  #define OS_IO_SPI3               51 /* SPI3 global Interrupt */
  #define OS_IO_UART4              52 /* UART4 global Interrupt */
  #define OS_IO_UART5              53 /* UART5 global Interrupt */
  #define OS_IO_TIM6               54 /* TIM6 global Interrupt */
  #define OS_IO_TIM7               55 /* TIM7 global Interrupt */
  #define OS_IO_DMA2_Channel1      56 /* DMA2 Channel 1 global Interrupt */
  #define OS_IO_DMA2_Channel2      57 /* DMA2 Channel 2 global Interrupt */
  #define OS_IO_DMA2_Channel3      58 /* DMA2 Channel 3 global Interrupt */
  #define OS_IO_DMA2_Channel4_5    59 /* DMA2 Channel 4 and Channel 5 global Interrupt */
#elif defined (STM32F10X_CL)
  #define OS_IO_ADC1_2             18 /* ADC1 and ADC2 global Interrupt */
  #define OS_IO_CAN1_TX            19 /* USB Device High Priority or CAN1 TX Interrupts */
  #define OS_IO_CAN1_RX0           20 /* USB Device Low Priority or CAN1 RX0 Interrupts */
  #define OS_IO_CAN1_RX1           21 /* CAN1 RX1 Interrupt */
  #define OS_IO_CAN1_SCE           22 /* CAN1 SCE Interrupt */
  #define OS_IO_EXTI9_5            23 /* External Line[9:5] Interrupts */
  #define OS_IO_TIM1_BRK           24 /* TIM1 Break Interrupt */
  #define OS_IO_TIM1_UP            25 /* TIM1 Update Interrupt */
  #define OS_IO_TIM1_TRG_COM       26 /* TIM1 Trigger and Commutation Interrupt */
  #define OS_IO_TIM1_CC            27 /* TIM1 Capture Compare Interrupt */
  #define OS_IO_TIM2               28 /* TIM2 global Interrupt */
  #define OS_IO_TIM3               29 /* TIM3 global Interrupt */
  #define OS_IO_TIM4               30 /* TIM4 global Interrupt */
  #define OS_IO_I2C1_EV            31 /* I2C1 Event Interrupt */
  #define OS_IO_I2C1_ER            32 /* I2C1 Error Interrupt */
  #define OS_IO_I2C2_EV            33 /* I2C2 Event Interrupt */
  #define OS_IO_I2C2_ER            34 /* I2C2 Error Interrupt */
  #define OS_IO_SPI1               35 /* SPI1 global Interrupt */
  #define OS_IO_SPI2               36 /* SPI2 global Interrupt */
  #define OS_IO_USART1             37 /* USART1 global Interrupt */
  #define OS_IO_USART2             38 /* USART2 global Interrupt */
  #define OS_IO_USART3             39 /* USART3 global Interrupt */
  #define OS_IO_EXTI15_10          40 /* External Line[15:10] Interrupts */
  #define OS_IO_RTCAlarm           41 /* RTC Alarm through EXTI Line Interrupt */
  #define OS_IO_OTG_FS_WKUP        42 /* USB OTG FS WakeUp from suspend through EXTI Line
                                         Interrupt */
  #define OS_IO_TIM5               50 /* TIM5 global Interrupt */
  #define OS_IO_SPI3               51 /* SPI3 global Interrupt */
  #define OS_IO_UART4              52 /* UART4 global Interrupt */
  #define OS_IO_UART5              53 /* UART5 global Interrupt */
  #define OS_IO_TIM6               54 /* TIM6 global Interrupt */
  #define OS_IO_TIM7               55 /* TIM7 global Interrupt */
  #define OS_IO_DMA2_Channel1      56 /* DMA2 Channel 1 global Interrupt */
  #define OS_IO_DMA2_Channel2      57 /* DMA2 Channel 2 global Interrupt */
  #define OS_IO_DMA2_Channel3      58 /* DMA2 Channel 3 global Interrupt */
  #define OS_IO_DMA2_Channel4      59 /* DMA2 Channel 4 global Interrupt */
  #define OS_IO_DMA2_Channel5      60 /* DMA2 Channel 5 global Interrupt */
  #define OS_IO_ETH                61 /* Ethernet global Interrupt */
  #define OS_IO_ETH_WKUP           62 /* Ethernet Wakeup through EXTI line Interrupt */
  #define OS_IO_CAN2_TX            63 /* CAN2 TX Interrupt */
  #define OS_IO_CAN2_RX0           64 /* CAN2 RX0 Interrupt */
  #define OS_IO_CAN2_RX1           65 /* CAN2 RX1 Interrupt */
  #define OS_IO_CAN2_SCE           66 /* CAN2 SCE Interrupt */
  #define OS_IO_OTG_FS             67 /* USB OTG FS global Interrupt */
#endif

#endif /* _ZOTTAOS_INTERRUPTS_ */

