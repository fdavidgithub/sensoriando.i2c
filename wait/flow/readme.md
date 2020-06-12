# Read flow sensor and publish in MQTT server
Francis David (www.fdavid.com.br) 


  Build with IDE Arduino 1.8.4
 
  SCHEME LED RGB
  GPIO      Function    On
  1        Green       GPIO
  2        Ground      GND           
  3        Blue        GPIO
  4        Red         GPIO
  
  SCHEME FLOW SENSOR
  GPIO      Function    On
  Red      VCC         5v
  Brown    Ground      GND           
  Yellow   Signal      GPIO
 
  SCHEME PUSH BUTTON
  GPIO      Function    On
  1                    GPIO
  2              GND           
  3              GPIO
  4              GPIO
 
# Requirements
## Hardware 
* Nodemcu ESP8266
* Water flow sensor
* Conversor level logic
* Suppy power 5 volts

## Libraries
   Libraries (Sketch >> Include Library >> Manage Libraies)
       PubSubClient (MQTT Client)  
       WifiManager (Wifi ESSID)
   Server MQTT
 

