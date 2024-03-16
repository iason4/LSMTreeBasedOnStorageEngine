//AMALIA NAKA	3295
//ATHANASIOS KATSILIERIS  3247
//IASONAS TSATSIS  4819
#include "bench.h"


void _random_key(char *key,int length) {
	int i;
	char salt[36]= "abcdefghijklmnopqrstuvwxyz0123456789";

	for (i = 0; i < length; i++)
		key[i] = salt[rand() % 36];
}

void _print_header(int count)
{
	double index_size = (double)((double)(KSIZE + 8 + 1) * count) / 1048576.0;
	double data_size = (double)((double)(VSIZE + 4) * count) / 1048576.0;

	printf("Keys:\t\t%d bytes each\n",
			KSIZE);
	printf("Values: \t%d bytes each\n",
			VSIZE);
	printf("Entries:\t%d\n",
			count);
	printf("IndexSize:\t%.1f MB (estimated)\n",
			index_size);
	printf("DataSize:\t%.1f MB (estimated)\n",
			data_size);

	printf(LINE1);
}

void _print_environment()
{
	time_t now = time(NULL);

	printf("Date:\t\t%s",
			(char*)ctime(&now));

	int num_cpus = 0;
	char cpu_type[256] = {0};
	char cache_size[256] = {0};

	FILE* cpuinfo = fopen("/proc/cpuinfo", "r");
	if (cpuinfo) {
		char line[1024] = {0};
		while (fgets(line, sizeof(line), cpuinfo) != NULL) {
			const char* sep = strchr(line, ':');
			if (sep == NULL || strlen(sep) < 10)
				continue;

			char key[1024] = {0};
			char val[1024] = {0};
			strncpy(key, line, sep-1-line);
			strncpy(val, sep+1, strlen(sep)-1);
			if (strcmp("model name", key) == 0) {
				num_cpus++;
				strcpy(cpu_type, val);
			}
			else if (strcmp("cache size", key) == 0)
				strncpy(cache_size, val + 1, strlen(val) - 1);
		}

		fclose(cpuinfo);
		printf("CPU:\t\t%d * %s",
				num_cpus,
				cpu_type);

		printf("CPUCache:\t%s\n",
				cache_size);
	}
}

void *threadWrite(void *args){	//This function is used by pthread_create to execute the PUT function.

	struct forArgs *t = (struct forArgs *) args;
	_write_test(t -> str_count, t -> str_r, t -> str_num);	//This function is called from kiwi.c

	return 0;
}

void *threadRead(void *args){	//This function is used by pthread_create to execute the GET function.

	struct forArgs *t = (struct forArgs *) args;
	_read_test(t -> str_count, t -> str_r, t -> str_num);	//This function is also called from kiwi.c

	return 0;
}


void createThreadsW(int r,int given_threads, long int count){//This function is used to create threads for the PUT function.
	int i;
	s.str_count = count;
	s.str_r = r;
	s.str_num = given_threads;//This is the number of the threads
	for (i = 0; i < given_threads; i++){
		pthread_create(&needles[i],NULL,threadWrite,(void *) &s);
	}
	for (i = 0; i < given_threads; i++){
		pthread_join(needles[i],NULL);
	}
}

void createThreadsR(int r,int given_threads, long int count){//This function is used to create threads for the GET function.
	int i;
	s.str_count = count;
	s.str_r = r;
	s.str_num = given_threads;
	for (i = 0; i < given_threads; i++){
		pthread_create(&needles[i],NULL,threadRead,(void *) &s);
	}
	for (i = 0; i < given_threads; i++){
		pthread_join(needles[i],NULL);
	}
}

void printStatsW(long int count){//This  function is used in order to print the statistics of the PUT function. 
	s.str_count = count;
	printf(LINE);
		printf("|Random-Write	(done:%ld): %.6f sec/op; %.1f writes/sec(estimated); cost:%.3f(sec);\n"
		,s.str_count, (double)(total_costW / s.str_count)
		,(double)(s.str_count / total_costW)
		,total_costW);
}

void printStatsR(long int count){//This  function is used in order to print the statistics of the GET function. 
	s.str_count = count;
	printf(LINE);
		printf("|Random-Read	(done:%ld): %.6f sec/op; %.1f reads/sec(estimated); cost:%.3f(sec);\n"
		,s.str_count, (double)(total_costR / s.str_count)
		,(double)(s.str_count / total_costR)
		,total_costR);
}

