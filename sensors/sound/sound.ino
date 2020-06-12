/* 
 * Read sound sensor
 * 
 * Build with IDE Arduino 1.8.12
 *
 * REQUIREMENT COMPILE
 * 1x Board Arduino
 *    Tools>>Board (Arduino Nano -> Processor 328P Old Botloader
 *      
 * Libraries (Sketch >> Include Library >> Manage Libraies)  
 * 
 */
#include <Wire.h>
#include "common.h"
#include "sound.h"

//#define DEBUG
#define SENSOR_COUNT  1

/* 
 * Global variables
 */
float value=0;
int counter=0;

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
        case NORMAL: if (mode_sensor == 0) sound();
                     break;  
    }
}

void sound() {
    char stream[I2C_CMD_NORMAL_LEN];
    int i, sum=0;
    
    for (i=0; i<=counter; i++) {
        sum=sum+i;
    }
        
    value = value / sum;
//    https://circuitdigest.com/microcontroller-projects/arduino-sound-level-measurement/
    value = (value+83.2073) / 11.003; 

    if (value < 0)
        value = 0;
        
#ifdef DEBUG
Serial.print("Data request: ");Serial.println(value);
#endif

    datastream(value, SOUND_ID, stream);
     
#ifdef DEBUG
Serial.print("Stream request: ");Serial.println(stream);
#endif

    Wire.write(stream, I2C_CMD_NORMAL_LEN);

#ifdef DEBUG
for (int dg=0; dg<I2C_CMD_NORMAL_LEN; dg++) Serial.println(stream[dg], HEX);
#endif

    counter=0;
    value=0;
}

/*
 * Setting
 */
void setup()
{
#ifdef DEBUG
    Serial.begin(9600);
#endif

    Wire.begin(I2C_ADDRESS_SOUND); 
    Wire.onRequest(requestEvent);
    Wire.onReceive(receiveEvent);
    
    if ( ! sound_init() ) {
#ifdef DEBUG
Serial.println("Sound sensor not found: rebooting...");
#endif 
        resetFunc();    
    }

#ifdef DEBUG
Serial.print("Address: 0x");Serial.println(I2C_ADDRESS_SOUND, HEX);
#endif

#ifdef DEBUG
Serial.println("Waiting...");
#endif    
}

void loop()
{ 
  value = value + (counter * sound_read());
  counter++;
}
 
