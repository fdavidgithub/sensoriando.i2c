/* 
 * Read temperature and humility sensor  
 * Libraries (Sketch >> Include Library >> Manage Libraies)  
 *  SimpleDHT
 */
#include <SimpleDHT.h>

/*
 * MACROS
 */
#define DEBUG

#define TEMPERATURE_ID    4    //id of temperature on table Sensors
#define HUMIDITY_ID       15   //id of himidity on table Sensors
#define TEMPHUMI_GPIO  2
 
/* 
 * GlobalVariables
 */
SimpleDHT11 dht11(TEMPHUMI_GPIO);

/*
 * functions
 */
int temphumi_init()
{
    return 1;
}

int temperature_read()
{ 
  byte temperature = 0;
  int err = SimpleDHTErrSuccess;
  
  if ((err = dht11.read(&temperature, NULL, NULL)) != SimpleDHTErrSuccess) {
      temperature = err;

#ifdef DEBUG
      Serial.print("Read DHT11 failed, err="); 
      Serial.println(err);
#endif
  }

  return (int)temperature;
}


int humidity_read()
{ 
  byte humidity = 0;
  int err = SimpleDHTErrSuccess;
  
  if ((err = dht11.read(NULL, &humidity, NULL)) != SimpleDHTErrSuccess) {
      humidity = err;

#ifdef DEBUG
      Serial.print("Read DHT11 failed, err="); 
      Serial.println(err);
#endif
  }

  return (int)humidity;
}
