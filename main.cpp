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

#include"TaskConfiguration.h"

#include "cycleCounter.h"

#define DEVICE_SEMIHOST 0
//#define cycleCounter 1

DigitalOut myled(LED1);
DigitalOut myled1(LED3);
DigitalOut myled2(LED2);
DigitalOut myled4(LED4);

// #define USR_POWERDOWN    (0x104)
// int semihost_powerdown() {
//     uint32_t arg;
//     return __semihost(USR_POWERDOWN, &arg); 
// }

#define ld2         18 // P2_1

/* TIMER REGISTERS */
#define T0TCR      0x40004004
#define WRITE_T0TCR(val) ((*(volatile uint32_t *)T0TCR) = (val))
#define T0PR      0x4000400C
#define WRITE_T0PR(val) ((*(volatile uint32_t *)T0PR) = (val))







/*-----------------------------------------------------------*/
void vTask1(void * pvParameters);
void vTask2(void * pvParameters);
void vTask3(void * pvParameters);
void vTask4(void * pvParameters);
void vTask5(void * pvParameters);
Serial pc(USBTX, USBRX);
int led =0;
volatile long contador=0;
volatile long deadline1=0;
volatile long deadline2=0;
volatile long dealine3=0;

/* DVFS Variables*/
//uint8_t DVFS_ON=1;
volatile short frequencyDivider = 1;
extern int test;
extern TickType_t useridleTime;
volatile tm t3;
#define minDivider 1

//DEBUG VARIABLES
char ptrTaskList[500]={ 0 };
static void PrintTaskInfo();
//Changed min stack size to 300


/* bit position of CCR register */
#define SBIT_CLKEN     0    /* RTC Clock Enable*/
#define SBIT_CTCRST    1    /* RTC Clock Reset */
#define SBIT_CCALEN    4    /* RTC Calibration counter enable */


class Watchdog {
public:
// Load timeout value in watchdog timer and enable
    void kick(float s) {
        LPC_WDT->WDCLKSEL = 0x02;               // Set CLK src to RTC for DeepSleep wakeup
        LPC_WDT->WDTC = (s/4.0)*32768;
        LPC_WDT->WDMOD = 0x3;                   // Enabled and Reset
        kick();
    }
// "kick" or "feed" the dog - reset the watchdog timer
// by writing this required bit pattern
    void kick() {
        LPC_WDT->WDFEED = 0xAA;
        LPC_WDT->WDFEED = 0x55;
    }
};
 
// Setup the watchdog timer
Watchdog wdt;
 
// Blink LED
void blink() {
    myled2 = 0;
    myled1 = 1;
}

int printNextDeadline(int **deadlinesArray, int numberOfTasks){
    	int nextDeadline = *deadlinesArray[0]; // The element in the arrayis a pointer to the deadline of the First task
	for (int i = 0; i < numberOfTasks; i++)
	{
		if (nextDeadline > *deadlinesArray[i])
			nextDeadline = *deadlinesArray[i];
        
	}
    return nextDeadline;
}

int taskNumbers[5] = {0,1,2,3,4};

#define VANILLA 0
#ifdef VANILLA
int main(){

    time_t t;
    srand((unsigned)time(&t));
    struct RTC_DATA now = defaultTime();
    initRTC(now);
    frequencyLevelSelector(0);
    KIN1_InitCycleCounter(); /* enable DWT hardware */
    //Peripheral_PowerDown(0xFFFF7FFF);
    PHY_PowerDown();
    //PHY_PowerDown();
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

   // xTaskCreate(vTask2, "Task2", mainDEFAULT_STACK_SIZE, &deadlineTask2, mainTASK2_PRIORITY, NULL);

    setupCycleConservingDVS(*deadlines, mainWorstCaseComputeTime);
    Serial pc(USBTX, USBRX); // Descobrir como arrumar o serial quando a frequencia muda
    xTaskCreate(vTask3, "Task1", mainDEFAULT_STACK_SIZE, &taskNumbers[0], mainTASK1_PRIORITY, NULL);
    xTaskCreate(vTask3, "Task2", mainDEFAULT_STACK_SIZE, &taskNumbers[1], mainTASK2_PRIORITY, NULL);
    xTaskCreate(vTask3, "Task3", mainDEFAULT_STACK_SIZE, &taskNumbers[2], mainTASK3_PRIORITY, NULL);

    pc.printf("AAA %d Hz, %d\n", SystemCoreClock, currentFrequencyLevel);


    // *deadlines[0]= 2000;
    // printNextDeadline(deadlines, 3,pc);

    /* Start the tasks and timer running. */
    vTaskStartScheduler();

    /* If all is well, the scheduler will now be running, and the following
    line will never be reached.  If the following line does execute, then
    there was insufficient FreeRTOS heap memory available for the idle and/or
    timer tasks to be created.  See the memory management section on the
    FreeRTOS web site for more details. */
}

