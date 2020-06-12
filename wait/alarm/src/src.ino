#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <NTPClient.h>

#define PIN_RELAY 30
#define PIN_SENSOR 32

#define MQTT_NAME   "ArduinoMEGA"
#define MQTT_SERVER "broker.sensoriando.com.br"
#define MQTT_PORT   1883
#define MQTT_USER   "fdavid"
#define MQTT_PASSWD "12345678"
#define MQTT_TOPIC  "Barreira"
#define MQTT_QOS    1         //Only 0 and 1, 2 not supported

#define ALARM_PAYLOAD_ON  "DANGER"
#define ALARM_PAYLOAD_OFF "Safe"

//#define DEBUG

//Prototypes
void callback(char*, byte*, unsigned int);
void brokerConnect();
void ethernetConnect();
void alarm_on();
void alarm_off();

//Classes
EthernetClient ethClient;
PubSubClient mqttClient(MQTT_SERVER, MQTT_PORT, callback, ethClient);

//Global Variables
byte mac[] = {0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02};
int trigged = 0;

void setup() {
  pinMode(PIN_RELAY, OUTPUT);
  pinMode(PIN_SENSOR, INPUT_PULLUP);

  alarm_off();

#ifdef DEBUG
  Serial.begin(9600);
#endif

  // start the Ethernet connection:
#ifdef DEBUG
  Serial.println("Initialize Ethernet with DHCP:");
#endif

  delay(5000); //wait to init router 
  ethernetConnect();

  //Connect Broker MQTT
#ifdef DEBUG
  Serial.println("");
  Serial.print("Connecting Broker: ");
#endif

  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  brokerConnect();

  //Waiting
#ifdef DEBUG
  Serial.println("");
  Serial.println("Waiting signals");
#endif
}

void loop() {
  int sensor = digitalRead(PIN_SENSOR);

  if (  sensor || trigged ) {    
    if ( sensor && !trigged && mqttClient.connected() ) {
      mqttClient.publish(MQTT_TOPIC, ALARM_PAYLOAD_ON);
    } else if ( sensor && !mqttClient.connected() ) {
      trigged = 1;
    }

    alarm_on();
  } else {
    alarm_off();
  }

  if ( mqttClient.connected() ) {
    mqttClient.loop();  
  } else {
    brokerConnect();
  }
}

//Functions
void ethernetConnect() {
  int EthSuccess;

  EthSuccess = Ethernet.begin(mac);

#ifdef DEBUG
  if (EthSuccess == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    } else if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
   }
  }
#endif

#ifdef DEBUG
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());
#endif

}

void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived

#ifdef DEBUG
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
#endif

  if ( strncmp(payload, ALARM_PAYLOAD_OFF, length) == 0 ) {
    trigged = 0;
  } else if ( strncmp(payload, ALARM_PAYLOAD_ON, length) == 0 ) {
    trigged = 1;
  }
}

void alarm_on() {
#ifdef DEBUG
  Serial.println("SOS");
#endif

  if ( !digitalRead(PIN_RELAY) ) {
    digitalWrite(PIN_RELAY, 1);
  }
}

void alarm_off() {
  digitalWrite(PIN_RELAY, 0);
}

void brokerConnect() {
  int result = mqttClient.connect(MQTT_NAME, MQTT_USER, MQTT_PASSWD); 

  if ( result ) {
    mqttClient.publish(MQTT_TOPIC, ALARM_PAYLOAD_OFF);
    mqttClient.subscribe(MQTT_TOPIC, MQTT_QOS);  

#ifdef DEBUG
    Serial.print("OK");
#endif
  } else {
#ifdef DEBUG
    Serial.print("FAIL");
#endif
  }
}
