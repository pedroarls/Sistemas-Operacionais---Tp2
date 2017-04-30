#ifndef LISTA_H_INCLUDED
#define LISTA_H_INCLUDED

#include "Processo.h"

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
void Limprime(Tlista *lista);


#endif // LISTA_H_INCLUDED
