#ifndef NTPLIB
#define NTPLIB
/*
 * 
 */
#include <NTPClient.h>

#ifndef WIFILIB
  #include "wifi.h"
#endif

/*
 * MACROS
 */
//#define DEBUG
#define TIMEZONE        0   //UTC

/*
 * GlobalVariables
 */

/*
 * functions
 */
DateTime ntp_get() 
{
  WiFiUDP ntpUDP;
  DateTime ntp = NULL;

  NTPClient timeClient(ntpUDP, "pool.ntp.br", TIMEZONE*3600, 60000);

  timeClient.begin();

  if ( timeClient.update() ) { 
#ifdef DEBUG    
      Serial.print("NTP: ");
      Serial.print(timeClient.getFormattedTime());
#endif

      ntp = DateTime(timeClient.getEpochTime());
  }
      
  timeClient.end();
  
  return ntp;
}
 
#endif
