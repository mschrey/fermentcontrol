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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>  //for strcpy
#include <time.h>

//28 00 4b 46 ff ff 0a 10 39 : crc=39 YES
//28 00 4b 46 ff ff 0a 10 39 t=20125

const char SENSOR1[50]     = "/sys/bus/w1/devices/10-000802f7cdae/w1_slave";
const char SENSOR2[50]     = "/sys/bus/w1/devices/10-000802f89e49/w1_slave";
const char SENSOR3[50]     = "/sys/bus/w1/devices/10-0008032e3d80/w1_slave";
const char SENSOR4[50]     = "/sys/bus/w1/devices/28-0316a4a4eaff/w1_slave";

const char COMMAND_ON[100]  = "/home/pi/raspberry-remote/send 11001 1 1 >> /home/pi/brewcontrol_raspberry-remote_output.log";  //outlet A
const char COMMAND_OFF[100] = "/home/pi/raspberry-remote/send 11001 1 0 >> /home/pi/brewcontrol_raspberry-remote_output.log";  //outlet A

char heaterStatus[] = "OFF";
const int HYSTERESIS = 2;

const int TIMEOUT_RAST_WAIT = 10;
const int TIMEOUT_RAST_HEATUP = 10;

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


int get_temp(const char * sensor)
{
    char temp_str1[20], temp_str2[10];
    char str_long[100];
    int temp_millicelsius;
    FILE * fh;
    fh = fopen(sensor, "r");
    if (fh != NULL) {
        fgets(str_long, 100, fh); //read first line
        fscanf(fh, "%*2x %*2x %*2x %*2x %*2x %*2x %*2x %*2x %*2x %s\n", temp_str1);
        fclose(fh);
        sprintf(temp_str2, temp_str1+2, 5);
        
        temp_millicelsius = atoi(temp_str2);
        //printf("temp    : %5.2f°C\n", (double)temp_millicelsius/1000);
        return temp_millicelsius;
    } else {
        printf("sensor %s not found\n", sensor);
        return -1000;
    }

}

long get_time(char *charpointer, time_t starttime)
{ 
    time_t rawtime;
    struct tm * timeinfo;
    
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(charpointer, 80, "%F %T", timeinfo);
    
    long seconds_since_start = difftime(rawtime, starttime);
}




int main()
{
    char status[10];
    char logfile[] = "brewcontrol_2018-10-16_pilsfuersvolk.log";
    FILE *fp;
    char time_string [80];
    int ret;
    long seconds_since_start;
    double temp_avg;
    double temp_soll = 18;
    time_t starttime;
    time(&starttime);  // get current time; same as: timer = time(NULL)  
    
    if(getFileExistence(logfile) == 0) { 
        fp = fopen(logfile,"a");
        fprintf(fp,"Welcome to BrewControl v0.4\n\n");
        fprintf(fp,"date/time, seconds-since-start, t1(at_bucket)[°C], t2 (fridge_top)[°C], t3[°C](liquid),status\n");
        fclose(fp);
    }


    int temp1 = get_temp(SENSOR1);   //10-000802f7cdae
    int temp2 = get_temp(SENSOR2);   //10-000802f89e49
    int temp3 = get_temp(SENSOR3);   //10-0008032e3d80
    //int temp3 = get_temp(SENSOR4);

    seconds_since_start = get_time(time_string, starttime);

//    if(temp2 < temp_soll) {
//        ret = system(COMMAND_OFF);
//        strcpy(status, "OFF");
//    } else {
//        ret = system(COMMAND_ON);
//        strcpy(status, "ON");
//    }

    fp=fopen(logfile,"a");       
    fprintf(fp,"%s, %5d, %5.2f, %5.2f, %5.2f, %s\n", time_string, seconds_since_start, (double)temp1/1000, (double)temp2/1000,(double)temp3/1000,status);
    fclose(fp);
}
