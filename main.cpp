#include "mbed.h"
/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "PowerControl.h"
#include "EthernetPowerControl.h"
#include "ClockControl.h"
#include "calculations.h"
#include "RTC.h"
#include "TARGET_LPC1768_N/LPC17xx.h"
#include "lpc1768_rtc.h"

#include "LPC1768_LOW_POWER_TICK_MANAGEMENT.h"

#include "TaskConfiguration.h"

#include "cycleCounter.h"

//#define cycleCounter 1

DigitalOut myled1(LED1);
DigitalOut myled3(LED3);
DigitalOut myled2(LED2);
DigitalOut myled4(LED4);

#define ld2 18 // P2_1

/* Turn Off Magic Interface */

#define magicINTERFACEDISABLE 1

/*-----------------------------------------------------------*/
void vTask1(void *pvParameters);
void vTask2(void *pvParameters);
void vTaskFibonnaciFixedTime210ms(void *pvParameters);
void vTaskFibonnaciFixedTime420ms(void *pvParameters);
Serial pc(USBTX, USBRX);
int led = 0;
volatile long contador = 0;
volatile long deadline1 = 0;
volatile long deadline2 = 0;
volatile long dealine3 = 0;

/* Test application timers */
int fixedFibonnaciTaskPeriod210ms = 1000;
int fixedFibonnaciTaskPeriod420ms = 2000;
int dynamicFibonnaciTaskPeriod = 500;

/* DVFS Variables*/
//uint8_t DVFS_ON=1;
volatile short frequencyDivider = 1;
extern int test;
extern TickType_t useridleTime;
volatile tm t3;
#define minDivider 1

//DEBUG VARIABLES
DigitalIn vUSBIN(p10); // USED TO NOT TURN OFF THE MAGIC INTERFACE WHEN CONNECTED TO PC - It is connected to VUSB

char ptrTaskList[500] = {0};
static void PrintTaskInfo();
//Changed min stack size to 300

/* bit position of CCR register */
#define SBIT_CLKEN 0  /* RTC Clock Enable*/
#define SBIT_CTCRST 1 /* RTC Clock Reset */
#define SBIT_CCALEN 4 /* RTC Calibration counter enable */

