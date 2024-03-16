//AMALIA NAKA	3295
//ATHANASIOS KATSILIERIS  3247
//IASONAS TSATSIS  4819
#include "bench.h"


void _write_test(long int count, int r, int num)//We added the number of the threads as argument.
{
	int i;
	double cost;
	long int shared_count;//This variable is used to calculate the  count per threads.
	long long start,end;
	Variant sk, sv;

	char key[KSIZE + 1];
	char val[VSIZE + 1];
	char sbuf[1024];

	memset(key, 0, KSIZE + 1);
	memset(val, 0, VSIZE + 1);
	memset(sbuf, 0, 1024);

	start = get_ustime_sec();
	shared_count = count / num;
	for (i = 0; i < shared_count; i++) {//We use the for loop until the count per threads.
		if (r)
			_random_key(key, KSIZE);
		else
			snprintf(key, KSIZE, "key-%d", i);
		fprintf(stderr, "%d adding %s\n", i, key);
		snprintf(val, VSIZE, "val-%d", i);

		sk.length = KSIZE;
		sk.mem = key;
		sv.length = VSIZE;
		sv.mem = val;

		db_add(db, &sk, &sv);
		if ((i % 10000) == 0) {
			fprintf(stderr,"random read finished %d ops%30s\r", i, "");

			fflush(stderr);
		}
	}


	end = get_ustime_sec();
	cost = end -start;
	pthread_mutex_lock(&Wm);//We use the lock for the PUT mutex.
	total_costW = total_costW + cost;//We calculate the total cost of the PUT.
	pthread_mutex_unlock(&Wm);//We use the unlock for the PUT mutex.
}

void _read_test(long int count, int r,int num)//We added the number of the threads as argument.
{
	int i;
	int ret;
	long int shared_count;//This variable is used to calculate the  count per threads.
	int found = 0;
	double cost;
	long long start,end;
	Variant sk;
	Variant sv;

	char key[KSIZE + 1];

	start = get_ustime_sec();
	shared_count = count / num;
	for (i = 0; i < shared_count; i++) {//We use the for loop until the count per threads.
		memset(key, 0, KSIZE + 1);

		/* if you want to test random write, use the following */
		if (r)
			_random_key(key, KSIZE);
		else
			snprintf(key, KSIZE, "key-%d", i);
		fprintf(stderr, "%d searching %s\n", i, key);
		sk.length = KSIZE;
		sk.mem = key;
		ret = db_get(db, &sk, &sv);
		if (ret) {
			found++;
		} else {
			INFO("not found key#%s",
					sk.mem);
    	}

		if ((i % 10000) == 0) {
			fprintf(stderr,"random read finished %d ops%30s\r",
					i,
					"");

			fflush(stderr);
		}
	}

	end = get_ustime_sec();
	cost = end - start;

	pthread_mutex_lock(&Rm);//We use the lock for the GET mutex.
	total_costR = total_costR + cost;//We calculate the total cost of the GET.
	pthread_mutex_unlock(&Rm);//We use the unlock for the GET mutex.

}
