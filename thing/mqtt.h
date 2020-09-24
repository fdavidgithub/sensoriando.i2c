#ifndef MQTTLIB
#define MQTTLIB
/*
 * 
 */
#include <PubSubClient.h>

#ifndef WIFILIB
  #include "wifi.h"
#endif

/*
 * MACROS
 */
//#define DEBUG

#define MESSAGE_ID      1   //id of message on table Sensors
#define TIME_ID         6   //id of message on table Sensors

#define MQTT_SERVER "broker.sensoriando.com.br"
#define MQTT_UUID   "27c36465-7356-4fe7-b726-cf619a9a22f0"
#define MQTT_PORT   1883
#define MQTT_USER   "fdavid"
#define MQTT_PASSWD "12345678"

/*
 * GlobalVariables
 */
PubSubClient MqttClient(EspClient);

/*
 * functions
 */
void mqtt_reconnect() 
{
    while ( ! MqttClient.connected() ) {
#ifdef DEBUG
        Serial.print("Attempting MQTT connection...");
#endif
        if (MqttClient.connect("ESP8266 Client", MQTT_USER, MQTT_PASSWD)) {
#ifdef DEBUG
            Serial.println("MQTT Server Connected");
#endif

        } else {

#ifdef DEBUG
            Serial.print("failed, rc=");
            Serial.print(MqttClient.state());
            Serial.println(" try again in 5 seconds");
#endif      
      
            delay(5000);
        }
    }
}


void mqtt_sendvalue(DateTime dt, float value_sensor, int id_sensor) {
    char msg[256], topic[256];

    sprintf(msg, "{\"dt\":\"%04d%02d%02d%02d%02d%02d\", \"value\":%f}", \
                    dt.year(), dt.month(), dt.day(), \
                    dt.hour(), dt.minute(), dt.second(), \
                    value_sensor);   
    sprintf(topic, "%s/%d", MQTT_UUID, id_sensor);
    
    MqttClient.publish(topic, msg);
    
}

void mqtt_senddatetime(DateTime dt, int value_dt) {
    char msg[256], topic[256];
    
    sprintf(msg, "{\"dt\":\"%04d%02d%02d%02d%02d%02d\", \"value\":%d}", \
                  dt.year(), dt.month(), dt.day(), \
                  dt.hour(), dt.minute(), dt.second(), \
                  value_dt);   
    sprintf(topic, "%s/%d", MQTT_UUID, TIME_ID);

    MqttClient.publish(topic, msg);
}

void mqtt_sendmessage(DateTime dt, char *msg) {
    char topic_msg[256], topic[256];
      
    sprintf(topic_msg, "{\"dt\":\"%04d%02d%02d%02d%02d%02d\", \"value\":\"%s\"}", \
                    dt.year(), dt.month(), dt.day(), \
                    dt.hour(), dt.minute(), dt.second(), \
                    msg);   
    sprintf(topic, "%s/%d", MQTT_UUID, MESSAGE_ID);

    MqttClient.publish(topic, msg);
}



#endif
