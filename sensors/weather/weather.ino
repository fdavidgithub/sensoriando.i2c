/* 
 * Read temperature and humidity sensors
 * 
 * Build with IDE Arduino 1.8.12
 *
 * REQUIREMENT COMPILE
 * 1x Board Arduino
 *    Tools>>Board (Arduino Nano -> Processor 328P Old Botloader
 *      
 * Libraries (Sketch >> Include Library >> Manage Libraies)  
 * SimpleDHT    by Winlin v1.0.12
 * 
 */
#include <Wire.h>
#include "common.h"
#include "temphumi.h"

#define DEBUG
#define SENSOR_COUNT  2

/* 
 * Global variables
 */

/*
 * Functions
 */
void requestEvent() { 
    switch (mode_slave) {
        case INFO: Wire.write(STX);
                   Wire.write(SENSOR_COUNT);
                   Wire.write(ETX);      
#ifdef DEBUG
Serial.print("Counter: ");Serial.println(SENSOR_COUNT);
#endif
                   break;
        case NORMAL: if (mode_sensor == 0) temperature();
                     if (mode_sensor == 1) humidity();
                     break;  
    }
}
 
void temperature() { 
    char stream[I2C_CMD_NORMAL_LEN];
    float value;
            
    value = temperature_read();
        
#ifdef DEBUG
Serial.print("(temperature) Data request: ");Serial.println(value);
#endif

    datastream(value, TEMPERATURE_ID, stream);
     
#ifdef DEBUG
Serial.print("(temperature) Stream request: ");Serial.println(stream);
#endif

    Wire.write(stream, I2C_CMD_NORMAL_LEN);

#ifdef DEBUG
for (int dg=0; dg<I2C_CMD_NORMAL_LEN; dg++) Serial.println(stream[dg], HEX);
#endif
}

void humidity() { 
    char stream[I2C_CMD_NORMAL_LEN];
    float value;
            
    value = humidity_read();
        
#ifdef DEBUG
Serial.print("(humidity) Data request: ");Serial.println(value);
#endif

    datastream(value, HUMIDITY_ID, stream);
     
#ifdef DEBUG
Serial.print("(humidity) Stream request: ");Serial.println(stream);
#endif

    Wire.write(stream, I2C_CMD_NORMAL_LEN);

#ifdef DEBUG
for (int dg=0; dg<I2C_CMD_NORMAL_LEN; dg++) Serial.println(stream[dg], HEX);
#endif
}

/*
 * Setting
 */
void setup()
{
#ifdef DEBUG
    Serial.begin(9600);
#endif

    Wire.begin(I2C_ADDRESS_WEATHER); 
    Wire.onRequest(requestEvent);
    Wire.onReceive(receiveEvent);
       
    if ( ! temphumi_init() ) {
#ifdef DEBUG
Serial.println("Weather sensors not found: rebooting...");
#endif 
        resetFunc();    
    }
    
#ifdef DEBUG
Serial.print("Address: 0x");Serial.println(I2C_ADDRESS_WEATHER, HEX);
#endif

#ifdef DEBUG
    Serial.println("Waiting...");
#endif    

}

void loop()
{

}
 
