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

int Linsere(Tlista *lista,int pid,int posicao)
{
    Tcelula *atual,*anterior,*aux;
    int i;

    if(posicao==-1)//Insere no final
    {
        lista->pultimo->pprox=(Tcelula*)malloc(sizeof(Tcelula));
        lista->pultimo=lista->pultimo->pprox;
        lista->pultimo->pid=pid;
        lista->pultimo->pprox=NULL;
        return 1;
    }

    if(posicao==0)//Insere na primeira posição
    {
        aux=(Tcelula*)malloc(sizeof(Tcelula));
        aux->pid = pid;
        aux->pprox=lista->pprimeiro->pprox;
        lista->pprimeiro->pprox=aux;
        if(LEhVazia(lista))
        {
            lista->pultimo=aux;
        }
        return 1;
    }

    //Insere na posição desejada,se for maior que o tamanho da lista insere no final da lista
    atual=lista->pprimeiro->pprox;
    anterior=lista->pprimeiro;
    for(i=1; i<posicao; i++)
    {
        anterior=atual;
        atual=atual->pprox;
        if(atual==NULL)
        {
            atual=(Tcelula*)malloc(sizeof(Tcelula));
            atual->pid=pid;
            atual->pprox=NULL;
            anterior->pprox=atual;
            lista->pultimo=atual;
            return 1;
        }
    }
    atual=(Tcelula*)malloc(sizeof(Tcelula));
    atual->pid=pid;
    atual->pprox=anterior->pprox;
    anterior->pprox=atual;
    return 1;
}

int Lremove(Tlista* p)
{
    Tcelula* aux;
    int pid = -1;

    if (!LEhVazia(p))
    {
        //printf("DEQ(pid=%d)\n", (*p)->proc.pid);
        pid = p->pprimeiro->pid;
        aux = p->pprimeiro;
        p->pprimeiro = p->pprimeiro->pprox;
        free(aux);
        return pid;
    }
    else
    {
        printf("A fila está vazia\n");
        return pid;
    }
}

void Limprime(Tlista *lista)
{
    Tcelula *aux;
    int pid;

    if(LEhVazia(lista))
        return;
    //printf("A lista esta vazia!");
    aux=lista->pprimeiro->pprox;
    while(aux!=NULL)
    {
        //proc = pcbTable[n->pid];
        pid = aux->pid;
        printf("pid = %2d\n",pid);
        aux=aux->pprox;
    }
}

void LRetiraNoMeio(Tcelula* p, Tlista *Lista, int *Item)
{
    Tcelula* q;

    if ((LEhVazia(Lista)) || (p == NULL) || (p->pprox == NULL))
    {
        printf(" Erro   Lista vazia ou posicao nao existe\n");
        return;
    }

    q = p->pprox;
    *Item = q->pid;
    p->pprox = q->pprox;
    if (p->pprox == NULL )
    {
        Lista->pultimo = p;
    }
    free(q);
}
