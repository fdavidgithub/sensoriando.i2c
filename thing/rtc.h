#ifndef RTCLIB
#define RTCLIB
/*
 * Date and time functions using a DS1307 RTC connected via I2C and Wire lib
 */
#include <RTClib.h>
#include <Nanoshield_RTC.h>

/*
 * MACROS
 */
//#define DEBUG

/*
 * GlobalVariables
 */
Nanoshield_RTC rtc;

/*
 * functions
 */
int rtc_init () { 
  int result = rtc.begin();    
  return result;
}

DateTime rtc_get () {  
    rtc.read();
    return DateTime(rtc.getYear(), rtc.getMonth(), rtc.getDay(), rtc.getHours(), rtc.getMinutes(), rtc.getSeconds()); 
}

void rtc_sync(DateTime dt_ntp, DateTime dt_rtc) {
    rtc.writeYear(dt_ntp.year());
    rtc.writeMonth(dt_ntp.month());
    rtc.writeDay(dt_ntp.day());
    rtc.writeHours(dt_ntp.hour());
    rtc.writeMinutes(dt_ntp.minute());
    rtc.writeSeconds(dt_ntp.second());
    
    delay(100);
}

int rtc_check() {
  return rtc.getDay() != 0;  
}

#endif
