/*
 * FreeRTOS Kernel V10.3.1
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/* Standard includes. */
#include <limits.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* User defined Variables */
#include <stdbool.h>
extern const int frequencyLevels[];
extern const int staticTickIncrement[];
extern const int periodicTickIncrement[];
extern volatile int currentFrequencyLevel;
extern volatile short periodicTickIncrementCount;
extern const short availableFrequencyLevels;
extern volatile bool frequencyChanged;

#define SBIT_TIMER1 2
#define SBIT_MR0I 0
#define SBIT_MR0R 1
#define SBIT_CNTEN 0
#define SBIT_CNRST 1
#define PCLK_TIMER1 4

/* The frequency at which TIM2 will run. */
#define lpCLOCK_INPUT_FREQUENCY (10000UL)

/* Calculate how many clock increments make up a single tick period. */
static const uint32_t ulReloadValueForOneTick = ((lpCLOCK_INPUT_FREQUENCY / configTICK_RATE_HZ) - 1);
unsigned int getPrescalarFor100Us(uint8_t timerPclkBit);

/* Flag set from the tick interrupt to allow the sleep processing to know if
sleep mode was exited because of an tick interrupt or a different interrupt. */
static volatile uint32_t ulTickFlag = pdFALSE;

/* Holds the maximum number of ticks that can be suppressed - which is
basically how far into the future an interrupt can be generated. Set during
initialisation. */
static TickType_t xMaximumPossibleSuppressedTicks = 0;

// /* For debugging*/
#ifndef FILE_H
#define FILE_H
#include "PowerControl.h"
#include "ClockControl.h"
#endif

#define LED1 18 //
#define LED2 20 //
#define LED3 21 //
#define LED4 23 //

/* Write to GPIO 2 on Port 18,20,21,23
 Initialized on main  otherwise initialize here
 Can't use mbed.h cause its not a .cpp file, and a .cpp file complains about the normal port tick 
 interrupt
LED1 = P1_18,
LED2 = P1_20,
LED3 = P1_21,
LED4 = P1_23,
*/

#ifdef DEBUG

#define configPRE_SLEEP_PROCESSING(x) \
	LPC_GPIO1->FIOPIN = (1 << LED1);  \
	Sleep();
#define configPOST_SLEEP_PROCESSING(x) LPC_GPIO1->FIOPIN = (0 << LED1);

#else
#define configPRE_SLEEP_PROCESSING(x) Sleep();

#endif

/*
 * When configDYNAMIC_FREQUENCY_LOW_POWER_MODE is set to 1 then the tick interrupt
 * is generated by the TIM2 peripheral.  The TIM2 configuration and handling
 * functions are defined in this file.  Note the RTC is not used as there does
 * not appear to be a way to read back the RTC count value, and therefore the
 * only way of knowing exactly how long a sleep lasted is to use the very low
 * resolution calendar time.
 *
 * When configDYNAMIC_FREQUENCY_LOW_POWER_MODE is set to 0 the tick interrupt is
 * generated by the standard FreeRTOS Cortex-M port layer, which uses the
 * SysTick timer.
 */
#if configDYNAMIC_FREQUENCY_LOW_POWER_MODE == 1

void dynamicFrequencySysTickHandler(void)
{
	/* The SysTick runs at the lowest interrupt priority, so when this interrupt
	executes all interrupts must be unmasked.  There is therefore no need to
	save and then restore the interrupt mask value as its value is already
	known. */
	portDISABLE_INTERRUPTS();

	/* Increment the RTOS tick. */

	if (xTaskIncrementTick() != pdFALSE)
	{
		/* A context switch is required.  Context switching is performed in
		the PendSV interrupt.  Pend the PendSV interrupt. */
		portNVIC_INT_CTRL_REG = portNVIC_PENDSVSET_BIT;
	}

	if (frequencyChanged)
	{
		periodicTickIncrementCount = 0;
		frequencyChanged = false;
	}

	short extraTicks = staticTickIncrement[currentFrequencyLevel];
	if (periodicTickIncrementCount == periodicTickIncrement[currentFrequencyLevel] && periodicTickIncrement[currentFrequencyLevel] != 0)
	{
		extraTicks++;
		periodicTickIncrementCount = 0;
	}

	for (int i = 0; i < extraTicks; i++)
	{
		/* Increment the RTOS tick. */
		if (xTaskIncrementTick() != pdFALSE)
		{
			/* A context switch is required.  Context switching is performed in
			the PendSV interrupt.  Pend the PendSV interrupt. */
			portNVIC_INT_CTRL_REG = portNVIC_PENDSVSET_BIT;
		}
	}

	periodicTickIncrementCount++;

	portENABLE_INTERRUPTS();

	/* The CPU woke because of a tick. */
	ulTickFlag = pdTRUE;
}