#else
int main()
{

    int result;

 
    // On reset, determine if a watchdog, power on reset or a pushbutton caused reset and display on LED 4 or 3
    // Check reset source register
    result = LPC_SC->RSID;
    if ((result & 0x03 )==0) {
    // was not POR or reset pushbutton so
    // Mbed is out of debug mode and reset so can enter DeepSleep now and wakeup using watchdog

        myled4 = 1;
        // wait(.00625);
        // wdt.kick(2);
        // blink();
        // DeepSleep();
        //         myled4 = 1;
        //  int periods[3] = {8,10,14};
        //  int compute[3] = {1,3,1};

        //  int test = staticVoltageScalingFrequencyLevelSelector(3,periods,compute,1);
        // pc.printf(" Frequency chosen by SVS RM level %d : %d MHz \n", test, frequencyLevels[test]);


    // pc.printf("M level %d : %d MHz \n", *deadlines[0],  *deadlines[1]);
 



    time_t t;
    srand((unsigned)time(&t));
    struct RTC_DATA now = defaultTime();
    initRTC(now);
    frequencyLevelSelector(5);
    KIN1_InitCycleCounter(); /* enable DWT hardware */

    //PHY_PowerDown();
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

   // xTaskCreate(vTask2, "Task2", mainDEFAULT_STACK_SIZE, &deadlineTask2, mainTASK2_PRIORITY, NULL);

    xTaskCreate(vTask3, "Task3", mainDEFAULT_STACK_SIZE, &deadlineTask2, mainTASK2_PRIORITY, NULL);



    /* Start the tasks and timer running. */
    vTaskStartScheduler();

    /* If all is well, the scheduler will now be running, and the following
    line will never be reached.  If the following line does execute, then
    there was insufficient FreeRTOS heap memory available for the idle and/or
    timer tasks to be created.  See the memory management section on the
    FreeRTOS web site for more details. */
    for (;; );
    } else {
    // Was an initial manual or Power on Reset
    // This codes only executes the first time after initial POR or button reset
        LPC_SC->RSID = 0x0F;
    // Clear reset source register bits for next reset
            //myled3 = 1;
    // LED3 on to indicate initial reset at full power level
    // Normal mbed power level for this setup is around 690mW
    // assuming 5V used on Vin pin
    // If you don't need networking...
    // Power down Ethernet interface - saves around 175mW
    // Also need to unplug network cable - just a cable sucks power
        PHY_PowerDown();
    // If you don't need the PC host USB interface....
    // Power down magic USB interface chip - saves around 150mW
    // Needs new firmware (URL below) and USB cable not connected
    // Power coming from Vin pin
    // http://mbed.org/handbook/Beta
    // Must supply power to mbed using Vin pin for powerdown
    // Also exits debug mode - must not be in debug mode
    // for deep power down modes
        myled2=1;
        result = mbed_interface_powerdown();
    // Power consumption is now around half
    // Turn off clock enables on unused I/O Peripherals (UARTs, Timers, PWM, SPI, CAN, I2C, A/D...)
    // To save just a tiny bit more power - most are already off by default in this short code example
    // See PowerControl.h for I/O device bit assignments
    // Don't turn off GPIO - it is needed to blink the LEDs

        Peripheral_PowerDown(0xFFFF7FFF);
    // Now can do a reset to free mbed of debug mode
    // NXP manual says must exit debug mode and reset for DeepSleep or lower power levels to wakeup
        wait(1);
        
        NVIC_SystemReset();
    }



}
#endif

