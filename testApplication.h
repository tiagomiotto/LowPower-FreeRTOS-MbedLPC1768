
#define fibonnaciCycles1MS_96Mhz 11998
//Fibanacci calculation = 52 + Runs*15 processor cycles at 96MHz
//6397 goes over by around 0.00729167 %, 6396 goes under by 0.008333333 %
//For 1ms x= (96000000-52)/(15*1000) = 6396.533333333333333333333333333333
//Took 96009 cycles at 96000000 to calculate fibonnaci 5253 times and 1, resulting in a compute time of 1.000094 ms 
// (96000000-44)/(8*1000) 11999.9945
//[Task 1] Took 96043 cycles at 96000000 to calculate fibonnaci 12000 times and 1, resulting in a compute time of 1.000448 ms 
/*
[Task 1] Starting calculation Tick Count 0 
[Task 1] Took 144032018 cycles at 96000000 to calculate fibonnaci 18000000 times and 1, resulting in a compute time of 1500.333521 ms 
[Task 2] Starting calculation Tick Count 164 
[Task 2] Took 144032237 cycles at 96000000 to calculate fibonnaci 18000000 times and 1, resulting in a compute time of 1500.335802 ms 
[Task 3] Starting calculation Tick Count 331 
[Task 3] Took 48010633 cycles at 96000000 to calculate fibonnaci 6000000 times and 1, resulting in a compute time of 500.110760 ms 
[Task 1] Starting calculation Tick Count 800 
[Task 1] Took 288064349 cycles at 96000000 to calculate fibonnaci 36000000 times and 1, resulting in a compute time of 3000.670302 ms 
[Task 2] Starting calculation Tick Count 1113 
[Task 2] Took 144032018 cycles at 96000000 to calculate fibonnaci 18000000 times and 1, resulting in a compute time of 1500.333521 ms 
[Task 3] Starting calculation Tick Count 1400 
[Task 3] Took 48010636 cycles at 96000000 to calculate fibonnaci 6000000 times and 1, resulting in a compute time of 500.110792 ms 


*/

#ifdef __cplusplus
extern "C"
#endif
void setupTaskParameters();


// void vDummyTask(void *pvParameters);



struct taskProperties
{
    int taskNumber;
    int taskPriority;
    int xDelay;
    long xFibonnaciCycles;
    long xFibonnaciCyclesWorstCase;
    int *xPowerConsumptionTestIsWorstCase;
};
#ifdef __cplusplus
extern "C" 
#endif
struct taskProperties task1Properties;
#ifdef __cplusplus
extern "C"
#endif
struct taskProperties task2Properties;
#ifdef __cplusplus
extern "C"
#endif
struct taskProperties task3Properties;