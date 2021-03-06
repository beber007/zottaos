/* Copyright (c) 2006-2010 MIS Institute of the HEIG affiliated to the University of
** Applied Sciences of Western Switzerland. All rights reserved.
** Permission to use, copy, modify, and distribute this software and its documentation
** for any purpose, without fee, and without written agreement is hereby granted, pro-
** vided that the above copyright notice, the following three sentences and the authors
** appear in all copies of this software and in the software where it is used.
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
/* File ZottaOSHardPA.h: Defines the user interface with ZottaOSHardPA.c for TI MSP430
**                       and CC430 families of ultralow-power microcontrollers.
** Version identifier: October 2011
** Authors: MIS-TIC
*/
/* Building an application typically involves 5 steps:
** (1) Initialize processor specifics.
** (2) Perform application specific initializations.
** (3) Create application tasks.
** (4) Set the timer and chip core source and frequency settings.
** (5) Start the application.
** Step (2) can be scattered throughout the code and intertwined between task creations,
** and steps (4) and (5) are the very last instructions executed in the main function.
** Once OSStartMultitasking() is called, the kernel gets the entire control of the pro-
** cessor and never returns. Memory usage of local variables in main is recycled and
** wiped-out. There is therefore no point in sacrificing readability for variable reuse.
** Each application task is defined as a separate function that must end with a call to
** OSEndTask().
**
** Typical example with a single task:
**       void Task1(void *argument);  // Task prototype
**
**       int main(void)
**       {
**          // do some initializations here and then create a periodic task, passing it
**          // the value 34 as a parameter. This task has a period and deadline of 10000
**          // clock-ticks.
**          OSCreateTask(Task1,0,10000,10000,(void *)34);
**          // Do other initializations, e.g. configure peripheral hardware modules to
**          // the requirements of the application.
**          // Step 3: Set the timer and chip core source and frequency settings. See
**          // one of the sample programs provided with this distribution.
**          return OSStartMultitasking();
**       } // end of main
**
**       void Task1(void *argument)
**       {
**          // do something useful here, i.e. what this task was designed for
**          // (int)argument is equal to 34
**          OSEndTask(); // Tell the kernel that the task finished
**       } // end of Task1
**
** Besides periodic tasks, ZottaOS also supports tasks with irregular arrival times that
** can be used to handle processing requirements of irregular random events. These tasks
** are globally called aperiodic tasks, and when they have hard deadlines and a minimum
** interarrival time restriction to guarantee that the task's deadline can always be met,
** they are called sporadic tasks. Only sporadic tasks are supported by ZottaOS and they
** are characterized by a minimum interarrival time and a processor load. The processor
** load is defined as the bandwidth that is set aside to handle the task.
** There are 2 ways to have aperiodic processing:
**  (1) Have the processing done in an interrupt handler;
**  (2) Trigger an event that schedules a new task instance providing this processing.
** The first possibility is self-explicit. The second possibility is slightly more invol-
** ved but allows extra flexibility. Aperiodic tasks are created by a specific function
** that indicates which event is associated with the task. When created or when termina-
** ted, an aperiodic task becomes blocked until it is signaled. Signaling is done via
** function OSScheduleSuspendedTask(eventId). Once signaled a new instance is created
** and scheduled. Because of their synchronization aspect, these tasks are also referred
** to as event-driven tasks.
** The following example illustrates these function calls.
**       void SignalerTask(void *argument);   // Periodic task prototype
**       void AperiodicTask(void *argument);  // Aperiodic task prototype
**       void *Event;                         // Event waited by an aperiodic task
**
**       int main(void)
**       {
**          // Create a periodic task that triggers an event.
**          OSCreateTask(SignalerTask,0,10000,10000,NULL);
**          // Create an event that will be triggered by SignalerTask
**          Event = OSCreateEventDescriptor();
**          // Create an aperiodic task that processes event Event
**          OSCreateSynchronousTask(AperiodicTask,100,2000,Event,NULL);
**          // Set the system clock characteristics
**          // Start the OS so that it starts scheduling the user tasks
**          return OSStartMultitasking();
**       } // end of main
**
**       void SignalerTask(void *argument)
**       {
**          // do something useful here, i.e. what this task was designed for
**          // Wake-up the aperiodic task
**          OSScheduleSuspendedTask(Event);
**          // do something useful here, i.e. what this task was designed for
**          OSEndTask(); // Tell the kernel that this instance is finished
**       } // end of SignalerTask
**
**       void AperiodicTask(void *argument)
**       {
**          // do something useful here, i.e. what this task was designed for
**          OSSuspendSynchronousTask();
**       } // end of AperiodicTask
*/

#ifndef ZOTTAOS_H
#define ZOTTAOS_H


