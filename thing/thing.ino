/* 
 * Read sound sensor and publish in MQTT server
 * 
 * Build with IDE Arduino 1.8.12
 * Addicional Board Manager (File >> Preferences)
 * https://dl.espressif.com/dl/package_esp32_index.json
 * http://arduino.esp8266.com/stable/package_esp8266com_index.json
 *
 * REQUIREMENT COMPILE
 * 1x Board NodeMCU
 *    Tools>>Board>>Board Manager (esp8266 by ESP8266 Community v2.7.1)
 *    Tools>>Board (NodeMCU 1.0)
 *      
 * Libraries (Sketch >> Include Library >> Manage Libraies)  
 *    ESP_WifiManager   by tzapu              v0.15.0  
 *    RTCLib            by Adafruit           v1.8.0
 *    NTPClient         by Fabrice Weinberg   v3.2.0
 *    PubSubClient      by Nick O' Leary      v2.7.0
 *    Nanoshield_RTC    by Circuitar          v
 *
 */
#include <ESP8266WiFi.h> 
#include <Wire.h>

#include "gpio.h"
#include "rtc.h"
#include "ntp.h"
#include "wifi.h"
#include "messages.h"
#include "mqtt.h"
#include "i2c.h"
#include "sd.h"

/*
 * MACROS
 */
#define DEBUG

#define THING_UPDATE    1000    //Update server in miliseconds
#define THING_RESET     5000
#define THING_DEBOUNCE  1000

/* 
 * GlobalVariables
 */
long update_elapsedtime;
DataInfo sensors[I2C_SCAN_MAX];


/*
 * functions
 */
void logthing(char *msg) {
#ifdef DEBUG
    Serial.println(msg);
    delay(1000);
#endif
}


void mode_error() {
/*   digitalWrite(GPIO_RED, 255);
   digitalWrite(GPIO_GREEN, 0);
   digitalWrite(GPIO_BLUE, 0);
*/}

void mode_config()
{
/*    digitalWrite(GPIO_RED, 0);
    digitalWrite(GPIO_GREEN, 0);
    digitalWrite(GPIO_BLUE, 255);
*/}

void mode_normal()
{
/*    digitalWrite(GPIO_RED, 0);
    digitalWrite(GPIO_GREEN, 0);
    digitalWrite(GPIO_BLUE, 0);
*/}

void mode_send()
{
/*    digitalWrite(GPIO_RED, 0);
    digitalWrite(GPIO_GREEN, 255);
    digitalWrite(GPIO_BLUE, 0);
*/} 

          
/*
 * Setting
 */
void setup()
{
    char namedevice[25];
    char logmsg[256];
    DateTime dt_ntp;
    DateTime dt_rtc;
    byte i2c_address;
    int device;
    
#ifdef DEBUG
    Serial.begin(9600);
#endif

    Wire.begin(GPIO_SDA, GPIO_SCL);

    /*
     * GPIO
     */
#ifdef GPIO_RED && GPIO_GREEN && GPIO_BLUE
    pinMode(GPIO_RED, OUTPUT);
    pinMode(GPIO_GREEN, OUTPUT);
    pinMode(GPIO_BLUE, OUTPUT);
#endif

#ifdef GPIO_RESET
    pinMode(GPIO_RESET, INPUT);
#endif

    /* 
     * Inits 
     */
    mode_config();

    // Wifi
    sprintf(namedevice, "SensoriandoAP%d", 0);  
    WifiManager.setTimeout(180);    
    WifiManager.autoConnect(namedevice);
  
    // Broker MQTT
    MqttClient.setServer(MQTT_SERVER, MQTT_PORT);
   
    // NTP
    dt_ntp = ntp_get();
    
    // RTC
    if ( ! rtc_init() ) {
        mode_error();
        logthing(RTC_FAIL);
        ESP.reset();
    } else {
        rtc_sync(dt_ntp, dt_rtc);
    }

    // Scanning sensors
    logthing(I2C_SCAN);

    device = 0;
    for (i2c_address = 1; i2c_address <= I2C_SCAN_MAX; i2c_address++) {
        if ( i2c_sendcommand(i2c_address, I2C_CMD_INFO, NULL) == 0 /* success */) {
            sprintf(logmsg, "%s: 0x%02x\n", I2C_DEVICEFOUND, i2c_address);
            logthing(logmsg);
            
            sensors[device].address = i2c_address;
            sensors[device].count = i2c_receivecommand(i2c_address);
            device++;
        } else {
            sprintf(logmsg, "%s: 0x%02x\n", I2C_DEVICENOTFOUND, i2c_address);
            logthing(logmsg);

            sensors[device].address = 0x00;
            sensors[device].count = 0;
        }       

        delay(I2C_SCAN_DELAY);
    }
   
    device=0;
    while ( sensors[device].address != 0 ) {
        sprintf(logmsg, "[%d] %s: 0x%02x #%d\n", device, I2C_DEVICEATTACH, sensors[device].address, sensors[device].count);
        logthing(logmsg);
        device++;
    }
 
    // TIMER
    update_elapsedtime = millis();

    if (device) {
      logthing(WAIT_READ);
    } else {
      logthing(WAIT_SENSOR);
    }
}
 