int main(int argc,char** argv)
{

	long int count;
	int given_threads;//This is the number of the threads
	int threads_to_write;//For the 5th question ,this is the threads of the PUT.
	int threads_to_read;//For the 5th question ,this is the threads of the GET.
	int write_percent;//For the 5th question ,this is the percentage of the PUT.
	int read_percent;//For the 5th question ,this is the percentage of the GET.
	long int count_Write;//For the 5th question ,this is the count which corrensponds of the PUT function.
	long int count_Read;//For the 5th question ,this is the count which corrensponds of the GET function.
	srand(time(NULL));
	if (argc < 3) {
		fprintf(stderr,"Usage: kiwi-bench <write | read> <count> <given_threads>\n");
		exit(1);
	}
	//We start here the 3rd question
	if (strcmp(argv[1], "write") == 0) {
		int r = 0;

		count = atoi(argv[2]);

		_print_header(count);
		_print_environment();

		given_threads = atoi(argv[3]);//We ask from the user to give the number of threads.
		
		db = db_open(DATAS);//We open the database here.

		if (argc == 5){//If the 5th argument exists then we consider that the keys are random.
			if(strcmp(argv[4], "on") == 0){//If the 5th argument is "on" then the random keys are activated.
				r = 1;
			}else{
				fprintf(stderr,"To use random keys usage is: kiwi-bench <write | read> <count> <given_threads> on\n");//We print this message if the 5th argument is not "on". 
				exit(1);
			}
		}

		createThreadsW(r,given_threads,count);//We call the pthread_create function.

		db_close(db);//We close the database.

		printStatsW(count);//The print function of PUT is called.
	} else if (strcmp(argv[1], "read") == 0) {
		int r = 0;

		count = atoi(argv[2]);

		_print_header(count);
		_print_environment();

		given_threads = atoi(argv[3]);

		db = db_open(DATAS);

		if (argc == 5){
			if(strcmp(argv[4], "on") == 0){
				r = 1;
			}else{
				fprintf(stderr,"To use random keys usage is: kiwi-bench <write | read> <count> <given_threads> on\n");
				exit(1);
			}
		}
		createThreadsR(r,given_threads,count);

		db_close(db);
		printStatsR(count);
	//The "readwrite" case is used for the 5th question
	} else if(strcmp(argv[1], "readwrite") == 0){//We go in this case if the 2nd argument is "readwrite".
		int r = 0;
		
		if(argc < 6){//If the number of the arguments is lower than 6 the program exits
			fprintf(stderr,"Usage:kiwi-bench <readwrite> <count> <given_threads> <write_percent> <read_percent>\n");
			exit(1);
		}
		
		
		count = atoi(argv[2]);
			
		_print_header(count);
		_print_environment();
		
		given_threads = atoi(argv[3]);
		
		write_percent = atoi(argv[4]);//We ask from the user to give the PUT percentage
			
		read_percent = atoi(argv[5]);//We ask from the user to give the GET percentage
		
		threads_to_write = (int) given_threads * write_percent / 100; //Here we calculate the number of the PUT threads based on the PUT percentage.
		
		threads_to_read = (int) given_threads * read_percent / 100;//Here we calculate the number of the GET threads based on the GET percentage.
		
		count_Write = (long) count * write_percent/100;//Here we calculate the count of the PUT  based on the PUT percentage.
		
		count_Read = (long) count * read_percent/100;//Here we calculate the count of the GET  based on the GET percentage.
		
		if(write_percent + read_percent != 100){//In this case we check  the percentage is equal to 100.
			fprintf(stderr,"The sum of write and read percentage must be equal to 100\n");//In case the user gives wrong percentages this message appears
			exit(1);
		}
		
		db = db_open(DATAS);
			
		if (argc == 7){//In this specific case because of the percentage arguments the random keys take the next argument.
			if(strcmp(argv[6], "on") == 0){
				r = 1;
			}else{
				fprintf(stderr,"To use random keys usage is: kiwi-bench <readwrite> <count> <given_threads> <write_percent> <read_percent> on\n");
				exit(1);
			}
		}
		
		createThreadsR(r,threads_to_read,count_Read);
		createThreadsW(r,threads_to_write,count_Write);
		
		db_close(db);
		printStatsW(count_Write);
		printStatsR(count_Read);
	}else {
		fprintf(stderr,"Usage: kiwi-bench <write | read> <count> <given_threads>\n");
		exit(1);
	}

	return 1;
}
