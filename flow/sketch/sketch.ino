/* Read flow sensor and publish in MQTT server
 * Francis David (www.fdavid.com.br) 
 * 
 * Build with IDE Arduino 1.8.4
 *
 * SCHEME LED RGB
 * GPIO      Function    On
 * 1        Green       GPIO
 * 2        Ground      GND           
 * 3        Blue        GPIO
 * 4        Red         GPIO
 * 
 * SCHEME FLOW SENSOR
 * GPIO      Function    On
 * Red      VCC         5v
 * Brown    Ground      GND           
 * Yellow   Signal      GPIO
 *
 * SCHEME PUSH BUTTON
 * GPIO      Function    On
 * 1                    GPIO
 * 2              GND           
 * 3              GPIO
 * 4              GPIO
 *
 * REQUIREMENT
 *  Board ESP8266
 *  Libraries (Sketch >> Include Library >> Manage Libraies)
 *      PubSubClient (MQTT Client)  
 *      WifiManager (Wifi ESSID)
 *  Server MQTT
 */
#include <ESP8266WiFi.h>    
#include <PubSubClient.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>  

#include <RTClib.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

//#define DEBUG
//#define RESET_OFF

#define GPIO_RESET   16
#define GPIO_RED     12      
#define GPIO_GREEN   13      
#define GPIO_BLUE    15          
#define GPIO_SENSOR  4    //except GPIO 16

#define UPD_SENSOR  1000  //Update server in miliseconds
#define DEBOUNCE    1000
#define ELAPSEDTIME 1000

#define MQTT_SERVER "broker.sensoriando.com.br"
#define MQTT_PORT   1883
#define MQTT_TOPIC  "Ufoa0gopXZsu53xo2cCSf6RVJVZfNZUZjaDcxaHsEZRIJQ5faEOJruDbZhOVMHP7"

#define LITERS(PULSE)   (PULSE / (60 * 4.5))

WiFiClient espClient;
PubSubClient mqtt_client(espClient);
WiFiManager wifiManager;

/* 
 * Global variables
 */
volatile int pulseflow=0;

/*
 * Counter pulse sensor
 */
void SensorInterrupt()
{
    pulseflow++;
}

/*
 * Indicator mode operation
 */
void mode_error()
{
   digitalWrite(GPIO_RED, 255);
   digitalWrite(GPIO_GREEN, 0);
   digitalWrite(GPIO_BLUE, 0);
}

void mode_config()
{
    digitalWrite(GPIO_RED, 0);
    digitalWrite(GPIO_GREEN, 0);
    digitalWrite(GPIO_BLUE, 255);
}

void mode_normal()
{
    digitalWrite(GPIO_RED, 0);
    digitalWrite(GPIO_GREEN, 0);
    digitalWrite(GPIO_BLUE, 0);
}

void mode_send()
{
    digitalWrite(GPIO_RED, 0);
    digitalWrite(GPIO_GREEN, 255);
    digitalWrite(GPIO_BLUE, 0);
} 

void reconnect() 
{
    while (!mqtt_client.connected()) {
#ifdef DEBUG
        Serial.print("Attempting MQTT connection...");
#endif
        if (mqtt_client.connect("ESP8266 Client")) {
#ifdef DEBUG
            Serial.println("MQTT Server Connected");
#endif
            mode_normal();
        } else {
            mode_error();

#ifdef DEBUG
            Serial.print("failed, rc=");
            Serial.print(mqtt_client.state());
            Serial.println(" try again in 5 seconds");
#endif      
      
            delay(5000);
        }
    }
}

DateTime time_ntp() 
{
  int16_t utc = -3; //UTC -3:00 Brazil;
  WiFiUDP ntpUDP;
  DateTime ntp = NULL;

  NTPClient timeClient(ntpUDP, "pool.ntp.br", utc*3600, 60000);

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

void setup()
{
    char namedevice[25];
    Serial.begin(9600);

    mode_config();

    pinMode(GPIO_RED, OUTPUT);
    pinMode(GPIO_GREEN, OUTPUT);
    pinMode(GPIO_BLUE, OUTPUT);

#ifndef RESET_OFF
    pinMode(GPIO_RESET, INPUT);
#endif

    sprintf(namedevice, "SensoriandoAP%d", 0);  
    wifiManager.setTimeout(180);    
    wifiManager.autoConnect(namedevice);
    
    mqtt_client.setServer(MQTT_SERVER, MQTT_PORT);
    attachInterrupt(GPIO_SENSOR, SensorInterrupt, RISING);

    mode_normal();
}
 
void loop()
{
    char msg[256];
    int mycounter=0;
    float volume=0;
    int elapsedtime=0;
    DateTime ntp;

    /*
     * Check if is necessary new ESSID
     */
#ifndef RESET_OFF
    if ( digitalRead(GPIO_RESET) ) {
        mode_error();
        
        while ( elapsedtime < ELAPSEDTIME && digitalRead(GPIO_RESET) ) {
            elapsedtime++;
            delay(1);
#ifdef DEBUG
            Serial.println(elapsedtime);
#endif
        }

        if ( elapsedtime >= ELAPSEDTIME && digitalRead(GPIO_RESET) ) {
          wifiManager.resetSettings();
#ifdef DEBUG          
          Serial.println("ESP Reseted");
#endif          
          delay(DEBOUNCE);
          ESP.reset();
        } else {
          mode_normal();
        }
    }
#endif

    /*
     * Flow Sensor
     */
    while ( pulseflow > 0 ) {
        mycounter = mycounter + pulseflow;
        pulseflow = 0;

        mode_send();
        volume = LITERS(mycounter);

#ifdef DEBUG
        Serial.printf("\nPulses count: %d or %.2f liters.", mycounter, volume);
#endif

        delay(UPD_SENSOR);
    }

    /*
     * Server MQTT
     */
    if  ( volume > 0 ) {
        if (!mqtt_client.connected()) {
            reconnect();
        }

        ntp = time_ntp();
        sprintf(msg, "{\"dt\":\"%04d%02d%02d%02d%02d%02d\", \"value\":%f}", \
                      ntp.year(), ntp.month(), ntp.day(), \
                      ntp.hour(), ntp.minute(), ntp.second(), \
                      volume);
         
        mqtt_client.publish(MQTT_TOPIC, msg);
        mode_normal();
    }
   
}

