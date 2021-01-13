// **********************************************
// *                                            *
// *            FermentControl                  *
// *                                            *
// **********************************************
// 2017-03-21
// Version 0.1: Initial
// Version 0.2: Improved output data format
// Version 0.333: three sensors, output directly to file
// Version 0.4: * No crash when sensors are missing (temporary unconnected), 
//              * append to logfile if it exists already
//              * single temp logging per program start (for autostart and use with cron)
// Version 0.5 Commented Output Switching
// Version 0.6 Discovery and Labels of One-Wire Sensors introduced
// Version 0.7 Output Switching moved back in - adapted to new sensor list concept

#include <stdio.h>
#include <stdlib.h>
#include <string.h>  //for strcpy
#include <time.h>
#include "onewirediscover.h"

//28 00 4b 46 ff ff 0a 10 39 : crc=39 YES
//28 00 4b 46 ff ff 0a 10 39 t=20125

const char COMMAND_ON[100]  = "/home/pi/raspberry-remote/send 11001 1 1 >> /home/pi/brewcontrol_raspberry-remote_output.log";  //outlet A
const char COMMAND_OFF[100] = "/home/pi/raspberry-remote/send 11001 1 0 >> /home/pi/brewcontrol_raspberry-remote_output.log";  //outlet A


//checks if a file exists. 
// it returns
// * 0 if file cannot be read (non-existent or no reading permissions)
// * 1 if file can be read but not written
// * 2 if file can be read and written
int getFileExistence(char * fileName)
{
    FILE * fp;
    int retval = -1;
    fp = fopen(fileName, "r");
    if(fp == NULL) {
        retval = 0;      //reading not possible or file doesn't exist
    } 
    else {
        fclose(fp);      //close valid read pointer
        fp = fopen(fileName, "a");
        if(fp == NULL) {
            retval = 1;   //only reading possible
        } else {
            retval = 2;   //writing possible
            fclose(fp);   //close valid write pointer
        }
    }
    return retval;
}



long get_time(char *charpointer, time_t starttime)
{ 
    time_t rawtime;
    struct tm * timeinfo;
    
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(charpointer, 80, "%F %T", timeinfo);
    
    long seconds_since_start = difftime(rawtime, starttime);
    return seconds_since_start;
}



int main()
{
    char logfile[] = "/home/pi/fermentcontrol/fermentcontrol_2020-08-30_Trappist_2020.log";
    char status[10];
    FILE *fp;
    char time_string [80];
    long seconds_since_start;
    const double TEMP_SOLL = 24;
    time_t starttime;
    time(&starttime);  // get current time; same as: timer = time(NULL)  

    struct tempsensorlist *mytempsensorlist = NULL;
    mytempsensorlist = get_temp_sensor(mytempsensorlist, "/sys/bus/w1/devices");   
    struct tempsensorlist *current;
    if(mytempsensorlist == NULL) {
        printf("Error! No temp sensor found\n");
        exit(-1);
    }    
    get_temp_sensor_labels(mytempsensorlist);
    
    print_tempsensorlist(mytempsensorlist);
       
    if(getFileExistence(logfile) == 0) { 
        fp = fopen(logfile,"a");
        fprintf(fp,"Welcome to BrewControl v0.5\n\n");
        fprintf(fp,"date/time, seconds-since-start");
        current = mytempsensorlist;
        while(current != NULL) {
            fprintf(fp, ", %s", current->label);
            current = current->next;
        }
        fprintf(fp, "\n");        
        fclose(fp);
    }

    get_all_temps(mytempsensorlist);

    seconds_since_start = get_time(time_string, starttime);
    
    current = mytempsensorlist;
    //find correct temp sensor in list
    while(strcmp(current->label, "1") != 0) {
        current = current->next;
    }
    if ((double)current->temp_mc/1000 > TEMP_SOLL) {
        system(COMMAND_OFF);
        strcpy(status, "OFF");
    } else {
        system(COMMAND_ON);
        strcpy(status, " ON");
    }

    fp=fopen(logfile,"a");       
    fprintf(fp,"%s, %5ld", time_string, seconds_since_start);
    current = mytempsensorlist;
    while(current != NULL) {
        fprintf(fp, ", %5.2f", (double)current->temp_mc/1000);
        current = current->next;
    }
    fprintf(fp, ", %s\n", status);
    fclose(fp);
    
    return 0;
}