void loop()
{
    char error[256];
    DataStream data;
    DateTime dt;
    long reset_elapsedtime=0;
    byte i,j;
    int cmd;

    /*
     * Check if is necessary new ESSID
     */
#ifdef GPIO_RESET
    if ( digitalRead(GPIO_RESET) ) {
        mode_error();
        
        while ( reset_elapsedtime < THING_RESET && digitalRead(GPIO_RESET) ) {
            reset_elapsedtime++;
            delay(1);

#ifdef DEBUG
Serial.println(reset_elapsedtime);
#endif
        }

        if ( reset_elapsedtime >= THING_RESET && digitalRead(GPIO_RESET) ) {
            logthing(WIFI_RESET);
            
            WifiManager.resetSettings();
            delay(THING_DEBOUNCE);
            ESP.reset();
        } else {
            mode_normal();
        }
    }
#endif

    if ( abs(millis() - update_elapsedtime) >= THING_UPDATE ) {
        update_elapsedtime = millis();

        /*
         * Server MQTT
         */       
        if ( ! MqttClient.connected()) {
            mqtt_reconnect();
        }

        /*
         * Send value of sensor
         */
        mode_send();
 
        dt = rtc_get(); 
        mqtt_senddatetime(dt, dt.unixtime());

#ifdef DEBUG
Serial.println("Payload[datetime] sent!");
#endif

        i = 0;
        while ( sensors[i].address != 0 ) {           
#ifdef DEBUG
Serial.printf("Reading Address: 0x%02x\n", sensors[i].address);
Serial.printf("with %d sensors\n", sensors[i].count);
#endif

            for (j=0;j<sensors[i].count;j++) {
                cmd = i2c_sendcommand(sensors[i].address, I2C_CMD_NORMAL, j);

#ifdef DEBUG
Serial.print("Command sent:");Serial.println(cmd);
#endif

                if (cmd == 0 /* success */ ) {
                    data = i2c_read(sensors[i].address);
        
                    if ( data.crc ) {
                        mqtt_sendvalue(dt, data.value, data.id);
#ifdef DEBUG
Serial.println("Payload[value] sent!");
#endif
                    } else {
                        sprintf(error, "[id %d] %s", data.id, MQTT_CRC);
                        mqtt_sendmessage(dt, error);
#ifdef DEBUG
Serial.println("Payload[Error CRC] sent!");
#endif
                    }
                } else {
                        sprintf(error, "[0x%02x] %s", sensors[i], MQTT_I2CFAIL);
                        mqtt_sendmessage(dt, error);
#ifdef DEBUG
Serial.println("Payload[Error I2C] sent!");
#endif
                }
            }

            i++;
        }
            
        /* 
         * Done
         */
        mode_normal();
   }
}
