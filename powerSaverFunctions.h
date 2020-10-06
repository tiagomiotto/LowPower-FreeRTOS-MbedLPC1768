#ifndef powerSaverFunctions
#define powerSaverFunctions

bool phyInterfaceStatus = false;
int peripheralsNotNeeded = 0xFF;

int numberOfTasks, operationalMode;
int *deadlines, *worstCaseComputeTime;

void setup(bool phyStatus, int unnecessaryPeripherals, int numberOfTasks,
           int *taskDeadlines, int *taskWorstCases, int mode)
{
    operationalMode=mode;
    
    phyInterfaceStatus=phyStatus;
    if(phyInterfaceStatus) disablePhyInterface();

    peripheralsNotNeeded= unnecessaryPeripherals;
    disablePeripherals();


    deadlines = (int*) malloc(numberOfTasks * sizeof(int));
    worstCaseComputeTime = (int*) malloc(numberOfTasks * sizeof(int));

    memcpy(deadlines, taskDeadlines, numberOfTasks * sizeof(int));
    memcpy(worstCaseComputeTime, taskWorstCases, numberOfTasks * sizeof(int));



}


/* Hardware specific functions to be implemented depending on the 
** proccessor to be used
*/
void disablePhyInterface(){

}

void disablePeripherals(){

}


void setFrequency(){

}


void setVoltage(){

}


int convertTickToCycles(int ticks)
{
	return ticks * (frequencyLevels[currentFrequencyLevel] / configTICK_RATE_HZ);
}
/* Static voltage Scak=ling Implementation
** 
** Mode 0: RM
	Mode 1 EDF*/

#ifdef __cplusplus
extern "C"
#endif
	int
	staticVoltageScalingFrequencyLevelSelector(int numberOfTasks,
											   int *taskPeriods, int *taskWorstCaseComputeTime, int mode)
{

	float alphaToTest = 0.0f;
	bool validAlpha = true;
	int i = 0;
	int selectedFrequencyLevel = 0;

	/* Even if it fails in the max frequency, will return 0 and operate at max frequency
		If it is valid through all levels the second condition will act and stop the loop*/
	if (mode == 0) //RM
	{
		while (validAlpha && i < availableFrequencyLevels)
		{
			alphaToTest = (frequencyLevels[i] * 1.0f) / (frequencyLevels[0] * 1.0f);
			validAlpha = staticVoltageScalingRM_Test(numberOfTasks, taskPeriods, taskWorstCaseComputeTime, alphaToTest);
			if (validAlpha)
				i++;
		}
		selectedFrequencyLevel = i - 1;
		frequencyLevelSelector(selectedFrequencyLevel);
		return selectedFrequencyLevel;
	}

	if (mode == 1) //EDF
	{
		while (validAlpha && i < availableFrequencyLevels)
		{
			alphaToTest = (frequencyLevels[i] * 1.0f) / (frequencyLevels[0] * 1.0f);
			validAlpha = staticVoltageScalingEDF_Test(numberOfTasks, taskPeriods, taskWorstCaseComputeTime, alphaToTest);
			if (validAlpha)
				i++;
		}
		selectedFrequencyLevel = i - 1;
		frequencyLevelSelector(selectedFrequencyLevel);
		return selectedFrequencyLevel;
	}
	return 0;
}

bool staticVoltageScalingRM_Test(int numberOfTasks,
								 int *taskPeriods, int *taskWorstCaseComputeTime, float alpha)
{

	int currentTaskInTest = 0;
	float computeTimeSum = 0;

	while (1)
	{
		for (int i = 0; i < currentTaskInTest + 1; i++)
		{
			computeTimeSum += ceil((taskPeriods[currentTaskInTest] / 1.0f) / (taskPeriods[i] / 1.0f)) * taskWorstCaseComputeTime[i];
		}

		if (alpha * taskPeriods[currentTaskInTest] >= computeTimeSum)
		{

			if (currentTaskInTest + 1 == numberOfTasks)
				return true; //Alpha is valid and deadlines are not violated

			/* Reset the sum and test the next Task*/
			computeTimeSum = 0;
			currentTaskInTest++;
		}
		else
			return false;
	}
}

bool staticVoltageScalingEDF_Test(int numberOfTasks,
								  int *taskPeriods, int *taskWorstCaseComputeTime, float alpha)
{

	float computeTimeSum = 0;

	for (int i = 0; i < numberOfTasks; i++)
	{
		computeTimeSum += (taskWorstCaseComputeTime[i] * 1.0f) / (taskPeriods[i] * 1.0f);
	}

	if (alpha >= computeTimeSum)
		return true;
	return false;
}

/* Deadlines array has to be an array of pointers, to allow for the tasks to change the values inside the array
** during execution by changing the value of the pointer directly, intead of accessing the array
**
** To note: This function is not optimized for a huge amount of tasks, but having alarge amount of tasks 
** would break all the algorithms, therefore optimization here is not that important
*/
static int findNextDeadline(int **deadlinesArray, int numberOfTasks)
{
	int nextDeadline = *deadlinesArray[0]; // The element in the arrayis a pointer to the deadline of the First task
	for (int i = 1; i < numberOfTasks; i++)
	{
		if (nextDeadline > *deadlinesArray[i])
			nextDeadline = *deadlinesArray[i];
	}
	return nextDeadline;
}

