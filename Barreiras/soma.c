#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>

///o tamanho do vetor nao vai poder ser definido via define porque eu altero ele no meio do codigo
//#define tamanhoVetor 6

int tamanhoVetor=6;
int somaThread=0, contadorThreads=0, novoVetor=0, vetor[tamanhoVetor];
sem_t mutex;
pthread_barrier_t barreira;

///declaracao de funcoes

void * soma(void *);
int geraRand(int);


///funcao principal

int main(){

  srand(time(NULL));
  sem_init(&mutex,0,1);

  int i, parteInutil, rc, nIteracoes;
  pthread_t threadSoma[tamanhoVetor/2];

  //printf("Insira o tamanho do vetor inicial: ");
  //scanf("%d",&tamanhoVetor);

  //if(tamanhoVetor%2!=0){
    //printf("\nVetor deve ser de numero par!\Sera arredondado para cima.");
    //tamanhoVetor+=1;
  //}

  //vetor=malloc(sizeof(int*tamanhoVetor));

  if(pthread_barrier_init(&barreira, NULL, tamanhoVetor/2)){
    printf("Could not create a barrier\n");
    exit(-1);
  }

  ///conferir a maneira de fazer alocacao dinamica para um vetor de threads
  //threadSoma=malloc(sizeof(pthread_t*(tamanhoVetor/2));

  for(i=0;i<tamanhoVetor/2;i++){
    if(pthread_create(&threadSoma[i], NULL, &soma, (void*)i)){
      printf("\n ERROR creating thread");
      exit(1);
    }
  }

  for(i=0;i<tamanhoVetor/2;i++){
    if(pthread_join(threadSoma[i], NULL)){
      printf("\n ERROR joining thread");
      exit(1);
    }
  }


  for(i=0;i<tamanhoVetor;i++){
    vetor[i]=geraRand(10);
  }


  nIteracoes=0;

  parteInutil=tamanhoVetor/(2^nIteracoes);
  ///eu posso fazer isso???

  while(contadorThreads!=1){

  ///quando todos os processos chegarem na barreira, metade serao mortos, metade sera desalocada, volta a funcionar
    rc = pthread_barrier_wait(&barreira);

    if(rc != 0 && rc != PTHREAD_BARRIER_SERIAL_THREAD){
      printf("Could not wait on barrier\n");
      exit(-1);
    }

    for(i=parteInutil;i<tamanhoVetor/2;i++){
      destroy(threadSoma[i]);
    }

    /*for(i=parteInutil;i<tamanhoVetor/2;i++){
      vetor[i]=free();
    }*/

    tamanhoVetor=tamanhoVetor/2;
    contadorThreads=contadorThreads/2;
  }
  printf("\n Soma = %d \n",vetor[0]);
  return 0;
}


///explicitacao de funcoes

int geraRand(int max){
  int r;
  r=rand()%max;
  return r;
}

void * soma (void * arg){
  ///i seriam as posicoes que vao ser somadas
  int i = (int)arg;
  contadorThreads+=1;
  while(1){
    int temp, rc; ///varivel temporaria para armazenar a soma e substituir no vetor original.
    ///trabalhar com dois semaforos?
    sem_wait(&mutex);
    temp=vetor[somaThread]+vetor[somaThread+1];
    ///novoVetor seriam os novos itens substitutos aos anteriores.
    vetor[novoVetor]=temp;
    novoVetor++;
    somaThread+=2;
    sem_post(&mutex);
    rc = pthread_barrier_wait(&barreira);
    if(rc != 0 && rc != PTHREAD_BARRIER_SERIAL_THREAD){
      printf("Could not wait on barrier\n");
      exit(-1);
    }
  }
}
