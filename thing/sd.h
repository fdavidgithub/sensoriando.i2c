#ifndef SDLIB
#define SDLIB
/*
 * 
 */
#include <SPI.h>
#include <SD.h>

#include "gpio.h"
#include "messages.h"

/*
 * MACROS
 */
//#define DEBUG

#define MESSAGE_LOG   "filelog.txt"
#define DATA_LOG      "datalog.csv"

/*
 * GlobalVariables
 */
//Sd2Card SdCard;
//SdVolume SdVolume;

/*    
 * Prototypes
 */
int sd_init ();
void sd_writemsg(char *);
void sd_writedata(char *);
float sd_freespace();
int checkspace();
 
/*
 * functions
 */
int sd_init () { 
  int res=1;
  File file;
  char datalayout[] = "data;sensor;value";
/*
#ifdef DEBUG
Serial.println("Initializing SD card...");
#endif

  //Check card
  
  res = SD.begin(GPIO_SD);    

  if ( res ) {         
          //Check message file or create new
          while ( ! SD.exists(MESSAGE_LOG) ) {
#ifdef DEBUG
Serial.println("Don't exists message file");
#endif                
              file = SD.open(MESSAGE_LOG, FILE_WRITE);
              file.close();
          }     

          //Check data file or create new
          while ( ! SD.exists(DATA_LOG) ) {
#ifdef DEBUG
Serial.println("Don't exists data file");
#endif                
              file = SD.open(DATA_LOG, FILE_WRITE);
              file.write(datalayout);
              file.close();
          }  
  }
*/    
  return res;
}

void sd_writemsg(char *msg) {
/*
    File file;

    if ( checkspace() ) { 
        file = SD.open(MESSAGE_LOG, FILE_WRITE);
        file.write(msg);
        file.close();    
    } else {
        SD.remove(DATA_LOG);  
    }
*/
}

void sd_writedata(char *msg) {
/*
    File file;

    if ( checkspace() ) { 
        file = SD.open(DATA_LOG, FILE_WRITE);
        file.write(msg);
        file.close();    
    } else {
        SD.remove(DATA_LOG);  
    }
*/
}

int checkspace() {
  float volumesize = 0;
//  volumesize = sd_freespace();
  return volumesize >= 1024;
}

float sd_freespace() {
    uint32_t volumesize=0;
/*
    volumesize = SdVolume.blocksPerCluster();     // clusters are collections of blocks
    volumesize *= SdVolume.clusterCount();        // we'll have a lot of clusters
    volumesize /= 2;                              // SD card blocks are always 512 bytes (2 blocks are 1KB)
    //volumesize /= 1024;                           //Megabytes

#ifdef DEBUG
Serial.println(volumesize);
#endif
*/
    return volumesize;  
}

#endif
