/*

Yasmin Pimentel -- 11/0144341
Projeto 6 - Estrutura de Dados
Algoritmo Amigos Viajantes

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*VARIAVEIS GLOBAIS*/
///declaracao dos ponteiros para arquivo para permitir acesso pelas funcoes mais facilmente



/*  DECLARACAO DE ESTRUTURAS  */


struct aresta{
    int distancia;
    int identificadorDestino; ///Referente ao identificador da lista de nos
    aresta *prox;
};
typedef struct aresta Aresta;

struct no {
    int identificador; ///Referente ao vertice
    char *nome_cidade;
    struct no *prox;
    aresta *ListaAdj;
};
typedef struct no No;
