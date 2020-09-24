/* 
 * Read temperature and humility sensor  
 * Libraries (Sketch >> Include Library >> Manage Libraies)  
 *  SimpleDHT
 */
#include <SimpleDHT.h>

/*
 * MACROS
 */
//#define DEBUG

#define TEMPERATURE_ID    4    //id of temperature on table Sensors
#define HUMIDITY_ID       15   //id of himidity on table Sensors
#define WEATHER_GPIO  2
 
/* 
 * GlobalVariables
 */
SimpleDHT11 dht(WEATHER_GPIO);

/*
 * functions
 */
int weather_init()
{
    return 1;
}

void weather_read(byte *temperature, byte *humidity) {
  int res = dht.read(temperature, humidity, NULL);

#ifdef DEBUG
switch (res) {
  case SimpleDHTErrSuccess: break;
  case SimpleDHTErrStartLow: Serial.println("Error to wait for start low signal.");break;
  case SimpleDHTErrStartHigh: Serial.println("Error to wait for start high signal.");break;
  case SimpleDHTErrDataLow: Serial.println("Error to wait for data start low signal.");break;
  case SimpleDHTErrDataRead: Serial.println("Error to wait for data read signal.");break;
  case SimpleDHTErrDataEOF: Serial.println("Error to wait for data EOF signal.");break;
  case SimpleDHTErrDataChecksum: Serial.println("Error to validate the checksum.");break;
  case SimpleDHTErrZeroSamples: Serial.println("Error when temperature and humidity are zero, it shouldn't happen.");break;
  case SimpleDHTErrNoPin: Serial.println(" Error when pin is not initialized.");break;    
  default: Serial.print("Error code: ");Serial.println(res);break;
}
#endif
  
#ifdef DEBUG
Serial.print((int)*temperature); Serial.print(" *C, "); 
Serial.print((int)*humidity); Serial.println(" H");
#endif
  
  delay(200); //need, less than result error    
}
