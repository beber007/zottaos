/* Copyright (c) 2006-2009 MIS Institute of the HEIG affiliated to the University of
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
*/
/* File ZottaOS.c: Kernel implementation for TI MSP430F2XX family of microcontrollers.
** Version identifier: July 2009
** Compiler and linker: CCE v3.1 Build: 3.2.4.3.8 (www.ti.com)
** Authors: MIS-TIC
*/
/* CURRENT CHARACTERISTICS
** (1) Non-blocking implementation of priority lists. In a previous version of the ker-
**     nel, task context switching was simply done by masking all interrupts. Unsatisfied
**     by the fact that the context switching may be long, we entirely redesigned the
**     priority queue.
** (2) Configurable source oscillator.
** (3) Common execution stack for all tasks to minimize memory RAM usage.
** (4) API for interrupt handling whereby application processed interrupt routines can be
**     dynamically inserted.
** (5) Implements two RS232 I/O communication ports. (need to define flag UART_IO)
** (6) Advanced power management schemes other than a simple sleep mode.
** (7) Periodic tasks with maximum periods of 68 years with a 32 kHz timer quartz and 2
**     years with a 1 MHz quartz.
** (8) Event-driven tasks that can also be used to synchronize tasks of a precedence
**     process graph or be started from an external event.
**
** LIMITATIONS
** (1) TIMER_A3 cannot be used as it is dedicated for the interval-timer.
** (2) Digital I/O port 2 pins 6 and 7 cannot be used as these are treated as software
**     interrupts.
*/
#include "ZottaOS.h"
#include "ZottaOS_Timer.h"

#define STATIC static
#define LIB_EXTERN 

/*%DEFINITIONS_BLOCK_START%*/

/* COMMON DATA TYPES AND DEFINES THAT ARE ARCHITECTURE SPECIFIC */
/* Non-blocking algorithms use a marker that needs to be part of the address */
typedef UINT16 UINTPTR;
#define MARKEDBIT    0x8000u
#define UNMARKEDBIT  0x7FFFu


/* TASK STATE BIT DEFINITION
**
**  STATE_INIT   <-  STATE_TERMINATED
**       |                    ^
**       V                    |
**  STATE_RUNNING  -> STATE_ZOMBIE
** In state INIT, a task is in the ready queue but hasn't yet started it's execution.
** In the RUNNING state, the task is also in the ready queue and has begun its execution.
** The difference between these two states lies with the context switch. In the former
** state there are no registers to restore. This is not the case in the READY state.
** The second difference lies with the (m,k)-firm deadlines. When an optional task (see
** further down) comes at the head of the ready queue, a feasibility test is done to de-
** termine whether or not the task is schedule. This test is only done for optional tasks
** in state INIT.
** When a task terminates its execution, it becomes a ZOMBIE and it needs to stay in the
** ready queue until it finishes its preparation because otherwise and when a timer in-
** terruption occurs, the task will never have the opportunity to finish. In ZOMBIE state,
** the task cannot begin its next period until it is removed from the ready queue. To
** circumvent this problem, a timer interrupt asserts that an active zombie task is no
** longer in the ready queue. A task with state ZOMBIE and no longer in the ready queue
** is considered to be in state TERMINATED, which is a fictitious state. */
#define STATE_INIT          0x00 /* Must be equal to zero */
#define STATE_RUNNING       0x01 /* These values are also used in ZottaOS_a_CortexM3.S */
#define STATE_ZOMBIE        0x02
#define STATE_TERMINATED    0x04
/* Because the task structure is different for event-driven tasks, we need to distinguish
** them. By default all tasks are periodic unless specified. */
#define TASKTYPE_BLOCKING       0x08
/* Flag indicating whether an event-drive task is currently in the task arrival queue.
** This bit is toggled when inserting into and removing from the arrival queue. */
#if SCHEDULER_REAL_TIME_MODE != DEADLINE_MONOTONIC_SCHEDULING
   #define ARRIVALTYPE_BLOCKING 0x10
#endif


/* CONSTANT PART OF A PERIODIC TASK CONTROL BLOCK */

typedef struct TCB {
  struct TCB *Next[2];                 // Next TCB in the list where this task is located
                                       // [0]: ready queue link, [1]: arrival queue link
  UINT8 TaskState;                     // Current state of the task
  INT32 ArrivalTimeLow;                // Remaining time before reappearing
  void (*TaskCodePtr)(void *);         // Pointer to the first instruction of the code task
                                       // (Needed to reinitialize a new instance execution)
  #if SCHEDULER_REAL_TIME_MODE == DEADLINE_MONOTONIC_SCHEDULING
     UINT8 Priority;                   // Current instance running priority
  #endif
  void *Argument;                      // An instance specific 32-bit value
  INT32 PeriodLow;                     // Interarrival period >= WCET (user input)
  UINT16 PeriodHigh;                   // Number of full 2^30 cycles of the interarrival period
  #if SCHEDULER_REAL_TIME_MODE != DEADLINE_MONOTONIC_SCHEDULING
     INT32 NextDeadline;               // Current instance deadline
     INT32 Deadline;                   // Task's deadline <= Period (user input)
  #endif
  UINT16 ArrivalTimeHigh;              // Number of full 2^30 cycles of the next arrival time
} TCB;


/* List head sentinel */
typedef struct {
  TCB *Next[2];                // [0]: first TCB in the ready queue link Next
                               // [1]: first TCB in the arrival queue link
} TCB_HEAD;

/* List tail sentinel */
typedef struct {
  TCB *Next[2];                // [0] and [1] are always equal to NULL
  UINT8 TaskState;             // Current state of the idle task
  INT32 ArrivalTimeLow;        // Remaining time before reappearing
  void (*TaskCodePtr)(void *); // Pointer to the first instruction of the idle task
  #if SCHEDULER_REAL_TIME_MODE == DEADLINE_MONOTONIC_SCHEDULING
     UINT8 Priority;           // Task priority. Corresponds to the number of application tasks.
  #endif
} TCB_TAIL;


/* EVENT-DRIVEN OR SYNCHRONOUS TASK CONTROL BLOCK */
struct FIFOQUEUE;
typedef struct ETCB {
  struct ETCB *Next[2];          // Next TCB in the list where this task is located
                                 // [0]: ready queue link, [1]: event queue link
  UINT8 TaskState;               // Current state of the task
  INT32 ArrivalTimeLow;          // Remaining time before reappearing
  void (*TaskCodePtr)(void *);   // Pointer to the first instruction of the code task
                                 // (Needed to reinitialize a new instance execution)
  #if SCHEDULER_REAL_TIME_MODE == DEADLINE_MONOTONIC_SCHEDULING
     UINT8 Priority;             // Static task priority
  #endif
  void *Argument;                // An instance specific 32-bit value
  #if SCHEDULER_REAL_TIME_MODE == DEADLINE_MONOTONIC_SCHEDULING
     INT32 PeriodLow;            // Inter-arrival period >= WCET (user input)
  #else
     INT32 WorkLoad;             // Equal to WCET / (available processor load)
     INT32 NextDeadline;         // Key criteria for EDF scheduling
  #endif
  struct FIFOQUEUE *EventQueue;  // Pointer to the event queue of this task
  struct ETCB *NextETCB;         // link to the next created ETCB
} ETCB;

/* Array based implementation of a FIFO queue that can be used as an event queue or as an
** ordinary FIFO queue. */
typedef struct FIFOQUEUE {
  UINTPTR *Q;              // Array of queued items
  void *PendingOp;         // Posted operation for the queue (dequeue or enqueue op.)
  UINT16 Head, Tail;       // Current head and tail indices
  UINT16 MaxIndex;         // Value of Head or Tail at wrap-around
  UINT8 QueueLength;       // Circular array queue size
} FIFOQUEUE;

typedef struct DEQUEUE_DESCRIPTOR {
  UINTPTR SlotPropose, SlotReturn;
  UINT16 HeadPropose, Head;
  volatile BOOL Done;
} DEQUEUE_DESCRIPTOR;

typedef struct ENQUEUE_DESCRIPTOR {
  UINTPTR SlotPropose, SlotReturn;
  UINTPTR Item;
  UINT16 TailPropose, Tail;
  volatile BOOL Done;
} ENQUEUE_DESCRIPTOR;


/* TASK SCHEDULING QUEUES
** The ready and arrival queues are implemented as wait-free concurrent linked-lists of
** TCBs. Compared to lock-free or non-blocking algorithms, the implemented queueing oper-
** ations are wait-free algorithms that take a bounded amount of time. Before performing
** an operation, enqueuers and dequeuers first check for any pending operation under way
** for the queue. If one exists, that operation is first completed. After this step, the
** enqueuer or dequeuer posts the operation it intends to do before actually starting the
** operation. If a higher priority enqueuer or dequeuer interrupts the queueing opera-
** tion, this task will complete the operation on behalf of the interrupted task. The
** point to observe and that meets the requirements of wait-free algorithms is that a
** task may need to complete the operation posted from a lower priority task at most once
** during its execution. The operations are also done in such a way that when completing
** the pending operation of another task, the operation starts where the interrupted task
** left off. */
/* All queues use the same head and tail sentinel blocks and there are 2 or 3 lists to
** manage the scheduling of the tasks:
** Ready queue: The first item of the list points to the currently active task instance
** (_OSActiveTask). */
#define READYQ    0
/* Queue of task instances that have yet to arrive: When a periodic task instance termi-
** nates, it places its TCB in the arrival queue which stays in the queue until its pe-
** riod expires at which time a new instance is created. */
#define ARRIVALQ  1
/* Queue of tasks that are blocked for an event: Event-driven tasks can be blocked, run-
** ning or waiting. In the blocked state, the task is placed in the queue associated with
** the event using the same link as ARRIVALQ. An event-drive task can also be in the
** arrival queue if it has not expired its processor load with it's last execution. */
#define BLOCKQ    ARRIVALQ

/*%DEFINITIONS_BLOCK_END%*/
/*%VARIABLES_BLOCK_START%*/

