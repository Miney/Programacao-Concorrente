#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define nThreads 10
#define qLeite 1000

int litros = 0; //variavel global contagem leite
sem_t mutex;
void *Add (void *a){
    
    while(litros<qLeite){
    sem_wait(&mutex);
    if(litros<qLeite){  
        printf("Comprando Leite\n");        
        litros++;
        printf("Voltando da loja. Total de leite: %d\n", litros);
}
sem_post(&mutex);    
}
	printf("Total alcancado\n");
	
pthread_exit(NULL); 
}

int main(int argc, char *argv[]){

    pthread_t t[nThreads];
    int i;
    sem_init(&mutex, 0, 1);

    for(i=0;i<nThreads;i++){
    	if(pthread_create(&t[i],NULL, Add, NULL)){
        printf("\nThread nao criada.\n");
        exit(1);
    	}
    }
    
    for(i=0;i<nThreads;i++){
        if(pthread_join(t[i], NULL)){
            printf("\n Erro dando join");
            exit(1);
        }
    }


    if(litros<qLeite) printf("\nNoob. Foi %d, deveria ser %d. Better luck next time.\n", litros, qLeite);

    else printf("%d litros =D\n", litros);
    sem_destroy(&mutex);
    return 0;
}
