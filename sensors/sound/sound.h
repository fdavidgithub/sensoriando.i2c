/* 
 * Read sound sensor  
 * Libraries (Sketch >> Include Library >> Manage Libraies)  
 */
//#define DEBUG

#define SOUND_ID    13    //id of sound on table Sensors
#define SOUND_GPIO  A0

/* 
 * Global variables
 */

/*
 * Functions
 */
int sound_init()
{
    pinMode(SOUND_GPIO, INPUT);
    return 1;
}

int sound_read()
{
    return analogRead(SOUND_GPIO); 
}