LIB_EXTERN TCB *_OSQueueHead = NULL;
LIB_EXTERN TCB_TAIL *_OSQueueTail = NULL;


/* Pointer to the current active task: This is the task that gets the processor when
** running the application tasks. */
LIB_EXTERN TCB *_OSActiveTask = NULL;

/*%VARIABLES_BLOCK_END%*/
/*%DEFINITIONS_BLOCK_START%*/

/* IsMarkedReference: Determines whether a node is marked for delete. This is could have
** an inline function but a macro is as simple. Note that markers use the MSB of the TCB
** address which is a negative integer value when marked.
** Parameter: node to test (should be TCB *)
** Returned value: 1 or 0 for TRUE and FALSE. */
#define IsMarkedReference(node) ((UINTPTR)node & MARKEDBIT)

/* GetMarkedReference: Returns the marked address of the TCB.
** Parameter: TCB to mark.
** Returned value: (TCB *) Marked address of corresponding to this TCB. Note that
** this function is processor dependent and uses the MSB of the TCB address as a marker.
** Hence, TCBs must be in the lower half memory locations. */
#define GetMarkedReference(node) (TCB *)((UINTPTR)node | MARKEDBIT)

/* GetUnmarkedReference: Returns the address of the next TCB in the list. Because this
** address also serves to mark the TCB as being deleted; when traversing the linked-list
** it is necessary to remove the marker before using the next address.
** Note that the marker is processor-dependent.
** Parameter: current TCB.
** Returned value: (UINTPTR) next TCB in the list. */
#define GetUnmarkedReference(node) (UINTPTR)((UINTPTR)node & UNMARKEDBIT)

/*%DEFINITIONS_BLOCK_END%*/
/*%VARIABLES_BLOCK_START%*/

/* TASK EXECUTION STACK
** Pointer to the stack base of currently running task: Local variables used in the task
** are located between the SP register and the stack base, and when resuming the previous
** task, we need to start popping registers from the base. */
LIB_EXTERN void *_OSStackBasePointer;

/*%VARIABLES_BLOCK_END%*/
/*%DEFINITIONS_BLOCK_START%*/

/* To avoid overflow of temporal variables, periodically these variables are shifted once
** Time is greater or equal to a given value. This value must be equal to 2^(16+i) where
** i > 0 (2^16 = 65536). The recommended value is i = 14. */
#define INT32_MAX 2147483647   /* 2^31 - 1 */

/*%DEFINITIONS_BLOCK_END%*/
/*%VARIABLES_BLOCK_START%*/

LIB_EXTERN STATIC const INT32 ShiftTimeLimit = 1073741824; // = 2^30 (i = 14)


#if SCHEDULER_REAL_TIME_MODE == DEADLINE_MONOTONIC_SCHEDULING
  /* EVENT-DRIVEN TASKING UNDER DEADLINE MONOTONIC SCHEDULING
  ** Event-driven tasks are characterized by a tuple (wcet,workload) where the workload
  ** is the equivalent of the task's deadline and period. Because of the priority assign-
  ** ment, the workload fixes the task's scheduling priority. However, a new instance may
  ** not begin before the end of the previous period to guarantee the task set is sched-
  ** ulable. Hence, when a task completes it's current instance, it joins a blocking
  ** queue to wait for the next event; when the event occurs, it must first finish it's
  ** period. This is done in the arrival queue of periodic tasks. Because the timer peri-
  ** odically resets the system time, the earliest starting time of each event-driven
  ** task must also be adjusted. To do this, we need to access them. */
  LIB_EXTERN STATIC ETCB *SynchronousTaskList = NULL;
#else
  /* APERIODIC SERVER SCHEDULING DEFINITIONS UNDER EDF
  ** Event-driven tasks are scheduled according to a sporadic server model, i.e., (1)
  ** these tasks only run during the slack time left by the periodic tasks, (2) if
  ** U(hard) denotes the processor utilization of all the hard periodic tasks, i.e. the
  ** sum of WCET_i/period_i of all tasks i, then the remaining utilization 1-U(hard) can
  ** be given to the synchronous tasks. The computed deadline for the task is
  **           d(j) = max(d(j-1),currentTime) + WCET/U(task)
  ** where U(task) < 1 - U(hard). */
  LIB_EXTERN STATIC INT32 SynchronousTaskDeadlines = 0;  // denotes d(j) in the above
  /* SynchronousTaskList serves the same purpose as for DM scheduling. */
  LIB_EXTERN STATIC ETCB *SynchronousTaskList = NULL;
#endif
/* RescheduleSynchronousTaskList is a temporary LIFO queue that is used to transfer event
** driven tasks that can be scheduled into the ready queue. Tasks in this queue are in-
** serted by a signaling task or by an event-driven task when this last task needs to re-
** start itself. The rational behind this queue is to allow the timer interrupt insert
** the tasks into ready queue because these tasks may take precedence over the caller.
** Hence, the caller needs to save its context before releasing the processor. This is
** done by invoking a timer interrupt. */
LIB_EXTERN ETCB *_OSRescheduleSynchronousTaskList = NULL;


/* Indicates when the context of the interrupted task must not be saved because the task
** is marked for delete and will not resume. At the end of the execution of a task, a
** task tries to clean-up its stack and prepares the arrival of its next instance. In do-
** ing so and once it has completed its vital operations, there is no need for it to com-
** pletely finish cleaning-up as this can be done by the interrupt handler. In this case,
** the task never resumes and must never be saved. (see function OSEndTask()) */
LIB_EXTERN BOOL _OSNoSaveContext = TRUE;

/*%VARIABLES_BLOCK_END%*/
/*%DEFINITIONS_BLOCK_START%*/

/* Internal function prototypes */
STATIC BOOL Initialize(void);
STATIC void IdleTask(void *);
#if SCHEDULER_REAL_TIME_MODE == DEADLINE_MONOTONIC_SCHEDULING
  STATIC UINT8 GetTaskPriority(INT32 deadline);
#endif
STATIC BOOL ReadyQueueInsertTestKey(const TCB *searchKey, const TCB *node);
STATIC BOOL ArrivalQueueInsertTestKey(const TCB *searchKey, const TCB *node);
#define ReadyQueueInsert(newNode) InsertQueue(READYQ,newNode)
#define ArrivalQueueInsert(newNode) InsertQueue(ARRIVALQ,newNode)
STATIC void InsertQueue(UINT8 offsetNext, TCB *newNode);
void _OSTimerInterruptHandler(void);
#if SCHEDULER_REAL_TIME_MODE != DEADLINE_MONOTONIC_SCHEDULING
  STATIC INT32 GetSuspendedSchedulingDeadline(ETCB *);
#endif
STATIC void EnqueueRescheduleQueue(ETCB *etcb);
STATIC void EmptyRescheduleSynchronousTaskList(INT32 currentTime);

/* Array based implementation of a FIFO queue */
STATIC UINT16 GetFIFOArrayMaxIndex(UINT16 queueSize);
STATIC UINTPTR FIFODequeue(FIFOQUEUE *queue, UINTPTR signal);
STATIC void FIFODequeueHelper(FIFOQUEUE *queue, UINTPTR signal, DEQUEUE_DESCRIPTOR *des);
STATIC BOOL FIFOEnqueue(FIFOQUEUE *queue, UINTPTR signal, UINTPTR item);
STATIC void FIFOEnqueueHelper(FIFOQUEUE *queue, ENQUEUE_DESCRIPTOR *des);
STATIC void IncrementFifoQueueIndex(UINT16 *index, UINT16 oldValue, UINT16 moduloBase);

/* Assembler routines defined in ZottaOS_a.asm */
void _OSEndInterrupt(void);       // Continue running a preempted task
void _OSStartNextReadyTask(void); // Start a new task instance
void _OSSleep(void);


/* ATOMIC INSTRUCTIONS
** LL & SC Atomic Instruction Emulation
** The LL/SC pair of instructions are becoming increasing popular among high-end microcon-
** trollers. Because they are not given for the MSP430 and constitute a fundamental atomic
** instruction building block used within ZottaOS, they are emulated here.
** Basically the LL atomically reads the content of a memory location and reserves it. The
** SC instruction used in combination with the LL takes a memory location and a value as
** parameters. It checks whether the memory location is still reserved and if so applies
** the value parameter to it and clears the reservation. On the other hand, if the memory
** location is not reserved at the time of the call, the memory location is left unchanged.

** To allow interruptible insertions in the ready and arrival queues (amongst other ac-
** tions that need to be done concurrently) and because there are no atomic instructions
** in the MSP430 instruction set that can be used for multitasking, it is convenient to
** define (i.e. emulate) an LL/SC pair of instructions and a Compare-and-Swap (CAS).
** Emulation of LL/SC atomic instructions defined in assembler. */
/* All but 16-bit pointer LL/SC manipulations are given in ZottaOS_5xx.h. In fact, 16-bit
** manipulations are exactly like unsigned 16-bit integers albeit with different types.
** As we do not foresee an application use of pointers, we define the prototypes here. */

#define _OSUINTPTR_LL OSUINT16_LL
#define _OSUINTPTR_SC OSUINT16_SC

/*%DEFINITIONS_BLOCK_END%*/
/*%VARIABLES_BLOCK_START%*/

LIB_EXTERN BOOL _OSLLReserveBit;

/*%VARIABLES_BLOCK_END%*/

/*$ OSUINT16_LL: This set of functions emulate an LL on 16-bit operands and takes the
** address of the memory location that is to be reserved and returned. */
UINT16 OSUINT16_LL(UINT16 *memory)
{
  UINT16 tmp;
  _disable_interrupts();
  _OSLLReserveBit = TRUE; // Mark reserved
  tmp = *memory;          // Return the contents of the memory location
  _enable_interrupts();
  return tmp;
} /*? end of OSUINT16_LL */

/*$ OSUINT16_SC: This set of functions emulate a SC operation on 16 bit operands and are
** paired with their respective LL functions. 
** This instruction takes 2 parameters: a memory location and its new contents; and
** return a boolean indicating whether or not the memory location was modified or not. */
BOOL OSUINT16_SC(UINT16 *memory, UINT16 newValue)
{
  BOOL tmp;
  _disable_interrupts();
  if (_OSLLReserveBit) { // Is the reservation still on?
     _OSLLReserveBit = FALSE;
     *memory = newValue;
     tmp = TRUE;
  }
  else
     tmp = FALSE;
  _enable_interrupts();
  return tmp;
} /*? end of OSUINT16_SC */


