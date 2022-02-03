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

#include "testApplication.h"

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
// void vTask1(void *pvParameters);
// void vTask2(void *pvParameters);
// void vTaskFibonnaciFixedTime210ms(void *pvParameters);
// void vTaskFibonnaciFixedTime420ms(void *pvParameters);
// void vTaskFibonnaciDynamicTime(void *pvParameters);
void printOutNumberAsLeds(int n);
void vDummyTask(void *pvParameters);
Serial pc(USBTX, USBRX);

// volatile long contador = 0;
// volatile long deadline1 = 0;
// volatile long deadline2 = 0;
// volatile long dealine3 = 0;

// extern struct taskProperties task1Properties;
// extern struct taskProperties task2Properties;
// extern struct taskProperties task3Properties;

// struct taskProperties
// {
//     int taskNumber;
//     int xDelay;
//     int xFibonnaciCycles;
//     int xFibonnaciCyclesWorstCase;
//     int *xPowerConsumptionTestIsWorstCase;
// };

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
#define lowPowerMode 1
DigitalIn vUSBIN(p10); // USED TO NOT TURN OFF THE MAGIC INTERFACE WHEN CONNECTED TO PC - It is connected to VUSB

/*
 * When magicINTERFACEDISABLE is set to one, run the alternative main code that will
 * disable the Magic Intraface from the Mbed LPC1768 before preparing the application
 * and the FreeRTOS code to run
 */
#if magicINTERFACEDISABLE == 1
int main()
{

    /* Check if the USB cable is connected */
    int usbCONNECTED = vUSBIN;
    if (usbCONNECTED == 0)
    {

        /*
         * On reset, determine through the reset source register if a watchdog,
         * power on reset or a pushbutton caused reset
         */
        int result = LPC_SC->RSID;
        if ((result & 0x03) == 0)
        {

            /*
             * The reset was not a Push button nor a Power on Reset, meaning the Magic Interface
             * is disabled and the Mbed is out of DEBUG mode
             */
            KIN1_InitCycleCounter(); /* enable DWT hardware for cycle counting */
            PHY_PowerDown();

            /*
             * Setup the task properties structure for each task
             */
            setupTaskParameters();
            xTaskCreate(vDummyTask, "Dummy Task", configMINIMAL_STACK_SIZE * 4, &task1Properties, task1Properties.taskPriority, NULL);
            xTaskCreate(vDummyTask, "Dummy Task", configMINIMAL_STACK_SIZE * 4, &task2Properties, task2Properties.taskPriority, NULL);
            xTaskCreate(vDummyTask, "Dummy Task", configMINIMAL_STACK_SIZE * 4, &task3Properties, task3Properties.taskPriority, NULL);

#ifdef lowPowerMode
            /* Start the scheduller in Low Power Mode */
            int main_taskWorstCaseComputeTime[3] = {21, 42, 84};
            int main_taskDeadlines[3] = {fixedFibonnaciTaskPeriod210ms, fixedFibonnaciTaskPeriod420ms, dynamicFibonnaciTaskPeriod};
            int main_frequencyLevels[6] = {96, 80, 72, 48, 24, 16};
            vTaskStartLowPowerScheduller(3, main_taskWorstCaseComputeTime, main_taskDeadlines, 6, main_frequencyLevels, lowPowerMode);
#else
            /* Start the normal scheduller. */
            vTaskStartScheduler();
#endif
        }
        else
        {
            /*
             * The reset was a manual buttonReset or a Power on Reset, so we clear the reset register
             * disable the Magic Interface and do a software Reset to bring the Mbed out of the DEBUG
             * mode.
             */
            LPC_SC->RSID = 0x0F;
            result = mbed_interface_powerdown();
            wait(1);
            NVIC_SystemReset();
        }
    }
    else
    {
        KIN1_InitCycleCounter(); /* enable DWT hardware for cycle counting */
        PHY_PowerDown();

        /*
         * Setup the task properties structure for each task
         */
        setupTaskParameters();
        xTaskCreate(vDummyTask, "Dummy Task", configMINIMAL_STACK_SIZE * 4, &task1Properties, task1Properties.taskPriority, NULL);
        xTaskCreate(vDummyTask, "Dummy Task", configMINIMAL_STACK_SIZE * 4, &task2Properties, task2Properties.taskPriority, NULL);
        xTaskCreate(vDummyTask, "Dummy Task", configMINIMAL_STACK_SIZE * 4, &task3Properties, task3Properties.taskPriority, NULL);

#ifdef lowPowerMode
        /* Start the scheduller in Low Power Mode */
        int main_taskWorstCaseComputeTime[3] = {3000, 3000, 1000};
        int main_taskDeadlines[3] = {task1Properties.xDelay, task2Properties.xDelay, task3Properties.xDelay};
        int main_frequencyLevels[6] = {96, 80, 72, 48, 24, 16};
        vTaskStartLowPowerScheduller(3, main_taskWorstCaseComputeTime, main_taskDeadlines, 6, main_frequencyLevels, lowPowerMode);
#else
        /* Start the normal scheduller. */
        vTaskStartScheduler();
#endif
        for (;;)
            ;
    }
}
#else
int main()
{
    KIN1_InitCycleCounter(); /* enable DWT hardware for cycle counting */
    PHY_PowerDown();

    /*
     * Setup the task properties structure for each task
     */
    setupTaskParameters();
    xTaskCreate(vDummyTask, "Dummy Task 1", configMINIMAL_STACK_SIZE * 4, &task1Properties, task1Properties.taskPriority, NULL);
    xTaskCreate(vDummyTask, "Dummy Task 2", configMINIMAL_STACK_SIZE * 4, &task2Properties, task2Properties.taskPriority, NULL);
    xTaskCreate(vDummyTask, "Dummy Task 3", configMINIMAL_STACK_SIZE * 4, &task3Properties, task3Properties.taskPriority, NULL);

#ifdef lowPowerMode
    /* Start the scheduller in Low Power Mode */
    int main_taskWorstCaseComputeTime[3] = {3000, 3000, 1000};
    int main_taskDeadlines[3] = {task1Properties.xDelay, task2Properties.xDelay, task3Properties.xDelay};
    int main_frequencyLevels[7] = {96,88,80, 72, 48, 24, 16};
    vTaskStartLowPowerScheduller(3, main_taskWorstCaseComputeTime, main_taskDeadlines, 7, main_frequencyLevels, lowPowerMode);
#else
    /* Start the normal scheduller. */
    vTaskStartScheduler();
#endif
    for (;;)
        ;
}
#endif