/* DEBUGGING HELP -------------------------------------------------------------------- */
/* There are 3 error sources that are not trivial to locate when developing an applica-
** tion. The first is detecting when an application runs out of memory with function
** OSMalloc, and the second is when a (periodic or event-driven) task overruns its period
** (under Deadline Monotonic Scheduling) or its allotted load (under EDF) and is immedi-
** ately rescheduled. This last case is caused when the instantaneous load of a processor
** is higher than 100% and can be corrected by increasing the task's period. The third
** and final error source is caused when interrupt for a peripheral device occurs for
** which no ISR handler was configured.
** To help the developer, these errors go into an infinite loop if DEBUG_MODE is defined;
** the developer may then tap into the error source with a JTAG. */
#define DEBUG_MODE


/* POWER MANAGEMENT SCHEMES ---------------------------------------------------------- */
/* On this version of ZottaOS, there are 4 possible power scheme modes: DRA and DR_OTE
** only work with EDF*, DM_SLACK works with deadline monotonic scheduling and OTE is
** suitable for any scheduling algorithm. */
#define NONE      0  /* No power management, uses sleep mode only */
#define DRA       1  /* Dynamic Reclaiming Algorithm (works only with EDF*) */
#define OTE       2  /* One Task Extension */
#define DR_OTE    3  /* DRA with OTE */
#define DM_SLACK  4  /* Deadline monotonic scheduling with slack reclamation */

#define POWER_MANAGEMENT DR_OTE

/* All power management schemes (even NONE) can be combined with a static frequency set-
** ting. This is the frequency of the task that guarantees that all tasks meet their
** deadlines when they execute their WCET. */
//#define STATIC_POWER_MANAGEMENT


/* REAL-TIME SCHEDULING MODE --------------------------------------------------------- */
/* ZottaOS can work under 3 possible scheduling algorithms. Under EDF, tasks are sche-
** duled according to their deadlines, the earliest one being placed in the ready queue
** before later ones. When ZottaOS is used with power-awareness, it is important that the
** order of the tasks inserted in the ready queue be deterministic. This is because we
** compare the forecasted task termination time with its actual termination time. EDF* is
** a deterministic EDF in that when tasks have the same deadline, the one that was first
** released has higher priority; ties are settled by choosing the smaller TCB address.
** Note that without power management, it is best not to use EDF*.
** The final scheduling algorithm is DMS where tasks have a static priority based on
** their deadline. A shorter deadline has the highest priority. */
#define EARLIEST_DEADLINE_FIRST        1
#define EARLIEST_DEADLINE_FIRST_STAR   2
#define DEADLINE_MONOTONIC_SCHEDULING  3

/* The following define sets the scheduling algorithm to use. */
#define SCHEDULER_REAL_TIME_MODE EARLIEST_DEADLINE_FIRST_STAR

/* Correct or adjust the scheduling algorithm to be coherent with the power management
** choice. */
#if POWER_MANAGEMENT == DRA || POWER_MANAGEMENT == DR_OTE
   #undef SCHEDULER_REAL_TIME_MODE
   #define SCHEDULER_REAL_TIME_MODE    EARLIEST_DEADLINE_FIRST_STAR
#elif POWER_MANAGEMENT == DM_SLACK
   #undef SCHEDULER_REAL_TIME_MODE
   #define SCHEDULER_REAL_TIME_MODE    DEADLINE_MONOTONIC_SCHEDULING
#endif


#ifndef _ASM_

/* COMMON DATA TYPES THAT ARE COMPILER SPECIFIC -------------------------------------- */
typedef unsigned char BOOL;
typedef unsigned char UINT8;
typedef signed char INT8;
typedef unsigned int UINT16;
typedef signed int INT16;
typedef unsigned long UINT32;
typedef signed long INT32;
#ifndef NULL
  #define NULL 0
#endif
#ifndef TRUE
  #define TRUE 1
#endif
#ifndef FALSE
  #define FALSE 0
#endif


/* MISCELLANEOUS FUNCTIONS: LAUNCHING OF ZOTTAOS, MEMORY MANAGEMENT ----------- ------ */

#if POWER_MANAGEMENT != NONE || defined(STATIC_POWER_MANAGEMENT)
/* OSSetMinimalProcessorSpeed: Sets the minimal speed that the processor should run. This
** function should only be called prior to starting the kernel, i.e. before OSStartMulti-
** tasking and should never be called by an application task or by an interrupt ISR.
** Parameter: (UINT8) A valid speed setting defined by a OS_xxMHZ_SPEED value. */
void OSSetMinimalProcessorSpeed(UINT8 speed);
#endif

