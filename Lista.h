#ifndef LISTA_H_INCLUDED
#define LISTA_H_INCLUDED

#include "SimuladorProcesso.h"

typedef struct celula
{
    Processo processo;
    struct celula *pprox;
}Tcelula;

typedef struct
{
    Tcelula *pprimeiro;
    Tcelula *pultimo;
}Tlista;

void FLvazia(Tlista *lista);
int LEhVazia(Tlista *lista);
int Linsere(Tlista *lista,Processo x,int posicao);
int Lremove(Tlista* p);
void Limprime(Tlista *lista);


#endif // LISTA_H_INCLUDED
