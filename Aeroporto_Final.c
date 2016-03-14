/*  Projeto Final Programacao Concorrente

    Universidade de Brasilia

    Yasmin Pimentel 11/0144341    */


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define normal 0
#define defeituoso 1
#define aberto 1
#define fechado 0
#define livre 1
#define ocupado 0
#define n_aeronaves 10
#define n_aeroportos 2
#define pistas 1
#define portoes 2
#define equipes 3
#define tempo_pouso 2
#define tempo_decolagem 2
#define tempo_preparacao 4


/*  DECLARACAO DAS ESTRUTURAS   */

struct aeroporto{
    int n_portoes;
    int n_equipes;
    int n_aeroporto;
    int vetor_portoes[portoes], vetor_equipes[equipes];

    ///contadores e variaveis de protecao
    int cont_portoes_ocupados, cont_equipes_ocupadas;
    int cont_sem_defeito, cont_sem_pouso, cont_sem_decolagem;

    sem_t sem_contador_portoes, sem_contador_equipes;
    sem_t sem_cont_defeito, sem_cont_pouso, sem_cont_decolagem;


    pthread_t pista_aeroporto;
    sem_t semaforo_pista, semaforo_portoes, semaforo_equipe;

    sem_t fila_defeito, fila_pouso, fila_decolagem;

};
typedef struct aeroporto Aeroporto;

struct aeronave{
    int localizador;
    int aeroporto_destino;
    int estado;
    int contador_processos_solo;
    sem_t sem_cont_ps;
};
typedef struct aeronave Aeronave;

/*      DECLARACAO DAS THREADS      */

void* thread_aeronave (void*);
void* thread_pista(void*);
void* thread_equipe(void*);

///metodos de inicializacao

//void inicializaAeroporto(Aeroporto*, int);
Aeronave* inicializaAeronave(int);

///metodos de simulacao
void embarque();
void preparacao();
void decolagem(int);
void voando(int);
int pousando(int);
void desembarcando();

///metodos auxiliares
int geraEstado();
void mudaBinario(int);

///metodos da torre de comando/aeronaves
void pede_pouso(int, int, int);
int pede_portao(int, int);
int pede_equipe(int, int);

/*  DECLARACAO DAS VARIAVEIS GLOBAIS*/
Aeroporto *Aeroportos[n_aeroportos];
pthread_t thread[n_aeronaves];
Aeronave *aeronaves[n_aeronaves];


/*    FUNCAO PRINCIPAL    */

/*   ocorre tambem a inicializacao das estruturas de aeronaves, alem das threads       */

int main(){
    srand(time(NULL));
    int i,j;

    printf("Main\n");

    for(i=0;i<n_aeroportos;i++){
        Aeroportos[i]= (Aeroporto*)malloc(sizeof(Aeroporto));
        printf("For de inicializacao dos aeroportos\n");
        Aeroportos[i]->n_aeroporto=i;
        Aeroportos[i]->n_equipes=equipes;
        Aeroportos[i]->n_portoes=portoes;

        for(j=0;j<portoes;j++){
            Aeroportos[i]->vetor_portoes[j]=aberto;
        }

        for(j=0;j<equipes;j++){
            Aeroportos[i]->vetor_equipes[j]=aberto;
        }

        sem_init(&Aeroportos[i]->sem_cont_defeito,0,1);
        Aeroportos[i]->cont_sem_defeito=0;

        sem_init(&Aeroportos[i]->sem_cont_pouso,0,1);
        Aeroportos[i]->cont_sem_pouso=0;

        sem_init(&Aeroportos[i]->sem_cont_decolagem,0,1);
        Aeroportos[i]->cont_sem_decolagem=0;

        sem_init(&Aeroportos[i]->sem_contador_portoes,0,1);
        Aeroportos[i]->cont_portoes_ocupados=0;

        sem_init(&Aeroportos[i]->sem_contador_equipes,0,1);
        Aeroportos[i]->cont_equipes_ocupadas=0;

        sem_init(&Aeroportos[i]->semaforo_pista, 0, 1);

        sem_init(&Aeroportos[i]->semaforo_portoes, 0, 1);

        sem_init(&Aeroportos[i]->semaforo_equipe, 0, 1);

        sem_init(&Aeroportos[i]->fila_defeito, 0, 0);
        sem_init(&Aeroportos[i]->fila_pouso, 0, 0);
        sem_init(&Aeroportos[i]->fila_decolagem, 0, 0);

        if(pthread_create(&Aeroportos[i]->pista_aeroporto, NULL, &thread_pista, (void*)i))
        {
            printf("\n ERROR creating thread 1");
            exit(1);
        }
        printf("Criou thread\n");




    }

    ///inicializacao das threads das aeronaves
    for(i=0;i<n_aeronaves;i++){
        if(pthread_create(&thread[i], NULL, &thread_aeronave, (void*)i)){
            printf("\n ERROR creating thread");
            exit(1);
        }
    }
    for(i=0;i<n_aeronaves;i++){
        if(pthread_join(thread[i], NULL))	/* wait for the thread 1 to finish */
        {
          printf("\n ERROR joining thread");
          exit(1);
        }
    }

}

