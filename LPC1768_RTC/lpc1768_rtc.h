#include "TARGET_LPC1768_N/LPC17xx.h"

/* bit position of CCR register */
#define SBIT_CLKEN     0    /* RTC Clock Enable*/
#define SBIT_CTCRST    1    /* RTC Clock Reset */
#define SBIT_CCALEN    4    /* RTC Calibration counter enable */

struct RTC_DATA{
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
    uint8_t day;
    uint8_t month;
    uint16_t year;
};

void initRTC(struct RTC_DATA current);
void setTime(struct RTC_DATA current);
struct RTC_DATA getTime();    
struct RTC_DATA defaultTime();