void vDummyTask(void *pvParameters)
{
    /* Unpack parameters into local variables for ease of interpretation */
    struct taskProperties *parameters = (struct taskProperties *)pvParameters;
    const TickType_t xDelay = parameters->xDelay / portTICK_PERIOD_MS;
    int baseCycles = parameters->xFibonnaciCycles;               // Takes around 210ms at 96Mhz
    int worstCaseCycles = parameters->xFibonnaciCyclesWorstCase; // Takes around 210ms at 96Mhz
    int *isWorstCase = parameters->xPowerConsumptionTestIsWorstCase;
    int taskNumber = parameters->taskNumber;

    /* Define local variables for counting runs and delays */
    TickType_t xLastWakeTime = 0;
    int fibonnacciAuxiliar = 0;
    int runNumber = 0;
    long begin, end;
    double timeInMs;

    for (;;)
    {

        // Cycle conserving task ready to run
        if (dvfsMode == 2)
            cycleConservingDVSTaskReady(taskNumber, xTaskGetTickCount(), xLastWakeTime + xDelay);

        // Fazer um teste com 1 ciclo fibonnaci só para depois extrapolar, o fato de serem 40 por vez esta a dar problema
        pc.printf("[Task %d] Starting calculation Tick Count %d \n", taskNumber, xTaskGetTickCount());
        // long a = 1, b = 1, i = fibonnaciCycles1MS_96Mhz;
        // KIN1_ResetCycleCounter();  /* reset cycle counter */
        // KIN1_EnableCycleCounter(); /* start counting */
        fibonnacciAuxiliar = fibonnacciCalculation(isWorstCase[runNumber] == 0 ? baseCycles : worstCaseCycles);

        // // fibonnacciAuxiliar = fibonnacciCalculation(i);

        // end = KIN1_GetCycleCounter(); /* get cycle counter */
        // timeInMs = end / (SystemCoreClock / 1000.0);
        // pc.printf("[Task %d] Took %ld cycles at %ld to calculate fibonnaci %d times and %ld, resulting in a compute time of "
        //           "%f ms \n",
        //           taskNumber,
        //           end, SystemCoreClock, isWorstCase[runNumber] == 0 ? baseCycles : worstCaseCycles, a, timeInMs);
        // pc.printf("[Task %d] Compute time: %f ms, run %d, isWorstCase: %d, Tick Count %d \n"
        //   , taskNumber,
        //   timeInMs, runNumber, isWorstCase[runNumber], xTaskGetTickCount());

        // Actual task code
        //    fibonnacciAuxiliar = fibonnacciCalculation(isWorstCase[runNumber] == 0 ? baseCycles : worstCaseCycles);
        //     fibonnacciAuxiliar -=1;
        runNumber++;
        if (runNumber > 7)
            runNumber = 0;
        switch (taskNumber)
        {
        case 1:
            myled1 = !(myled1);
            break;
        case 2:
            myled2 = !(myled2);
            break;
        case 3:
            myled3 = !(myled3);
            break;

        default:
            break;
        }
        // Cycle conserving task ready finished running
        if (dvfsMode == 2)
            cycleConservingDVSTaskComplete(taskNumber, xTaskGetTickCount());

        // If the deadline is missed suspend all and print out a code
        if ((xTaskGetTickCount()) > xLastWakeTime + xDelay)
        {
            printOutNumberAsLeds(taskNumber);
            vTaskSuspendAll();
        }
        // Explicar esse delay no modelo
        vTaskDelayUntil(&xLastWakeTime, xDelay);
    }
}