int main()
{
    int periods[1] = {100};
    int compute[1] = {50};

    default_setupDVFS(1, compute, periods, 0);
    // int test = staticVoltageScalingFrequencyLevelSelector();
    // wait(1);
    // Serial pc(USBTX, USBRX);
    // pc.printf("Frequency chosen by SVS RM level %d : %d MHz \n", test, frequencyLevels[test]);
    // pc.printf("M level %d : %d MHz \n", *deadlines[0], SystemCoreClock);

#if magicINTERFACEDISABLE == 1
    int usbCONNECTED = vUSBIN;
    if (usbCONNECTED == 0)
    {
        int result;
        // On reset, determine if a watchdog, power on reset or a pushbutton caused reset and display on LED 4 or 3
        // Check reset source register
        result = LPC_SC->RSID;
        if ((result & 0x03) == 0)
        {
            // was not POR or reset pushbutton so
            // Mbed is out of debug mode and reset so can enter DeepSleep now and wakeup using watchdog
            time_t t;
            srand((unsigned)time(&t));
            struct RTC_DATA now = defaultTime();
            initRTC(now);

            KIN1_InitCycleCounter(); /* enable DWT hardware */
            PHY_PowerDown();
            //LPC_GPIO1->FIODIR = (1<<ld2);
            //RTC::alarm(&alarmFunction, t2);

            //pc.printf("bbbb %d\n", tee-1);
            //pc.printf("AAA %d Hz, %d\n", SystemCoreClock, LPC_SC->CCLKCFG);
            //Default divider is 3, and clock source is the 4Mhz IRC clock, m 36, n 1
            /* Start the two tasks as described in the comments at the top of this
    file. */
            //    xTaskCreate( vTask1,           /* The function that implements the task. */
            //                 "Task1",                           /* The text name assigned to the task - for debug only as it is not used by the kernel. */
            //                 mainDEFAULT_STACK_SIZE,       /* The size of the stack to allocate to the task. */
            //                 NULL,                           /* The parameter passed to the task - not used in this simple case. */
            //                 mainTASK1_PRIORITY,/* The priority assigned to the task. */
            //                 NULL );                         /* The task handle is not required, so NULL is passed. */

            xTaskCreate(vTask2, "Task2", mainDEFAULT_STACK_SIZE, &deadlineTask2, mainTASK2_PRIORITY, NULL);

            /* Start the tasks and timer running. */
            vTaskStartScheduler();
            for (;;)
                ;
        }
        else
        {
            // Was an initial manual or Power on Reset
            // This codes only executes the first time after initial POR or button reset
            LPC_SC->RSID = 0x0F;
            // Clear reset source register bits for next reset
            myled2 = 1;
            result = mbed_interface_powerdown();
            // Now can do a reset to free mbed of debug mode
            // NXP manual says must exit debug mode and reset for DeepSleep or lower power levels to wakeup
            wait(1);
            NVIC_SystemReset();
        }
    }
#endif

    time_t t;
    srand((unsigned)time(&t));
    struct RTC_DATA now = defaultTime();
    initRTC(now);

    KIN1_InitCycleCounter(); /* enable DWT hardware */
    PHY_PowerDown();
    //LPC_GPIO1->FIODIR = (1<<ld2);
    //RTC::alarm(&alarmFunction, t2);

    //pc.printf("bbbb %d\n", tee-1);
    //pc.printf("AAA %d Hz, %d\n", SystemCoreClock, LPC_SC->CCLKCFG);
    //Default divider is 3, and clock source is the 4Mhz IRC clock, m 36, n 1
    /* Start the two tasks as described in the comments at the top of this
    file. */
    //    xTaskCreate( vTask1,           /* The function that implements the task. */
    //                 "Task1",                           /* The text name assigned to the task - for debug only as it is not used by the kernel. */
    //                 mainDEFAULT_STACK_SIZE,       /* The size of the stack to allocate to the task. */
    //                 NULL,                           /* The parameter passed to the task - not used in this simple case. */
    //                 mainTASK1_PRIORITY,/* The priority assigned to the task. */
    //                 NULL );                         /* The task handle is not required, so NULL is passed. */

    xTaskCreate(vTask2, "Task2", mainDEFAULT_STACK_SIZE, &deadlineTask2, 3, NULL);
    //xTaskCreate(vTaskFibonnaciFixedTime210ms, "Fixed Fibonnaci Task 210 ms", configMINIMAL_STACK_SIZE, &fixedFibonnaciTaskPeriod210ms, 2, NULL);
    //xTaskCreate(vTaskFibonnaciFixedTime420ms, "Fixed Fibonnaci Task 420 ms", configMINIMAL_STACK_SIZE, &fixedFibonnaciTaskPeriod420ms, 2, NULL);

    /* Start the tasks and timer running. */
    vTaskStartScheduler();

    /* If all is well, the scheduler will now be running, and the following
    line will never be reached.  If the following line does execute, then
    there was insufficient FreeRTOS heap memory available for the idle and/or
    timer tasks to be created.  See the memory management section on the
    FreeRTOS web site for more details. */
    for (;;)
        ;
}

volatile unsigned long x = 0, y = 0;
void vTask1(void *pvParameters)
{
    const TickType_t xDelay = mainTASK1_PERIOD / portTICK_PERIOD_MS;
    TickType_t xLastWakeTime;
    long begin, end;
    struct RTC_DATA now;
    xLastWakeTime = xTaskGetTickCount();
    srand(time(NULL));
    long n;
    int cycles = 1012000 / 2; //Takes around 105ms at 96Mhz

    for (;;)
    {
        deadline2 = xLastWakeTime + xDelay;
#ifdef DEBUG
        myled4 = 1;
#endif

#ifdef cycleCounter
        KIN1_ResetCycleCounter();  /* reset cycle counter */
        KIN1_EnableCycleCounter(); /* start counting */
        n = fibonnacciCalculation(cycles);
#ifdef DEBUG
        end = KIN1_GetCycleCounter(); /* get cycle counter */
        double timeInMs = end / (SystemCoreClock / 1000.0);
        pc.printf("Took %ld cycles at %ld to calculate %ld, resulting in a compute time of "
                  "%lf ms \n",
                  end, SystemCoreClock, n, timeInMs);
#endif

#else
        begin = xTaskGetTickCount();
        n = fibonnacciCalculation(cycles);
        end = xTaskGetTickCount();
#ifdef DEBUG
        double timeInMs = (end - begin) * portTICK_PERIOD_MS;
        pc.printf("Took %ld ticks at %ld to calculate %ld, resulting in a compute time of "
                  "%lf, %ld prescalar, %ld Tick Count ms\n",
                  end - begin, SystemCoreClock, n, timeInMs, LPC_TIM1->PR, end);
#endif
#endif

        vTaskDelayUntil(&xLastWakeTime, xDelay);
        //LPC_GPIO1->FIOPIN ^= (1<<ld2);
    }
}

