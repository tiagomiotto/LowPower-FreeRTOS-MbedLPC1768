/* mbed PowerControl Library
  * Copyright (c) 2010 Michael Wei
  */ 
  
//shouldn't have to include, but fixes weird problems with defines
#include "TARGET_LPC1768_N/LPC17xx.h"

#ifndef MBED_CLOCKCONTROL_H 
#define MBED_CLOCKCONTROL_H 


//Added this to make sure the function can be used in .c files
#ifdef __cplusplus
extern "C"
#endif 
unsigned int setSystemFrequency(unsigned char clkDivider, unsigned char clkSrc, unsigned short cfg_m, unsigned char cfg_n);
void setPLL0Frequency(unsigned char clkSrc, unsigned short cfg_m, unsigned char cfg_n);
void setPLL1Frequency(unsigned char clkSrc, unsigned short cfg_m, unsigned char cfg_n);
#endif