/*$ OSUINT8_LL: Same as OSUINT16_LL but applied to to a single byte.*/
UINT8 OSUINT8_LL(UINT8 *memory)
{
  UINT8 tmp;
  _disable_interrupts();
  _OSLLReserveBit = TRUE; // Mark reserved
  tmp = *memory;          // Return the contents of the memory location
  _enable_interrupts();
  return tmp;
} /*? end of OSUINT8_LL */


/*$ OSUINT8_SC: Same as OSUINT16_SC but applied to to a single byte. */
BOOL OSUINT8_SC(UINT8 *memory, UINT8 newValue)
{
  BOOL tmp;
  _disable_interrupts();
  if (_OSLLReserveBit) { // Is the reservation still on?
     _OSLLReserveBit = FALSE;
     *memory = newValue;
     tmp = TRUE;
  }
  else
     tmp = FALSE;
  _enable_interrupts();
  return tmp;
} /*? end of OSUINT8_SC */


/*$ Initialize: Initializes the internals of the OS. This function is called prior to
** creating the first task and sets up the needed queues. */
BOOL Initialize(void)
{
  /* Allocate TCB blocks to hold the sentinel heads and tails of the ready and arrival
  ** queues. */
  if ((_OSQueueHead = (TCB *)OSAlloca(sizeof(TCB_HEAD))) == NULL)
    return FALSE;
  if ((_OSQueueTail = (TCB_TAIL *)OSAlloca(sizeof(TCB_TAIL))) == NULL)
    return FALSE;
  _OSQueueHead->Next[READYQ] = (TCB *)_OSQueueTail;
  _OSQueueTail->Next[READYQ] = NULL;
  /* Create the idle task as the tail of the ready queue. */
  _OSQueueTail->TaskCodePtr = IdleTask;
  #if SCHEDULER_REAL_TIME_MODE == DEADLINE_MONOTONIC_SCHEDULING
     _OSQueueTail->Priority = 0;
  #endif
  /* Make an empty arrival queue. */
  _OSQueueHead->Next[ARRIVALQ] = (TCB *)_OSQueueTail;
  _OSQueueTail->Next[ARRIVALQ] = NULL;
  _OSQueueTail->TaskState = STATE_INIT | TASKTYPE_BLOCKING;
  _OSQueueTail->ArrivalTimeLow = INT32_MAX;
  return TRUE;
} /*? end of Initialize */


/*$ IdleTask: This task executes whenever there is no other task in the ready queue. Its
** sole purpose is to keep the processor busy until the next task arrival time.
** The argument parameter to the idle task is undefined. */
void IdleTask(void * argument)
{
  if (_OSQueueHead->Next[ARRIVALQ] != (TCB *)_OSQueueTail || SynchronousTaskList != NULL)
     _OSStartTimer();   // Start the interval timer
  /* enter in sleep mode */
  _OSSleep();  
}
 /*? end of IdleTask */


/*$ OSCreateTask: Creates a new task by allocating a new TCB to the task.
** The period and arrival time of periodic tasks are given respectively by the relation
**     period = PeriodHigh * 2^30 + PeriodLow
**     arrival time = ArrivalTimeHigh * 2^30 + ArrivalTimeLow */
BOOL OSCreateTask(void task(void *), UINT16 periodCycles, INT32 periodOffset,
                   INT32 deadline, void *argument)
{
  TCB *ptcb;
  if (_OSQueueHead == NULL && !Initialize())
     return FALSE;
  /* Get a new TCB and initialize it. */
  if ((ptcb = (TCB*)OSAlloca(sizeof(TCB))) == NULL)
     return FALSE;
  ptcb->TaskCodePtr = task;
  ptcb->PeriodHigh = periodCycles;
  ptcb->PeriodLow = periodOffset;
  ptcb->ArrivalTimeHigh = 0;
  ptcb->Argument = argument;
  #if SCHEDULER_REAL_TIME_MODE == DEADLINE_MONOTONIC_SCHEDULING
     /* Temporarily save the deadline until the user calls OSStartMultitasking(). */
     ptcb->ArrivalTimeLow = deadline;
  #else
     ptcb->Deadline = deadline;
     ptcb->NextDeadline = deadline;  // Initial deadline at the start of a hyper-period
     ptcb->ArrivalTimeLow = 0;
  #endif
  ptcb->TaskState = STATE_ZOMBIE;
  ptcb->Next[READYQ] = NULL;
  #if SCHEDULER_REAL_TIME_MODE == DEADLINE_MONOTONIC_SCHEDULING
     ptcb->Priority = GetTaskPriority(deadline);
  #endif
  ArrivalQueueInsert(ptcb);
  return TRUE;
} /*? end of OSCreateTask */


/*$ GetTaskPriority: Returns the priority for a task given its deadline. */
#if SCHEDULER_REAL_TIME_MODE == DEADLINE_MONOTONIC_SCHEDULING
UINT8 GetTaskPriority(INT32 deadline)
{
  TCB *nextTCB = (TCB *)_OSQueueHead;
  UINT8 nbTasks = 0;
  /* Periodic tasks are initially sorted according to their deadlines and temporarily in-
  ** serted in the ready queue; the priority of these tasks can be found by traversing
  ** this queue. Note that the task arrival times are reset to their initial values when
  ** starting ZottaOS. */
  do {
     nextTCB = nextTCB->Next[ARRIVALQ];
     if (nextTCB->ArrivalTimeLow > deadline)
        nextTCB->Priority++;
     else
        nbTasks++;
  } while (nextTCB != (TCB *)_OSQueueTail);
  return nbTasks;
}
#endif
 /*? end of GetTaskPriority */
 

/*$ OSEndTask: Called by a periodic task when it terminates its instance. The task now
** needs to wait until its next arrival time before it can again become ready. */
void OSEndTask(void)
{
  /* Set task to zombie to indicate that it is about to remove itself from the ready
  ** queue and that its context should not be saved. */
  _OSActiveTask->TaskState |= STATE_ZOMBIE;
  _OSNoSaveContext = TRUE; // Don't save the context of this task
  /* Remove the task from the ready queue */
  _OSQueueHead->Next[READYQ] = _OSActiveTask->Next[READYQ];
  _OSActiveTask = _OSQueueHead->Next[READYQ];
  if (_OSActiveTask->TaskState & STATE_RUNNING)
     _OSEndInterrupt();       // Continue running a preempted task
  else
     _OSStartNextReadyTask(); // Start a new task instance
} /*? end of OSEndTask */


/*$ InsertQueue: Inserts a TCB into one of the queues in the OS. This can be the ready
** queue if offsetNext=0, the arrival queue if offsetNext=1.
** Parameters:
**  (2) (UINT8) offsetNext: one of 0 through 1;
**  (3) (TCB *) the node to insert. */
void InsertQueue(UINT8 offsetNext, TCB *newNode)
{
  BOOL insertNow;
  TCB *right, *left;
  /* Get left and right TCB neighbors */
  left = _OSQueueHead;
  while (TRUE)
     if ((right = left->Next[offsetNext]) == (TCB *)_OSQueueTail)
        break; // always insert before the tail
     else { 
     	switch (offsetNext) {
           case READYQ:
              insertNow = ReadyQueueInsertTestKey(newNode,right);
              break;
           case ARRIVALQ:
              insertNow = ArrivalQueueInsertTestKey(newNode,right);
              break;
           default:
              #ifdef DEBUG_MODE
                 _disable_interrupts();
                 while (TRUE); // If you get here, call us!
              #endif
     	}
        if (insertNow)
           break; // found the right node
        else
           left = right;
     }
  newNode->Next[offsetNext] = right;
  left->Next[offsetNext] = newNode;
} /*? end of InsertQueue */


/*$ ReadyQueueInsertTestKey: Search function used to insert a new TCB in the ready
** queue.
** Parameters:
**   (1) (const TCB *) new TCB to insert;
**   (2) (const TCB *) next TCB in the queue that will be scheduled after the new TCB.
** Returned value: (BOOL) TRUE if the node is before the next TCB and FALSE otherwise. */
BOOL ReadyQueueInsertTestKey(const TCB *insert, const TCB *next)
{
  #if SCHEDULER_REAL_TIME_MODE == EARLIEST_DEADLINE_FIRST
     INT32 insertDeadline, nextDeadline;
  #endif
  #if SCHEDULER_REAL_TIME_MODE == DEADLINE_MONOTONIC_SCHEDULING
     return insert->Priority < next->Priority;
  #elif SCHEDULER_REAL_TIME_MODE == EARLIEST_DEADLINE_FIRST
     if (insert->TaskState & TASKTYPE_BLOCKING)
        insertDeadline = ((ETCB *)insert)->NextDeadline;
     else
        insertDeadline = insert->NextDeadline;
     if (next->TaskState & TASKTYPE_BLOCKING)
        nextDeadline = ((ETCB *)next)->NextDeadline;
     else
        nextDeadline = next->NextDeadline;
     return insertDeadline <= nextDeadline;
  #endif
} /*? end of ReadyQueueInsertTestKey */


/*$ ArrivalQueueInsertTestKey: Search function used to insert a new TCB in the arrival
** queue.
** Parameters:
**   (1) (const TCB *) new TCB to insert;
**   (2) (const TCB *) next TCB in the queue that will arrive after the new TCB.
** Returned value: (BOOL) TRUE if the node is before the next TCB and FALSE otherwise. */
BOOL ArrivalQueueInsertTestKey(const TCB *insert, const TCB *next)
{
  UINT16 insertHigh, nextHigh;
  insertHigh = ((insert->TaskState & TASKTYPE_BLOCKING) == 0) ? insert->ArrivalTimeHigh : 0;
  nextHigh = ((next->TaskState & TASKTYPE_BLOCKING) == 0) ? next->ArrivalTimeHigh : 0;
  return insertHigh < nextHigh || (insertHigh == nextHigh &&
                                         insert->ArrivalTimeLow <= next->ArrivalTimeLow);
} /*? end of ArrivalQueueInsertTestKey */