/* OSSetProcessorSpeed: Sets the speed that the processor should run.
** The function can be called anywhere from a task or from an interrupt ISR. It is provid-
** ed here so that an application, not using any dynamic power management, can manually
** adjust the core frequency. We however recommend that this task be left to ZottaOS.
** Parameter: (UINT8) A valid speed setting defined by a OS_xxMHZ_SPEED value. */
void OSSetProcessorSpeed(UINT8 speed);

/* OSStartMultitasking: This is the last function to call in main and it gives control of
** the processor to the kernel and to the application tasks. This function never returns
** and once the kernel gets control of the processor, it loops indefinitely switching
** between the user tasks. When no application task is created, the idle task gets the
** control of the processor, and the only means by which an application can be executed
** is solely by an interrupt approach.
** Returned value: Returns FALSE when an error occurs. */
BOOL OSStartMultitasking(void);

/* OSMalloc: This function replaces the regular malloc when used before calling OSStart-
** Multitasking. Memory allocations performed by this function can never be freed once
** allocated. The purpose of this function is to set-up memory blocks prior to executing
** the application tasks without having the burden of having a malloc function that keeps
** track of the free blocks.
** Important: On the MSP430 line of microcontrollers, this function should NEVER be used
** in the application tasks or in user defined ISRs and it can only be employed prior to
** calling OSStartMultitasking(). Although a standard C alloca function would be more ap-
** propriate, the MSP430 microcontroller does not have the necessary frame pointer needed
** to implement such a function.
** Parameter: (UINT16) size: requested memory block size in bytes.
** Returned value: (void *) Pointer to the base of the newly allocated block, and NULL in
** case of an error, i.e., when more than half of the memory is consumed by dynamic allo-
** cations done in main. */
void *OSMalloc(UINT16 size);


/* PERIODIC HARD REAL-TIME TASK FUNCTIONS -------------------------------------------- */
/* OSCreateTask: Creates a periodic task with its timing characteristics. The period of a
** task is expressed in terms of the system clock cycle which is defined as 2^30. The pe-
** riod of a task is then specified by 2 parameters: The number of full system clock
** cycles (periodCycles) and a cycle remainder (periodOffset). Hence, the actual period
** of a task is equal to
**            P = periodCycles * SystemClockCycle + periodOffset
** The maximum period that can be assumed by a period task is
**       68 years with a 32 kHz timer quartz
**        2 years with a 1 MHz quartz.
** Parameters:
**   (1) Pointer to the task's code;
**   (2) (INT32) wcet (worst-case execution time): Number of cycles needed to execute the
**       task; this parameter is only used for dynamic power management, and can be set
**       to 0 when no dynamic power management is in effect.
**   (3) (UINT16) periodCycles: Number of full 2^30 cycles.
**   (4) (INT32) periodOffset: Remaining period cycles, must be < 2^30.
**   (5) (INT32) deadline: Task's deadline (worstCaseExecutionTime <= deadline <= P);
**   (6) (void *) argument: An instance specific 32-bit value.
** Returned value: TRUE if the task creation was successful and FALSE otherwise. The
**   function fails when there's a memory allocation failure. */
BOOL OSCreateTask(void task(void *), INT32 wcet, UINT16 periodCycles, INT32 periodOffset,
                  INT32 deadline, void *argument);

/* _OSCreateTask: Same as OSCreateTask() but adds a static frequency index to the list of
** parameters. This frequency index is then used whenever the task is scheduled.
** Parameters:
**   (1) void task(void) Pointer to the task's code;
**   (2) (INT32) wcet: Worst-case execution time of a task instance; for all dynamic
**       power management schemes this value is in timer cycles after correction for the
**       nominal speed used by the task. This value is only used for advanced dynamic
**       power management schemes, and can be set to 0 when none is requested.
**   (3) (UINT16) periodCycles: Number of full 2^30 cycles.
**   (4) (INT32) periodOffset: Remaining period cycles, must be < 2^30.
**   (5) (INT32) deadline for the task's execution;
**   (6) (void *) argument: An instance specific 32-bit value.
**   (7) (UINT8) static frequency index of the task; this index corresponds to the nomi-
**       nal speed of task and is one of
**               {OS_8MHZ_SPEED, OS_12MHZ_SPEED, OS_20MHZ_SPEED, OS_25MHZ_SPEED}
** Returned value: TRUE if the task creation was successful and FALSE otherwise. The
**   function fails when there's a memory allocation failure. */
#ifdef STATIC_POWER_MANAGEMENT
BOOL _OSCreateTask(void task(void *), INT32 wcet, UINT16 periodCycles, INT32 periodOffset,
                   INT32 deadline, void *argument, UINT8 frequencyIndex);
#endif

