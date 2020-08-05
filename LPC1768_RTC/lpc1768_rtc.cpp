#include "lpc1768_rtc.h"

void initRTC(struct RTC_DATA current){
        /* Disable RTC clock, reset clock, Enable RTC calibration */
    LPC_RTC->CCR = ((1 << SBIT_CTCRST ) | (1 << SBIT_CCALEN));
    LPC_RTC->CALIBRATION = 0x00;
    LPC_RTC->CCR = (1 << SBIT_CLKEN);    /* Enable the clock for RTC */
    
    setTime(current);
    
}

void setTime(struct RTC_DATA current){
                                     
    // Set Date 
    LPC_RTC->DOM    = current.day;   // Update date value 
    LPC_RTC->MONTH  = current.month;   // Update month value
    LPC_RTC->YEAR   = current.year; // Update year value

    // Set Time 
    LPC_RTC->HOUR   = current.hour;   // Update hour value 
    LPC_RTC->MIN    = current.min;   // Update min value
    LPC_RTC->SEC    = current.sec;   // Update sec value     

}



struct RTC_DATA getTime(){
    struct RTC_DATA rtc;
    rtc.hour = LPC_RTC->HOUR;
    rtc.min  = LPC_RTC->MIN; 
    rtc.sec  = LPC_RTC->SEC; 
    rtc.day  = LPC_RTC->DOM;   
    rtc.month = LPC_RTC->MONTH;  
    rtc.year  = LPC_RTC->YEAR;   
    
    return rtc;
}

struct RTC_DATA defaultTime(){
    struct RTC_DATA rtc;
    rtc.hour = 0;
    rtc.min  = 0; 
    rtc.sec  = 0; 
    rtc.day  = 1;   
    rtc.month = 1;  
    rtc.year  = 1970;   
    
    return rtc;
    
}