/*%DEFINITIONS_BLOCK_START%*/

/* Macro definition of result=min(result-sub,0) done on 32-bit integer arithmetic where
** sub > 0. */
#define SubOrZeroIfNeg(result,sub) \
{ \
  result -= sub;  \
  if (result < 0) \
     result = 0;  \
}

/*%DEFINITIONS_BLOCK_END%*/

/*$ _OSTimerInterruptHandler: Sotware interrupt handler for timer that manages task instance
** arrivals. Because the timer is a bit counter with a predefined number of bits, when a
** timer event occurs, it can be that there are no arrivals. In this case, we only need
** check if we need to shift temporal variables. */
void _OSTimerInterruptHandler(void)
{
  ETCB *etcb;
  TCB *arrival;
 /* At this point there can only be one current timer interrupt under way. */
  #ifdef DEBUG_MODE
     static UINT8 nesting = 0;
     if (++nesting > 1) {
     	_disable_interrupts();
        while (TRUE); // If you get here, call us! 
     }
  #endif
  /* Mark that a software timer interrupt is under way so that new interrupts will not
  ** be generated with P2IFG |= 0x80 (see EnqueueRescheduleQueue). */          
  while (!_OSUINTPTR_SC((UINTPTR *)&_OSRescheduleSynchronousTaskList,
   (UINTPTR)GetMarkedReference(_OSUINTPTR_LL((UINTPTR *)&_OSRescheduleSynchronousTaskList))));
  /* Update the wall clock with the timer comparator that was saved when the timer gene-
  ** rated an interrupt. */
  _OSUpdateTime();
  /* Transfer all new arrivals to the ready queue. */
  arrival = _OSQueueHead->Next[ARRIVALQ];
  while (arrival->ArrivalTimeLow <= _OSGetTime() &&
           ((arrival->TaskState & TASKTYPE_BLOCKING) || arrival->ArrivalTimeHigh == 0)) {
     _OSQueueHead->Next[ARRIVALQ] = arrival->Next[ARRIVALQ];
     if (!(arrival->TaskState & STATE_ZOMBIE)) { // Is task still in the ready queue?
        /* At this point an arriving task should not be in state STATE_RUNNING */
        #ifdef DEBUG_MODE
           _disable_interrupts();
           while (TRUE); // If get here, the processor utilization > 100%.
        #endif
     }
     #if SCHEDULER_REAL_TIME_MODE != DEADLINE_MONOTONIC_SCHEDULING
        if ((arrival->TaskState & TASKTYPE_BLOCKING) == 0)
           arrival->NextDeadline = arrival->ArrivalTimeLow + arrival->Deadline;
        else
           ((ETCB *)arrival)->NextDeadline = GetSuspendedSchedulingDeadline((ETCB *)arrival);
     #endif
     arrival->TaskState &= TASKTYPE_BLOCKING; // Set task to INIT while keeping flag TASKTYPE_BLOCKING
     ReadyQueueInsert(arrival);
     
     if ((arrival->TaskState & TASKTYPE_BLOCKING) == 0) {
        arrival->ArrivalTimeHigh = arrival->PeriodHigh;
        arrival->ArrivalTimeLow += arrival->PeriodLow;
        if (arrival->ArrivalTimeLow > 0x3FFFFFFF) {
           arrival->ArrivalTimeHigh += 1;
           arrival->ArrivalTimeLow &= 0x3FFFFFFF;
        }
        ArrivalQueueInsert(arrival);
     }
     #if SCHEDULER_REAL_TIME_MODE == DEADLINE_MONOTONIC_SCHEDULING
     else
        arrival->ArrivalTimeLow += ((ETCB *)arrival)->PeriodLow;
     #endif
     arrival = _OSQueueHead->Next[ARRIVALQ];
  }
  /* To avoid overflow of the wall clock (Time), a time shift is done on all temporal
  ** variables. Because all these variables are signed, their relative values are pre-
  ** served. */
  if (_OSGetTime() >= ShiftTimeLimit) {
     #if SCHEDULER_REAL_TIME_MODE != DEADLINE_MONOTONIC_SCHEDULING
        /* Time shift periodic tasks in the ready queue. */
        for (arrival = _OSQueueHead; (arrival = arrival->Next[READYQ]) != (TCB *)_OSQueueTail; )
           if ((arrival->TaskState & TASKTYPE_BLOCKING) == 0)
              arrival->NextDeadline -= ShiftTimeLimit;
     #endif
     /* Time shift all periodic tasks in the arrival queue. */
     for (arrival = _OSQueueHead; (arrival = arrival->Next[ARRIVALQ]) != (TCB *)_OSQueueTail; )
        if ((arrival->TaskState & TASKTYPE_BLOCKING) == 0) {
           if (arrival->ArrivalTimeHigh > 0)
              arrival->ArrivalTimeHigh--;
           else
              arrival->ArrivalTimeLow -= ShiftTimeLimit;
        }
     /* Time shift all event-driven tasks. */
     for (etcb = SynchronousTaskList; etcb != NULL; etcb = etcb->NextETCB) {
        #if SCHEDULER_REAL_TIME_MODE == DEADLINE_MONOTONIC_SCHEDULING
           SubOrZeroIfNeg(etcb->ArrivalTimeLow,ShiftTimeLimit);
        #else
           SubOrZeroIfNeg(etcb->NextDeadline,ShiftTimeLimit);
           if (etcb->TaskState & ARRIVALTYPE_BLOCKING)
              SubOrZeroIfNeg(etcb->ArrivalTimeLow,ShiftTimeLimit);
        #endif
     }
     #if SCHEDULER_REAL_TIME_MODE != DEADLINE_MONOTONIC_SCHEDULING
        SubOrZeroIfNeg(SynchronousTaskDeadlines,ShiftTimeLimit);
     #endif
     _OSShiftTime(ShiftTimeLimit);
  }
  /* Process pending aperiodic tasks found in the RescheduleSynchronousTaskList. */
  EmptyRescheduleSynchronousTaskList(_OSGetTime());
  /* If we get a timer interrupt, there's no point saving the context of the current ISR
  ** since we will have to restart it anyways. */
  _OSNoSaveContext = TRUE;
  #ifdef DEBUG_MODE
     --nesting;
  #endif
  _OSEnableSoftTimerInterrupt();
  /* At this point another software timer can preempt and not save the current context
  ** as this interrupt restarts from the beginning. */
  /* Set arrival timer to the next arrival time. */
  arrival = _OSQueueHead->Next[ARRIVALQ];
  if (arrival != (TCB *)_OSQueueTail &&
             ((arrival->TaskState & TASKTYPE_BLOCKING) || arrival->ArrivalTimeHigh == 0))
     /* Set the timer comparator to the next periodic task arrival time. */
     _OSSetTimer(arrival->ArrivalTimeLow);
  /* Return to the task with highest priority or start a new instance. */
  _OSActiveTask = _OSQueueHead->Next[READYQ];
  if (_OSActiveTask->TaskState & STATE_RUNNING)
     _OSEndInterrupt();       // Continue running a preempted task
  else
     _OSStartNextReadyTask(); // Start a new task instance
} /*? end of _OSTimerInterruptHandler */

/*%DEFINITIONS_BLOCK_START%*/

/* The FIFO queue of suspended tasks associated with an event may also contain a special
** marker called SIGNAL. When a task signals another but there is no waiting event-driven
** task at that moment, SIGNAL is inserted into the queue where the next enqueue takes
** place. The next event-driven task that becomes blocked (and enqueues itself) can then
** immediately restart a new instance of itself. */
#define SIGNAL 0xFFFFu

/* EnqueueEventTask: Adds an event-driven task TCB into a FIFO list of tasks that are
** suspended for a particular event. If the slot where the task is to be stored holds a
** SIGNAL marker, the queue was empty when the event occurred; the task is not inserted
** as it is already signaled.
** Parameters:
**   (1) (FIFOQUEUE *) event descriptor with the queue of suspended tasks;
**   (2) (ETCB *) TCB to enqueue.
** Returned value: (BOOL) TRUE if the caller is not queued and should be rescheduled.
**    Otherwise the task is added at the end of the event queue. */
#define EnqueueEventTask(eventQueue,etcb) FIFOEnqueue(eventQueue,SIGNAL,etcb)

/* DequeueEventTask: Returns the first task in the FIFO list of tasks that is suspended
** and waiting for a particular event. If no such task exists, the event is marked so
** that the next time that a task suspends itself in the FIFO list it catches the event.
** This function is called by the task that generated an event.
** Parameter: (FIFOQUEUE *) event to signal and holding suspended tasks for it.
** Returned value: (ETCB *) Suspended task to schedule or NULL if none exist. */
#define DequeueEventTask(eventQueue) (ETCB *)FIFODequeue(eventQueue,SIGNAL)

/*%DEFINITIONS_BLOCK_END%*/

/*$ OSCreateEventDescriptor: Creates and returns a descriptor with all the need informa-
** tion to block (suspend) and wake-up an event-driven task instance. The implementation
** requires a circular list that can be created and completed once the number of tasks
** that can be blocked is known. */
void *OSCreateEventDescriptor(void)
{
  FIFOQUEUE *eventQueue;
  if ((eventQueue = (FIFOQUEUE *)OSAlloca(sizeof(FIFOQUEUE))) != NULL) {
     eventQueue->Head = eventQueue->Tail = eventQueue->QueueLength = 0;
     eventQueue->Q = NULL;
     eventQueue->PendingOp = NULL;
  }
  return eventQueue;
} /*? end of OSCreateEventDescriptor */