void vPortSetupTimerInterrupt(void)
{

	LPC_SC->PCONP |= (1 << SBIT_TIMER1); /* Power ON Timer1 */

	LPC_TIM1->MCR = (1 << SBIT_MR0I) | (1 << SBIT_MR0R); /* Clear TC on MR0 match and Generate Interrupt*/
	LPC_TIM1->PR = getPrescalarFor100Us(PCLK_TIMER1);	 /* Prescalar for 1us */
	LPC_TIM1->MR0 = (1000 / configTICK_RATE_HZ) * 10;	 /* Load timer value to generate Tick Rate delay*/
	LPC_TIM1->TCR = (1 << SBIT_CNTEN);					 /* Start timer by setting the Counter Enable*/
	NVIC_EnableIRQ(TIMER1_IRQn);

	/* See the comments where xMaximumPossibleSuppressedTicks is declared. */
	xMaximumPossibleSuppressedTicks = ((unsigned long)USHRT_MAX) / ulReloadValueForOneTick;
}

void disableTIMER1(void)
{

	LPC_TIM1->TCR = (0 << SBIT_CNTEN); /* Start timer by setting the Counter Enable*/
	NVIC_DisableIRQ(TIMER1_IRQn);	   /* Enable Timer1 Interrupt */
}

void enableTIMER1(void)
{

	LPC_TIM1->TCR = (1 << SBIT_CNTEN); /* Stop timer by setting the Counter Enable to 0*/
	NVIC_EnableIRQ(TIMER1_IRQn);	   /* Disable Timer1 Interrupt */
}

void resetTIMER1(void)
{

	LPC_TIM1->TCR = (1 << SBIT_CNRST); /* Reset the timer by setting the Counter Reset*/
	LPC_TIM1->TCR = (0 << SBIT_CNRST); /* Reset the timer by setting the Counter Reset*/
}

void setReloadValueTIMER1(int reloadValue)
{
	LPC_TIM1->MR0 = reloadValue;
}

void setCounterTIMER1(uint32_t counterValue)
{
	LPC_TIM1->TC = counterValue;
}

uint32_t getCounterTIMER1(void)
{
	return LPC_TIM1->TC;
}

void TIMER1_IRQHandler(void)
{
	LPC_TIM1->IR |= (1 << 0); //Clear MR0 Interrupt flag
	dynamicFrequencySysTickHandler();
}

/* Courtesy of https://www.exploreembedded.com/wiki/LPC1768:_Timers */
unsigned int getPrescalarFor100Us(uint8_t timerPclkBit)
{
	unsigned int pclk, prescalarFor100Us;
	pclk = (LPC_SC->PCLKSEL0 >> timerPclkBit) & 0x03; /* get the pclk info for required timer */

	switch (pclk) /* Decode the bits to determine the pclk*/
	{
	case 0x00:
		pclk = SystemCoreClock / 4;
		break;

	case 0x01:
		pclk = SystemCoreClock;
		break;

	case 0x02:
		pclk = SystemCoreClock / 2;
		break;

	case 0x03:
		pclk = SystemCoreClock / 8;
		break;
	}

	prescalarFor100Us = pclk / 10000 - 1; /* Prescalar for 1us (1000000Counts/sec) */
	//prescalarForMs = pclk / 1000 - 1; /* Prescalar for 1us (1000000Counts/sec) */
	//prescalarForUs = pclk / 1000000 - 1; /* Prescalar for 1us (1000000Counts/sec) */

	return prescalarFor100Us;
}

