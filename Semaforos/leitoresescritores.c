///problema dos escritores e leitores
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#define n_leitores 50
#define n_escritores 5

sem_t mutex;      ///controlador de acesso a variavel de contagem de processos leitores
sem_t db;         ///controlador do acesso ao banco de dados
int rc=0;         ///variavel de processos leitores(lendo ou na fila)

void * leitor(void *);    ///funcao que cria a thread leitora
void * escritor(void *);  ///funcao que cria a thread escritora
void escreve();   ///funcao que simula o tempo de edicao do arquivo acessado
void le();        ///funcao que simula o tempo de leitura do arquivo acessado

int main(){
  int i;
  pthread_t tl[n_leitores],te[n_escritores];
  sem_init(&mutex,0,1);
  sem_init(&db,0,1);

  for(i=0;i<n_leitores;i++){
    if(pthread_create(&tl[i],NULL,&leitor,(void*)i)){
      printf("Erro criando thread leitor\n");
      exit(1);
    }
  }


  for(i=0;i<n_escritores;i++){
    if(pthread_create(&te[i],NULL,&escritor,(void*)i)){
      printf("Erro criando thread escritor\n");
      exit(1);
    }
  }


  for(i=0;i<n_leitores;i++){
    if(pthread_join(tl[i],NULL)){
      printf("Erro dando join\n");
      exit(1);
    }
  }

  for(i=0;i<n_escritores;i++){
    if(pthread_join(te[i],NULL)){
      printf("Erro dando join\n");
      exit(1);
    }
  }

  return 0;
}

void * leitor(void * a){
  while(1){
    sem_wait(&mutex);
    rc+=1;
    if(rc==1){
      sem_wait(&db);
    }
    sem_post(&mutex);
    le();
    sem_wait(&mutex);
    rc-=1;
    if(rc==0){
      sem_post(&db);
    }
    sem_post(&mutex);
    sleep(5);
  }
}


void * escritor(void * a){
  while(1){
    sleep(2);
    sem_wait(&db);
    escreve();
    sem_post(&db);
  }
}

void escreve(){
  printf("Escrevendo\n");
  sleep(3);
}

void le(){
  printf("Lendo\n");
  sleep(1);
}