void vTask1(void * pvParameters)
{
    const TickType_t xDelay = mainTASK1_PERIOD / portTICK_PERIOD_MS;
    TickType_t xLastWakeTime;
    long begin, end;
    struct RTC_DATA now;
    xLastWakeTime = xTaskGetTickCount();
    srand(time(NULL));
    long n;
    int cycles = 1012000/2; //Takes around 105ms at 96Mhz

    for (;; ) {
        deadline2= xLastWakeTime + xDelay;
        #ifdef DEBUG
        myled4=1;
        #endif

        #ifdef cycleCounter
        KIN1_ResetCycleCounter(); /* reset cycle counter */
        KIN1_EnableCycleCounter(); /* start counting */
        n= fibonnacciCalculation(cycles);
        #ifdef DEBUG
        end = KIN1_GetCycleCounter(); /* get cycle counter */
        double timeInMs= end/(SystemCoreClock/1000.0);
        pc.printf("Took %ld cycles at %ld to calculate %ld, resulting in a compute time of "
            "%lf ms \n", end, SystemCoreClock, n, timeInMs);
        #endif

        #else
        begin = xTaskGetTickCount();
        n= fibonnacciCalculation(cycles);
        end = xTaskGetTickCount();
        #ifdef DEBUG
        double timeInMs= (end-begin)*portTICK_PERIOD_MS;
        pc.printf("Took %ld ticks at %ld to calculate %ld, resulting in a compute time of "
            "%lf, %ld prescalar, %ld Tick Count ms\n", end-begin, SystemCoreClock, n, timeInMs, LPC_TIM1->PR , end );
        #endif
        #endif

        vTaskDelayUntil(&xLastWakeTime, xDelay);
        //LPC_GPIO1->FIOPIN ^= (1<<ld2);
    }
}

void vTask2(void * pvParameters)
{
    const TickType_t xDelay = mainTASK2_PERIOD / portTICK_PERIOD_MS;
    TickType_t xLastWakeTime;
    long begin, end;
    //int* taskDeadline = (int*) pvParameters[0];
    xLastWakeTime = xTaskGetTickCount();
    srand(time(NULL));
    long n;
    int cycles = 1012000; //Takes around 210ms at 96Mhz

    for (;; ) {
        //*taskDeadline= xLastWakeTime + xDelay;
        
        #ifdef DEBUG
        myled4=1;
        #endif
        

        #ifdef cycleCounter
        KIN1_ResetCycleCounter(); /* reset cycle counter */
        KIN1_EnableCycleCounter(); /* start counting */
        n= fibonnacciCalculation(cycles);
        #ifdef DEBUG
        end = KIN1_GetCycleCounter(); /* get cycle counter */
        double timeInMs= end/(SystemCoreClock/1000.0);
        pc.printf("Took %ld cycles at %ld to calculate %ld, resulting in a compute time of "
            "%lf ms \n", end, SystemCoreClock, n, timeInMs);
        #endif

        #else
        begin = xTaskGetTickCount();
        n= fibonnacciCalculation(cycles);
        end = xTaskGetTickCount();
        #ifdef DEBUG
        double timeInMs= (end-begin)*portTICK_PERIOD_MS;
        pc.printf("Took %ld ticks at %ld to calculate %ld, resulting in a compute time of "
            "%lf, %ld prescalar, %ld Tick Count ms\n", end-begin, SystemCoreClock, n, timeInMs, LPC_TIM1->PR , end );
        #endif
        #endif

        //pc.printf(" \n");
        //if(xTaskGetTickCount()>(xLastWakeTime+ xDelay)) myled2=1;
        //char stats[300]={0};
        //vTaskGetRunTimeStats( stats );
        //vTaskList(stats);
        //pc.printf("%s", stats);
        /* Read Time */
        //PrintTaskInfo();
        //now = getTime();
        
        //pc.printf("Hello world %d\n", contador);
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
        vTaskDelayUntil(&xLastWakeTime, xDelay);
        //LPC_GPIO1->FIOPIN ^= (1<<ld2);
    }
}


