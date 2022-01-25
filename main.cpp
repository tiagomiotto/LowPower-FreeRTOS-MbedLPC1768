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



/*-----------------------------------------------------------*/
void vTask1(void *pvParameters);
void vTask2(void *pvParameters);
void vTaskFibonnaciFixedTime210ms(void *pvParameters);
void vTaskFibonnaciFixedTime420ms(void *pvParameters);
void vTaskFibonnaciDynamicTime(void *pvParameters);

void vDummyTask(void *pvParameters);
Serial pc(USBTX, USBRX);

volatile long contador = 0;
volatile long deadline1 = 0;
volatile long deadline2 = 0;
volatile long dealine3 = 0;


struct taskProperties
{
    int taskNumber;
    int xDelay;
    int xFibonnaciCycles;
    int xFibonnaciCyclesWorstCase;
    int* xPowerConsumptionTestIsWorstCase;
};



// Shunt_cal = 13107.2 * 10^6 * (200ma/2^19) *0.1 Ohm = 500
// Add the conversion to ticks ( it is currently in ticks but we want in ms)
int fixedFibonnaciTaskPeriod210ms = 400;
int fixedFibonnaciTaskPeriod420ms = 500;
int dynamicFibonnaciTaskPeriod = 700;
bool ConsumptionTest = false;
bool deadlinesMissed = false;


// DEBUG VARIABLES
/* Turn Off Magic Interface */
#define magicINTERFACEDISABLE 1
DigitalIn vUSBIN(p10); // USED TO NOT TURN OFF THE MAGIC INTERFACE WHEN CONNECTED TO PC - It is connected to VUSB


int main()
{
#define DVFSMODE 2
    int main_taskWorstCaseComputeTime[3] = {21, 42, 84};
    int main_taskDeadlines[3] = {fixedFibonnaciTaskPeriod210ms, fixedFibonnaciTaskPeriod420ms, dynamicFibonnaciTaskPeriod};
    int main_frequencyLevels[6] = {96, 80, 72, 48, 24, 16};

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

            KIN1_InitCycleCounter(); /* enable DWT hardware */
            PHY_PowerDown();

            // xTaskCreate(vTaskFibonnaciFixedTime210ms, "Fixed Fibonnaci Task 210 ms", configMINIMAL_STACK_SIZE, &fixedFibonnaciTaskPeriod210ms, 4, NULL);
            // xTaskCreate(vTaskFibonnaciFixedTime420ms, "Fixed Fibonnaci Task 420 ms", configMINIMAL_STACK_SIZE, &fixedFibonnaciTaskPeriod420ms, 3, NULL);
            // xTaskCreate(vTaskFibonnaciDynamicTime, "Dynamic Fibonnaci Task 210-840 ms", configMINIMAL_STACK_SIZE, &dynamicFibonnaciTaskPeriod, 2, NULL);
            ConsumptionTest = true;
            // #define POWERSAVINGMODE  3


            vTaskStartLowPowerScheduller(3, main_taskWorstCaseComputeTime, main_taskDeadlines, 6, main_frequencyLevels, DVFSMODE);
            vTaskStartScheduler();

            for (;;)
                ;
        }
        else
        {
            // Was an initial manual or Power on Reset
            // Clear reset source register bits for next reset
            // This codes only executes the first time after initial POR or button reset
            // Now can do a reset to free mbed of debug mode
            LPC_SC->RSID = 0x0F;
            result = mbed_interface_powerdown(); 
            wait(1);
            NVIC_SystemReset();
        }
    }
#endif




    KIN1_InitCycleCounter(); /* enable DWT hardware  IS THIS NECESSARY???*/
    PHY_PowerDown();

    struct taskProperties dummyProperties;
    int task1WorstCase[8] = {0,1,0,0,1,1,0,1};
    dummyProperties.taskNumber = 1;
    dummyProperties.xDelay = 1000;
    dummyProperties.xFibonnaciCycles = 2;
    dummyProperties.xFibonnaciCyclesWorstCase = 4;
    dummyProperties.xPowerConsumptionTestIsWorstCase = task1WorstCase;
    xTaskCreate(vDummyTask, "Dummy Task", configMINIMAL_STACK_SIZE * 4, &dummyProperties, 2, NULL);

    // vTaskStartLowPowerScheduller(3, main_taskWorstCaseComputeTime, main_taskDeadlines, 6, main_frequencyLevels, DVFSMODE);
    // wait(3);
    vTaskStartScheduler();
    /* Start the tasks and timer running. */
    // vTaskStartScheduler();

    for (;;)
        ;
}


