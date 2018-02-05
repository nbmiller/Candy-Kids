#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <inttypes.h>
#include <signal.h>
#include <pthread.h>
#include <time.h>
#include <math.h>
#include <semaphore.h>
#include "bbuff.h"
#include "stats.h"

int factorySignal=0;
int printToScreen(char *words);
double current_time_in_ms(void);
pthread_mutex_t mutex;
sem_t haveData, haveSpace;
void * assemblyLine(void * param);
void * buyCandy(void * param);

typedef struct {
 int factoryNumber;
 double timeStampInMS;
} candy_t;

int main( int argc, char* argv[] )
{
    int numFactories=0;
    int numKids=0;
    int seconds = 0;
    int success =0;

    /// 1. Extract arguments

    if( argc == 4 ){
        char *endptr;
        numFactories = strtol(argv[1],&endptr,10);
        numKids = strtol(argv[2],&endptr,10);
        seconds= strtol(argv[3],&endptr,10);

        if(numFactories <= 0 || numKids <= 0 || seconds <= 0 ){
                printToScreen("One of the arguments contains a character or is less than or equal to zero.\n");
                return -1;
        }
    }
    else{
        printToScreen("Insufficient/Too Many Arguments\n");
        return -2;
    }

    /// 2. Initialize modules

    bbuff_init();
    stats_init(numFactories);

    /// 3. Launch candy-factory threads

    srand(time(NULL));                         // seed random number gen
    static pthread_t * factoryThreads;
    factoryThreads = malloc(sizeof(pthread_t) * numFactories);

    for (int i = 0; i<numFactories; i++) {
        int *fNum = malloc(sizeof(int));
        success =0;
        *fNum = i;
        success = pthread_create(&factoryThreads[i], NULL, assemblyLine, fNum);///////////////////////////////////////////////////////
        if(success != 0){
            printToScreen("Error creating factory thread!\n");
        }
        //free(fNum);
    }

    /// 4. Launch kid threads

    static pthread_t * kidThreads;
    kidThreads = malloc(sizeof(pthread_t) * numKids);

    for (int p = 0; p<numKids; p++) {
        success = 0;
        success= pthread_create(&kidThreads[p], NULL, buyCandy, NULL);
        if(success != 0){
            printToScreen("Error creating kid thread!\n");
        }

    }

    /// 5. Wait for requested time

    time_t startTime = time(0);  //start time
    //for(int time = (difftime(time(0), startTime); time < seconds; time{
    while (difftime(time(0), startTime) < seconds) {  //return difference of (current - start time)
        sleep(1);
        int secondsPast = (int) difftime(time(0), startTime);
        printf("Time %ds:\n", secondsPast);/////////////////////////////////////////////////////
    }


    /// 6. Stop candy-factory threads

    factorySignal = 1;                                //STOP SIGNAL!!!
    for (int i = 0; i<numFactories; i++) {
        success = 0;
        success = pthread_join(factoryThreads[i],NULL);
        if(success != 0){
                printToScreen("Error exiting Factory thread!\n");
            }
    }

    /// 7. Wait until no more candy

    while (!bbuff_is_empty()) {
        printToScreen("Waiting for all candy to be consumed\n");
        sleep(1);
      }

    /// 8. Stop kid threads

    for (int i = 0; i<numKids; i++) {
        success=0;
        success =pthread_cancel(kidThreads[i]);
        if(success != 0){
                printToScreen("Error cancelling Kid thread!\n");
        }

        success=0;
        success= pthread_join(kidThreads[i],NULL);
        if(success != 0){
                printToScreen("Error exiting Kid thread!\n");
        }
      }

    /// 9. Print statistics

    stats_display();

    /// 10. Cleanup any allocated memory

    //free(fNum);
    free(factoryThreads);
    free(kidThreads);
    stats_cleanup();


    return 0;
}
/**
** Takes a string and prints it using write()
** Returns zero if successful, else -1
*/
int printToScreen(char *words){
      int length = strlen(words);
      if (write(1, words, length) != length) {
        write(2, "Error writing to standard out\n", 31);
        return -1;
    }

    return 0;
}

/**
** Gives current time in ms
** returns time
*/
double current_time_in_ms(void)
{
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    return now.tv_sec * 1000.0 + now.tv_nsec/1000000.0;
}

/**
** Performs the tasks of each Factory thread
**
*/
void * assemblyLine(void * argument) {
  int * factoryNumber = (int*)argument;
  while (factorySignal == 0) {             //Stop when signal received

    int randomNum = rand() % 4;
    printf("\tFactory %d ships candy & waits %ds.\n", *factoryNumber, randomNum);

    candy_t *candy = malloc(sizeof(candy_t));      // Create candy an give its properties
    candy->factoryNumber = *factoryNumber;
    candy->timeStampInMS = current_time_in_ms();
    bbuff_blocking_insert(candy);                  // Put in buffer
    stats_record_produced(*factoryNumber);         // Record Stats
    sleep(randomNum);
  }
  printf("\tFactory %d done.\n", *factoryNumber );
  free(argument);
  return NULL;
}
/**
** Performs the tasks of each Child thread
**
*/
void * buyCandy(void * argument) {
  while (1) {
      candy_t* candy = (candy_t*) bbuff_blocking_extract(); //take candy from buffer
      stats_record_consumed(candy->factoryNumber,current_time_in_ms()-candy->timeStampInMS);
      free(candy);
      //printf("\tKid bought candy.\n");
      sleep(rand() % 2);
  }
  return NULL;
}
