
/*
 * The tick hook function.  This compensates
 * the tick count to compensate for frequency changes
 */
const uint8_t mValues[12] = {36, 33, 30, 27, 24, 18, 12, 9, 6, 3};
const int frequencyLevels[] = {96, 88, 80, 72, 64, 48, 32, 24, 16, 8};
const int staticTickIncrement[] = {0, 0, 0, 0, 0, 1, 2, 3, 5, 11};
const int periodicTickIncrement[] = {0, 11, 5, 3, 2, 0, 0, 0, 0, 0};
volatile int currentFrequencyLevel = 0;
volatile short periodicTickIncrementCount = 0;
volatile bool frequencyChanged = false;
const short availableFrequencyLevels = 10;

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
#define mainTASK1_PERIOD 500
#define mainTASK2_PERIOD 1000
#define mainTASK3_PERIOD 1500
