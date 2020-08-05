#include "mbed.h"
/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "PowerControl.h"
#include "ClockControl.h"
#include "calculations.h"
#include "RTC.h"
#include "TARGET_LPC1768_N/LPC17xx.h"
#include "lpc1768_rtc.h"


DigitalOut myled(LED1);
DigitalOut myled1(LED3);
DigitalOut myled2(LED2);
DigitalOut myled4(LED4);


/* TIMER REGISTERS */
#define T0TCR      0x40004004
#define WRITE_T0TCR(val) ((*(volatile uint32_t *)T0TCR) = (val))
#define T0PR      0x4000400C
#define WRITE_T0PR(val) ((*(volatile uint32_t *)T0PR) = (val))



/* Priorities at which the tasks are created. */
#define mainQUEUE_RECEIVE_TASK_PRIORITY     ( tskIDLE_PRIORITY + 2 )
#define mainQUEUE_SEND_TASK_PRIORITY        ( tskIDLE_PRIORITY + 1 )

/* The rate at which data is sent to the queue.  The times are converted from
milliseconds to ticks using the pdMS_TO_TICKS() macro. */
#define mainTASK_SEND_FREQUENCY_MS          pdMS_TO_TICKS( 200UL )
#define mainTIMER_SEND_FREQUENCY_MS         pdMS_TO_TICKS( 2000UL )

/* The number of items the queue can hold at once. */
#define mainQUEUE_LENGTH                    ( 2 )

/* The values sent to the queue receive task from the queue send task and the
queue send software timer respectively. */
#define mainVALUE_SENT_FROM_TASK            ( 100UL )
#define mainVALUE_SENT_FROM_TIMER           ( 200UL )

/*-----------------------------------------------------------*/
void vTask1( void * pvParameters );
void vTask2( void * pvParameters );
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
char ptrTaskList[500]= {0} ;
static void PrintTaskInfo();
//Changed min stack size to 300

void alarmFunction( void )
{
    if(myled2==0)myled2=1;
    else myled2=0;
    //RTC::alarm(&alarmFunction, t2);
    //error("Not most useful alarm function");
}

/* bit position of CCR register */
#define SBIT_CLKEN     0    /* RTC Clock Enable*/
#define SBIT_CTCRST    1    /* RTC Clock Reset */
#define SBIT_CCALEN    4    /* RTC Calibration counter enable */

class Watchdog
{
public:
// Load timeout value in watchdog timer and enable
    void kick(float s)
    {
        LPC_WDT->WDCLKSEL = 0x02;               // Set CLK src to RTC for DeepSleep wakeup
        LPC_WDT->WDTC = (s/4.0)*32768;
        LPC_WDT->WDMOD = 0x3;                   // Enabled and Reset
        kick();
    }
// "kick" or "feed" the dog - reset the watchdog timer
// by writing this required bit pattern
    void kick()
    {
        LPC_WDT->WDFEED = 0xAA;
        LPC_WDT->WDFEED = 0x55;
    }
};

int main()
{


    //int result;
//    Watchdog wdt;
//    
//        set_time(1256729737);
//    tm t2 = RTC::getDefaultTM();
//    t2.tm_min = 0;
//    t2.tm_sec = 3;
//    
//
//// On reset, determine if a watchdog, power on reset or a pushbutton caused reset and display on LED 4 or 3
//// Check reset source register
//    result = LPC_SC->RSID;
//    if ((result & 0x03 )==0) {
//// was not POR or reset pushbutton so
//// Mbed is out of debug mode and reset so can enter DeepSleep now and wakeup using watchdog
//        myled1 = 0;
//        myled4 = 1;
//        wait(.00625);
//        wdt.kick(2);
//        //RTC::alarm(&alarmFunction, t2);
//// Power level in DeepSleep around 135mW (27MA at 5VDC)
//// LED1 is several MAs of this
//        DeepSleep();
//// Watch dog timer wakes it up with a reset
////
//    myled4=0;
//    myled1=1;
//    wait(1);
//    } else {
//// Was an initial manual or Power on Reset
//// This codes only executes the first time after initial POR or button reset
//        LPC_SC->RSID = 0x0F;
//// Clear reset source register bits for next reset
//        myled2 = 1;
//        result = mbed_interface_powerdown();
//// Now can do a reset to free mbed of debug mode
//// NXP manual says must exit debug mode and reset for DeepSleep or lower power levels to wakeup
//        wait(1);
//        NVIC_SystemReset();
//    }
    time_t t;
    srand((unsigned) time(&t));
    struct RTC_DATA now = defaultTime();
    initRTC(now);

    //RTC::alarm(&alarmFunction, t2);
    //setSystemFrequency(3,0,36, 1);
    ///Serial pc(USBTX, USBRX);
    //pc.printf("bbbb %d\n", tee-1);
    //pc.printf("AAA %d Hz, %d\n", SystemCoreClock, LPC_SC->CCLKCFG);
    //Default divider is 3, and clock source is the 4Mhz IRC clock, m 36, n 1
    /* Start the two tasks as described in the comments at the top of this
    file. */
   xTaskCreate( vTask1,           /* The function that implements the task. */
                "Rx",                           /* The text name assigned to the task - for debug only as it is not used by the kernel. */
                configMINIMAL_STACK_SIZE,       /* The size of the stack to allocate to the task. */
                NULL,                           /* The parameter passed to the task - not used in this simple case. */
                mainQUEUE_SEND_TASK_PRIORITY,/* The priority assigned to the task. */
                NULL );                         /* The task handle is not required, so NULL is passed. */

    // xTaskCreate( vTask2, "TX", 2048, NULL, mainQUEUE_RECEIVE_TASK_PRIORITY, NULL );



    /* Start the tasks and timer running. */
    vTaskStartScheduler();

    /* If all is well, the scheduler will now be running, and the following
    line will never be reached.  If the following line does execute, then
    there was insufficient FreeRTOS heap memory available for the idle and/or
    timer tasks to be created.  See the memory management section on the
    FreeRTOS web site for more details. */
    for( ;; );
}

