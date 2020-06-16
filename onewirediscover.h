#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>


struct tempsensorlist {
    char * tempsensor;
    char * label;
    int temp_mc;
    struct tempsensorlist *next;
};


struct tempsensorlist * create(char *path, char *tempsensor);

void push(struct tempsensorlist *head, char *path, char *tempsensor);

void print_tempsensorlist(struct tempsensorlist *head);

struct tempsensorlist* get_temp_sensor(struct tempsensorlist *head, char *path);

int get_temp(const char * sensor);

void get_all_temps(struct tempsensorlist *list);

void get_temp_sensor_labels(struct tempsensorlist *list);


