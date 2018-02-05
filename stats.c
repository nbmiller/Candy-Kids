
#include "stats.h"
#include <stdio.h>
#include <stdlib.h>

int totalFactories;
double *totalDelay;
double *consumedMaxTime;
int *numProduced;
int *numConsumed;
double *currentMinimum;
double *currentMaximum;

/**
** Initialize Statistics
**
*/
void stats_init(int num_producers)
{
    currentMinimum = (double*)malloc(sizeof(double)*num_producers);
	currentMaximum = (double*)malloc(sizeof(double)*num_producers);
	totalDelay = (double*)malloc(sizeof(double)*num_producers);
	numConsumed = (int*)malloc(sizeof(int)*num_producers);
	numProduced = (int*)malloc(sizeof(int)*num_producers);
	totalFactories = num_producers;

	for (int i = 0; i < num_producers; i++){
		totalDelay[i] = 0;
		currentMaximum[i] = 0;
		currentMinimum[i] = 999999999999;  //make higher than any possible minimum
        numProduced[i] = 0;
		numConsumed[i] = 0;
	}
}

/**
** Record Statistics
**
*/
void stats_record_consumed(int factory_number, double delay_in_ms)
{
	numConsumed[factory_number] += 1;
	if (currentMinimum[factory_number] >= delay_in_ms){  //if current < min
		currentMinimum[factory_number] = delay_in_ms;
	}

	if (currentMaximum[factory_number] <= delay_in_ms){ //if current > max
		currentMaximum[factory_number] = delay_in_ms;
	}

	totalDelay[factory_number] += delay_in_ms;
}

/**
** Increment Number Produced for factory
**
*/
void stats_record_produced(int factory_number)
{
	numProduced[factory_number] += 1;
}


/**
** Display Statistics
**
*/
void stats_display()
{
	printf("Statistics:\n");
	printf("Factory#    #Made    #Eaten    Min_Delay[ms]    Avg Delay[ms]    Max Delay[ms]\n");
	for(int i = 0; i < totalFactories; i++)
	{
		printf("%5d%9d%12d%17f%17f%17f\n", i, numProduced[i], numConsumed[i], currentMinimum[i], totalDelay[i]/numConsumed[i], currentMaximum[i]);
		if (numConsumed[i] != numProduced[i])
		{
			printf("ERROR: Mismatch between number made and eaten.\n");
			//exit(1);
		}
	}
}


/**
** Free Memory
**
*/
void stats_cleanup()
{
	free(numProduced);
	free(numConsumed);
	free(currentMinimum);
	free(totalDelay);
	free(currentMaximum);
}