void vTask3(void * pvParameters)
{
    int *pvTaskNumberPTR;
    pvTaskNumberPTR = (int *) pvParameters;

    
    const TickType_t xDelay = *deadlines[*pvTaskNumberPTR] ;
    
    TickType_t xLastWakeTime;
    struct RTC_DATA now;
    //pc.printf("Starting task %d\n",*pvTaskNumberPTR);
    xLastWakeTime = xTaskGetTickCount();

    for (;; ) {
        
        *deadlines[*pvTaskNumberPTR]= (xLastWakeTime + xDelay);
        int aux = cycleConservingDVSTaskReady(*pvTaskNumberPTR, xTaskGetTickCount());
        //Serial pc(USBTX, USBRX);
        //Serial pc(USBTX, USBRX);

        // fibonnacciCalculation(1000);

        if (led==0) {
            myled=1;
            led=1;
        }
        else {
            myled1=0;
            led=0;
        }

        fibonnacciCalculation(10000);
        if (xTaskGetTickCount()>(xLastWakeTime+ xDelay)) {
            //myled2=1;
            contador++;

        }
        
        //pc.printf("BLAA %d\n", xTaskGetTickCount());
        //pc.printf("Time: %2d:%2d:%2d\n",now.hour,now.min,now.sec);
        aux = cycleConservingDVSTaskComplete(*pvTaskNumberPTR, xTaskGetTickCount());
        LPC_GPIO1->FIOPIN = (1 << 20);
        // myled=0;
        // pc.printf("Task %d, New deadline %d, level %d\n", *pvTaskNumberPTR,*deadlines[*pvTaskNumberPTR], xTaskGetTickCount());
        // fibonnacciCalculation(100000);
        // while(1);
        // pc.printf("CCC %d\n",*pvTaskNumberPTR);
        vTaskDelayUntil(&xLastWakeTime, xDelay);
        //myled =0;
        //vTaskDelay( xDelay );

    }
}

#ifdef __cplusplus
extern "C"
#endif
void vApplicationIdleHook(void)
{
    if (led==0) {
        myled =0;
        myled1=1;
        myled4=0;
    }
    else {
        myled =1;
        myled1=0;
        myled4=0;
        
    }


    Sleep();
    //pc.printf("%d ticks, %d deadline1, %d deadline2\n",xTaskGetTickCount(),deadline1,deadline2);
    //wdt.kick(2);
    //DeepSleep();
    //PowerDown();
    //DeepPowerDown();
}

#ifdef __cplusplus
extern "C"
#endif
void vApplicationMallocFailedHook(void)
{
    while (1) {
        pc.printf("Malloc error !! Hello, world!\n");
    }
}

#ifdef __cplusplus
extern "C"
#endif
void vApplicationStackOverflowHook(TaskHandle_t xTask,
    signed char *pcTaskName) {
    while (1) {
        pc.printf("Stack Overflow error !! Hello, world!\n");
    }
}



/* Defined in main.c. */
#ifdef __cplusplus
extern "C"
#endif
void vConfigureTimerForRunTimeStats(void)
{
    const unsigned long TCR_COUNT_RESET = 2, CTCR_CTM_TIMER = 0x00, TCR_COUNT_ENABLE = 0x01;

    /* This function configures a timer that is used as the time base when
    collecting run time statistical information - basically the percentage
    of CPU time that each task is utilising.  It is called automatically when
    the scheduler is started (assuming configGENERATE_RUN_TIME_STATS is set
    to 1). */

    /* Power up and feed the timer. */
    LPC_SC->PCONP |= 0x02UL;
    LPC_SC->PCLKSEL0 = (LPC_SC->PCLKSEL0 & (~(0x3<<2))) | (0x01 << 2);

    /* Reset Timer 0 */
    LPC_TIM0->TCR = TCR_COUNT_RESET;

    /* Just count up. */
    LPC_TIM0->CTCR = CTCR_CTM_TIMER;

    /* Prescale to a frequency that is good enough to get a decent resolution,
    but not too fast so as to overflow all the time. */
    LPC_TIM0->PR =  (configCPU_CLOCK_HZ / 10000UL) - 1UL;

    /* Start the counter. */
    LPC_TIM0->TCR = TCR_COUNT_ENABLE;

}

static void PrintTaskInfo()
{

    pc.printf("**********************************\n");
    pc.printf("Task  State   Prio    Stack    Num\n");
    pc.printf("**********************************\n");
    vTaskList(ptrTaskList);
    pc.printf(ptrTaskList);
    pc.printf("**********************************\n");
}

/* 
* Pre sleep processing for tickless
*/

