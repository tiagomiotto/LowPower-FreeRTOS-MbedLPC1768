#include "testApplication.h"

 
struct taskProperties task1Properties;


struct taskProperties task2Properties;


struct taskProperties task3Properties;

#ifdef __cplusplus
extern "C"
#endif
void setupTaskParameters()
{

    task1Properties.taskNumber = 1;
    task1Properties.taskPriority = 4;
    task1Properties.xDelay = 8000;
    task1Properties.xFibonnaciCycles = fibonnaciCycles1MS_96Mhz * 1500;
    task1Properties.xFibonnaciCyclesWorstCase = fibonnaciCycles1MS_96Mhz * 3000; // Roughly 800ms at 96Mhz
    task1Properties.xPowerConsumptionTestIsWorstCase  = (int*) malloc(8 * sizeof(int));
    memcpy(task1Properties.xPowerConsumptionTestIsWorstCase, (int[8]) {0, 1, 0, 0, 1, 1, 0, 1}, sizeof(int) * 8);

    
    task2Properties.taskNumber = 2;
    task2Properties.taskPriority = 3;
    task2Properties.xDelay = 10000;
    task2Properties.xFibonnaciCycles = fibonnaciCycles1MS_96Mhz * 1500;
    task2Properties.xFibonnaciCyclesWorstCase = fibonnaciCycles1MS_96Mhz * 3000;
    task2Properties.xPowerConsumptionTestIsWorstCase = (int*) malloc(8 * sizeof(int));
    memcpy(task2Properties.xPowerConsumptionTestIsWorstCase, (int[8]) {0, 0, 1, 0, 1, 0, 1, 1}, sizeof(int) * 8);


    task3Properties.taskNumber = 3;
    task3Properties.taskPriority = 2;
    task3Properties.xDelay = 14000;
    task3Properties.xFibonnaciCycles = fibonnaciCycles1MS_96Mhz * 500;
    task3Properties.xFibonnaciCyclesWorstCase = fibonnaciCycles1MS_96Mhz * 1000;
    task3Properties.xPowerConsumptionTestIsWorstCase = (int*) malloc(8 * sizeof(int));
    memcpy(task3Properties.xPowerConsumptionTestIsWorstCase, (int[8]) {0, 0, 0, 1, 0, 1, 1, 1}, sizeof(int) * 8);
}