/* OSEndTask: Very last instruction in an application periodic task. The purpose of this
** call is to schedule the next instance of the task. Any code after the call will never
** be executed. Also, without this call, the task instance can never terminate. */
void OSEndTask(void);


/* EVENT-DRIVEN (BLOCKING OR SYNCHRONOUS) TASK FUNCTIONS ----------------------------- */
/* OSCreateEventDescriptor: Creates and returns a descriptor with all the needed informa-
** tion to block and wake-up an aperiodic or event-driven task instance.
** Returned value: NULL when a memory failure occurs, or a valid descriptor. */
void *OSCreateEventDescriptor(void);

/* OSCreateSynchronousTask: Like OSCreateTask() but applied to event-driven (sporadic)
** tasks.
** Parameters:
**   (1) Pointer to the task's code;
**   (2) (INT32) wcet (worst-case execution time): Number of cycles needed to execute the
**       task; this parameter is only used for dynamic power management, and can be set
**       to 0 when no dynamic power management is in effect.
**   (3) (INT32) workload: equal to the task's worst-case execution time divided by the
**       available processor load; this is the same as the period and deadline of the
**       task; under EDF this task is scheduled when all previous synchronous tasks have
**       completed, but under deadline monotonic scheduling, this task behaves like any
**       other periodic task;
**   (4) (UINT8) Total processor utilization of all aperiodic tasks * 256; this parameter
**       is only used for EDF with dynamic power management, and can be set to 0 when no
**       dynamic power management is in effect or if Deadline Monotonic Schedule is in
**       effect.
**   (5) (void *) event: Descriptor returned by OSCreateSynchronousTask();
**   (6) (void *) arg: An instance specific 32-bit value.
** Returned value: TRUE if the task creation was successful and FALSE otherwise. The
**   function fails when there's a memory allocation failure. */
BOOL OSCreateSynchronousTask(void task(void *), INT32 wcet, INT32 workLoad,
                             UINT8 aperiodicUtilization, void *event, void *arg);

/* _OSCreateSynchronousTask: Same as OSCreateSynchronousTask() but adds a static frequen-
** cy index to the list of parameters. This frequency index is then used whenever the
** task is scheduled.
** Parameters:
**   (1) Pointer to the task's code;
**   (2) (INT32) wcet: (worst-case execution time): see OSCreateSynchronousTask();
**   (3) (INT32) workload: see OSCreateSynchronousTask();
**   (4) (UINT8) Total processor utilization of all aperiodic tasks * 256;
**   (5) (void *) event: see OSCreateSynchronousTask();
**   (6) (void *) arg: An instance specific 32-bit value.
**   (7) (UINT8) static frequency index of the task; this index corresponds to the nomi-
**       nal speed of task and is one of
**               {OS_8MHZ_SPEED, OS_12MHZ_SPEED, OS_20MHZ_SPEED, OS_25MHZ_SPEED}
** Returned value: TRUE if the task creation was successful and FALSE otherwise. The
**   function fails when there's a memory allocation failure. */
#ifdef STATIC_POWER_MANAGEMENT
BOOL _OSCreateSynchronousTask(void task(void *), INT32 wcet, INT32 workLoad, 
                              UINT8 aperiodicUtilization, void *event, void *arg,
                              UINT8 frequencyIndex);
#endif

/* OSSuspendSynchronousTask: The very last instruction of an event-driven task. This
 * function checks if the calling instance should restart a new instance (an event was
 * signaled) or if it should suspend itself until the event is signaled by another task
** which calls OSScheduleSuspendedTask(). */
void OSSuspendSynchronousTask(void);

/* OSScheduleSuspendedTask: Removes a blocked event-driven task that is waiting for an
 * event and schedules it. This function acts as a memorized signal. If there are no sus-
 * pended tasks at the moment of the call, the signal is stored and when an aperiodic
 * task terminates, it is immediately rescheduled. An aperiodic task can therefore signal
 * itself.
** Parameter: (void *) event descriptor holding the blocked tasks. */
void OSScheduleSuspendedTask(void *event);