/*   EXPLICITACAO DAS THREADS   */

///o argumento passado e o numero da aeronave, enquanto seu estado de funcionamento e definido enquanto na funcao de voo


void* thread_aeronave (void *arg){
    int localizador=(int)arg;
    int portao, n_aeroporto, equipe;
    pthread_t t_equipe;

    aeronaves[localizador]=inicializaAeronave(localizador);
    printf("Aeronave %d nasceu!\n", localizador);
    //pede_portao();
    //pede_equipe();

    while(1){
        decolagem(localizador);

        n_aeroporto=aeronaves[localizador]->aeroporto_destino;

        voando(localizador);
        portao=pousando(localizador);

        ///se chegou aqui ja tem um portao
        ///iniciam os processos em solo
        aeronaves[localizador]->contador_processos_solo=0;

        equipe=pede_equipe(n_aeroporto, localizador);

        ///recebe equipe, cria thread equipe

        if(pthread_create(&t_equipe, NULL, &thread_equipe, (void*)equipe))
        {
            printf("\n ERROR creating thread equipe");
            exit(1);
        }
        printf("Passageiros da aeronave %d estao desembarcando\n\n", localizador);
        desembarcando(localizador);
        //aeronaves[localizador]->contador_processos_solo++;
        printf("Passageiros da aeronave %d estao embarcando\n\n", localizador);
        embarque(localizador);
        //aeronaves[localizador]->contador_processos_solo++;

        ///thread dara o post
        if(pthread_join(t_equipe, NULL))	/* wait for the thread 1 to finish */
        {
          printf("\n ERROR joining thread");
          exit(1);
        }


        ///libera as equipes e os portoes e tenta decolar

        Aeroportos[n_aeroporto]->vetor_equipes[equipe]=livre;
        sem_post(&Aeroportos[n_aeroporto]->semaforo_equipe);
        sem_wait(&Aeroportos[n_aeroporto]->sem_contador_equipes);
        Aeroportos[n_aeroporto]->cont_equipes_ocupadas--;
        sem_post(&Aeroportos[n_aeroporto]->sem_contador_equipes);





        sem_wait(&Aeroportos[n_aeroporto]->sem_contador_portoes);
        Aeroportos[n_aeroporto]->vetor_portoes[portao]=aberto;
        Aeroportos[n_aeroporto]->cont_portoes_ocupados--;
        sem_post(&Aeroportos[n_aeroporto]->sem_contador_portoes);
        sem_post(&Aeroportos[n_aeroporto]->semaforo_portoes);
        ///aeronave esta pronta para decolar
    }
}

void* thread_pista(void *arg){

    int n_aeroporto=(int)arg;
    printf("Pista %d nasceu!\n", n_aeroporto);

    //sem_init(&semaforo_pista,0,1);
    ///inicializacao dos semaforos

    while(1){

        sem_wait(&Aeroportos[n_aeroporto]->semaforo_pista);
        ///Os contadores estao todos protegidos
        if(Aeroportos[n_aeroporto]->cont_sem_defeito>0){

            sem_wait(&Aeroportos[n_aeroporto]->sem_cont_defeito);
            Aeroportos[n_aeroporto]->cont_sem_defeito--;
            sem_post(&Aeroportos[n_aeroporto]->sem_cont_defeito);

            sem_post(&Aeroportos[n_aeroporto]->fila_defeito);
        }
        else if(Aeroportos[n_aeroporto]->cont_sem_pouso>0){

            sem_wait(&Aeroportos[n_aeroporto]->sem_cont_pouso);
            Aeroportos[n_aeroporto]->cont_sem_pouso--;
            sem_post(&Aeroportos[n_aeroporto]->sem_cont_pouso);

            sem_post(&Aeroportos[n_aeroporto]->fila_pouso);
        }
        else if(Aeroportos[n_aeroporto]->cont_sem_decolagem>0){

            sem_wait(&Aeroportos[n_aeroporto]->sem_cont_decolagem);
            Aeroportos[n_aeroporto]->cont_sem_decolagem--;
            sem_post(&Aeroportos[n_aeroporto]->sem_cont_decolagem);

            sem_post(&Aeroportos[n_aeroporto]->fila_decolagem);
        }
        else{
            sem_post(&Aeroportos[n_aeroporto]->semaforo_pista);
        }
    }

}