/*$ OSCreateSynchronousTask: Like OSCreateTask() but applied to synchronous tasks. */
BOOL OSCreateSynchronousTask(void task(void *), INT32 workLoad, void *event, void *argument)
{
  ETCB *etcb;
  if (_OSQueueHead == NULL && !Initialize())
     return FALSE;
  /* Get a new TCB and initialize it. */
  if ((etcb = (ETCB *)OSAlloca(sizeof(ETCB))) == NULL)
     return FALSE;
  etcb->TaskCodePtr = task;
  etcb->TaskState = STATE_ZOMBIE | TASKTYPE_BLOCKING;
  etcb->Argument = argument;
  /* Because FIFO enqueueing and dequeueing uses a table, the size of this table is only
  ** known after all the synchronous tasks have been created. Hence we need to tempora-
  ** rily save this task in a list, and enqueue the task later. */
  etcb->NextETCB = SynchronousTaskList;
  etcb->EventQueue = (FIFOQUEUE *)event;
  #if SCHEDULER_REAL_TIME_MODE == DEADLINE_MONOTONIC_SCHEDULING
     etcb->Priority = GetTaskPriority(workLoad);
     etcb->PeriodLow = workLoad;
     etcb->ArrivalTimeLow = 0;
  #else
     etcb->WorkLoad = workLoad;
     etcb->NextDeadline = 0;
  #endif
  SynchronousTaskList = etcb;
  etcb->EventQueue->QueueLength++;
  return TRUE;
} /*? end of OSCreateSynchronousTask */


/*$ OSSuspendSynchronousTask: The very last instruction of a synchronous task. This func-
** tion checks if a new task instance should restart (an event was signalled) or if it
** should suspend itself until the event is signalled by OSScheduleSuspendedTask(). */
void OSSuspendSynchronousTask(void)
{
  FIFOQUEUE *eq = ((ETCB *)_OSActiveTask)->EventQueue;
  /* Insert the task into the event queue as soon as possible so that a task signalling
  ** an event will detect this task. When EnqueueEventTask() returns TRUE, there is a
  ** pending event and the task is not appended to the queue. */
  if (EnqueueEventTask(eq,(UINTPTR)_OSActiveTask)) {
     /* Now we need to reschedule this task, i.e. we need to resort the ready queue with
     ** new deadline or priority parameters. However because the needed information can
     ** be updated concurrently by the timer interrupt handler, it is simpler to resche-
     ** dule the task by the timer interrupt handler. */
     EnqueueRescheduleQueue((ETCB *)_OSActiveTask);
  }
  /* Set task to zombie to indicate that the task it is about to remove itself from the
  ** ready queue and that its context should not be saved. */
  _OSActiveTask->TaskState |= STATE_ZOMBIE;
  _OSNoSaveContext = TRUE;    // Don't save the context of this task
  /* Remove the task from the ready queue */
  _OSQueueHead->Next[READYQ] = _OSActiveTask->Next[READYQ];
  _OSActiveTask = _OSQueueHead->Next[READYQ];
  if (_OSActiveTask->TaskState & STATE_RUNNING)
     _OSEndInterrupt();       // Continue running a preempted task
  else
     _OSStartNextReadyTask(); // Start a new task instance
} /*? end of OSSuspendSynchronousTask */


/*$ OSScheduleSuspendedTask: Removes a blocked task that is waiting for an event and sche-
** dules it. */
void OSScheduleSuspendedTask(void *eq)
{
  ETCB *etcb;
  if ((etcb = DequeueEventTask((FIFOQUEUE *)eq)) != NULL)
     EnqueueRescheduleQueue(etcb);
} /*? end of OSScheduleSuspendedTask */


/*$ EnqueueRescheduleQueue: Inserts an aperiodic ETCB into a list of work that needs to be
** processed by the timer handler (see EmptyRescheduleSynchronousTaskList). This indirect
** scheduling of aperiodic tasks is necessary because it is possible that the timer hand-
** ler receives an interrupt and performs a shift of all temporal variables while this
** scheduling is taking place. Note that the timer handler is invoked by forcing an in-
** terrupt. */
void EnqueueRescheduleQueue(ETCB *etcb)
{
  UINTPTR tmp;
  while (TRUE) {
     tmp = _OSUINTPTR_LL((UINTPTR *)&_OSRescheduleSynchronousTaskList);
     if (IsMarkedReference(tmp)) {
        etcb->Next[BLOCKQ] = (ETCB *)GetUnmarkedReference(tmp);
        if (_OSUINTPTR_SC((UINTPTR *)&_OSRescheduleSynchronousTaskList,(UINTPTR)GetMarkedReference(etcb)));
           return;
     } 
     else {
        etcb->Next[BLOCKQ] = (ETCB *)tmp;
        if (_OSUINTPTR_SC((UINTPTR *)&_OSRescheduleSynchronousTaskList,(UINTPTR)etcb)) {
           _OSGenerateSoftTimerInterrupt(); // Generate a soft timer interrupt
           return;
        }
     }
  }
} /*? end of EnqueueRescheduleQueue */


/*$ EmptyRescheduleSynchronousTaskList: This function schedules all aperiodic tasks that
** are in a temporary lifo list by placing them either in the ready queue or in the arri-
** val queue if the task hasn't finished its previous load imposed on the processor. */
void EmptyRescheduleSynchronousTaskList(INT32 currentTime)
{
  BOOL wait;
  ETCB *etcb;
  do {
     while ((etcb = (ETCB *)GetUnmarkedReference(_OSUINTPTR_LL((UINTPTR *)&_OSRescheduleSynchronousTaskList))) != NULL) {
        if (_OSUINTPTR_SC((UINTPTR *)&_OSRescheduleSynchronousTaskList,(UINTPTR)GetMarkedReference(etcb->Next[BLOCKQ]))) {
           #if SCHEDULER_REAL_TIME_MODE != DEADLINE_MONOTONIC_SCHEDULING
              /* Under EDF, the task that is to process the event cannot execute until it
              ** has finished its previous deadline. */
              wait = currentTime < etcb->NextDeadline;
           #else
              /* Under deadline monotonic scheduling, the task that is to process the event
              ** cannot execute until it has finished its previous period. */
              if (etcb->TaskState & STATE_ZOMBIE)
                 wait = currentTime < etcb->ArrivalTimeLow;
              else {
                 #ifdef DEBUG_MODE
                    if (currentTime >= etcb->ArrivalTimeLow)
                       while (TRUE); // Event-driven task WCET overrun: increase task's period
                 #endif
                 for (wait = TRUE; currentTime >= etcb->ArrivalTimeLow;)
                 etcb->ArrivalTimeLow += etcb->PeriodLow;
              }
           #endif
           if (wait) {
              #if SCHEDULER_REAL_TIME_MODE != DEADLINE_MONOTONIC_SCHEDULING
                 etcb->ArrivalTimeLow = etcb->NextDeadline;
                 etcb->TaskState |= ARRIVALTYPE_BLOCKING;
              #endif
              ArrivalQueueInsert((TCB *)etcb);
           }
           else {
              #ifdef DEBUG_MODE
                 if ((etcb->TaskState & STATE_ZOMBIE) == 0)
                    while (TRUE); // Event-driven task WCET overrun: increase task's period
              #endif
              etcb->TaskState = TASKTYPE_BLOCKING;
              #if SCHEDULER_REAL_TIME_MODE != DEADLINE_MONOTONIC_SCHEDULING
                 etcb->NextDeadline = GetSuspendedSchedulingDeadline(etcb);
              #else
                 etcb->ArrivalTimeLow = currentTime + etcb->PeriodLow;
              #endif
              ReadyQueueInsert((TCB *)etcb);
           }
        }
     }
  } while (!_OSUINTPTR_SC((UINTPTR *)&_OSRescheduleSynchronousTaskList,NULL));
} /*? end of EmptyRescheduleSynchronousTaskList */


/*$ GetSuspendedSchedulingDeadline: Updates the next deadline of a synchronous task. This
** function is used only for EDF scheduling. Note that this function is not concurrent
** and is only called the timer service routine. */
#if SCHEDULER_REAL_TIME_MODE != DEADLINE_MONOTONIC_SCHEDULING
INT32 GetSuspendedSchedulingDeadline(ETCB *etcb)
{
  if (SynchronousTaskDeadlines > _OSGetTime())
     SynchronousTaskDeadlines += etcb->WorkLoad;
  else
     SynchronousTaskDeadlines = _OSGetTime() + etcb->WorkLoad;
  return SynchronousTaskDeadlines;
}
#endif
/*? end of GetSuspendedSchedulingDeadline */

/*$ OSStartMultitasking: This function is called only once and after the application tasks
** have all been defined in the application. This function never returns and schedules
** all user tasks. When these terminate, an idle task runs with the lowest priority to
** keep the processor busy until the next task arrives. This function also creates a
** timer handler when there are tasks to be scheduled. */
BOOL OSStartMultitasking(void)
{
  ETCB *etcb;
  FIFOQUEUE *eq;
  UINT8 i;
  #if SCHEDULER_REAL_TIME_MODE == DEADLINE_MONOTONIC_SCHEDULING
     TCB *tcb;
  #endif
   /* Assert that ZottaOS begins with disabled maskable interruptions. */
  _disable_interrupts();
  if (_OSQueueHead == NULL)
     Initialize();
  /* Create the FIFO array of tasks for all created event descriptors. */
  for (etcb = SynchronousTaskList; etcb != NULL; etcb = etcb->NextETCB) {
     if (etcb->EventQueue->Q == NULL) {
        eq = etcb->EventQueue;
        if ((eq->Q = (UINTPTR *)OSAlloca(eq->QueueLength*sizeof(UINTPTR))) == NULL)
           return FALSE;
        for (i = 0; i < eq->QueueLength; i += 1)
           eq->Q[i] = NULL;
        eq->MaxIndex = GetFIFOArrayMaxIndex(eq->QueueLength);
     }
     EnqueueEventTask(etcb->EventQueue,(UINTPTR)etcb);
  }
  #if SCHEDULER_REAL_TIME_MODE == DEADLINE_MONOTONIC_SCHEDULING
     /* Reset the arrival time of each periodic task to its first arriving instance. */
     for (tcb = _OSQueueHead->Next[ARRIVALQ]; tcb != (TCB *)_OSQueueTail; tcb = tcb->Next[ARRIVALQ])
        tcb->ArrivalTimeLow =  0;
     /* Now that all tasks have been initialized, the idle task running priority can be
     ** set. For N application tasks, priorities 0..N-1 are for mandatory instances and
     ** priorities N..2N-1 are for the optional ones. Hence, the idle task priority is
     ** simply N. Note that at task creation time, the Priority field holds the base pri-
     ** ority and for the idle, this value is N when we get here. */
  #endif
  /* There are periodic tasks in the system when the arrival queue is not empty. Note
  ** that periodic tasks are initially inserted in this queue before starting ZottaOS. */
  if (_OSQueueHead->Next[ARRIVALQ] != (TCB *)_OSQueueTail || SynchronousTaskList != NULL)
     /* Initialize the timer which starts counting as soon as the idle task begins. At
     ** this point, the timer's input divider is selected but it is halted. */
     _OSInitializeTimer();
  /* Start the first task in the ready queue, i.e. the Idle task. */
  _OSActiveTask = _OSQueueHead->Next[READYQ];
  _OSStartNextReadyTask();     // Start the Idle task
  return FALSE;
} /*? end of OSStartMultitasking */