/* INTERRUPT PROCESSING -------------------------------------------------------------- */
/* Associated with each peripheral device is a memory location called interrupt vector
** that contains the address of a descriptor for each device. The minimal content of the
** descriptor is the address of the interrupt service routine (ISR):
**     typedef struct myDescriptorDef {
**        void (*myInterruptHandler)(struct myDescriptorDef *);
**        // other information used by myInterruptHandler
**     } myDescriptorDef;
** When an interrupt occurs, the processor registers are saved onto the stack and control
** is then given to the ISR specified within the descriptor. This is equivalent to the
** following code snippet:
**     interruptVector[sourceEntry]->myInterruptHandler(&interruptVector[sourceEntry])
** Because the ISR receives a pointer to the descriptor, it can store in that descriptor
** all the information that is needed to process the interrupt without resorting to glob-
** al or static variables.
** The last instruction performed by an ISR must be OSEndInterrupt(), which restores the
** saved registers of the interrupted task instance before resuming the instance:
**     void myInterruptHandler(struct myDescriptorDef *myDescriptor)
**     {
**        // process the interrupt
**        OSEndInterrupt(); // return to the interrupted task instance
**     }
** On MSP430 and CC430, some peripheral devices also have their own interrupt vector that
** can be accessed via a dedicated register. In this case a specific ISR is registered
** per processed interrupt. For example, a port typically has 8 possible interrupt sour-
** ces, and each interrupt source can have its own entry.
**
** All ISR descriptors are stored in an internal table in the kernel. To limit the size
** of this table, we recommend using ZottaOS MSP430 Configurator Tool, which allows you
** to define the interrupts needed for your application and to automatically generate
** code to jump to specific ISR routines that you register with OSSetISRDescriptor. The
** valid entries to the internal table can be found in the generated ZottaOS_msp430XXX.h
** or ZottaOS_cc430XXX.h file.
**
** IMPORTANT: When an interrupt is raised, its source is masked. For single-sourced devi-
** ces, this is equivalent to disabling the whole device. However, for multiple-sourced
** devices, only the particular source within the device is masked. In both cases, it is
** important to re-enable the particular source before exiting the interrupt handler.
** Failure to do so prevents all future interrupts from the source that raised the inter-
** rupt.
*/
/* OSSetISRDescriptor: Associates an interrupt service descriptor with a specific inter-
 * rupt source of a peripheral device. The very first field of the descriptor is a point-
 * er to an interrupt service routine that is called when an interrupt from the device
 * and for the particular source should be handled.
** Parameters:
**   (1) (UINT8) entry: entry to the internal ISR interrupt vector;
**   (2) (void *) descriptor: ISR descriptor for the interrupt. */
void OSSetISRDescriptor(UINT8 entry, void *descriptor);

/* OSGetISRDescriptor: Returns the ISR descriptor associated with the internal table of
** the kernel. This function is made available to application tasks so that the inserted
** ISR descriptor can be returned to the user in order to read or to store his applica-
** tion data.
** Parameter: (UINT8) Index in the ISR device interrupt vector where the ISR descriptor
**    is held.
** Returned value: (void *) The requested ISR descriptor is returned. If no previous call
**    to OSSetISRDescriptor was previously made for the specified entry, the returned
**    value is undefined. */
void *OSGetISRDescriptor(UINT8 entry);

/* OSEndInterrupt: Very last instruction in an interrupt service routine. The purpose of
** this call is to resume the interrupted task instance. Any code after the call will
** never be executed. Also, without this call, the interrupt service routine can never
** terminate. */
void OSEndInterrupt(void);


/* INTER-TASK COMMUNICATION AND I/O BUFFERS FOR INTERRUPT HANDLERS ------------------- */
/* In a general setting, communicating tasks can be divided into 2 sets: those that pro-
** duce and post data, and those that read and process the posted data. The first tasks
** are called writers and the second readers. Note that the writer task can be a handler
** for an input device, and the reader can be a handler for an output device. Now if the
** posted datum can be posted into and copied from a global memory location, there is no
** need for the kernel to provided any assistance to the user application: readers and
** writers agree on the memory location, and each task access the memory location in a
** single uninterrupted machine cycle. On the other hand, if the data shared between the
** writer and the reader cannot be accessed in a single machine cycle, there is a need
** for a synchronizing mechanism. The simplest of these is the handshake protocol:
**
**   global variables: sharedLocation, status = free;
**   writerTask() {                          readerTask() {
**      produce(newData);                       while (status == free); // wait for data
**      while (status == busy);                 dataItem = sharedLocation
**      sharedLocation = newDataItem;           status = free;
**      status = busy;                          consume(dataItem);
**   }                                       }
**
** There are a number of problems with the above reader-writer scheme:
** (1) When one the tasks has a higher priority (e.g. a smaller deadline) than the other,
**     and the lowest priority task is preempted, the highest priority task has to wait
**     until it can write into or read from the shared location. This is an instance of
**     the so-called priority inversion problem.
** (2) If one of the tasks is an interrupt handler, while the task is blocked, waiting
**     for the shared location to become free, other interrupts cannot be serviced.
** One way to alleviate the above problem is to implement a fifo queue between the read-
** er and the writer; the writer appends new data items to the queue and the reader de-
** queues the data items from the head of the queue. This is the classic producer-consu-
** mer paradigm found in all concurrent textbooks and is extremely useful when there is a
** need to process all data produced. An API providing buffer management for this para-
** digm is provided in ZottaOS and described below.

** --------- CONCURRENT FIFO QUEUE API --------------------------------------------------
** Although simple in its use, this API contains some subtle features and restrictions,
** which we now explain.
** The very first operation to perform is to create the fifo queue. It is best to do this
** operation before starting ZottaOS since the queue should exist prior to performing any
** action on it and namely from a peripheral handler. Usually, memory blocks are dynami-
** cally created by an enqueuer task and later freed by a dequeuer task once the memory
** block is no longer needed. Memory allocation and reclamation are non-determinist oper-
** ations and are not recommended for real-time operating systems like ZottaOS. Instead,
** real-time systems favor pre-allocations of memory pools that are created once and for
** all. Hence, an enqueuer task first acquires a buffer from the kernel, fills it with
** data and then enqueues the buffer into the fifo queue. A dequeuer task does the oppo-
** site operations; it first dequeues a buffer from the queue, processes the data con-
** tained in the buffer and then releases the buffer so that it can later be reused by
** the enqueuer task.
** Restrictions:
** (1) When using multiple fifo queues, an available buffer taken from one queue, must be
**     released to the same queue (see OSInitFIFOQueue).
** (2) The data content size of a buffer is bounded when the fifo queue is created. */

