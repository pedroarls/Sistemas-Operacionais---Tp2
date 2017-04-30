#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Lista.h"


void FLvazia(Tlista *lista)
{
    lista->pprimeiro=(Tcelula*)malloc(sizeof(Tcelula));
    lista->pultimo=lista->pprimeiro;
    lista->pprimeiro->pprox=NULL;
}

int LEhVazia(Tlista *lista)
{
    return(lista->pprimeiro==lista->pultimo);
}

int Linsere(Tlista *lista,Processo x,int posicao)
{
    Tcelula *atual,*anterior,*aux;
    int i;

    if(posicao==0)//Insere no final
    {
        lista->pultimo->pprox=(Tcelula*)malloc(sizeof(Tcelula));
        lista->pultimo=lista->pultimo->pprox;
        lista->pultimo->processo=x;
        lista->pultimo->pprox=NULL;
        return 1;
    }
    if(posicao==1)//Insere na primeira posição
    {
        aux=(Tcelula*)malloc(sizeof(Tcelula));
        aux->processo = x;
        aux->pprox=lista->pprimeiro->pprox;
        lista->pprimeiro->pprox=aux;
        if(LEhVazia(&lista))
        {
            lista->pultimo=aux;
        }
        return 1;
    }

    //Insere na posição desejada,se for maior que o tamanho da lista insere no final da lista
    atual=lista->pprimeiro->pprox;
    anterior=lista->pprimeiro;
    for(i=1;i<posicao;i++)
    {
        anterior=atual;
        atual=atual->pprox;
        if(atual==NULL)
        {
            atual=(Tcelula*)malloc(sizeof(Tcelula));
            atual->processo=x;
            atual->pprox=NULL;
            anterior->pprox=atual;
            lista->pultimo=atual;
            return 1;
        }
    }
    atual=(Tcelula*)malloc(sizeof(Tcelula));
    atual->processo=x;
    atual->pprox=anterior->pprox;
    anterior->pprox=atual;
    return 1;
}

void Limprime(Tlista *lista)
{
    Tcelula *aux;
    Processo proc;

    if(LEhVazia(lista))
       return;
        //printf("A lista esta vazia!");
    aux=lista->pprimeiro->pprox;
    while(aux!=NULL)
    {
        //proc = pcbTable[n->pid];
        proc = aux->processo;
        printf("pc, pid, ppid, priority, value, start time, CPU time used so far\n");
        printf("%2d, %3d,  %3d, %8d, %5d, %10d, %3d\n",
        proc.pc, proc.pid, proc.ppid, proc.prioridade,
        proc.valor, proc.tempoInicio, proc.tempoUsado);
        aux=aux->pprox;

    }
}
