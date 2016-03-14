#include <stdio.h>
#include <pthread.h>

#define BUFFER 100
///declaracao de variaveis globais

pthread_cond_t condicao_cheio, condicao_vazio;
pthread_mutex_t mutex_contador;
pthread_t threadProdutora, threadConsumidora;
int contador;

///declaracao de funcoes
void * produtor (void* );
void * consumidor (void *);
void produzir(void);
void consumir(void);

///funcao principal
int main(){
  pthread_mutex_init(&mutex_contador, NULL);
  pthread_cond_init(&condicao_vazio,NULL);
  pthread_cond_init(&condicao_cheio,NULL);

  pthread_create(&threadProdutora,NULL,&produtor,NULL);
  pthread_create(&threadConsumidora,NULL,&consumidor,NULL);

  pthread_join(threadProdutora, NULL);
  pthread_join(threadConsumidora, NULL);

  return 0;

}

///explicitacao de funcoes

void * produtor(void *arg){
  while(1){
    produzir();
  }
}

void * consumidor(void *arg){
  while(1){
    consumir();
  }
}


void produzir(void){
  pthread_mutex_lock(&mutex_contador);
  if(contador==BUFFER){
     printf("Vai esperar o cheio\n");
     pthread_cond_wait(&condicao_cheio, &mutex_contador);
  }
  contador++;
  printf("contador: %d\n", contador);
  if(contador==1){
     printf("Vai sinalizar para o vazio\n");
     pthread_cond_signal(&condicao_vazio);
  }
  pthread_mutex_unlock(&mutex_contador);
}

void consumir(void){
  pthread_mutex_lock(&mutex_contador);
  if(contador==0){
    printf("Vai esperar para o vazio\n");
    pthread_cond_wait(&condicao_vazio, &mutex_contador);
  }
  contador--;
  printf("contador: %d\n", contador);
  if(contador==BUFFER-1){
     printf("Vai sinalizar para o cheio\n");
     pthread_cond_signal(&condicao_cheio);
  }
  pthread_mutex_unlock(&mutex_contador);
}
