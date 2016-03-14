#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#define N 5
#define LEFT (i+N-1)%N
#define RIGHT (i+1)%N
#define PENSANDO 0
#define FOME 1
#define EAT 2



    void * filosofo(void *);
    void pegar_garfos(int);
    void put_forks(int);
    void pensar(int);
    void comer(int);
    void test(int);



    int state[N];
    sem_t mutex;
    sem_t s[N];

int main(){

    int i, j;
    pthread_t thread[N];
    sem_init(&mutex, 0, 1);

    for(j=0;i<N;i++){
    sem_init(&s[i], 0, 1);
    }

    for(i=0;i<N;i++){

        if(pthread_create(&thread[i], NULL, &filosofo, (void*)i))           {
            printf("\n ERROR creating thread 1");
            exit(1);
    	    }
        }
    for(i=0;i<N;i++){
        if(pthread_join(thread[i], NULL))	/* wait for the thread 1 to finish */
        {
          printf("\n ERROR joining thread");
          exit(1);
        }
    }
    return 0;
}


    void * filosofo(void *arg){
	int i= (int)arg;
        while(1){
            pensar(i);
            pegar_garfos(i);
            comer(i);
            put_forks(i);
        }
    }

    void pegar_garfos(int i){
        sem_wait(&mutex);
        state[i]=FOME;
        printf("Ofilosofo %d esta com fome\n", i);
        test(i);
        sem_post(&mutex);
        sem_wait(&s[i]);
    }

    void put_forks(int i){
        sem_wait(&mutex);
        state[i]=PENSANDO;
        printf("O filosofo %d esta pensando\n", i);
        test(LEFT);
        test(RIGHT);
        sem_post(&mutex);
    }

    void test(i){
        if(state[i]==FOME&&state[LEFT]!=EAT&&state[RIGHT]!=EAT){
            state[i]=EAT;
            printf("O filosofo %d esta comendo\n", i);
            //printf("O filosofo %d pode comer.\n", i);
            sem_post(&s[i]);
        }
    }

    void pensar(int i){
        sleep(3);
    }

    void comer(int i){
        sleep(3);
    }