void vDummyTask(void *pvParameters)
{
    /* Unpack parameters into local variables for ease of interpretation */
    struct taskProperties *parameters = (struct taskProperties*) pvParameters;
    const TickType_t xDelay = parameters->xDelay;
    int baseCycles = parameters->xFibonnaciCycles; // Takes around 210ms at 96Mhz
    int worstCaseCycles = parameters->xFibonnaciCyclesWorstCase; // Takes around 210ms at 96Mhz
    int *isWorstCase = parameters->xPowerConsumptionTestIsWorstCase;
    int taskNumber = parameters->taskNumber;
    
    /* Define local variables for counting runs and delays */
    TickType_t xLastWakeTime = xTaskGetTickCount();
    long fibonnacciAuxiliar = 0;
    int runNumber =0;
    int n =0;


    for (;;)
    {
        
#if DVFSMODE == 2
        // taskENTER_CRITICAL();
        cycleConservingDVSTaskReady(taskNumber, xTaskGetTickCount(), xLastWakeTime + xDelay);
        // taskEXIT_CRITICAL();
#endif
        n = fibonnacciCalculation(isWorstCase[runNumber] == 0 ? baseCycles : worstCaseCycles);
        runNumber++;
        if(runNumber>7) runNumber=0;
#if DVFSMODE == 2
        // taskENTER_CRITICAL();
        cycleConservingDVSTaskComplete(taskNumber, xTaskGetTickCount());
        // taskEXIT_CRITICAL();
#endif
        if ((xTaskGetTickCount()) > xLastWakeTime + xDelay)
        {
            myled1 = 1;
            myled2 = 0;
            myled3 = 0;
            myled4 = 1;
            vTaskSuspendAll();
        }
        vTaskDelayUntil(&xLastWakeTime, xDelay);
    }
        
}

#ifdef __cplusplus
extern "C"
#endif
    void
    vApplicationIdleHook(void)
{
    Sleep();
    // DeepSleep();
    // PowerDown();
    // DeepPowerDown();
}