void* thread_equipe (void *arg){
    ///notar que o localizador esta armazenado dentro da thread equipe
    int localizador_equipe=(int)arg;
    int controle_post=0;
    ///checar se o tempo de preparacao e random ou se ele e pre definido(qualquer coisa so usar funcao pra gerar random)
    printf("Esquipe %d esta preparando a aeronave\n", localizador_equipe);
    sleep(tempo_preparacao);
    printf("Equipe %d acabou a manutencao\n", localizador_equipe);
    /*
    while(controle_post=0){
        if(aeronaves[localizador_aeronave]->contador_processos_solo==2){
            controle_post=1;
        }
    }

    sem_post(&aeronaves[localizador_aeronave]->sem_cont_ps);
    */
}


/*      EXPLICITACAO DOS METODOS    */

///metodos de calculo

int gera_random(int max, int min){
    int retorno;
    retorno=(rand() % ( max- min + 1 ) + min);
    return retorno;
}

void mudaBinario(int localizador){
    int aeroporto_destino;
    aeroporto_destino=aeronaves[localizador]->aeroporto_destino;

    if(aeroporto_destino==0){
        aeronaves[localizador]->aeroporto_destino=1;
    }
    else{
        aeronaves[localizador]->aeroporto_destino=0;
    }
}

///metodos de inicializacao

Aeronave* inicializaAeronave(int localizador){
    Aeronave *Aeronave_inicializada;
    Aeronave_inicializada=(Aeronave*) malloc(sizeof(Aeronave));
    Aeronave_inicializada->localizador=localizador;
    sem_init(&Aeronave_inicializada->sem_cont_ps,0,1);

    if(localizador<(n_aeronaves/2)){
        Aeronave_inicializada->aeroporto_destino=0;
    }
    else{
        Aeronave_inicializada->aeroporto_destino=1;
    }

    return Aeronave_inicializada;

}

///metodos de simulacao

void voando(int localizador){
    printf("%d Entrou na funcao de voo\n", localizador);
    int tempo_voo,estado;
    estado=gera_random(0,100);

    if(estado<=20){
        estado=defeituoso;
    }
    else{
        estado=normal;
    }

    tempo_voo=gera_random(20,30);
    printf("Aeronave %d esta com estado %d e voando rumo ao aeroporto %d\n", localizador, estado, aeronaves[localizador]->aeroporto_destino);
    sleep(tempo_voo);
    aeronaves[localizador]->estado=estado;
}

void embarque(){
    int tempo_embarque;
    tempo_embarque=gera_random(3, 6);
    sleep(tempo_embarque);
}

void desembarcando(){
    //signal para as equipes, usar o localizador da aeronave e do portao, a thread da equipe
    //pega a barreira da aeronave e da o wait para poder liberar pra decolagem
    int tempo_desembarque;

    tempo_desembarque=gera_random(2, 4);
    sleep(tempo_desembarque);
}


int pousando(int n_aeronave){
    int estado, n_aeroporto, portao;
    n_aeroporto=aeronaves[n_aeronave]->aeroporto_destino;
    estado=aeronaves[n_aeronave]->estado;

    pede_pouso(estado , n_aeronave , n_aeroporto);
    sleep(tempo_pouso);
    printf("Aeronave %d pousou\n",n_aeronave);
    sem_post(&Aeroportos[n_aeroporto]->semaforo_pista);
    portao=pede_portao(n_aeroporto, n_aeronave);
    return portao;
}

///quando libera o semaforo o proximo da fila pede o pouso
///3 "listas", uma pra defeituoso, uma pra pouso outra pra decolagem

