#ifndef RTCLIB
/*
 * 
 */
#include <RTClib.h>

#ifndef I2CLIB
  #include "i2c.h"
#endif

/*
 * MACROS
 */
#define RTCLIB
//#define DEBUG

#ifndef GPIO_SDA
  #define GPIO_SDA D1 
#endif

#ifndef GPIO_SCL
  #define GPIO_SCL D2
#endif

/*
 * GlobalVariables
 */
RTC_DS1307 Rtc;

/*
 * Functions
 */
int rtc_init () { 
  int result=1;

#ifdef DEBUG
    Serial.println("Init. RTC");
#endif    
 
  if ( ! Rtc.begin()) {
#ifdef DEBUG
    Serial.println("Couldn't find RTC");
#endif    

    result=0;
  } else if ( ! Rtc.isrunning() ) {
#ifdef DEBUG
    Serial.println("RTC is NOT running...");
#endif    
    // following line sets the RTC to the date & time this sketch was compiled
    Rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
     
    delay(3000);      
  }
  
  return result;
}

DateTime rtc_get () {
    DateTime now = Rtc.now();

#ifdef DEBUG    
    Serial.print(now.day(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.year(), DEC);
   
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.println(now.second(), DEC);
#endif

  return now;
}

void rtc_sync(DateTime dt_ntp, DateTime dt_rtc)
{
  //If NTP is less than RTC, it ocurred error when connect with NTP 
  if ( dt_ntp.unixtime() >= dt_rtc.unixtime() ) {
#ifdef DEBUG
    Serial.println("Updating RTC with NTP");
#endif
  
    Rtc.adjust(DateTime(dt_ntp.year(), dt_ntp.month(), dt_ntp.day(), dt_ntp.hour(), dt_ntp.minute(), dt_ntp.second()));  

#ifdef DEBUG    
    Serial.print("RTC Updated to: ");
    Serial.print(dt_rtc.year(), DEC);
    Serial.print('/');
    Serial.print(dt_rtc.month(), DEC);
    Serial.print('/');
    Serial.print(dt_rtc.day(), DEC);
    Serial.print(" ");
    Serial.print(dt_rtc.hour(), DEC);
    Serial.print(':');
    Serial.print(dt_rtc.minute(), DEC);
    Serial.print(':');
    Serial.print(dt_rtc.second(), DEC);
    Serial.println("---------------------------------");
#endif
  }
}


#endif
