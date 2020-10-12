
/* Defines the default stack size for the Tasks */
#define mainDEFAULT_STACK_SIZE configMINIMAL_STACK_SIZE * 10

/* The number of items the queue can hold at once. */
#define mainQUEUE_LENGTH (2)

/* The values sent to the queue receive task from the queue send task and the
queue send software timer respectively. */
#define mainVALUE_SENT_FROM_TASK (100UL)
#define mainVALUE_SENT_FROM_TIMER (200UL)

/* Priorities at which the tasks are created. Placeholder for future RM 
priority setter */

#define mainTASK1_PRIORITY (tskIDLE_PRIORITY + 3)
#define mainTASK2_PRIORITY (tskIDLE_PRIORITY + 2)
#define mainTASK3_PRIORITY (tskIDLE_PRIORITY + 1)

/* Period of execution of the Tasks in mS */
/* TASKS HAVE TO BE SORTED BY PERIOD */

#if configTICK_RATE_HZ != 100
#error TASKS HAVE TO BE SORTED BY PERIOD
#endif 

#define mainTASK1_PERIOD 50
#define mainTASK2_PERIOD 100
#define mainTASK3_PERIOD 150

#if mainTASK1_PERIOD > mainTASK2_PERIOD || mainTASK1_PERIOD > mainTASK3_PERIOD || mainTASK2_PERIOD > mainTASK3_PERIOD
#error TASKS HAVE TO BE SORTED BY PERIOD
#endif



int mainTaskPeriods[3] = {mainTASK1_PERIOD,mainTASK2_PERIOD,mainTASK3_PERIOD};

/* Worst case of execution of the Tasks in Ticks  - Assuming Tick rate == 100. Correct if different*/
#define mainTASK1_WORSTCASE 21
#define mainTASK2_WORSTCASE 21
#define mainTASK3_WORSTCASE 21

int mainWorstCaseComputeTime[3] = {mainTASK1_WORSTCASE,mainTASK2_WORSTCASE,mainTASK3_WORSTCASE};


/* To be used by the tasks to update the in which Tick they will be ready*/
int deadlineTask1 = mainTASK1_PERIOD;
int deadlineTask2 = mainTASK2_PERIOD;
int deadlineTask3 = mainTASK3_PERIOD;

int* deadlines[3] = {&deadlineTask1,&deadlineTask2,&deadlineTask3};

