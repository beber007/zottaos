## Copyright (c) 2006-2012 MIS Institute of the HEIG-VD affiliated to the University of
## Applied Sciences of Western Switzerland. All rights reserved.
## Permission to use, copy, modify, and distribute this software and its documentation
## for any purpose, without fee, and without written agreement is hereby granted, pro-
## vided that the above copyright notice, the following three sentences and the authors
## appear in all copies of this software and in the software where it is used.
## IN NO EVENT SHALL THE MIS INSTITUTE NOR THE HEIG-VD NOR THE UNIVERSITY OF APPLIED
## SCIENCES OF WESTERN SWITZERLAND BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL,
## INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS
## DOCUMENTATION, EVEN IF THE MIS INSTITUTE OR THE HEIG-VD OR THE UNIVERSITY OF APPLIED
## SCIENCES OF WESTERN SWITZERLAND HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
## THE MIS INSTITUTE, THE HEIG-VD AND THE UNIVERSITY OF APPLIED SCIENCES OF WESTERN SWIT-
## ZERLAND SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
## IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFT-
## WARE PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE MIS INSTITUTE NOR THE HEIG-VD
## AND NOR THE UNIVERSITY OF APPLIED SCIENCES OF WESTERN SWITZERLAND HAVE NO OBLIGATION
## TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

# Version identifier: August 2012
# Authors: MIS-TIC 

FAMILLY = STM32F0xx
LINKERFILE = STM32F051R8_Flash.ld

FAMILLY_LC = `echo $(FAMILLY) | tr A-Z a-z`

DEF_HEADER_FILE = $(FAMILLY_LC).h
CONF_HEADER_FILE = ZottaOS_$(FAMILLY_LC).h

PATH_LIB_ST = ../../Libraries

INCLUDE_LIB_ST = -I. -I${PATH_LIB_ST}/${FAMILLY}_StdPeriph_Driver/inc -I${PATH_LIB_ST}/CMSIS/Device/${FAMILLY} -I${PATH_LIB_ST}/CMSIS/Include

CC = /root/CodeSourcery/Sourcery_CodeBench_Lite_for_ARM_EABI/bin/arm-none-eabi-gcc

CPU = cortex-m0

all: build/TaskLEDF0.elf build/UARTSimpleEchoF0.elf build/TestTimerEventF0.elf build/TestTimerEventF0b.elf