/* OSInitFIFOQueue: Creates and initializes a concurrent FIFO queue that can be used for
** inter-task communications following a multiple producer and consumer paradigm. To
** guarantee that there will be sufficient memory when items are created and filled be-
** fore being enqueued, a free list of unused nodes (buffers) is also created and associ-
** ated with the created queue. To use these, a task should obtain a buffer via function
** OSGetFreeNodeFIFO, fill the buffer with its data and then enqueue the node via OSEn-
** queueFIFO into the same fifo queue it got the buffer from. When dequeued nodes are no
** longer required, these should be released via OSReleaseNodeFIFO so that the buffer can
** be recycled.
** Parameters:
**   (1) (UINT8) maximum number of nodes that can be enqueued. This value also corres-
**       ponds to the initial number of free buffers that are created and inserted into
**       the free list associated with the fifo queue.
**   (2) (UINT8) byte size of the buffers that are in the free list.
** Returned value: (void *) On success, the descriptor of the created queue is returned.
**   On memory allocation failure, the function returns NULL. */
void *OSInitFIFOQueue(UINT8 maxNodes, UINT8 maxNodeSize);

/* OSEnqueueFIFO: Inserts a node (buffer) into a fifo queue.
** Parameters:
**   (1) (void *) fifo queue descriptor that was created by function OSInitFIFOQueue;
**   (2) (void *) buffer to insert into the queue;
**   (3) (UINT16) useful buffer byte size. The value of this parameter is returned with
**       OSDequeueFIFO and should not be confused with the maximum size of each node cre-
**       ated when calling OSInitFIFOQueue. Note that if an application uses constant
**       sized information in its buffers or if an identifier can determine the useful
**       information contained within the node, this parameter can also denote a message
**       type.
** Returned value: (BOOL) TRUE if the node has been successfully inserted into the queue.
**   Otherwise the function returns FALSE to indicate that the queue is full at the time
**   of the call. This last result can occur if the inserted node was not obtained from
**   the queue it is being inserted into (also see OSGetFreeNodeFIFO). */
BOOL OSEnqueueFIFO(void *fifoQueue, void *node, UINT16 size);

/* OSDequeueFIFO: Removes and returns the oldest stored node (buffer) from a fifo queue.
** Parameters:
**   (1) (void *) fifo queue descriptor that was created by function OSInitFIFOQueue;
**   (2) (UINT16 *) output parameter denoting the useful data byte size of the returned
**       node, if the returned value is different than NULL (also see OSEnqueueFIFO).
** Returned value: NULL if the queue is empty at the time of the call and no node is re-
**   turned. Otherwise the returned value is the beginning address of the dequeued node.
**   Because the number of nodes is restricted, it is important that the returned nodes
**   are released as soon as possible with OSReleaseNodeFIFO.*/
void *OSDequeueFIFO(void *fifoQueue, UINT16 *size);

/* OSGetFreeNodeFIFO: Returns a free node that an application can fill with useful data
** and then insert into the fifo queue.
** Parameter: (void *) fifo queue descriptor that was created with function OSInitFIFO-
**    Queue.
** Returned value: NULL when all created nodes (buffers) are in use and none is available
**   at the time of the call. Otherwise the function returns the beginning address of the
**   buffer that can be filled. This same address is then transferred to OSEnqueueFIFO to
**   insert the buffer into the fifo queue. */
void *OSGetFreeNodeFIFO(void *fifoQueue);

