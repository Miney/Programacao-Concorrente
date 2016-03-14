#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>


#define ROWS1 3
#define COLS1 15

#define ROWS2 15
#define COLS2 3


int matrix1[ROWS1][COLS1];
int matrix2[ROWS2][COLS2];
int matrix_final[ROWS1][COLS2];
int sums[COLS1];
// Barrier variable
pthread_barrier_t barr;


void * multiplica(void *arg){
    int col = (int)arg;
    int r1,c2,rc;
    for(r1=0;r1<ROWS1;r1++){
      for(c2=0;c2<COLS2;c2++){
    	sums[col] = matrix1[r1][col]*matrix2[col][c2];
    	// Synchronization point (espera todos multiplicarem)
    	rc = pthread_barrier_wait(&barr);
    	if(rc != 0 && rc != PTHREAD_BARRIER_SERIAL_THREAD)
    	{
        	printf("Could not wait on barrier\n");
        	exit(-1);
    	}
	// Synchronization (espera pela soma)
    	rc = pthread_barrier_wait(&barr);
    	if(rc != 0 && rc != PTHREAD_BARRIER_SERIAL_THREAD){
        	printf("Could not wait on barrier\n");
        	exit(-1);
    	}
     }
    }
}



void * soma(void *arg){
    int r,c,i,finalSum,rc;
    for(r=0;r<ROWS1;r++){
      for(c=0;c<COLS2;c++){
	// Synchronization (espera pela multiplicacao)
    	rc = pthread_barrier_wait(&barr);
    	if(rc != 0 && rc != PTHREAD_BARRIER_SERIAL_THREAD){
        	printf("Could not wait on barrier\n");
        	exit(-1);
    	}
        finalSum = 0;
        for(i=0;i<COLS1;i++){
            finalSum = finalSum + sums[i];
        }
	matrix_final[r][c] = finalSum;
        // Synchronization point (barreira para liberar após a soma)
    	rc = pthread_barrier_wait(&barr);
    	if(rc != 0 && rc != PTHREAD_BARRIER_SERIAL_THREAD){
        	printf("Could not wait on barrier\n");
        	exit(-1);
    	}
      }
    }
}

void init_matrix(){
        int i,j; 
	for(i = 0; i <  ROWS1; i++){
              for(j = 0; j < COLS1; j++){
		 matrix1[i][j] = i+j;
              }
        }

        for(i = 0; i <  ROWS2; i++){
              for(j = 0; j < COLS2; j++){
                 if(i >= j){  
		   matrix2[i][j] = i-j;
                 }else{
		   matrix2[i][j] = j-i;	
                 }
              }
        }
}

int main(int argc, char **argv)
{
    pthread_t thr[COLS1];
    pthread_t sumThread;
    init_matrix();

    // Barrier initialization
    if(pthread_barrier_init(&barr, NULL, COLS1+1)){
        printf("Could not create a barrier\n");
        return -1;
    }

    int i;
    for(i = 0; i < COLS1; ++i){
        if(pthread_create(&thr[i], NULL, &multiplica, (void*)i)){
            printf("Could not create thread %d\n", i);
            return -1;
        }
    }
    if(pthread_create(&sumThread, NULL, &soma, NULL)){
            printf("Could not create thread %d\n", i);
            return -1;
    }
    

    for(i = 0; i < COLS1; ++i){
        if(pthread_join(thr[i], NULL)){
            printf("Could not join thread %d\n", i);
            return -1;
        }
    }
    if(pthread_join(sumThread, NULL)){
            printf("Could not join thread %d\n", i);
            return -1;
    }

    int r,c;
    printf("Matriz 1:\n");
    
    for(r=0;r<ROWS1;r++){
       for(c=0;c<COLS1;c++){
         printf("%d ",matrix1[r][c]);   
      
       }
       printf("\n");   
    }

    printf("Matriz 2:\n");
    for(r=0;r<ROWS2;r++){
      for(c=0;c<COLS2;c++){
          printf("%d ",matrix2[r][c]);   
       }
       printf("\n");   
    }
    printf("Matriz Final:\n");
    for(r=0;r<ROWS1;r++){
      for(c=0;c<COLS2;c++){
         printf("%d ",matrix_final[r][c]);   
       }
       printf("\n");   
    }
    printf("TERMINOU!\n");
    return 0;
}
