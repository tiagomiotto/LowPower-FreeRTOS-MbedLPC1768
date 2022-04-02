#include "testApplication.h"

struct taskProperties task1Properties;

struct taskProperties task2Properties;

struct taskProperties task3Properties;

#ifdef __cplusplus
extern "C"
#endif
    void
    setupTaskParameters()
{

    task1Properties.taskNumber = 0;
    task1Properties.taskPriority = 4;
    task1Properties.xDelay = 8000;
    task1Properties.taskWorstCaseExecuteTime = 3000;
    task1Properties.xFibonnaciCycles = fibonnaciCycles1MS_96Mhz * (task1Properties.taskWorstCaseExecuteTime/2.0);
    task1Properties.xFibonnaciCyclesWorstCase = fibonnaciCycles1MS_96Mhz * task1Properties.taskWorstCaseExecuteTime; // Roughly 800ms at 96Mhz
    task1Properties.xPowerConsumptionTestIsWorstCase = (int *)pvPortMalloc(8 * sizeof(int));
    memcpy(task1Properties.xPowerConsumptionTestIsWorstCase, (int[8]){0, 1, 0, 0, 1, 1, 0, 1}, sizeof(int) * 8);

    task2Properties.taskNumber = 1;
    task2Properties.taskPriority = 3;
    task2Properties.xDelay = 10000;
    task2Properties.taskWorstCaseExecuteTime = 2000;
    task2Properties.xFibonnaciCycles = fibonnaciCycles1MS_96Mhz * (task2Properties.taskWorstCaseExecuteTime/2.0);
    task2Properties.xFibonnaciCyclesWorstCase = fibonnaciCycles1MS_96Mhz * task2Properties.taskWorstCaseExecuteTime;
    task2Properties.xPowerConsumptionTestIsWorstCase = (int *)pvPortMalloc(8 * sizeof(int));
    memcpy(task2Properties.xPowerConsumptionTestIsWorstCase, (int[8]){0, 0, 1, 0, 1, 0, 1, 1}, sizeof(int) * 8);

    task3Properties.taskNumber = 2;
    task3Properties.taskPriority = 2;
    task3Properties.xDelay = 14000;
    task3Properties.taskWorstCaseExecuteTime = 1000;
    task3Properties.xFibonnaciCycles = fibonnaciCycles1MS_96Mhz * (task3Properties.taskWorstCaseExecuteTime/2.0);
    task3Properties.xFibonnaciCyclesWorstCase = fibonnaciCycles1MS_96Mhz * task3Properties.taskWorstCaseExecuteTime;
    task3Properties.xPowerConsumptionTestIsWorstCase = (int *)pvPortMalloc(8 * sizeof(int));
    memcpy(task3Properties.xPowerConsumptionTestIsWorstCase, (int[8]){0, 0, 0, 1, 0, 1, 1, 1}, sizeof(int) * 8);
}