/*%DEFINITIONS_BLOCK_START%*/

extern void * _OSTabDevice[]; /* Defined in ZottaOS_msp430xxx.asm */

/*%DEFINITIONS_BLOCK_END%*/

/*$ OSSetIODescriptor: Associates an I/O descriptor into a _OSTabDevice entry. This func-
** tion is made available to application tasks so that default I/O processing can be
** altered.
** Parameters:
**   (1) (UINT8) index in the I/O device interrupt vector;
**   (2) (void *) processing descriptor for the specified device.
** Returned value: none. */
void OSSetIODescriptor(UINT8 index, void *descriptor)
{
  _OSTabDevice[index >> 1] = descriptor;
} /*? end of OSSetIODescriptor */


/*$ OSGetIODescriptor: Returns the I/O descriptor associated with an _OSTabDevice entry.
** This function is made available to application tasks so that the inserted I/O descrip-
** tor can be returned to the user in order to read or store his application data.
** Parameter: (UINT8) index in the I/O device interrupt vector where the I/O descriptor
**    is held.
** Returned value: (void *) The requested I/O descriptor is returned. If no previous
**    OSSetIODescriptor was previously made for the specified I/O port, the returned
**    value is undefined. */
void *OSGetIODescriptor(UINT8 index)
{
  return _OSTabDevice[index >> 1];
} /*? end of OSGetIODescriptor */


/* FIFO queue implementation */
/*$ GetFIFOArrayMaxIndex: Given the size, say S, of a FIFO array, this function returns
** the largest natural number A such that A mod S = 0 and A != 2^n - 1 for any n > 0. */
UINT16 GetFIFOArrayMaxIndex(UINT16 queueSize)
{
  UINT16 tmp, maxIndex = 0xFFFF - queueSize;
  if ((tmp = maxIndex % queueSize) != 0)
     maxIndex += queueSize - tmp;
  return maxIndex;
} /*? end of GetFIFOArrayMaxIndex */


/*$ FIFODequeue: Returns and dequeues the first item of a FIFO queue. If there is no such
** item, i.e. the queue is empty at the time of the call, and the signal marker (used to
** implement event queues) is passed as parameter, the marker is inserted into the array
** entry that will be read by the next enqueue operation.
** Parameters:
**   (1) (FIFOQUEUE *) array-based queue descriptor;
**   (2) (UINTPTR) signal marker: can be NULL or SIGNAL.
** Return value: (UINTPTR) the oldest enqueued item or NULL if none exist. */
UINTPTR FIFODequeue(FIFOQUEUE *queue, UINTPTR signal)
{
  DEQUEUE_DESCRIPTOR des;
  void *op;
  /* Initialize of fields of the descriptor to uninitialized markers. */
  des.HeadPropose = des.Head = 0xFFFF;
  des.SlotPropose = des.SlotReturn = (UINTPTR)&des;
  des.Done = FALSE;
  /* Do all pending operation before the current dequeue. */
  if ((op = queue->PendingOp) != NULL) { // Is there any pending operations?
     if (IsMarkedReference(op))          // Is pending operation a dequeue?
        FIFODequeueHelper(queue,signal,(DEQUEUE_DESCRIPTOR *)GetUnmarkedReference(op));
     else
        FIFOEnqueueHelper(queue,(ENQUEUE_DESCRIPTOR *)op);
  }
  /* Post current dequeue operation and do it. */
  queue->PendingOp = GetMarkedReference(&des);
  FIFODequeueHelper(queue,signal,&des);  // Do the operation
  queue->PendingOp = NULL;               // Assert that no other task does this operation
  if (des.SlotReturn == (UINTPTR)SIGNAL) // Was there already a signal?
     return NULL;                        // Do not return the signal
  else
     return des.SlotReturn;              // Returns NULL or the dequeued item
} /*? end of FIFODequeue */


/*$ FIFODequeueHelper: Performs a pending dequeue on the FIFO array. If there is no item
** to dequeue, a special marker (SIGNAL) can be inserted into the slot pointed by Tail
** (=Head) so that the next posted enqueue operation receives the signal and does not add
** the item it would otherwise enqueue. This feature is used in conjunction with events
** so that a dequeue operation posts an event or dequeues the blocked task waiting for
** the event.
** Parameters:
**   (1) (FIFOQUEUE *) array-based queue descriptor;
**   (2) (UINTPTR) signal marker: can be NULL or SIGNAL;
**   (3) (DEQUEUE_DESCRIPTOR *) parameters of pending dequeue operation.
** Return value: None, but the result of the operation is stored in field SlotReturn. */
void FIFODequeueHelper(FIFOQUEUE *queue, UINTPTR signal, DEQUEUE_DESCRIPTOR *des)
{
  UINT16 h;
  UINTPTR slot;
  if (des->HeadPropose == 0xFFFF)
     des->HeadPropose = queue->Head;
  if (des->Head == 0xFFFF)
     des->Head = des->HeadPropose;
  h = des->Head % queue->QueueLength;
  if (des->SlotPropose == (UINTPTR)des)
     des->SlotPropose = queue->Q[h];
  if (des->SlotReturn == (UINTPTR)des)
     des->SlotReturn = des->SlotPropose;
  if (des->SlotReturn != signal)
     while (TRUE) {
        slot = _OSUINTPTR_LL(&queue->Q[h]);
        if (des->Done || slot == SIGNAL)
           break;
        else if (slot == NULL)
           if (des->SlotReturn == NULL) {
              if (_OSUINTPTR_SC(&queue->Q[h],SIGNAL)) {
                 des->Done = TRUE;
                 break;
              }
           }
           else {
              IncrementFifoQueueIndex(&queue->Head,des->Head,queue->MaxIndex);
              des->Done = TRUE;
              break;
           }
        else if (_OSUINTPTR_SC(&queue->Q[h],NULL)) {
           IncrementFifoQueueIndex(&queue->Head,des->Head,queue->MaxIndex);
           des->Done = TRUE;
           break;
        }
     }
} /*? end of FIFODequeueHelper */


/*$ FIFOEnqueue: Inserts an item into a FIFO array-based queue. If the array entry holds
** the optional SIGNAL marker, the queue was empty when a dequeue operation occurred and
** the item is not inserted into the queue. This feature is used together with events so
** that a task is not inserted into an event queue when it is already signaled.
** Parameters:
**   (1) (FIFOQUEUE *) array-based queue descriptor;
**   (2) (UINTPTR) signal marker: can be NULL or SIGNAL;
**   (3) (UINTPTR) item to enqueue.
** Returned value: (BOOL) TRUE if the item is successfully enqueued or if the queue holds
**    the SIGNAL marker at the time of the call. The function returns FALSE if the queue
**    is full or if the queue does not contain the SIGNAL marker and the item (task) is
**    successfully inserted. */
BOOL FIFOEnqueue(FIFOQUEUE *queue, UINTPTR signal, UINTPTR item)
{
  ENQUEUE_DESCRIPTOR des;
  void *op;
  /* Initialize all fields with uninitialized markers and insert the item to enqueue. */
  des.TailPropose = des.Tail = 0xFFFF;
  des.SlotPropose = des.SlotReturn = (UINTPTR)&des;
  des.Item = item;
  des.Done = FALSE;
  /* Finish all pending operations before enqueueing the item. */
  if ((op = queue->PendingOp) != NULL) { // Is there any pending operations?
     if (IsMarkedReference(op))          // Is pending operation a dequeue?
        FIFODequeueHelper(queue,signal,(DEQUEUE_DESCRIPTOR *)GetUnmarkedReference(op));
     else
        FIFOEnqueueHelper(queue,(ENQUEUE_DESCRIPTOR *)op);
  }
  /* Post and do this enqueue operation. */
  queue->PendingOp = &des;
  FIFOEnqueueHelper(queue,&des);
  queue->PendingOp = NULL;           // Assert that no other task does this operation
  return des.SlotReturn == signal;   // Extract the value to return.
} /*? end of FIFOEnqueue */


/*$ FIFOEnqueueHelper: Performs a pending enqueue in the array-based FIFO queue.
** Parameters:
**   (1) (FIFOQUEUE *) array-based queue descriptor;
**   (2) (ENQUEUE_DESCRIPTOR *) parameters of pending enqueue operation.
** Return value: None, but the result of the operation is stored in field SlotReturn.*/
void FIFOEnqueueHelper(FIFOQUEUE *queue, ENQUEUE_DESCRIPTOR *des)
{
  UINT16 t;
  UINTPTR slot;
  if (des->TailPropose == 0xFFFF)
     des->TailPropose = queue->Tail;
  if (des->Tail == 0xFFFF)
     des->Tail = des->TailPropose;
  t = des->Tail % queue->QueueLength;
  if (des->SlotPropose == (UINTPTR)des)
     des->SlotPropose = queue->Q[t];
  if (des->SlotReturn == (UINTPTR)des)
     des->SlotReturn = des->SlotPropose;
  if (des->SlotReturn == NULL || des->SlotReturn == SIGNAL)
     while (TRUE) {
        slot = _OSUINTPTR_LL(&queue->Q[t]);
        if (des->Done)
           break;
        else if (slot == SIGNAL) {
           if (_OSUINTPTR_SC(&queue->Q[t],NULL)) {
              des->Done = TRUE;
              break;
           }
        }
        else if (slot == NULL) {
           if (des->SlotReturn == SIGNAL) {
              des->Done = TRUE;
              break;
           }
           else if (_OSUINTPTR_SC(&queue->Q[t],des->Item)) {
              IncrementFifoQueueIndex(&queue->Tail,des->Tail,queue->MaxIndex);
              des->Done = TRUE;
              break;
           }
        }
        else {
           IncrementFifoQueueIndex(&queue->Tail,des->Tail,queue->MaxIndex);
           des->Done = TRUE;
           break;
        }
     }
} /*? end of FIFOEnqueueHelper */


