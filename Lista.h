#ifndef LISTA_H_INCLUDED
#define LISTA_H_INCLUDED

#include "SimuladorProcesso.h"

typedef struct celula
{
    int pid;
    struct celula *pprox;
}Tcelula;

typedef struct
{
    Tcelula *pprimeiro;
    Tcelula *pultimo;
}Tlista;

void FLvazia(Tlista *lista);
int LEhVazia(Tlista *lista);
int Linsere(Tlista *lista,int pid,int posicao);
int Lremove(Tlista* p);
void Limprime(Tlista *lista);


#endif // LISTA_H_INCLUDED
