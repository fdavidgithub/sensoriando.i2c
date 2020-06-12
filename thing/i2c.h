#ifndef I2CLIB
/*
 * 
 */
#include <Wire.h>

/*
 * MACROS
 */
#define I2CLIB
//#define DEBUG

// ESP8266 have 2xSDA and 2xSCL (D1,D2/D4,D3), but you can use only one
#define GPIO_SDA     D1     //GPIO 5
#define GPIO_SCL     D2     //GPIO 4

/*
 * Layout Data Stream
 * byte 1:              STX   
 * byte 2:              ID
 * byte 3:              VALUE+ETX
 * byte STREAM_LENGTH:  CRC
 */ 
#define I2C_STX         0x02    //start of text
#define I2C_ETX         0x03    //end of text

#define I2C_SCAN_DELAY      10
#define I2C_SCAN_MAX        5          //Max ID in table sensors
#define I2C_CMD_INFO        0x01
#define I2C_CMD_INFO_LEN    3
#define I2C_CMD_NORMAL      0x00
#define I2C_CMD_NORMAL_LEN  16

/*
 * Structures
 */
struct DataStream {
    int id;
    float value;
    byte crc;
};

struct DataInfo {
    byte address;
    int count;
};

/*
 * GlobalVariables
 */


/*
 * functions
 */
int8_t i2c_gencrc(int8_t *stream)
{
    uint8_t crc = 0xff;
    size_t i, j, len;

    for (len=0; len<16 /*sizeof(stream)*/;len++) {
        if (stream[len] == I2C_ETX) {
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
 
int i2c_sendcommand(byte address, byte command, byte param)
{
    Wire.beginTransmission(address);
    Wire.write(I2C_STX);
    Wire.write(command);

    if (command == I2C_CMD_NORMAL) {
        Wire.write(param);  
    }
    
    Wire.write(I2C_ETX);
    
    return Wire.endTransmission();
}

int i2c_receivecommand(byte address)
{
    int count;

    Wire.requestFrom(address, I2C_CMD_NORMAL_LEN);

    if ( Wire.available() && (Wire.read() == I2C_STX) ) {   
        count = int(Wire.read());

        if ( Wire.read() != I2C_ETX ) {
            count = 0;  
        }
    }

    return count;
}

DataStream i2c_read(byte address) {
    int8_t crc;
    int8_t stream[I2C_CMD_NORMAL_LEN];
    char stream_value[sizeof(float)];
    byte i;
    DataStream data;

    Wire.requestFrom(address, I2C_CMD_NORMAL_LEN);

    i=0;  
    while( Wire.available() ){
        stream[i] = Wire.read();
        i++;
    }

#ifdef DEBUG
for (int dg=0; dg<I2C_CMD_NORMAL_LEN; dg++) Serial.printf("[%d] 0x%02x\n", dg, stream[dg]);
#endif

    data.id = 0;
    data.value = 0;
    data.crc = 0;

    for (i=1; i<I2C_CMD_NORMAL_LEN; i++) {
        if ( (stream[0] != I2C_STX) || (stream[i] == I2C_ETX) ){
            break;
        }
            
        stream_value[i-1] = stream[i];
    }

    crc = i2c_gencrc(stream);
    
    data.id = stream[1];
    data.value = atof(stream_value);
    data.crc = crc == stream[I2C_CMD_NORMAL_LEN-1];
    
#ifdef DEBUG
Serial.printf("from ID: %d with value: %f and CRC: Calc 0x%02x Rev 0x%02x\n", data.id, data.value, crc, stream[I2C_CMD_NORMAL_LEN-1]);
#endif

    return data;
}

 


#endif