/*$ IncrementFifoQueueIndex: Increments an index (Head or Tail) of a circular array-based
** FIFO queue.
** Parameters:
**   (1) (UINT16 *) address of the index to increment;
**   (2) (UINT16) current value of the index;
**   (3) (UINT16) value whereby the index wraps-around. */
void IncrementFifoQueueIndex(UINT16 *index, UINT16 oldValue, UINT16 moduloBase)
{
  UINT16 tmp = oldValue + 1;
  if (tmp == moduloBase)
     tmp = 0;
  while (OSUINT16_LL(index) == oldValue)
     if (OSUINT16_SC(index,tmp))
        break;
} /*? end of IncrementFifoQueueIndex */

/*%DEFINITIONS_BLOCK_START%*/

/* 3 and 4 Slot buffer implementation */
typedef struct {      // Defines one slot that can be used with either slot schemes
  UINT8 *Data;        // buffer that holds a data
  UINT8 BufferItems;  // number of bytes currently used in the buffer
} BUFFER_DATA;


typedef struct {              // 4-slotted buffer descriptor
  BUFFER_DATA *CurrentWriter; // current writer task's slot
  BUFFER_DATA Slot[2][2];     // slots used by the writer and reader tasks
  BOOL Reading, Latest;       // 4-slot selection specifics
  BOOL CurrentWriterPair, CurrentWriterIndex, Index[2];
} BUFFER_4_SLOT;


typedef struct {              // 3-slotted buffer descriptor
  BUFFER_DATA *CurrentWriter; // current writer task's slot
  BUFFER_DATA Slot[3];        // slots used by the writer and reader tasks
  UINT8 Reading, Latest;      // 3-slot selection specifics
  UINT8 CurrentWriterIndex;   // current slit index being filled by the writer task
} BUFFER_3_SLOT;


/* General structure suitable for both a 3- or 4-slot reader-writer protocols. This
** structure can also be used by an interrupt hander defining the I/O buffers */
typedef struct {
  void *Buffer;               // 3- or 4-slot mechanism descriptor
  UINT8 BufferSize;           // number of bytes composing a full data item
  UINT8 Status;               // one of {BUFFER_INIT,BUFFER_UNREAD,BUFFER_READ}
  UINT8 BufferSlotType;       // one of {OS_BUFFER_TYPE_3_SLOT,OS_BUFFER_TYPE_4_SLOT}
  void *EventQueue;           // optional event associated the defined buffer
} BUFFER_DESCRIPTOR;


/* Internal slot buffer function prototypes */
STATIC BUFFER_DATA *GetReadyBuffer4Slot(BUFFER_DESCRIPTOR *descriptor);
STATIC BUFFER_DATA *GetReadyBuffer3Slot(BUFFER_DESCRIPTOR *descriptor);


/* Buffer states:
** BUFFER_INIT indicates an unfilled and unread buffer. This is the initial state of the
** buffer and its sole purpose is to distinguish an unfilled buffer from a valid read or
** unread buffer. This is a transient state that can no longer re-entered once exited.
** BUFFER_UNREAD indicates that there is an available and unread data buffer. The state
** of the buffer is set to BUFFER_READ only when an application task calling one of the
** OSGetBuffer() function and specifies OS_READ_ONLY_ONCE.
** BUFFER_READ indicates that the latest and most recent available data buffer has al-
** ready been read. */
#define BUFFER_INIT   0x1
#define BUFFER_UNREAD 0x2
#define BUFFER_READ   0x4

/*%DEFINITIONS_BLOCK_END%*/

/*$ OSInitBuffer: Creates a 3- or 4-slot buffer that can be used for I/Os and inter-task
** communications. Note that the scheme only applies to a single writer/reader task pair.
** Parameters:
**   (1) (UINT8) required buffer size;
**   (2) (UINT8) buffer slot type: this can either be OS_BUFFER_TYPE_3_SLOT or
**               OS_BUFFER_TYPE_4_SLOT;
**   (3) (void *) an optional event queue, which when specified can be used to signal a
**                task.
** Returned value: (void *) On success, the function returns a descriptor holding all
** state and buffer information that can be used for a device or for task communications.
** On memory allocation failure, the function returns NULL. */
void *OSInitBuffer(UINT8 bufferSize, UINT8 bufferSlotType, void *eventQueue)
{
  UINT8 i;
  BUFFER_DESCRIPTOR *descriptor;
  BUFFER_4_SLOT *buffer4;
  BUFFER_3_SLOT *buffer3;
  if ((descriptor = (BUFFER_DESCRIPTOR *)OSAlloca(sizeof(BUFFER_DESCRIPTOR))) == NULL)
     return NULL;
  descriptor->Status = BUFFER_INIT;
  descriptor->BufferSize = bufferSize;
  descriptor->BufferSlotType = bufferSlotType;
  descriptor->EventQueue = eventQueue;
  switch (bufferSlotType) {
     case OS_BUFFER_TYPE_4_SLOT:
        if ((descriptor->Buffer = OSAlloca(sizeof(BUFFER_4_SLOT))) == NULL)
           return NULL;
        buffer4 = (BUFFER_4_SLOT *)descriptor->Buffer;
        buffer4->Reading = 0;
        buffer4->Latest = 0;
        buffer4->CurrentWriterPair = 1;
        buffer4->CurrentWriterIndex = !(buffer4->Index[buffer4->CurrentWriterPair]);
        buffer4->CurrentWriter = &buffer4->Slot[buffer4->CurrentWriterPair][buffer4->CurrentWriterIndex];
        buffer4->CurrentWriter->BufferItems = 0;
        for (i = 0u; i < 2; i++)  // Allocate the buffer of each slot
           for (bufferSize = 0u; bufferSize < 2; bufferSize++)
              if ((buffer4->Slot[i][bufferSize].Data = (UINT8 *)OSAlloca(descriptor->BufferSize)) == NULL)
                 return NULL;
        break;
     case OS_BUFFER_TYPE_3_SLOT:
        if ((descriptor->Buffer = OSAlloca(sizeof(BUFFER_3_SLOT))) == NULL)
           return NULL;
        buffer3 = (BUFFER_3_SLOT *)descriptor->Buffer;
        buffer3->Reading = 3;  buffer3->Latest = 0;
        buffer3->CurrentWriterIndex = 2;
        buffer3->CurrentWriter = &buffer3->Slot[2];
        buffer3->CurrentWriter->BufferItems = 0;
        for (i = 0u; i < 3; i++)  // Allocate the buffer of each slot
           if ((buffer3->Slot[i].Data = (UINT8 *)OSAlloca(descriptor->BufferSize)) == NULL)
              return NULL;
     default: break;
  }
  return descriptor;
} /*? end of OSInitBuffer */


/*$ OSWriteBuffer: Copies the data to the current writer buffer and truncates those that
** exceeds the buffer size.
** Parameters:
**   (1) (void *) a buffer descriptor created by OSInitBuffer();
**   (2) (UINT8 *) data to copy into the writer buffer;
**   (3) (UINT8) number of data bytes to consider.
** Returned value: (UINT8 ) number of bytes accepted into the buffer. */
UINT8 OSWriteBuffer(void *descriptor, UINT8 *data, UINT8 size)
{
  static const UINT8 next [4][3] = {{1,2,1},{2,2,0},{1,0,0},{1,2,0}};
  BUFFER_DESCRIPTOR *descript = (BUFFER_DESCRIPTOR *)descriptor;
  BUFFER_DATA *element;
  UINT8 i = 0;
  if (descript != NULL) { // Check that the port was initialized
     element = ((BUFFER_4_SLOT *)descript->Buffer)->CurrentWriter;
     for ( ; i < size && i < descript->BufferSize &&
                                      element->BufferItems != descript->BufferSize; i++)
        element->Data[element->BufferItems++] = data[i]; // copy the byte
     if (element->BufferItems == descript->BufferSize) {
        /* Indicate to the reader that a full buffer is now ready for reading */
        descript->Status = BUFFER_UNREAD;
        /* Get a new slot for the next time the writer is invoked. */
        if (descript->BufferSlotType == OS_BUFFER_TYPE_4_SLOT) {
           BOOL wpair;
           BUFFER_4_SLOT *buf = (BUFFER_4_SLOT*)((BUFFER_DESCRIPTOR*)descriptor)->Buffer;
           wpair = buf->CurrentWriterPair;      // Continue with the previous buffer pair
           buf->Index[wpair] = buf->CurrentWriterIndex; // Writer indicates slot
           buf->Latest = wpair;                         // Writer indicates pair
           /* Prepare for the next time the writer gets a new byte. */
           buf->CurrentWriterPair = !buf->Reading;
           buf->CurrentWriterIndex = !buf->Index[wpair];
           buf->CurrentWriter = &buf->Slot[buf->CurrentWriterPair][buf->CurrentWriterIndex];
           buf->CurrentWriter->BufferItems = 0u;
        }
        else {
           UINT8 windex;
           BUFFER_3_SLOT *buffer;
           buffer = (BUFFER_3_SLOT*)((BUFFER_DESCRIPTOR*)descriptor)->Buffer;
           buffer->Latest = windex = buffer->CurrentWriterIndex;
           if (OSUINT8_LL(&buffer->Reading) == 3)
              OSUINT8_SC(&buffer->Reading,windex);
           /* Prepare for the next time the writer gets a new byte. */
           buffer->CurrentWriterIndex = windex = next[buffer->Reading][buffer->Latest];
           buffer->CurrentWriter = &buffer->Slot[windex];
           buffer->CurrentWriter->BufferItems = 0u;
        }
        /* Unblock a task if there is an event associated with a full buffer */
        if (descript->EventQueue != NULL)
           OSScheduleSuspendedTask(descript->EventQueue);
     }
  }
  return i;
} /*? end of OSWriteBuffer */