volatile unsigned long x = 0, y = 0;
void vTask1( void * pvParameters )
{
    const TickType_t xDelay = 5000 / portTICK_PERIOD_MS;
    TickType_t xLastWakeTime;

    xLastWakeTime = xTaskGetTickCount();
    for( ;; ) {
        deadline1= xLastWakeTime + xDelay;
        if(led==0) {
            myled=0;
            led=1;
        } else {
            myled1=0;
            led=0;
        }

        if(xTaskGetTickCount()>(xLastWakeTime+ xDelay)) {
            //myled2=1;
            contador++;

        }
        vTaskDelayUntil( &xLastWakeTime, xDelay );
        //myled =0;
        //vTaskDelay( xDelay );

    }
}
void vTask2( void * pvParameters )
{
    const TickType_t xDelay = 5000 / portTICK_PERIOD_MS;
    TickType_t xLastWakeTime;
    TickType_t begin;
    struct RTC_DATA now;
    xLastWakeTime = xTaskGetTickCount();
    srand(time(NULL));

    for( ;; ) {
        deadline2= xLastWakeTime + xDelay;
        myled4=1;
        begin= xTaskGetTickCount();
        //long cycles =rand()%100000+10000;
        long cycles = 50000;
        fibonnacciCalculation(cycles);
        //if(xTaskGetTickCount()>(xLastWakeTime+ xDelay)) myled2=1;
        //char stats[300]={0};
        //vTaskGetRunTimeStats( stats );
        //vTaskList(stats);
        //pc.printf("%s", stats);
        /* Read Time */
        //PrintTaskInfo();
        now = getTime();
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
        if(contador%5==0){
            if(frequencyDivider<3){
                setSystemFrequency(2+frequencyDivider,0,36, 1);       
                    Serial pc2(USBTX, USBRX); // Descobrir como arrumar o serial quando a frequencia muda
                   
                    frequencyDivider++;
                }
                else{
                    frequencyDivider=1;
                    setSystemFrequency(2+frequencyDivider,0,36, 1);       
                    Serial pc2(USBTX, USBRX); 
                }
            }    
          pc.printf("%d Hz, %d, divider %d\n", SystemCoreClock, LPC_SC->CCLKCFG,frequencyDivider);       
        contador++;
        vTaskDelayUntil( &xLastWakeTime, xDelay );

    }
}

#ifdef __cplusplus
extern "C"
#endif
void vApplicationIdleHook( void )
{
    if(led==0)myled =1;
    else {
        myled1=1;
        myled4=0;
    }
    
    
    Sleep();
    //pc.printf("%d ticks, %d deadline1, %d deadline2\n",xTaskGetTickCount(),deadline1,deadline2);

    //DeepSleep();
    //PowerDown();
    //DeepPowerDown();
}


void vApplicationMallocFailedHook( void )
{
    while (1) {
        pc.printf("Hello, world!\n");
    }
}



/* Defined in main.c. */
#ifdef __cplusplus
extern "C"
#endif
void vConfigureTimerForRunTimeStats( void )
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
    LPC_TIM0->PR =  ( configCPU_CLOCK_HZ / 10000UL ) - 1UL;

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