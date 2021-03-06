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
** To set up this sample program, you should:
** (1) Run ZottaOS Configurator Tool for your MSP430 and select the TX and RX interrupts
**     of either USART0, USCIA0 or USCI_A0 (depends on your particular MSP), and set ACLK
**     to XT1 with a watch crystal (MCLK can be set to any reasonable frequency):
**              MSP430
**        -----------------
**       |             XIN |-
**       |                 | 32,768 kHz
**       |            XOUT |-
**       |                 |
**       |             TX  |----------->
**       |                 | 9600 - 8N1
**       |             RX  |<-----------
**
**   (2) Configure the pins I/O pins for your MSP (see lines 48 - 50 below).
**   (3) You can then run a hyperterminal on a PC and transmit characters that are echoed
**       back.
** Version identifier: March 2012
** Authors: MIS-TIC */

#include "ZottaOS.h"
#include "ZottaOS_UART.h"

/* UART transmit FIFO buffer size definitions */
#define UART_TRANSMIT_FIFO_NB_NODE    1
#define UART_TRANSMIT_FIFO_NODE_SIZE  10

/* These parameters must be modified to accommodate the specific msp430 that is used.
** Example for msp430F2471 or msp430F149 */
#define UART0_PIN_SEL_REGISTER  P3SEL
#define UART0_TX_PIN_NUMBER BIT4
#define UART0_RX_PIN_NUMBER BIT5

/* UART entries to the interrupt table generated by ZottaOS Configurator Tool */
#if defined(OS_IO_USART0RX) && defined(OS_IO_USART0TX)
   #define UART_RX_VECTOR OS_IO_USART0RX
   #define UART_TX_VECTOR OS_IO_USART0TX
#elif defined(OS_IO_USCIA0RX) && defined(OS_IO_USCIA0TX)
   #define UART_RX_VECTOR OS_IO_USCIA0RX
   #define UART_TX_VECTOR OS_IO_USCIA0TX
#elif defined(OS_IO_USCI_A0_RX) && defined(OS_IO_USCI_A0_RX)
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
  return OSStartMultitasking(NULL,NULL);
} /* end of main */


/* InitializeUARTHardware: Initializes USART or USCI 0 hardware. */
void InitializeUARTHardware(void)
{
  UART0_PIN_SEL_REGISTER |= UART0_RX_PIN_NUMBER + UART0_TX_PIN_NUMBER;
  #if defined(OS_IO_USART0RX) && defined(OS_IO_USART0TX)
     #if defined (__MSP430F122__) || defined (__MSP430F1222__)
        ME2 |= UTXE0 + URXE0;  // Enable USART0 TXD/RXD
     #else
        ME1 |= UTXE0 + URXE0;  // Enable USART0 TXD/RXD
     #endif
     UCTL0 |= CHAR;            // 8-bit characters
     UTCTL0 |= SSEL0;          // UCLK = ACLK (32,768 kHz)
     UBR00 = 0x03;             // 9600 Baud -> UxBR0 = 3, UxBR1 = 0, UxMCTL = 0x4A
     UBR10 = 0x00;
     UMCTL0 = 0x4A;
     UCTL0 &= ~SWRST;          // Initialize USART state machine
     #if defined (__MSP430F122__) || defined (__MSP430F1222__)
        IE2 |= URXIE0;         // Enable USART0 RX interrupt
     #else
        IE1 |= URXIE0;         // Enable USART0 RX interrupt
     #endif
  #elif defined(OS_IO_USCIA0RX) && defined(OS_IO_USCIA0TX)
     UCA0CTL1 |= UCSWRST;      // Put state machine in reset
     UCA0CTL1 |= UCSSEL_1;     // Select ACLK (32,768 kHz) as clock source
     UCA0BR0 = 3;              // 9600 Baud -> UCBRx = 3, UCBRSx = 3, UCBRFx = 0
     UCA0BR1 = 0;
     UCA0MCTL |= UCBRS_3 + UCBRF_0;
     UCA0CTL1 &= ~UCSWRST;     // Initialize USCI state machine
     IE2 |= UCA0RXIE;          // Enable USCIA0 RX interrupt
  #elif defined(OS_IO_USCI_A0_RX) && defined(OS_IO_USCI_A0_TX)
     UCA0CTL1 |= UCSWRST;      // Put state machine in reset
     UCA0CTL1 |= UCSSEL_1;     // Select ACLK (32,768 kHz) as clock source
     UCA0BR0 = 3;              // 9600 Baud -> UCBRx = 3, UCBRSx = 3, UCBRFx = 0
     UCA0BR1 = 0;
     UCA0MCTL |= UCBRS_3 + UCBRF_0;
     UCA0CTL1 &= ~UCSWRST;     // Initialize USCI state machine
     UCA0IE |= UCRXIE;         // Enable USCI_A0 RX interrupt
  #else
     #error UART RX and TX vector are not define correctly
  #endif
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