void printOutNumberAsLeds(int n)
{
    myled1 = n & 1;
    myled2 = n >> 1 & 1;
    myled3 = n >> 2 & 1;
    myled4 = n >> 3 & 1;
}

void lightUpTaskDebugLed(int taskNumber)
{
    switch (taskNumber)
    {
    case 1:
        myled1 = !(myled1);
        break;
    case 2:
        myled2 = !(myled2);
        break;
    case 3:
        myled3 = !(myled3);
        break;

    default:
        break;
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
// int main()
// {
// #define DVFSMODE 2
//     int main_taskWorstCaseComputeTime[3] = {21, 42, 84};
//     int main_taskDeadlines[3] = {fixedFibonnaciTaskPeriod210ms, fixedFibonnaciTaskPeriod420ms, dynamicFibonnaciTaskPeriod};
//     int main_frequencyLevels[6] = {96, 80, 72, 48, 24, 16};

// #if magicINTERFACEDISABLE == 1
//     int usbCONNECTED = vUSBIN;
//     if (usbCONNECTED == 0)
//     {
//         int result;
//         // On reset, determine if a watchdog, power on reset or a pushbutton caused reset and display on LED 4 or 3
//         // Check reset source register
//         result = LPC_SC->RSID;
//         if ((result & 0x03) == 0)
//         {
//             // was not POR or reset pushbutton so
//             // Mbed is out of debug mode and reset so can enter DeepSleep now and wakeup using watchdog

//             KIN1_InitCycleCounter(); /* enable DWT hardware */
//             PHY_PowerDown();

//             // xTaskCreate(vTaskFibonnaciFixedTime210ms, "Fixed Fibonnaci Task 210 ms", configMINIMAL_STACK_SIZE, &fixedFibonnaciTaskPeriod210ms, 4, NULL);
//             // xTaskCreate(vTaskFibonnaciFixedTime420ms, "Fixed Fibonnaci Task 420 ms", configMINIMAL_STACK_SIZE, &fixedFibonnaciTaskPeriod420ms, 3, NULL);
//             // xTaskCreate(vTaskFibonnaciDynamicTime, "Dynamic Fibonnaci Task 210-840 ms", configMINIMAL_STACK_SIZE, &dynamicFibonnaciTaskPeriod, 2, NULL);
//             ConsumptionTest = true;
//             // #define POWERSAVINGMODE  3

//             vTaskStartLowPowerScheduller(3, main_taskWorstCaseComputeTime, main_taskDeadlines, 6, main_frequencyLevels, DVFSMODE);
//             vTaskStartScheduler();

//             for (;;)
//                 ;
//         }
//         else
//         {
//             // Was an initial manual or Power on Reset
//             // Clear reset source register bits for next reset
//             // This codes only executes the first time after initial POR or button reset
//             // Now can do a reset to free mbed of debug mode
//             LPC_SC->RSID = 0x0F;
//             result = mbed_interface_powerdown();
//             wait(1);
//             NVIC_SystemReset();
//         }
//     }
// #endif

//     KIN1_InitCycleCounter(); /* enable DWT hardware  IS THIS NECESSARY???*/
//     PHY_PowerDown();

//     struct taskProperties task1Properties;
//     int task1WorstCase[8] = {0, 1, 0, 0, 1, 1, 0, 1};
//     task1Properties.taskNumber = 1;
//     task1Properties.xDelay = 800;
//     task1Properties.xFibonnaciCycles = 2;
//     task1Properties.xFibonnaciCyclesWorstCase = 4;
//     task1Properties.xPowerConsumptionTestIsWorstCase = task1WorstCase;
//     xTaskCreate(vDummyTask, "Dummy Task", configMINIMAL_STACK_SIZE * 4, &task1Properties, 2, NULL);

//     struct taskProperties task2Properties;
//     int task2WorstCase[8] = {0, 0, 1, 0, 1, 0, 1, 1};
//     task2Properties.taskNumber = 2;
//     task2Properties.xDelay = 1000;
//     task2Properties.xFibonnaciCycles = 2;
//     task2Properties.xFibonnaciCyclesWorstCase = 4;
//     task2Properties.xPowerConsumptionTestIsWorstCase = task2WorstCase;
//     xTaskCreate(vDummyTask, "Dummy Task", configMINIMAL_STACK_SIZE * 4, &task2Properties, 2, NULL);

//     struct taskProperties task3Properties;
//     int task3WorstCase[8] = {0, 0, 0, 1, 0, 1, 1, 1};
//     task3Properties.taskNumber = 3;
//     task3Properties.xDelay = 1400;
//     task3Properties.xFibonnaciCycles = 2;
//     task3Properties.xFibonnaciCyclesWorstCase = 4;
//     task3Properties.xPowerConsumptionTestIsWorstCase = task3WorstCase;
//     xTaskCreate(vDummyTask, "Dummy Task", configMINIMAL_STACK_SIZE * 4, &task3Properties, 2, NULL);

//     // vTaskStartLowPowerScheduller(3, main_taskWorstCaseComputeTime, main_taskDeadlines, 6, main_frequencyLevels, DVFSMODE);
//     // wait(3);
//     vTaskStartScheduler();
//     /* Start the tasks and timer running. */
//     // vTaskStartScheduler();

//     for (;;)
//         ;
// }

// void vDummyTask(void *pvParameters)
// {
//     /* Unpack parameters into local variables for ease of interpretation */
//     struct taskProperties *parameters = (struct taskProperties *)pvParameters;
//     const TickType_t xDelay = parameters->xDelay;
//     int baseCycles = parameters->xFibonnaciCycles;               // Takes around 210ms at 96Mhz
//     int worstCaseCycles = parameters->xFibonnaciCyclesWorstCase; // Takes around 210ms at 96Mhz
//     int *isWorstCase = parameters->xPowerConsumptionTestIsWorstCase;
//     int taskNumber = parameters->taskNumber;

//     /* Define local variables for counting runs and delays */
//     TickType_t xLastWakeTime = xTaskGetTickCount();
//     long fibonnacciAuxiliar = 0;
//     int runNumber = 0;
//     int n = 0;

//     for (;;)
//     {

//         // Cycle conserving task ready to run
//         //  taskENTER_CRITICAL();
//         if (dvfsMode == 2)
//             cycleConservingDVSTaskReady(taskNumber, xTaskGetTickCount(), xLastWakeTime + xDelay);
//         // taskEXIT_CRITICAL();

//         // Actual task code
//         n = fibonnacciCalculation(isWorstCase[runNumber] == 0 ? baseCycles : worstCaseCycles);
//         runNumber++;
//         if (runNumber > 7)
//             runNumber = 0;

//         // taskENTER_CRITICAL();
//         // Cycle conserving task ready finished running
//         if (dvfsMode == 2)
//             cycleConservingDVSTaskComplete(taskNumber, xTaskGetTickCount());
//         // taskEXIT_CRITICAL();
//         // If the deadline is missed suspend all and print out a code
//         if ((xTaskGetTickCount()) > xLastWakeTime + xDelay)
//         {
//             printOutNumberAsLeds(taskNumber);
//             vTaskSuspendAll();
//         }
//         vTaskDelayUntil(&xLastWakeTime, xDelay);
//     }
// }

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

        taskDeadline= xLastWakeTime + xDelay;
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