/* OSReleaseNodeFIFO: Releases a node and returns it to the pool of available nodes used
** function OSGetFreeNodeFIFO. Once released, the node contents must neither be accessed
** or modified.
** Parameters:
**   (1) (void *) fifo queue descriptor that was created by function OSInitFIFOQueue;
**   (2) (void *) node to release.
** Returned value: None. */
void OSReleaseNodeFIFO(void *fifoQueue, void *releasedNode);


/* --------- ASYNCHRONOUS READER-WRITER PROTOCOL API --------------------------------- */
/* When dealing with sensor measurement readings, it turns out that the above FIFO scheme
** is inconvenient. Besides the problem of having a buffer overflow when the reader runs
** less frequently than the writer, for interrupt handlers that periodically read data
** from sensors and append the read data into the queue to be processed by an application
** task, the oldest and least useful data are processed first.
** Because the reader and writer protocol is the most useful protocol for a sensor OS
** like ZottaOS, we also chose to provide an API to allow concurrent, and thus asynchro-
** nous, reading and writing into the data items. There are two different mechanisms to
** achieve this goal and both use multiple data items, called slots. The 3-slotted mecha-
** nism uses the least possible memory but is somewhat slower. The 4-slotted mechanism
** (Simpson's algorithm) trades speed for memory usage. In both mechanisms, a writer al-
** ways finds an available slot to write into, and the reader always gets the latest
** fully-completed data. */

/* Multiple slotted buffer type
** The 4-slot mechanism is appropriate for small-sized data requiring fast transfer bet-
** ween a writer task and a reader task. This mechanism does not use any atomic instruc-
** tion and is well targeted for processors like MSP430.
** The 3-slot mechanism is more memory efficient but somewhat slower because it uses
** atomic instructions. */
#define OS_BUFFER_TYPE_4_SLOT 0
#define OS_BUFFER_TYPE_3_SLOT 1

/* OSInitBuffer: Creates a 3- or 4-slot buffer that can be used for I/Os and inter-task
** communications. Note that the scheme only applies to a single writer task, and to a
** single concurrent reader task. The appropriate time to call this function is in main.c
** prior to calling OSStartMultitasking(), and to save the returned value in a global
** variable that is then shared by the communicating application tasks.
** Parameters:
**   (1) (UINT8) required slot size; once the writer has completed a full slot (see
**          function OSWriteBuffer(), the slot become eligible for reading. It is thus
**          recommended that the specified size be equal to those that are read as a
**          chunk by one of the provided reading functions to prevent data overlap.
**   (2) (UINT8) buffer slot type: this can either be OS_BUFFER_TYPE_3_SLOT or
**          OS_BUFFER_TYPE_4_SLOT;
**   (3) (void *) an optional event queue, which when specified can be used to signal a
**                task.
** Returned value: (void *) On success, the function returns a descriptor holding all
** state and buffer information that can be used for a device or for task communications.
** On memory allocation failure, the function returns NULL. */
void *OSInitBuffer(UINT8 slotSize, UINT8 bufferSlotType, void *eventQueue);

/* OSWriteBuffer: Copies the data contained in a slot into the current writer buffer.
** Once the current data slot becomes full, i.e. when it reaches the size specified at
** creation time with OSInitBuffer(), the writer-slot becomes available to the reader,
** and a new buffer slot is chosen for the next time this function is called. Should the
** amount of data exceed the available space in the current writer-slot, only the first
** bytes that do not cause a buffer overflow are considered.
** Parameters:
**   (1) (void *) a buffer descriptor created by OSInitBuffer();
**   (2) (UINT8 *) data to copy into the current writer-slot;
**   (3) (UINT8) number of data bytes to consider from the 2nd argument.
** Returned value: (UINT8 ) number of bytes accepted into the slot. A zero value indica-
** tes an error because there is always a free slot to copy the passed data. Furthermore,
** the returned value may not exceed the buffer size specified at buffer creation time
** with OSInitBuffer(). */
UINT8 OSWriteBuffer(void *descriptor, UINT8 *data, UINT8 size);

/* There are 2 sets of modes that can be used to read the data posted by a writer. All
** sets can be combined and provide a rich set of functions that can be used for all ap-
** plications communicating by means of fixed data sizes.
** The first set specifies whether the data should be read only once or if multiple reads
** of the same data is permitted. A flag that is used in conjunction with the two func-
** tions described below specifies this mode:
** OS_READ_ONLY_ONCE indicates that only the last and most recently unread slot can be
** read. Calling a reading function with this flag marks the slot so that the same slot
** can no longer be read if this flag is specified. This mode is useful when the reader
** needs to consume the data only once.
** OS_READ_MULTIPLE indicates that the most recently available slot should be read. Call-
** ing a reading function with this flag does not mark the buffer and the same buffer can
** be read more than once. If the most recent slot is marked by a previous call with flag
** OS_READ_ONLY_ONCE, reading functions with flag OS_READ_MULTIPLE still succeed and re-
** turn the data held in the slot, however, the slot state remains untouched. This mode
** is useful when the data is posted onto a bulletin board to be read continuously by the
** reader task. */
#define OS_READ_ONLY_ONCE  0
#define OS_READ_MULTIPLE   1