/*$ OSGetReferenceBuffer: Returns a pointer to the most recent data buffer held in the
** descriptor.
** Parameters:
**   (1) (void *) general buffer descriptor created and returned by OSInitBuffer();
**   (2) (UINT8) read mode: this can be OS_READ_ONLY_ONCE or OS_READ_MULTIPLE;
**   (3) (UINT8 **) pointer to the data buffer.
** Returned value: (UINT8) On success, the number of bytes that are available in the 3rd
** argument is returned. On failure or when there is nothing to read, 0 is returned and
** the data buffer is set to NULL. */
UINT8 OSGetReferenceBuffer(void *descriptor, UINT8 readMode, UINT8 **data)
{
  BUFFER_DESCRIPTOR *descript = (BUFFER_DESCRIPTOR *)descriptor;
  BUFFER_DATA *buffer;
  /* Check that the port was allocated and that there is something to read. */
  if (descript != NULL && descript->Status != BUFFER_INIT) {
     if (!readMode) {
        if (OSUINT8_LL(&descript->Status) != BUFFER_UNREAD)
           goto fail;
        if (!OSUINT8_SC(&descript->Status,BUFFER_READ))
           goto fail;
     }
     /* Get the slot holding the most recent written data. */
     if (descript->BufferSlotType == OS_BUFFER_TYPE_4_SLOT)
        buffer = GetReadyBuffer4Slot(descript);
     else
        buffer = GetReadyBuffer3Slot(descript);
     if (buffer->BufferItems > 0u) {
        *data = buffer->Data;
        return buffer->BufferItems;
     }
  }
fail:
  *data = NULL;
  return 0;
} /*? end of OSGetReferenceBuffer */


/*$ OSGetCopyBuffer: Returns a copy of the data held in the current buffer descriptor.
** Parameters:
**   (1) (void *) general buffer descriptor created and returned by OSInitBuffer();
**   (2) (UINT8) read mode: This can be OS_READ_ONLY_ONCE or OS_READ_MULTIPLE;
**   (3) (UINT8 *) data buffer where data is to be copied. This buffer should be greater
**          or equal to the buffer size specified when the general buffer descriptor was
**          created by OSInitBuffer().
** Returned value: (UINT8) Number of bytes copied into the specified data buffer. */
UINT8 OSGetCopyBuffer(void *descriptor, UINT8 readMode, UINT8 *data)
{
  UINT32 i;
  BUFFER_DESCRIPTOR *descript = (BUFFER_DESCRIPTOR *)descriptor;
  BUFFER_DATA *buffer;
  /* Check that the port was allocated and that there is something to read. */
  if (descript != NULL && descript->Status != BUFFER_INIT) {
     if (!readMode) {
        if (OSUINT8_LL(&descript->Status) != BUFFER_UNREAD)
           goto fail;
        if (!OSUINT8_SC(&descript->Status,BUFFER_READ))
           goto fail;
     }
     if (descript->BufferSlotType == OS_BUFFER_TYPE_4_SLOT)
        buffer = GetReadyBuffer4Slot(descript);
     else
        buffer = GetReadyBuffer3Slot(descript);
     for(i = 0; i < buffer->BufferItems; i++)
        data[i] = buffer->Data[i];
     return buffer->BufferItems;
  }
fail:
  return 0;
} /*? end of OSGetCopyBuffer */


/*$ GetReadyBuffer4Slot: Returns the next and most recent slot for the reader. This func-
** tion should be called each time an application task requests a copy or a reference to
** some shared data.
** Parameter: (void *) General buffer descriptor created and returned by OSInitBuffer().
** Returned value: Next available buffer slot to read from. */
BUFFER_DATA *GetReadyBuffer4Slot(BUFFER_DESCRIPTOR *descriptor)
{
  BOOL rpair, rindex;
  BUFFER_4_SLOT *buffer = (BUFFER_4_SLOT *)descriptor->Buffer;
  rpair = buffer->Latest;         // Reader chooses pair
  buffer->Reading = rpair;        // Reader indicates pair
  rindex = buffer->Index[rpair];  // Reader chooses slot
  return &buffer->Slot[rpair][rindex];
} /* end of GetReadyBuffer4Slot */


/*$ GetReadyBuffer3Slot: Same as GetReadyBuffer4Slot() but applied to a 3-slot mechanism.
** Parameter: (void *) General buffer descriptor created and returned by OSInitBuffer().
** Returned value: Next available buffer slot to read from. */
BUFFER_DATA *GetReadyBuffer3Slot(BUFFER_DESCRIPTOR *descriptor)
{
  BUFFER_3_SLOT *buffer = (BUFFER_3_SLOT *)descriptor->Buffer;
  buffer->Reading = 3;
  if (OSUINT8_LL(&buffer->Reading) == 3)
     OSUINT8_SC(&buffer->Reading,buffer->Latest);
  return &buffer->Slot[buffer->Reading];
} /*? end of GetReadyBuffer3Slot */

/*%DEFINITIONS_BLOCK_START%*/

/* User concurrent FIFO queue implementation */
typedef struct NODE {      // User nodes stored into the fifo queue
  UINT16 size;             // Encapsulated useful size of the node
  UINT16 info;             // Starting field of the node's content
} NODE;


typedef struct BUFFER_DESCRIPTOR_FIFO {
  NODE **Q;                // Array of queued items
  void *PendingOp;         // Posted operation for the queue (dequeue or enqueue op.)
  UINT16 Head, Tail;       // Current head and tail indices
  UINT16 MaxIndex;         // Value of Head or Tail at wrap-around
  UINT8 QueueLength;       // Circular array queue size
  FIFOQUEUE *FreeList;     // List of free blocks
} BUFFER_DESCRIPTOR_FIFO;

/*%DEFINITIONS_BLOCK_END%*/

/*$ OSInitFIFOQueue: Creates a circular array-based fifo queue that can be used for inter-
** task communication by the application. This function creates a fifo descriptor with an
** initialized empty queue along with its initial pool of free buffers. */
void *OSInitFIFOQueue(UINT8 maxNodes, UINT8 maxNodeSize)
{
  UINT8 i;
  BUFFER_DESCRIPTOR_FIFO *desc;
  /* Initalize FIFO */
  if ((desc = (BUFFER_DESCRIPTOR_FIFO *)OSAlloca(sizeof(BUFFER_DESCRIPTOR_FIFO))) == NULL)
     return NULL;
  desc->Head = desc->Tail = 0;
  desc->QueueLength = maxNodes;
  desc->MaxIndex = GetFIFOArrayMaxIndex(maxNodes);
  desc->PendingOp = NULL;
  if ((desc->Q = (NODE **)OSAlloca(maxNodes * sizeof(NODE *))) == NULL)
     return NULL;
  for (i = 0; i < maxNodes; i++)
     desc->Q[i] = NULL;
  /* Create blocks and initialize the free list */
  desc->FreeList = (FIFOQUEUE *)OSCreateEventDescriptor();
  desc->FreeList->QueueLength = maxNodes;
  desc->FreeList->MaxIndex = GetFIFOArrayMaxIndex(maxNodes);
  if ((desc->FreeList->Q = (UINTPTR *)OSAlloca(maxNodes * sizeof(NODE *))) == NULL)
     return NULL;
  for (i = 0; i < maxNodes; i++)
     if ((desc->FreeList->Q[i] = (UINTPTR)OSAlloca(sizeof(NODE) - sizeof(UINT16) + maxNodeSize)) == NULL)
        return NULL;
  return (void *)desc;
} /*? OSInitFIFOQueue */


/*$ OSEnqueueFIFO: Inserts a node into a fifo queue. The size parameter (user input) is
** stored into the node so that it can be transfered to the dequeuer task. */
BOOL OSEnqueueFIFO(void *queue, void *node, UINT16 size)
{
  NODE *tmpNode = (NODE *)(((UINTPTR)node) - sizeof(UINT16));
  tmpNode->size = size;
  return FIFOEnqueue((FIFOQUEUE *)queue,NULL,(UINTPTR)tmpNode);
} /*? end of OSEnqueueFIFO */


/*$ OSDequeueFIFO: Retrieves a node from the fifo queue. The size parameter (output para-
** meter corresponds to the value that was stored by the enqueue function. */
void *OSDequeueFIFO(void *queue, UINT16 *size)
{
  NODE *node;
  if ((node = (NODE *)FIFODequeue((FIFOQUEUE *)queue,NULL)) != NULL) {
     *size = node->size;
     node = (NODE *)&node->info;
  }
  return node;
} /*? end of OSDequeueFIFO */


/*$ OSGetFreeNodeFIFO: Returns a node from the free pool of a fifo queue. */
void *OSGetFreeNodeFIFO(void *descriptor)
{
  NODE *tmp = (NODE *)FIFODequeue(((BUFFER_DESCRIPTOR_FIFO *)descriptor)->FreeList,NULL);
  if (tmp == NULL)
     return NULL;
  else
     return &tmp->info;
} /*? end of OSGetFreeNodeFIFO */


/*$ OSReleaseNodeFIFO: Inserts a node into the free pool of nodes associated with a fifo
** queue. */
void OSReleaseNodeFIFO(void *descriptor, void *node)
{
  node = (void *)((UINTPTR)node - sizeof(UINT16));
  FIFOEnqueue(((BUFFER_DESCRIPTOR_FIFO *)descriptor)->FreeList,NULL,(UINTPTR)node);
} /*? end OSReleaseNodeFIFO */