void pede_pouso(int estado,int n_aeronave,int n_aeroporto){
        if(estado==defeituoso){
            ///contadores projegidos
            sem_wait(&Aeroportos[n_aeroporto]->sem_cont_defeito);
            Aeroportos[n_aeroporto]->cont_sem_defeito++;
            sem_post(&Aeroportos[n_aeroporto]->sem_cont_defeito);

            printf("A aeronave %d esta aguardando pouso no aeroporto %d\n", n_aeronave, n_aeroporto);
            sem_wait(&Aeroportos[n_aeroporto]->fila_defeito);
        }
        else{
            sem_wait(&Aeroportos[n_aeroporto]->sem_cont_pouso);
            Aeroportos[n_aeroporto]->cont_sem_pouso++;
            sem_post(&Aeroportos[n_aeroporto]->sem_cont_pouso);


            printf("A aeronave %d esta aguardando pouso no aeroporto %d\n", n_aeronave, n_aeroporto);
            sem_wait(&Aeroportos[n_aeroporto]->fila_pouso);
        }
}

void decolagem(int n_aeronave){
    int n_aeroporto;
    n_aeroporto=aeronaves[n_aeronave]->aeroporto_destino;

    sem_wait(&Aeroportos[n_aeroporto]->sem_cont_decolagem);
    Aeroportos[n_aeroporto]->cont_sem_decolagem++;
    sem_post(&Aeroportos[n_aeroporto]->sem_cont_decolagem);


    printf("A aeronave %d espera decolagem do aeroporto %d rumo ao aeroporto %d\n", n_aeronave, n_aeroporto, aeronaves[n_aeronave]->aeroporto_destino);
    sem_wait(&Aeroportos[n_aeroporto]->fila_decolagem);
    sleep(tempo_decolagem);
    sem_post(&Aeroportos[n_aeroporto]->semaforo_pista);
    mudaBinario(n_aeronave);
    printf("A aeronave %d decolou do aeroporto %d rumo ao aeroporto %d\n", n_aeronave, n_aeroporto, aeronaves[n_aeronave]->aeroporto_destino);
}


///funcao que retorna o portao utilizado para posterior reabertura do mesmo e para o codigo nao ficar parado
///nessa funcao

int pede_portao(int n_aeroporto, int n_aeronave){
    int i;
    printf("Aeronave %d pediu portao no aeroporto %d\n", n_aeronave, n_aeroporto);
    while(1){
        sem_wait(&Aeroportos[n_aeroporto]->sem_contador_portoes);
        int ocupados=Aeroportos[n_aeroporto]->cont_portoes_ocupados;
        sem_post(&Aeroportos[n_aeroporto]->sem_contador_portoes);

        if(ocupados<portoes){
            sem_wait(&Aeroportos[n_aeroporto]->sem_contador_portoes);
            for(i=0;i<portoes;i++){
                if(Aeroportos[n_aeroporto]->vetor_portoes[i]==aberto){


                    Aeroportos[n_aeroporto]->cont_portoes_ocupados++;


                    Aeroportos[n_aeroporto]->vetor_portoes[i]=fechado;
                    printf("Aeronave %d conseguiu o portao %d\n", n_aeronave, i);
                    ///retorna o numero do portao
                    sem_post(&Aeroportos[n_aeroporto]->sem_contador_portoes);
                    return i;

                }
            }
            sem_post(&Aeroportos[n_aeroporto]->sem_contador_portoes);
        }
        else{
            printf("Aeronave %d esta aguardando portao\n", n_aeronave);
            sem_wait(&Aeroportos[n_aeroporto]->semaforo_portoes);
        }
    }
}

///funcao que retorna a equipe encontrada e fecha aquele slot ou que espera liberar uma equipe
///logica semelhante a dos portoes

int pede_equipe(int n_aeroporto,int n_aeronave){
    int i;
    printf("A aeronave %d pede equipe no aeroporto %d\n",n_aeronave, n_aeroporto);
    if(Aeroportos[n_aeroporto]->cont_equipes_ocupadas<equipes){
        for(i=0;i<equipes;i++){
            if(Aeroportos[n_aeroporto]->vetor_equipes[i]==livre){

                sem_wait(&Aeroportos[n_aeroporto]->sem_contador_equipes);
                Aeroportos[n_aeroporto]->cont_equipes_ocupadas++;
                sem_post(&Aeroportos[n_aeroporto]->sem_contador_equipes);
                Aeroportos[n_aeroporto]->vetor_equipes[i]=fechado;
                printf("A aeronave %d consegue a equipe %d\n",n_aeronave, i);
                return i;
            }
        }
    }

    else{
        sem_wait(&Aeroportos[n_aeroporto]->semaforo_equipe);
        printf("A aeronave %d aguarda equipe no aeroporto %d\n",n_aeronave, n_aeroporto);
    }
}