/* Cycle Conserving DVS Implementation
   Only Implemented for RM
*/

#if numberOFTASKS != 3
#error Correct here to increase the number of Tasks, as this was a POC aimed at a system with 3 tasks
#endif

int c_left1, c_left2, c_left3;
int *c_lefti[3] = {&c_left1, &c_left2, &c_left3};

int d_1 = 0, d_2 = 0, d_3 = 0;
int *d_i[3] = {&d_1, &d_2, &d_3};

int frequencyChosenSVS = 96;

void setupCycleConservingDVS(
	int *taskPeriods, int *taskWorstCaseComputeTime)
{
	frequencyChosenSVS = staticVoltageScalingFrequencyLevelSelector(numberOFTASKS,
																	taskPeriods, taskWorstCaseComputeTime, 0);
	for (int i = 0; i < numberOFTASKS; i++)
	{
		*c_lefti[i] = taskWorstCaseComputeTime[i];
	}
}

int cycleConservingDVSFrequencySelector(int currentTick)
{
	// No need to convert into cycles, as the worst case computation time is in Ticks
	int maxTicksUntilNextDeadline = findNextDeadline(deadlines, numberOFTASKS);

	int totalD = 0;

	for (int i = 0; i < numberOFTASKS; i++)
	{
		totalD += *d_i[i];
	}

	float minimumFrequency = ((totalD * 1.0) / maxTicksUntilNextDeadline) * frequencyLevels[0];

	/* Can be optimized if we have a lot of frequencies to choose from, by only searching the bottom or top frequencies
	** Here it is not necessary as we have only 10 
	*/
	if (minimumFrequency > frequencyLevels[1])
		return 0; //If bigger than the 2 available frequency, we can only use the base

	int desiredFrequencyLevel = 0;
	for (int i = 1; i < availableFrequencyLevels; i++)
	{
		if (minimumFrequency > frequencyLevels[i])
			break;
		desiredFrequencyLevel++;
	}
	frequencyLevelSelector(desiredFrequencyLevel);
	return desiredFrequencyLevel;
}

void cycleConservingDVSAllocateCycles(int k)
{
	for (int i = 0; i < numberOFTASKS; i++)
	{
		if (*c_lefti[i] < k)
		{
			*d_i[i] = *c_lefti[i];
			k = k - *c_lefti[i];
		}
		else
		{
			*d_i[i] = k;
			k = 0;
		}
	}
}

void cycleConservingDVSTaskReady(int taskNumber, int currentTick)
{
	*c_lefti[taskNumber] = mainWorstCaseComputeTime[taskNumber];
	int s_m = findNextDeadline(deadlines, numberOFTASKS);
	int s_j = ceil(s_m * (frequencyChosenSVS * 1.0) / (frequencyLevels[0] * 1.0));
	cycleConservingDVSAllocateCycles(s_j);
	cycleConservingDVSFrequencySelector(currentTick);
}

void cycleConservingDVSTaskComplete(int taskNumber, int currentTick)
{
	*c_lefti[taskNumber] = 0;
	*d_i[taskNumber] = 0;
	cycleConservingDVSFrequencySelector(currentTick);
}

/* Cycle Conserving DVS EDF
*/

#if numberOFTASKS != 3
#error Correct here to increase the number of Tasks, as this was a POC aimed at a system with 3 tasks
#endif

float U_1 = 0, U_2 = 0, U_3 = 0;
float *U_i[3] = {&U_1, &U_2, &U_3};

int cycleConservingDVSFrequencySelectorEDF()
{

	float totalU = 0;

	for (int i = 0; i < numberOFTASKS; i++)
	{
		totalU += *U_i[i];
	}

	float minimumFrequency = totalU * frequencyLevels[0];

	/* Can be optimized if we have a lot of frequencies to choose from, by only searching the bottom or top frequencies
	** Here it is not necessary as we have only 10 
	*/
	if (minimumFrequency > frequencyLevels[1])
		return 0; //If bigger than the 2 available frequency, we can only use the base

	int desiredFrequencyLevel = 0;
	for (int i = 1; i < availableFrequencyLevels; i++)
	{
		if (minimumFrequency > frequencyLevels[i])
			break;
		desiredFrequencyLevel++;
	}
	frequencyLevelSelector(desiredFrequencyLevel);
	return desiredFrequencyLevel;
}

void cycleConservingDVSTaskReadyEDF(int taskNumber, int currentTick)
{
	int auxDeadline = *deadlines[taskNumber];
	*U_i[taskNumber] = mainWorstCaseComputeTime[taskNumber] * auxDeadline;
	cycleConservingDVSFrequencySelectorEDF(currentTick);
}

/* This one requires us to keep track of the execution time of the task. 
** Maybe should implement this for all the algorithms? 
** Or something available to all the the task
*/
void cycleConservingDVSTaskCompleteEDF(int taskNumber, int actualTaskExecutionTicks)
{
	int auxDeadline = *deadlines[taskNumber];
	*U_i[taskNumber] = actualTaskExecutionTicks / auxDeadline;
	cycleConservingDVSFrequencySelectorEDF();
}

#endif