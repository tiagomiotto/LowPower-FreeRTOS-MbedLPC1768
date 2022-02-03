#ifndef LPC1768PTM
#define LPC1768PTM

#include <stdbool.h>
#include "FreeRTOS.h"
/*
 * The tick hook function.  This compensates
 * the tick count to compensate for frequency changes
 */
const uint8_t mValues[6] = {36, 30, 27, 18, 9, 6};
const int frequencyLevels[] = {96,  80, 72,  48,  24,  8};
const int staticTickIncrement[] = {0, 0, 0, 0, 0, 1, 2, 3, 5, 11};
const int periodicTickIncrement[] = {0, 11, 5, 3, 2, 0, 0, 0, 0, 0};
// const short availableFrequencyLevels = 10;
// extern volatile int currentFrequencyLevel;
extern volatile short periodicTickIncrementCount;
extern volatile bool frequencyChanged;
#ifdef __cplusplus
extern "C" 
#endif
short dvfsMode;
int frequencyLevelSelector(int level);

void dynamicFrequencySysTickHandler(void);

void disableTIMER1(void);

void enableTIMER1(void);


void resetTIMER1(void);

void setReloadValueTIMER1(int reloadValue);

void setCounterTIMER1(uint32_t counterValue);

uint32_t getCounterTIMER1(void);

void updatePrescalerTIMER1(void);

void TIMER1_IRQHandler(void);

/* Courtesy of https://www.exploreembedded.com/wiki/LPC1768:_Timers */
unsigned int getPrescalarFor100Us(uint8_t timerPclkBit);

#ifdef __cplusplus
extern "C"
#endif
	void
	setupDVFS(int main_numberOfTasks, int *main_taskWorstCaseComputeTime, int *main_taskDeadlines, int main_availableFrequencyLevels, int *main_frequencyStages, int main_mode);

#ifdef __cplusplus
extern "C"
#endif
	void
	default_setupDVFS(int main_numberOfTasks, int *main_taskWorstCaseComputeTime, int *main_taskDeadlines, int main_mode);

#ifdef __cplusplus
extern "C"
#endif
	int
	staticVoltageScalingFrequencyLevelSelector(void);

bool staticVoltageScalingRM_Test(float alpha);

bool staticVoltageScalingEDF_Test(float alpha);

int setupCycleConservingDVS();
static int cycleConservingDVSFrequencySelector(int currentTick);
static void cycleConservingDVSAllocateCycles(int k);

#ifdef __cplusplus
extern "C"
#endif
int cycleConservingDVSTaskReady(int taskNumber, int currentTick, int taskNextExecution);

#ifdef __cplusplus
extern "C"
#endif
int cycleConservingDVSTaskComplete(int taskNumber, int currentTick);

#ifdef __cplusplus
extern "C"
#endif
void vTaskStartLowPowerScheduller(int main_numberOfTasks, int *main_taskWorstCaseComputeTime, int *main_taskDeadlines, int main_availableFrequencyLevels, int *main_frequencyStages, int main_mode);
bool sufficientSchedulabilityTest(int main_numberOfTasks, int *taskWorstCaseComputeTime, int *taskDeadlines );

#endif
