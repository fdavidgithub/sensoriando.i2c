/*
 * I2C config
 * Address: the 7-bit address of the device to transmit
 * GPIO: Arduino Nano SDA A4 / SCL A5 
 *
 * Reserved Address
 * 0x68     RTC
 */

/*
 * MACROS
 */
//#define DEBUG

#define I2C_CMD_NORMAL_LEN  16
#define I2C_ADDRESS_SOUND   0x01
#define I2C_ADDRESS_WEATHER 0x02

/*
 * Layout Data Stream
 * byte 1:              STX   
 * byte 2:              ID
 * byte 3:              VALUE+ETX
 * byte STREAM_LENGTH:  CRC
 */ 
#define STX                             0x02    //start of text
#define ETX                             0x03    //end of text

/*
 * GlobalVariables
 */
enum modes {NORMAL, INFO}; 
int mode_slave = NORMAL;
int mode_sensor;
 
/*
 * functions
 */
void(* resetFunc) (void) = 0; //declare reset function @ address 0

int8_t gencrc(int8_t *stream)
{
    uint8_t crc = 0xff;
    size_t i, j, len;

    for (len=0; len<16 /*sizeof(stream)*/;len++) {
        if (stream[len] == ETX) {
            break;
        }
    }
    
    for (i = 0; i < len; i++) {
        crc ^= stream[i];
        
        for (j = 0; j < 8; j++) {
            if ((crc & 0x80) != 0)
                crc = (uint8_t)((crc << 1) ^ 0x31);
            else
                crc <<= 1;
        }
    }
    
    return crc;
}


void datastream(float value, int id, char *stream) 
{
    String stream_value = String(value);
    byte i;

    stream[0] = STX;
    stream[1] = id;

    i=2;
    while ( stream_value[i-2] != '\0' ) {
        stream[i] = stream_value[i-2];
        i++;
    }

    stream[i] = ETX;
    
    i++;
    while ( i<I2C_CMD_NORMAL_LEN-1 ) {
        stream[i] = NULL;
        i++;
    }

    stream[i] = gencrc(stream);          
}

void receiveEvent() {
    int cmd;
  
    if ( Wire.available() && (Wire.read() == STX) ) {        
        cmd = int(Wire.read());

        switch ( cmd ) {
            case NORMAL:  mode_sensor = int(Wire.read());
                          if (Wire.read() == ETX) {
                              mode_slave = NORMAL;
                          }
                          break;  
            case INFO: if (Wire.read() == ETX) { 
                          mode_slave = INFO;
                       }
                       break;
            default: mode_sensor = 0;
                     mode_slave = NORMAL;
                     break;
        }        
    }    

#ifdef DEBUG
Serial.print(mode_slave);Serial.print("  /  ");Serial.println(mode_sensor);
#endif
}
