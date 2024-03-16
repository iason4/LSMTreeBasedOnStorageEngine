//AMALIA NAKA	3295
//ATHANASIOS KATSILIERIS  3247
//IASONAS TSATSIS  4819

//We tried to put all the global variables in bench.h
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>
#include "../engine/db.h"
#include "../engine/variant.h"
#define DATAS ("testdb")

#define KSIZE (16)
#define VSIZE (1000)

#define LINE "+-----------------------------+----------------+------------------------------+-------------------+\n"
#define LINE1 "---------------------------------------------------------------------------------------------------\n"

long long get_ustime_sec(void);
void _random_key(char *key,int length);

DB* db;//We took this from the kiwi.c

double total_costW;//This is the total cost for the PUT function. 
double total_costR;//This is the total cost for the GET function.

pthread_t needles[4298];//This is the total number of the ids that can be created

pthread_mutex_t Wm;//This IS the mutex for the PUT.
pthread_mutex_t Rm;//This IS the mutex for the GET.

struct forArgs {//We use this struct to pass multiple arguments.
	long int str_count;//This is the count argument.
	int str_r;//This is the for the random .
	int str_num;//This is the threads argument.
};

struct forArgs s;