/* The second set of modes specifies whether the data is to be copied into a buffer pro-
** vided by the calling task or whether a pointer to that data should be returned. This
** mode is specified by a specific function. The first mode is useful when handling small
** sized data, and the second mode is faster when huge chunks of data.
*/
/* OSGetCopyBuffer: Returns a copy of the data held in the most recently filled buffer
** slot held in the buffer descriptor.
** Parameters:
**   (1) (void *) general buffer descriptor created and returned by OSInitBuffer();
**   (2) (UINT8) read mode: This can be OS_READ_ONLY_ONCE or OS_READ_MULTIPLE;
**   (3) (UINT8 *) data buffer where data is to be copied. This buffer should be greater
**          or equal to the buffer size specified when the general buffer descriptor was
**          created by OSInitBuffer().
** Returned value: (UINT8) Number of bytes copied into the specified data buffer. */
UINT8 OSGetCopyBuffer(void *descriptor, UINT8 readMode, UINT8 *data);

/* OSGetReferenceBuffer: Returns a pointer to the most recent data buffer slot held in
** the descriptor that can be read.
** Parameters:
**   (1) (void *) general buffer descriptor created and returned by OSInitBuffer();
**   (2) (UINT8) read mode: This can be OS_READ_ONLY_ONCE or OS_READ_MULTIPLE;
**   (3) (UINT8 **) pointer to the data buffer.
** Returned value: (UINT8) The return value is the number of bytes that is available in
** the returned data buffer. When there is nothing to read or if the data has already
** been read (read mode = OS_READ_ONLY_ONCE), 0 is returned and the data buffer passed as
** argument is set to NULL. */
UINT8 OSGetReferenceBuffer(void *descriptor, UINT8 readMode, UINT8 **data);


/* ATOMIC INSTRUCTIONS --------------------------------------------------------------- */
/* OSUINT8_LL and OSUINT16_LL: The LL functions are used in conjunction with their cor-
** responding SC functions call to provide synchronization support for ZottaOS. The LL/SC
** pair of functions works very much like simple a get and a set function.
** The LL functions, in addition of returning the contents of a memory location, have the
** effect of setting a user transparent reservation bit. If this bit is still set when an
** SC function is executed, the store of SC occurs; otherwise the store fails and the
** specified memory location is left unchanged (see OSUINT8_SC and OSUINT16_SC).
** The LL function is semantically equivalent to the atomic execution of the following
** code:
**    TYPE OSTYPE_LL(TYPE *memAddr) {
**       reserveBit = TRUE;
**       return *memAddr;
**    }
** where TYPE can be one of UINT8 or UINT16.
** Parameter: (TYPE *) Address to a memory location that holds the value to read, where
** TYPE can be one of UINT8 or UINT16.
** Returned value: (TYPE) The contents stored in the memory location specified by the pa-
**    rameter. */
UINT8 OSUINT8_LL(UINT8 *memAddr);
UINT16 OSUINT16_LL(UINT16 *memAddr);

/* OSUINT8_SC and OSUINT16_SC: Store Memory Location if Reserved. If the reservation bit
** is set by a previous call to an LL function, the second parameter is written into the
** memory location specified by the first parameter.
** SC functions are semantically equivalent to the atomic execution of the following code
**    BOOL OStype_SC(TYPE *memAddr, TYPE newValue) {
**       if (reserveBit) {
**          *memAddr = newValue
**          reserveBit = FALSE;
**          return TRUE;
**       }
**       else
**          return FALSE;
**     }
** where TYPE is one of UINT8 or UINT16.
** Parameters:
**   (1) (TYPE *) Address to a memory location that holds the value to modify, where TYPE
**                can be one of UINT8 or UINT16.
**   (2) (TYPE) Value to insert into the memory location specified by the above parameter
**              if and only if the reservation bit is still set.
** Returned value: (BOOL) TRUE if the store took place and FALSE otherwise. */
BOOL OSUINT8_SC(UINT8 *memAddr, UINT8 newValue);
BOOL OSUINT16_SC(UINT16 *memAddr, UINT16 newValue);

#endif /* _ASM_ */
#endif /* ZOTTAOS_H */