void vTaskFibonnaciFixedTime210ms(void *pvParameters)
{
    int *intPvParameters = (int *)pvParameters;
    int delayinMs = intPvParameters[0];
    const TickType_t xDelay = delayinMs / portTICK_PERIOD_MS;
    TickType_t xLastWakeTime;
    int cycles = 1012000; //Takes around 210ms at 96Mhz
    xLastWakeTime = xTaskGetTickCount();
    long n; //Needed otherwise the compiler skips the fibonacci calculation
    for (;;)
    {
vTaskDelayUntil(&xLastWakeTime, xDelay);
#if POWERSAVINGMODE == 3
        cycleConservingDVSTaskReady(1, xTaskGetTickCount(), xLastWakeTime + xDelay);
#endif
        if (myled2 == 0)
            myled2 = 1;
        else
            myled2 = 0;
        n = fibonnacciCalculation(cycles);
        pc.printf("Delay : %d\n", xDelay);
#if POWERSAVINGMODE == 3
        cycleConservingDVSTaskComplete(1, xTaskGetTickCount());
#endif
        vTaskDelayUntil(&xLastWakeTime, xDelay);
    }
}

void vTaskFibonnaciFixedTime420ms(void *pvParameters)
{
    int *intPvParameters = (int *)pvParameters;
    int delayinMs = intPvParameters[0];
    const TickType_t xDelay = delayinMs / portTICK_PERIOD_MS;
    TickType_t xLastWakeTime;
    int cycles = 1012000; //Takes around 210ms at 96Mhz
    xLastWakeTime = xTaskGetTickCount();
    long n; //Needed otherwise the compiler skips the fibonacci calculation
    for (;;)
    {
 vTaskDelayUntil(&xLastWakeTime, xDelay);
#if POWERSAVINGMODE == 3
        cycleConservingDVSTaskReady(1, xTaskGetTickCount(), xLastWakeTime + xDelay);
#endif
        if (myled3 == 0)
            myled3 = 1;
        else
            myled3 = 0;
        n = fibonnacciCalculation(cycles);
        pc.printf("Delay : %d\n", delayinMs);
#if POWERSAVINGMODE == 3
        cycleConservingDVSTaskComplete(1, xTaskGetTickCount());
#endif
       
    }
}

void vTask2(void *pvParameters)
{
    const TickType_t xDelay = mainTASK2_PERIOD / portTICK_PERIOD_MS;
    TickType_t xLastWakeTime;
    long begin, end;
    //int* taskDeadline = (int*) pvParameters[0];
    xLastWakeTime = xTaskGetTickCount();
    srand(time(NULL));
    long n;
    int cycles = 1012000; //Takes around 210ms at 96Mhz

    for (;;)
    {

#if POWERSAVINGMODE == 3
        cycleConservingDVSTaskReady(1, xTaskGetTickCount(), xLastWakeTime + xDelay);
#endif

        //*taskDeadline= xLastWakeTime + xDelay;
        if (myled1 == 0)
            myled1 = 1;
        else
            myled1 = 0;

        // #ifdef DEBUG
        //         //myled4 = 1;
        // #endif

        // #ifdef cycleCounter
        //         KIN1_ResetCycleCounter();  /* reset cycle counter */
        //         KIN1_EnableCycleCounter(); /* start counting */
        //         n = fibonnacciCalculation(cycles);
        // #ifdef DEBUG
        //         end = KIN1_GetCycleCounter(); /* get cycle counter */
        //         double timeInMs = end / (SystemCoreClock / 1000.0);
        //         pc.printf("Took %ld cycles at %ld to calculate %ld, resulting in a compute time of "
        //                   "%lf ms \n",
        //                   end, SystemCoreClock, n, timeInMs);
        // #endif

        // #else
        //         begin = xTaskGetTickCount();
        //         n = fibonnacciCalculation(cycles);
        //         end = xTaskGetTickCount();
        // #ifdef DEBUG
        //         double timeInMs = (end - begin) * portTICK_PERIOD_MS;
        //         pc.printf("Took %ld ticks at %ld to calculate %ld, resulting in a compute time of "
        //                   "%lf, %ld prescalar, %ld Tick Count ms\n",
        //                   end - begin, SystemCoreClock, n, timeInMs, LPC_TIM1->PR, end);
        // #endif
        // #endif

        //pc.printf(" \n");
        //if(xTaskGetTickCount()>(xLastWakeTime+ xDelay)) myled2=1;
        //char stats[300]={0};
        //vTaskGetRunTimeStats( stats );
        //vTaskList(stats);
        //pc.printf("%s", stats);
        /* Read Time */
        //PrintTaskInfo();
        //now = getTime();

        pc.printf("Hello world %d\n", contador);
        //pc.printf("Time: %2d:%2d:%2d\n",now.hour,now.min,now.sec);
        //pc.printf("%d ms,%d ticks, %d last, %d deadline1, %d deadline2\n",cycles,xTaskGetTickCount(), xLastWakeTime, deadline1,deadline2);
        //pc.printf("%d ticks, %d idle time, %d deadline2\n",xTaskGetTickCount(),useridleTime,deadline2);

        //pc.printf("%s \n", stats);
        //        if(deadline2 >(xTaskGetTickCount()-xLastWakeTime)*2) {
        //            if(frequencyDivider<=minDivider) {
        //                if(frequencyDivider==0) setSystemFrequency(3,0,36, 1);
        //                else {
        //                    setSystemFrequency(3*frequencyDivider,0,36, 1);
        //                    Serial pc2(USBTX, USBRX); // Descobrir como arrumar o serial quando a frequencia muda
        //                    frequencyDivider++;
        //                }
        //            }
        //        }

        //pc.printf("%d Hz, %d, divider %d, 1s = %d ticks\n", SystemCoreClock, LPC_SC->CCLKCFG,frequencyDivider, 1000 / portTICK_PERIOD_MS);

        // if (contador != 0 && contador%5==0 && currentFrequencyLevel<=6) {

        // }
        contador++;
        // myled4 = 0;
        // myled1 = 1;

#if POWERSAVINGMODE == 3
        cycleConservingDVSTaskComplete(1, xTaskGetTickCount());
#endif
        vTaskDelayUntil(&xLastWakeTime, xDelay);
        //LPC_GPIO1->FIOPIN ^= (1<<ld2);
    }
}

