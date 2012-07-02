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
/* File UARTSimpleEcho.c: Receives characters that are then forwarded back to the sender.
** This application requires no special setting other than setting the system clocks.
** To build this application:
**    (1) Run "ZottaOS Configurator tool" and load the configuration file EXP430FR5739.zot
**        to generate the assembly and header files, and to get assembler function
**        OSInitializeSystemClocks that configures the clocks as shown below:
**          - MCLK sourced from the DCO (DCOCLK) divided by 1;
**          - ACLK sourced from the DCO (DCOCLK) divided by 8;
**          - SMCLK sourced from the DCO (DCOCLK) divided by 8;
**          - DCO set for factory calibrated value of 8.00 MHz(DCORSEL = 0, DCOFSEL = 3).
**    (2) You can then run a hyperterminal on a PC and transmit characters that are echoed
**        back.
** Tested on "MSP-EXP430FR5739 - Experimenter Board".
** Version identifier: March 2012
** Authors: MIS-TIC */

#include "ZottaOS.h"
#include "ZottaOS_UART.h"

/* UART transmit FIFO buffer size definitions */
#define UART_TRANSMIT_FIFO_NB_NODE    1
#define UART_TRANSMIT_FIFO_NODE_SIZE  10

/* UART entries to the interrupt table generated by ZottaOS Configurator Tool */
#if defined(OS_IO_USCI_A0_RX) && defined(OS_IO_USCI_A0_RX)
   #define UART_RX_VECTOR OS_IO_USCI_A0_RX
   #define UART_TX_VECTOR OS_IO_USCI_A0_TX
#else
   #error UART RX and TX interrupt vectors are not defined correctly
#endif

static BOOL UARTUserReceiveInterruptHandler(UINT8 data);
static void InitializeUARTHardware(void);

int main(void)
{
  WDTCTL = WDTPW + WDTHOLD;  // Disable watchdog timer

  /* Set the system clock characteristics */
  OSInitializeSystemClocks();

  /* Initialize ZottaOS I/O UART drivers */
  OSInitTransmitUART(UART_TRANSMIT_FIFO_NB_NODE,UART_TRANSMIT_FIFO_NODE_SIZE,UART_TX_VECTOR);
  OSInitReceiveUART(UARTUserReceiveInterruptHandler,UART_RX_VECTOR);

  /* Initialize USART or USCI 0 hardware */
  InitializeUARTHardware();

  /* Start the OS so that it runs the idle task, which puts the processor to sleep when
  ** there are no interrupts. */
  return OSStartMultitasking();
} /* end of main */


/* InitializeUARTHardware: Initializes USCI 0 hardware. */
void InitializeUARTHardware(void)
{
  P2SEL1 |= BIT0 + BIT1;
  P2SEL0 &= ~(BIT0 + BIT1);
  UCA0CTL1 |= UCSWRST;       // Put state machine in reset
  UCA0CTL1 |= UCSSEL__SMCLK; // Select SMCLK (1 MHz) as clock source
  UCA0BR0 = 6;               // 9600 Baud -> UCBRx = 6, UCBRSx = 0x20, UCBRFx = 8,
  UCA0BR1 = 0;               // UCOS16 -> 1
  UCA0MCTLW |= 0x2081;
  UCA0CTL1 &= ~UCSWRST;      // Initialize USCI state machine
  UCA0IE |= UCRXIE;          // Enable USCI_A0 RX interrupt
} /* end of InitializeUARTHardware */


/* UARTUserReceiveInterruptHandler: This function is called every time a new byte is
** received, and it simply copies the byte into the output queue of the UART. */
BOOL UARTUserReceiveInterruptHandler(UINT8 data)
{
  UINT8 *tmp;
  tmp = (UINT8 *)OSGetFreeNodeUART(UART_TX_VECTOR);
  *tmp = data;
  OSEnqueueUART(tmp,1,UART_TX_VECTOR);
  return TRUE; // Re-enable receive UART interrupt
} /* end of UARTUserReceiveInterruptHandler */