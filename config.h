#ifndef CONFIG_H
#define CONFIG_H

/* Application specific defines */
#define configCONSUMPTION_TEST true

/* Turn Off Magic Interface */
#define config_MAGIC_INTERFACE_DISABLE 1
// #define lowPowerMode 2

/* Sleep on idle Task Hook */
#define configUSE_IDLE_HOOK 0
#define configCPU_LOW_POWER_MODE 0


/* Set configDYNAMIC_FREQUENCY_LOW_POWER_MODE to one to run the simple blinky low power
demo, or 0 to run the more comprehensive test and demo application. */
// #define configDYNAMIC_FREQUENCY_LOW_POWER_MODE 0
#define configDYNAMIC_FREQUENCY_TICKS 1

#ifdef configDYNAMIC_FREQUENCY_TICKS
#define dynamicFrequencySysTickHandler SysTick_Handler

#else
#define xPortSysTickHandler SysTick_Handler

#endif


/* Config tickless idle */
#define configUSE_TICKLESS_IDLE 0

/* A few settings are dependent on the configDYNAMIC_FREQUENCY_LOW_POWER_MODE setting. */
#if configUSE_TICKLESS_IDLE == 2

#if configDYNAMIC_FREQUENCY_TICKS != 1
	#error "The dynamic frequency ticks is required, as it sets up the timer interrupt for tickless idle"
#endif

#define configTICK_RATE_HZ (100)
#define configEXPECTED_IDLE_TIME_BEFORE_SLEEP (20 + 1) /* ( ( 200 / portTICK_PERIOD_MS ) + 1 ) written out pre-processed to enable #error statements to check its value. */
#define configUSE_TIMERS 0
#define dynamicFrequencySysTickHandler SysTick_Handler
#define portSUPPRESS_TICKS_AND_SLEEP(xIdleTime) vApplicationSleep(xIdleTime)

#define configPRE_SLEEP_PROCESSING(x) \
	Sleep(); \
    x=0;
// #define configPOST_SLEEP_PROCESSING(x) LPC_GPIO1->FIOPIN = (1 << LED4);  

// LPC_GPIO1->FIOPIN = (1 << LED4);  
// LPC_GPIO1->FIOPIN = (1 << LED4);  
// #else

// #define xPortSysTickHandler SysTick_Handler

#endif


#if (configUSE_IDLE_HOOK == 1) && (configUSE_TICKLESS_IDLE !=0)
	#error "The idle hook and the tickless are not compatible with each other, choose one"
#endif

#endif //CONFIG_H
// /* Defines the default stack size for the Tasks */
// #define mainDEFAULT_STACK_SIZE configMINIMAL_STACK_SIZE * 10

// /* The number of items the queue can hold at once. */
// #define mainQUEUE_LENGTH (2)

// /* The values sent to the queue receive task from the queue send task and the
// queue send software timer respectively. */
// #define mainVALUE_SENT_FROM_TASK (100UL)
// #define mainVALUE_SENT_FROM_TIMER (200UL)

// /* Priorities at which the tasks are created. Placeholder for future RM 
// priority setter */

// #define mainTASK1_PRIORITY (tskIDLE_PRIORITY + 3)
// #define mainTASK2_PRIORITY (tskIDLE_PRIORITY + 2)
// #define mainTASK3_PRIORITY (tskIDLE_PRIORITY + 1)

// /* Period of execution of the Tasks in mS */
// /* TASKS HAVE TO BE SORTED BY PERIOD */
// #define mainTASK1_PERIOD 500
// #define mainTASK2_PERIOD 1000
// #define mainTASK3_PERIOD 1500

// #if mainTASK1_PERIOD > mainTASK2_PERIOD || mainTASK1_PERIOD > mainTASK3_PERIOD || mainTASK2_PERIOD > mainTASK3_PERIOD
// #error TASKS HAVE TO BE SORTED BY PERIOD
// #endif

// int mainTaskPeriods[3] = {mainTASK1_PERIOD,mainTASK2_PERIOD,mainTASK3_PERIOD};

// /* Worst case of execution of the Tasks in Ticks  - Assuming Tick rate == 100. Correct if different*/
// #define mainTASK1_WORSTCASE 21
// #define mainTASK2_WORSTCASE 42
// #define mainTASK3_WORSTCASE 66

// int mainWorstCaseComputeTime[3] = {mainTASK1_WORSTCASE,mainTASK2_WORSTCASE,mainTASK3_WORSTCASE};


// /* To be used by the tasks to update the in which Tick they will be ready*/
// int deadlineTask1 = mainTASK1_PERIOD;
// int deadlineTask2 = mainTASK2_PERIOD;
// int deadlineTask3 = mainTASK3_PERIOD;

// int* deadlines[3] = {&deadlineTask1,&deadlineTask2,&deadlineTask3};