#ifdef __cplusplus
extern "C"
#endif
    void
    vApplicationIdleHook(void)
{
    //Sleep();
    //DeepSleep();
    //PowerDown();
    //DeepPowerDown();
}

#ifdef __cplusplus
extern "C"
#endif
    void
    vApplicationMallocFailedHook(void)
{
    while (1)
    {
        pc.printf("Malloc error !! Hello, world!\n");
    }
}

#ifdef __cplusplus
extern "C"
#endif
    void
    vApplicationStackOverflowHook(TaskHandle_t xTask,
                                  char *pcTaskName)
{
    while (1)
    {
        pc.printf("Stack Overflow error !! Hello, world!\n");
    }
}

// /* Defined in main.c. */
// #ifdef __cplusplus
// extern "C"
// #endif
//     void
//     vConfigureTimerForRunTimeStats(void)
// {
//     const unsigned long TCR_COUNT_RESET = 2, CTCR_CTM_TIMER = 0x00, TCR_COUNT_ENABLE = 0x01;

//     /* This function configures a timer that is used as the time base when
//     collecting run time statistical information - basically the percentage
//     of CPU time that each task is utilising.  It is called automatically when
//     the scheduler is started (assuming configGENERATE_RUN_TIME_STATS is set
//     to 1). */

//     /* Power up and feed the timer. */
//     LPC_SC->PCONP |= 0x02UL;
//     LPC_SC->PCLKSEL0 = (LPC_SC->PCLKSEL0 & (~(0x3 << 2))) | (0x01 << 2);

//     /* Reset Timer 0 */
//     LPC_TIM0->TCR = TCR_COUNT_RESET;

//     /* Just count up. */
//     LPC_TIM0->CTCR = CTCR_CTM_TIMER;

//     /* Prescale to a frequency that is good enough to get a decent resolution,
//     but not too fast so as to overflow all the time. */
//     LPC_TIM0->PR = (configCPU_CLOCK_HZ / 10000UL) - 1UL;

//     /* Start the counter. */
//     LPC_TIM0->TCR = TCR_COUNT_ENABLE;
// }

// static void PrintTaskInfo()
// {

//     pc.printf("**********************************\n");
//     pc.printf("Task  State   Prio    Stack    Num\n");
//     pc.printf("**********************************\n");
//     vTaskList(ptrTaskList);
//     pc.printf(ptrTaskList);
//     pc.printf("**********************************\n");
// }

/* 
* Pre sleep processing for tickless
*/
