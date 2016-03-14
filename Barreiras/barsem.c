#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>

#define N_THREADS 5

///definicao de variaveis globais

int n_processos=0, n_threads=0;
sem_t entrada, saida, mutex;

///declaracao de funcoes
void * ThreadFuncional(void *);
void * initBarreira(void *);
void wait(void);

///main
int main(){

    int i;
    initBarreira((void*)N_THREADS);
    pthread_t thread[N_THREADS];


    for(i=0;i<N_THREADS;i++){
      if(pthread_create(&thread[i], NULL, &ThreadFuncional, (void *)i)){
        printf("\n ERROR creating thread 1");
        exit(1);
      }
    }

    for(i=0;i<N_THREADS;i++){
      if(pthread_join(thread[i], NULL)){
        printf("\n ERROR joining thread");
        exit(1);
      }
    }


    printf("Terminou!\n");
}


///definicao de funcoes
void * ThreadFuncional(void * arg){
  int i=(int)arg;
  //printf("Thread %d sendo executada\n", i);
    printf("Thread %d vai parar na bareira\n", i);
    wait();
    printf("Thread %d passou pela barreira\n", i);
}

void wait(void){

    int l_processos;
    sem_wait(&mutex);
    n_processos++;
    l_processos=n_processos;
    sem_post(&mutex);
    if(l_processos==n_threads){
      sem_wait(&saida);
      sem_post(&entrada);
    }

    sem_wait(&entrada);
    sem_post(&entrada);
    sem_wait(&mutex);
    n_processos--;
    l_processos=n_processos;
    sem_post(&mutex);

    if(l_processos==0){
      sem_wait(&entrada);
      sem_post(&saida);
    }
    sem_wait(&saida);
    sem_post(&saida);
}

void * initBarreira(void * arg){
  n_threads=(int)arg;
  sem_init(&entrada,0,0);
  sem_init(&saida,0,1);
  sem_init(&mutex,0,1);
}
