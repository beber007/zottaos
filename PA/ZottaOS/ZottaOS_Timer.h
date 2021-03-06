/* Copyright (c) 2006-2011 MIS Institute of the HEIG affiliated to the University of
** Applied Sciences of Western Switzerland. All rights reserved.
** IN NO EVENT SHALL THE MIS INSTITUTE NOR THE HEIG NOR THE UNIVERSITY OF APPLIED
** SCIENCES OF WESTERN SWITZERLAND BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL,
** INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS
** DOCUMENTATION, EVEN IF THE MIS INSTITUTE OR THE HEIG OR THE UNIVERSITY OF APPLIED
** SCIENCES OF WESTERN SWITZERLAND HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
** THE MIS INSTITUTE, THE HEIG AND THE UNIVERSITY OF APPLIED SCIENCES OF WESTERN SWIT-
** ZERLAND SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFT-
** WARE PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE MIS INSTITUTE NOR THE HEIG
** AND NOR THE UNIVERSITY OF APPLIED SCIENCES OF WESTERN SWITZERLAND HAVE NO OBLIGATION
** TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
*/
/* File ZottaOS_Timer.h: Defines the interface between the hardware timer and the ZottaOS
**                       family of real-time kernels.
** Version identifier: June 2011
** Authors: MIS-TIC
*/

#ifndef ZOTTAOS_TIMER_H
#define ZOTTAOS_TIMER_H

/* Keeping track of time and performing all its related events is a tricky business. In
** ZottaOS we divide these 2 actions in two. The first only keeps track of the time (the
** wall clock) with a dedicated hardware timer and is implemented by an ISR. The second
** part, which is triggered by the hardware timer ISR, takes care of all the actions that
** need to be done when the wall clock progresses. This includes processing the instance
** arrivals that occur at the current time, preventing temporal variable overflow and
** programming the next hardware timer interrupt. This second part is implemented as a
** software ISR and it may be triggered any time and as often as there is an event to
** process.
** This scheme provides 2 main advantages:
** (1) The latency associated with the time events can be kept to a minimal (18 machine
**     cycles for MSP430) as only the keeping of time executes in a critical section.
** (2) The scheme can be ported to any microcontroller independently of whether it has
**     prioritized interrupts or not. */

/* _OSInitializeTimer: Sets up the hardware timer to count in up mode without starting
** it. After the call, the timer is ready to produce an interrupt as soon as it is order-
** ed to start counting and at the first interrupt, the current wall clock is set to 0. */
void _OSInitializeTimer(void);

/* _OSStartTimer: Orders the hardware timer to begin counting. */
void _OSStartTimer(void);

/* _OSGenerateSoftTimerInterrupt: Provokes a software timer interrupt. This function is
** called when an event occurs and should be scheduled. */
void _OSGenerateSoftTimerInterrupt(void);

/* _OSSetTimer: Sets the timer to interrupt at a specified value. This function is called
** by the software timer ISR to set the next time event.
** Parameter: (INT32) nextTimeInterval: time duration until the next hardware timer in-
**                    terrupt, i.e. the time of the next event minus the current time. */
void _OSSetTimer(INT32 nextTimeInterval);

#if ZOTTAOS_VERSION == ZOTTAOS_VERSION_HARD_PA && POWER_MANAGEMENT != NONE
   /* _OSGetTimerCounter: Return the current value of the timer counter. */
   UINT16 _OSGetTimerCounter(void);
#endif

#endif /* ZOTTAOS_TIMER_H */
