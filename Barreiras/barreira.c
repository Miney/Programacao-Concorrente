#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>


#define N 10 //numero de threads
#define ITERATIONS 10 //numero de iterações

// Barrier variable
pthread_barrier_t barr;


void * entry_point(void *arg)
{
    int id = (int)arg;
    int i;
    for(i=0; i < ITERATIONS;i++){
       	sleep(3);
	printf(" Thread [%d] vai esperar na barreira!\n", id);
        // Synchronization point
    	int rc = pthread_barrier_wait(&barr);
    	if(rc != 0 && rc != PTHREAD_BARRIER_SERIAL_THREAD)
    	{
    	    printf("Não pode esperar na barreira\n");
    	    exit(-1);
    	}
	printf(" Thread [%d] passou pela barreira!\n", id);
    }
}


int main(int argc, char **argv)
{
    pthread_t thr[N];

    // Barrier initialization
    if(pthread_barrier_init(&barr, NULL, N))
    {
        printf("Não pode criar a barreira\n");
        return -1;
    }

    int i;
    for(i = 0; i < N; ++i)
    {
        if(pthread_create(&thr[i], NULL, &entry_point, (void*)i))
        {
            printf("Não pode criar a thread %d\n", i);
            return -1;
        }
    }

    for(i = 0; i < N; ++i)
    {
        if(pthread_join(thr[i], NULL))
        {
            printf("Could not join thread %d\n", i);
            return -1;
        }
    }

 
    printf("TERMINOU!\n");

    return 0;
}