void vApplicationSleep(TickType_t xExpectedIdleTime)
{
	uint32_t ulCounterValue, ulReloadValue, ulCompleteTickPeriods, ulCompletedSysTickDecrements;
	TickType_t xModifiableIdleTime;

	/* Make sure the SysTick reload value does not overflow the counter. */
	if (xExpectedIdleTime > xMaximumPossibleSuppressedTicks)
	{
		xExpectedIdleTime = xMaximumPossibleSuppressedTicks;
	}

	/* Stop the Tick Timer momentarily.  The time the Tick Timer is stopped for
	is accounted for as best it can be, but using the tickless mode will
	inevitably result in some tiny drift of the time maintained by the
	kernel with respect to calendar time. */
	disableTIMER1();

	/* Calculate the reload value required to wait xExpectedIdleTime
	tick periods.  -1 is used because this code will execute part way
	through one of the tick periods. */
	ulReloadValue = ulReloadValueForOneTick * xExpectedIdleTime;

	// if (ulReloadValue > ulStoppedTimerCompensation)
	// {
	// 	ulReloadValue -= ulStoppedTimerCompensation;
	// }

	/* Enter a critical section but don't use the taskENTER_CRITICAL()
	method as that will mask interrupts that should exit sleep mode. */
	__asm volatile("cpsid i" ::
					   : "memory");
	__asm volatile("dsb");
	__asm volatile("isb");

	/* The tick flag is set to false before sleeping.  If it is true when sleep
	mode is exited then sleep mode was probably exited because the tick was
	suppressed for the entire xExpectedIdleTime period. */
	ulTickFlag = pdFALSE;

	/* If a context switch is pending or a task is waiting for the scheduler
	to be unsuspended then abandon the low power entry. */
	if (eTaskConfirmSleepModeStatus() == eAbortSleep)
	{
		/* Restart from whatever is left in the count register to complete
		this tick period. */
		enableTIMER1();

		/* Re-enable interrupts - see comments above the cpsid instruction()
		above. */
		__asm volatile("cpsie i" ::
						   : "memory");
	}
	else
	{
		/* Adjust the TIM2 value to take into account that the current time
		slice is already partially complete. */
		ulReloadValue -= (uint32_t)getCounterTIMER1();
		/* Set the new reload value. */

		//portNVIC_SYSTICK_LOAD_REG = ulReloadValue;
		setReloadValueTIMER1(ulReloadValue);

		/* Clear the SysTick count flag and set the count value back to
		zero. */
		resetTIMER1();
		setCounterTIMER1(0);

		/* Restart SysTick. */
		enableTIMER1();

		/* Sleep until something happens.  configPRE_SLEEP_PROCESSING() can
		set its parameter to 0 to indicate that its implementation contains
		its own wait for interrupt or wait for event instruction, and so wfi
		should not be executed again.  However, the original expected idle
		time variable must remain unmodified, so a copy is taken. */
		xModifiableIdleTime = xExpectedIdleTime;
		configPRE_SLEEP_PROCESSING(xModifiableIdleTime);
		if (xModifiableIdleTime > 0)
		{
			__asm volatile("dsb" ::
							   : "memory");
			__asm volatile("wfi");
			__asm volatile("isb");
		}
		configPOST_SLEEP_PROCESSING(xExpectedIdleTime);

		/* Re-enable interrupts to allow the interrupt that brought the MCU
		out of sleep mode to execute immediately.  see comments above
		__disable_interrupt() call above. */
		__asm volatile("cpsie i" ::
						   : "memory");
		__asm volatile("dsb");
		__asm volatile("isb");

		/* Disable interrupts again because the clock is about to be stopped
		and interrupts that execute while the clock is stopped will increase
		any slippage between the time maintained by the RTOS and calendar
		time. */
		__asm volatile("cpsid i" ::
						   : "memory");
		__asm volatile("dsb");
		__asm volatile("isb");

		/* Disable the SysTick clock without reading the
		portNVIC_SYSTICK_CTRL_REG register to ensure the
		portNVIC_SYSTICK_COUNT_FLAG_BIT is not cleared if it is set.  Again,
		the time the SysTick is stopped for is accounted for as best it can
		be, but using the tickless mode will inevitably result in some tiny
		drift of the time maintained by the kernel with respect to calendar
		time*/
		disableTIMER1();

		/* Determine if the SysTick clock has already counted to zero and
		been set back to the current reload value (the reload back being
		correct for the entire expected idle time) or if the SysTick is yet
		to count to zero (in which case an interrupt other than the SysTick
		must have brought the system out of sleep mode). */
		if (ulTickFlag != pdFALSE)
		{
			/* The tick interrupt is already pending, and the SysTick count
			reloaded with ulReloadValue.  Reset the
			portNVIC_SYSTICK_LOAD_REG with whatever remains of this tick
			period. */
			ulCounterValue = ulReloadValueForOneTick - getCounterTIMER1();

			/* Trap under/overflows before the calculated value is used. */
			configASSERT(ulCounterValue >= 0);

			setReloadValueTIMER1(ulCounterValue);
			setCounterTIMER1(0);

			/* As the pending tick will be processed as soon as this
			function exits, the tick value maintained by the tick is stepped
			forward by one less than the time spent waiting. */
			ulCompleteTickPeriods = xExpectedIdleTime - 1UL;
		}
		else
		{
			/* Something other than the tick interrupt ended the sleep.
			Work out how long the sleep lasted rounded to complete tick
			periods (not the ulReload value which accounted for part
			ticks). */
			ulCompleteTickPeriods = (getCounterTIMER1()) / ulReloadValueForOneTick;

			/* The reload value is set to whatever fraction of a single tick
			period remains. */
			ulCounterValue = (getCounterTIMER1()) - (ulCompleteTickPeriods * ulReloadValueForOneTick);

			if (ulCounterValue == 0)
			{
				/* There is no fraction remaining. */
				ulCounterValue = ulReloadValueForOneTick;
				ulCompleteTickPeriods++;
			}

			setReloadValueTIMER1(ulCounterValue);
			setCounterTIMER1(0);
		}

		/* Restart SysTick so it runs from portNVIC_SYSTICK_LOAD_REG
		again, then set portNVIC_SYSTICK_LOAD_REG back to its standard
		value. */
		setCounterTIMER1(0);
		enableTIMER1();
		vTaskStepTick(ulCompleteTickPeriods);
		setReloadValueTIMER1(ulReloadValueForOneTick);

		/* Exit with interrupts enabled. */
		__asm volatile("cpsie i" ::
						   : "memory");
	}
}

#endif