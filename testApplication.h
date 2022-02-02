
#define fibonnaciCycles1MS_96Mhz 6397
//Fibanacci calculation = 52 + Runs*15 processor cycles at 96MHz
//6397 goes over by around 0.00729167 %, 6396 goes under by 0.008333333 %
//For 1ms x= (96000000-52)/(15*1000) = 6396.533333333333333333333333333333

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


struct taskProperties task1Properties;


struct taskProperties task2Properties;


struct taskProperties task3Properties;