clean: 
	rm -f build/*.o build/*.elf

INCLUDE_ZOTTAOS = -I. -I../../ -I../../../ -I../../../../

CFLAGS = -mcpu=${CPU} -mthumb -Wall -c -g -fdata-sections -ffunction-sections
AFLAGS = -mcpu=$(CPU) -mthumb -Wall -c -g

OBJECTS_ZOTTAOS = build/ZottaOS_UART.o \
          build/ZottaOS_TimerEvent.o \
          build/ZottaOS_Interrupts.o \
          build/ZottaOS_Timer.o \
          build/ZottaOS_Atomic.o \
          build/ZottaOS_CortexMx.o \
          build/ZottaOS_CortexMx_a.o \
          build/ZottaOSHard.o \
          build/ZottaOSSoft.o \
          
OBJECTS_LIB = build/system_stm32f0xx.o \
              build/stm32f0xx_adc.o \
              build/stm32f0xx_cec.o \
              build/stm32f0xx_crc.o \
              build/stm32f0xx_comp.o \
              build/stm32f0xx_dac.o \
              build/stm32f0xx_dbgmcu.o \
              build/stm32f0xx_dma.o \
              build/stm32f0xx_exti.o \
              build/stm32f0xx_flash.o \
              build/stm32f0xx_gpio.o \
              build/stm32f0xx_syscfg.o \
              build/stm32f0xx_i2c.o \
              build/stm32f0xx_iwdg.o \
              build/stm32f0xx_pwr.o \
              build/stm32f0xx_rcc.o \
              build/stm32f0xx_rtc.o \
              build/stm32f0xx_spi.o \
              build/stm32f0xx_tim.o \
              build/stm32f0xx_usart.o \
              build/stm32f0xx_wwdg.o \
              build/stm32f0xx_misc.o

build/ZottaOS_Atomic.o : ../../../ZottaOS_Atomic.c
	$(CC) $(CFLAGS) $(INCLUDE_ZOTTAOS) -o $@ $<
build/ZottaOS_Interrupts.o : ../../ZottaOS_Interrupts.c
	$(CC) $(CFLAGS) $(INCLUDE_ZOTTAOS) -o $@ $<
build/ZottaOS_Timer.o : ../../ZottaOS_Timer.c
	$(CC) $(CFLAGS) $(INCLUDE_ZOTTAOS) -o $@ $< 
build/ZottaOS_UART.o : ../../ZottaOS_UART.c
	$(CC) $(CFLAGS) $(INCLUDE_ZOTTAOS) -o $@ $< 
build/ZottaOS_CortexMx.o : ../../../ZottaOS_CortexMx.c
	$(CC) $(CFLAGS) $(INCLUDE_ZOTTAOS) -o $@ $<
build/ZottaOSHard.o : ../../../../ZottaOSHard.c
	$(CC) $(CFLAGS) $(INCLUDE_ZOTTAOS) -o $@ $< 
build/ZottaOSSoft.o : ../../../../ZottaOSSoft.c
	$(CC) $(CFLAGS) $(INCLUDE_ZOTTAOS) -o $@ $< 
build/ZottaOS_TimerEvent.o : ../../ZottaOS_TimerEvent.c 
	$(CC) $(CFLAGS) $(INCLUDE_ZOTTAOS) -o $@ $<
build/ZottaOS_CortexMx_a.o : ../../../ZottaOS_CortexMx_a.S
	$(CC) $(AFLAGS) $(INCLUDE_ZOTTAOS) -o $@ $< 


build/system_stm32f0xx.o : system_stm32f0xx.c
	$(CC) $(CFLAGS) $(INCLUDE_LIB_ST) -o $@ $<
build/stm32f0xx_adc.o : ${PATH_LIB_ST}/${FAMILLY}_StdPeriph_Driver/src/stm32f0xx_adc.c
	$(CC) $(CFLAGS) $(INCLUDE_LIB_ST) -o $@ $<
build/stm32f0xx_cec.o : ${PATH_LIB_ST}/${FAMILLY}_StdPeriph_Driver/src/stm32f0xx_cec.c
	$(CC) $(CFLAGS) $(INCLUDE_LIB_ST) -o $@ $<
build/stm32f0xx_crc.o : ${PATH_LIB_ST}/${FAMILLY}_StdPeriph_Driver/src/stm32f0xx_crc.c
	$(CC) $(CFLAGS) $(INCLUDE_LIB_ST) -o $@ $<
build/stm32f0xx_comp.o : ${PATH_LIB_ST}/${FAMILLY}_StdPeriph_Driver/src/stm32f0xx_comp.c
	$(CC) $(CFLAGS) $(INCLUDE_LIB_ST) -o $@ $<
build/stm32f0xx_dac.o : ${PATH_LIB_ST}/${FAMILLY}_StdPeriph_Driver/src/stm32f0xx_dac.c
	$(CC) $(CFLAGS) $(INCLUDE_LIB_ST) -o $@ $<
build/stm32f0xx_dbgmcu.o : ${PATH_LIB_ST}/${FAMILLY}_StdPeriph_Driver/src/stm32f0xx_dbgmcu.c
	$(CC) $(CFLAGS) $(INCLUDE_LIB_ST) -o $@ $<
build/stm32f0xx_dma.o : ${PATH_LIB_ST}/${FAMILLY}_StdPeriph_Driver/src/stm32f0xx_dma.c
	$(CC) $(CFLAGS) $(INCLUDE_LIB_ST) -o $@ $<
build/stm32f0xx_exti.o : ${PATH_LIB_ST}/${FAMILLY}_StdPeriph_Driver/src/stm32f0xx_exti.c
	$(CC) $(CFLAGS) $(INCLUDE_LIB_ST) -o $@ $<
build/stm32f0xx_flash.o : ${PATH_LIB_ST}/${FAMILLY}_StdPeriph_Driver/src/stm32f0xx_flash.c
	$(CC) $(CFLAGS) $(INCLUDE_LIB_ST) -o $@ $<
build/stm32f0xx_gpio.o : ${PATH_LIB_ST}/${FAMILLY}_StdPeriph_Driver/src/stm32f0xx_gpio.c
	$(CC) $(CFLAGS) $(INCLUDE_LIB_ST) -o $@ $<
build/stm32f0xx_syscfg.o : ${PATH_LIB_ST}/${FAMILLY}_StdPeriph_Driver/src/stm32f0xx_syscfg.c
	$(CC) $(CFLAGS) $(INCLUDE_LIB_ST) -o $@ $<
build/stm32f0xx_i2c.o : ${PATH_LIB_ST}/${FAMILLY}_StdPeriph_Driver/src/stm32f0xx_i2c.c
	$(CC) $(CFLAGS) $(INCLUDE_LIB_ST) -o $@ $<
build/stm32f0xx_iwdg.o : ${PATH_LIB_ST}/${FAMILLY}_StdPeriph_Driver/src/stm32f0xx_iwdg.c
	$(CC) $(CFLAGS) $(INCLUDE_LIB_ST) -o $@ $<
build/stm32f0xx_pwr.o : ${PATH_LIB_ST}/${FAMILLY}_StdPeriph_Driver/src/stm32f0xx_pwr.c
	$(CC) $(CFLAGS) $(INCLUDE_LIB_ST) -o $@ $<
build/stm32f0xx_rcc.o : ${PATH_LIB_ST}/${FAMILLY}_StdPeriph_Driver/src/stm32f0xx_rcc.c
	$(CC) $(CFLAGS) $(INCLUDE_LIB_ST) -o $@ $<
build/stm32f0xx_rtc.o : ${PATH_LIB_ST}/${FAMILLY}_StdPeriph_Driver/src/stm32f0xx_rtc.c
	$(CC) $(CFLAGS) $(INCLUDE_LIB_ST) -o $@ $<
build/stm32f0xx_spi.o : ${PATH_LIB_ST}/${FAMILLY}_StdPeriph_Driver/src/stm32f0xx_spi.c
	$(CC) $(CFLAGS) $(INCLUDE_LIB_ST) -o $@ $<
build/stm32f0xx_tim.o : ${PATH_LIB_ST}/${FAMILLY}_StdPeriph_Driver/src/stm32f0xx_tim.c
	$(CC) $(CFLAGS) $(INCLUDE_LIB_ST) -o $@ $<
build/stm32f0xx_usart.o : ${PATH_LIB_ST}/${FAMILLY}_StdPeriph_Driver/src/stm32f0xx_usart.c
	$(CC) $(CFLAGS) $(INCLUDE_LIB_ST) -o $@ $<
build/stm32f0xx_wwdg.o : ${PATH_LIB_ST}/${FAMILLY}_StdPeriph_Driver/src/stm32f0xx_wwdg.c
	$(CC) $(CFLAGS) $(INCLUDE_LIB_ST) -o $@ $<
build/stm32f0xx_misc.o : ${PATH_LIB_ST}/${FAMILLY}_StdPeriph_Driver/src/stm32f0xx_misc.c
	$(CC) $(CFLAGS) $(INCLUDE_LIB_ST) -o $@ $<
build/misc.o : ${PATH_LIB_ST}/${FAMILLY}_StdPeriph_Driver/src/misc.c
	$(CC) $(CFLAGS) $(INCLUDE_LIB_ST) -o $@ $<


build/TaskLEDF0.o : TaskLEDF0.c 
	$(CC) $(CFLAGS) $(INCLUDE_LIB_ST) $(INCLUDE_ZOTTAOS) -o $@ $<
build/UARTSimpleEchoF0.o : UARTSimpleEchoF0.c 
	$(CC) $(CFLAGS) $(INCLUDE_LIB_ST) $(INCLUDE_ZOTTAOS) -o $@ $<
build/TestTimerEventF0.o : TestTimerEventF0.c 
	$(CC) $(CFLAGS) $(INCLUDE_LIB_ST) $(INCLUDE_ZOTTAOS) -o $@ $<
build/TestTimerEventF0b.o : TestTimerEventF0b.c 
	$(CC) $(CFLAGS) $(INCLUDE_LIB_ST) $(INCLUDE_ZOTTAOS) -o $@ $<

build/TaskLEDF0.elf : ${OBJECTS_ZOTTAOS} ${OBJECTS_LIB} build/TaskLEDF0.o
	${CC} -mcpu=${CPU} -Wl,--gc-sections -mthumb -nostartfiles -o $@ ${OBJECTS_ZOTTAOS} ${OBJECTS_LIB} build/TaskLEDF0.o -T${LINKERFILE}
build/UARTSimpleEchoF0.elf : ${OBJECTS_ZOTTAOS} ${OBJECTS_LIB} build/UARTSimpleEchoF0.o
	${CC} -mcpu=${CPU} -Wl,--gc-sections -mthumb -nostartfiles -o $@ ${OBJECTS_ZOTTAOS} ${OBJECTS_LIB} build/UARTSimpleEchoF0.o -T${LINKERFILE}
build/TestTimerEventF0.elf : ${OBJECTS_ZOTTAOS} ${OBJECTS_LIB} build/TestTimerEventF0.o
	${CC} -mcpu=${CPU} -Wl,--gc-sections -mthumb -nostartfiles -o $@ ${OBJECTS_ZOTTAOS} ${OBJECTS_LIB} build/TestTimerEventF0.o -T${LINKERFILE}
build/TestTimerEventF0b.elf : ${OBJECTS_ZOTTAOS} ${OBJECTS_LIB} build/TestTimerEventF0b.o
	${CC} -mcpu=${CPU} -Wl,--gc-sections -mthumb -nostartfiles -o $@ ${OBJECTS_ZOTTAOS} ${OBJECTS_LIB} build/TestTimerEventF0b.o -T${LINKERFILE}
	