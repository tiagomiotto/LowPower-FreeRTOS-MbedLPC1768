#ifndef HEADER_FILE2
#define HEADER_FILE2

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
int staticVoltageScalingFrequencyLevelSelector(int numberOfTasks,
												   int* taskPeriods, int* taskWorstCaseComputeTime, int mode);
bool staticVoltageScalingRM_Test(int numberOfTasks,
								 int *taskPeriods, int *taskWorstCaseComputeTime, float alpha);                                                 

bool staticVoltageScalingEDF_Test(int numberOfTasks,
								  int *taskPeriods, int *taskWorstCaseComputeTime, float alpha);
#endif