#ifdef __cplusplus
extern "C"
#endif
    void
    vApplicationMallocFailedHook(void)
{
    myled4 = 1;
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
    myled4 = 1;
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

/*
#define EXECUTIONCOUNT 15
void vTaskFibonnaciFixedTime210ms(void *pvParameters)
{
    int *intPvParameters = (int *)pvParameters;
    int delayinMs = intPvParameters[0];
    const TickType_t xDelay = delayinMs;
    TickType_t xLastWakeTime;
    int cycles = 1012000; // Takes around 210ms at 96Mhz
    int powerConsumptionTestIsWorstCase[7] = {1,0,0,1,1,0,1};
    int executionCount = 0;
    xLastWakeTime = xTaskGetTickCount();
    int level = 0;
    long n; // Needed otherwise the compiler skips the fibonacci calculation
    for (;;)
    {
        //// Mudar o codigo do task ready pra poder ter um delay no inicio
#if DVFSMODE == 2
        // taskENTER_CRITICAL();
        cycleConservingDVSTaskReady(1, xTaskGetTickCount(), xLastWakeTime + xDelay);
        // taskEXIT_CRITICAL();
#endif
        n = fibonnacciCalculation(cycles);

        if (!ConsumptionTest)
        {
            if (executionCount < EXECUTIONCOUNT)
            {
                if (myled1 == 0)
                    myled1 = 1;
                else
                    myled1 = 0;
                executionCount++;
            }
            // Está a travar na segunda passada
            // Serial pc2(USBTX, USBRX);
            // printf("%d \n", SystemCoreClock);
        }

#if DVFSMODE == 2
        // taskENTER_CRITICAL();
        cycleConservingDVSTaskComplete(1, xTaskGetTickCount());
        // taskEXIT_CRITICAL();
#endif
        if ((xTaskGetTickCount()) > xLastWakeTime + xDelay)
        {
            myled1 = 1;
            myled2 = 0;
            myled3 = 0;
            myled4 = 1;
            vTaskSuspendAll();
        }
        vTaskDelayUntil(&xLastWakeTime, xDelay);
    }
}

void vTaskFibonnaciFixedTime420ms(void *pvParameters)
{
    int *intPvParameters = (int *)pvParameters;
    int delayinMs = intPvParameters[0];
    const TickType_t xDelay = delayinMs;
    TickType_t xLastWakeTime;
    int executionCount = 0;
    int powerConsumptionTestIsWorstCase[7] = {0,1,0,1,0,1,1};
    int cycles = 1012000 * 2; // Takes around 210ms at 96Mhz
    xLastWakeTime = xTaskGetTickCount();
    long n; // Needed otherwise the compiler skips the fibonacci calculation
    for (;;)
    {

#if DVFSMODE == 2
        // taskENTER_CRITICAL();
        cycleConservingDVSTaskReady(2, xTaskGetTickCount(), xLastWakeTime + xDelay);
        // taskEXIT_CRITICAL();
#endif
        n = fibonnacciCalculation(cycles);
        if (!ConsumptionTest)
        {
            if (executionCount < EXECUTIONCOUNT)
            {
                if (myled2 == 0)
                    myled2 = 1;
                else
                    myled2 = 0;
                executionCount++;
            }
        }

        // pc.printf("[Fixed420] Delay : %d\n", xDelay);
#if DVFSMODE == 2
        // taskENTER_CRITICAL();
        cycleConservingDVSTaskComplete(2, xTaskGetTickCount());
        // taskEXIT_CRITICAL();
#endif
        if ((xTaskGetTickCount()) > xLastWakeTime + xDelay)
        {
            myled1 = 0;
            myled2 = 1;
            myled3 = 0;
            myled4 = 1;
            // Serial pc2(USBTX, USBRX);
            // pc2.printf("[Fixed420] Deadline missed : %d, Tick Count %d \n", xLastWakeTime + xDelay, xTaskGetTickCount());
            vTaskSuspendAll();
        }
        vTaskDelayUntil(&xLastWakeTime, xDelay);
    }
}
void vTaskFibonnaciDynamicTime(void *pvParameters)
{
    int *intPvParameters = (int *)pvParameters;
    int delayinMs = intPvParameters[0];
    const TickType_t xDelay = delayinMs;
    TickType_t xLastWakeTime;
    int executionCount = 0;
    int powerConsumptionTestIsWorstCase[7] = {0,0,1,0,1,1,1};
    int cycles = 1012000 * (rand() % 1 + 1); // Takes between 210ms and 840 ms at 96Mhz
    xLastWakeTime = xTaskGetTickCount();
    long n; // Needed otherwise the compiler skips the fibonacci calculation
    for (;;)
    {

#if DVFSMODE == 2
        // taskENTER_CRITICAL();
        cycleConservingDVSTaskReady(3, xTaskGetTickCount(), xLastWakeTime + xDelay);
        // taskEXIT_CRITICAL();
#endif
        cycles = 1012000 * (rand() % 1 + 1);
        n = fibonnacciCalculation(cycles);
        if (!ConsumptionTest)
        {

            if (executionCount < EXECUTIONCOUNT)
            {
                if (myled3 == 0)
                    myled3 = 1;
                else
                    myled3 = 0;
            }
            executionCount++;
        }
        // if(executionCount==5) cycles = 1012000 * 4;
        // n = fibonnacciCalculation(cycles);
        // if(executionCount==2) LPC_GPIO1->FIOPIN = (1 << 23);
        // pc.printf("[Fixed210] Delay : %d\n", xDelay);;
#if DVFSMODE == 2
        // taskENTER_CRITICAL();
        cycleConservingDVSTaskComplete(3, xTaskGetTickCount());
        // taskEXIT_CRITICAL();
#endif
        if ((xTaskGetTickCount()) > xLastWakeTime + xDelay)
        {
            myled1 = 0;
            myled2 = 0;
            myled3 = 1;
            myled4 = 1;

            vTaskSuspendAll();
        }
        vTaskDelayUntil(&xLastWakeTime, xDelay);
    }
}


{
    const TickType_t xDelay = mainTASK2_PERIOD / portTICK_PERIOD_MS;
    TickType_t xLastWakeTime;
    long begin, end;
    // int* taskDeadline = (int*) pvParameters[0];
    xLastWakeTime = xTaskGetTickCount();
    srand(time(NULL));
    long n;
    int cycles = 1012000; // Takes around 210ms at 96Mhz

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

        // now = getTime();

        pc.printf("Hello world %d\n", contador);
        // pc.printf("Time: %2d:%2d:%2d\n",now.hour,now.min,now.sec);
        // pc.printf("%d ms,%d ticks, %d last, %d deadline1, %d deadline2\n",cycles,xTaskGetTickCount(), xLastWakeTime, deadline1,deadline2);
        // pc.printf("%d ticks, %d idle time, %d deadline2\n",xTaskGetTickCount(),useridleTime,deadline2);

        // pc.printf("%s \n", stats);
        //         if(deadline2 >(xTaskGetTickCount()-xLastWakeTime)*2) {
        //             if(frequencyDivider<=minDivider) {
        //                 if(frequencyDivider==0) setSystemFrequency(3,0,36, 1);
        //                 else {
        //                     setSystemFrequency(3*frequencyDivider,0,36, 1);
        //                     Serial pc2(USBTX, USBRX); // Descobrir como arrumar o serial quando a frequencia muda
        //                     frequencyDivider++;
        //                 }
        //             }
        //         }

        // pc.printf("%d Hz, %d, divider %d, 1s = %d ticks\n", SystemCoreClock, LPC_SC->CCLKCFG,frequencyDivider, 1000 / portTICK_PERIOD_MS);

        // if (contador != 0 && contador%5==0 && currentFrequencyLevel<=6) {

        // }
        contador++;
        // myled4 = 0;
        // myled1 = 1;

#if POWERSAVINGMODE == 3
        cycleConservingDVSTaskComplete(1, xTaskGetTickCount());
#endif
        vTaskDelayUntil(&xLastWakeTime, xDelay);
        // LPC_GPIO1->FIOPIN ^= (1<<ld2);
    }
}*/