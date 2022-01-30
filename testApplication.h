
#define fibonnaciCycles1MS_96Mhz 4700

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
