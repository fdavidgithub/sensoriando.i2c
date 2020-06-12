/* 
 * Read sound sensor
 * 
 * Build with IDE Arduino 1.8.9
 *
 * REQUIREMENT COMPILE
 * 1x Board Arduino
 *    Tools>>Board (Arduino Nano -> Processor 328P Old Botloader
 *      
 * Libraries (Sketch >> Include Library >> Manage Libraies)  
 */
#include <Wire.h>
#include "temphumi.h"

#define DEBUG
//#define GPIO_SDA A4 
//#define GPIO_SCL A5 

/* 
 * Global variables
 */
WireTemperature = new Wire();
WireHumidity = new Wire();

/*
 * Functions
 */
void requestEvent_temperature() { 
    char stream[10];
    String stream_value;
    int i, stream_len;
    int value = temperature_read();
        
#ifdef DEBUG
    Serial.print("(temperature) Data request: ");
    Serial.println(value);
#endif

    stream_value = String(value);
    
    while ( stream_value.length() < 10 ) {
      stream_value = "0" + stream_value;  
    }

    for (i=0; i<10; i++) {
      stream[i] = stream_value[i];  
    }
    
#ifdef DEBUG
    Serial.print("(temperature) Stream request: ");
    Serial.println(stream);
#endif
   
    Wire.write(stream);
}

void requestEvent_humidity() { 
    char stream[10];
    String stream_value;
    int i, stream_len;
    int value = humidity_read();
        
#ifdef DEBUG
    Serial.print("(humidity) Data request: ");
    Serial.println(value);
#endif

    stream_value = String(value);
    
    while ( stream_value.length() < 10 ) {
      stream_value = "0" + stream_value;  
    }

    for (i=0; i<10; i++) {
      stream[i] = stream_value[i];  
    }
    
#ifdef DEBUG
    Serial.print("(humidity) Stream request: ");
    Serial.println(stream);
#endif
   
    Wire.write(stream);
}

/*
 * Setting
 */
void setup()
{
#ifdef DEBUG
    Serial.begin(9600);
#endif

    WireTemperature.begin(TEMPERATURE_ID); 
    WireTemperature.onRequest(requestEvent_temperature);
    
    WireHumidity.begin(HUMIDITY_ID); 
    WireHumidity.onRequest(requestEvent_humidity);
    
    temphumi_init();

#ifdef DEBUG
    Serial.println("Waiting...");
#endif    

}

void loop()
{

}
 
