#include <stdio.h>
#include <stdlib.h>
#include "Lista.h"

int main()
{
    Tlista listaProc;
    Processo proc;

    FLvazia(&listaProc);
    proc.estado = 0;
    strcpy(proc.nomeArquivo,"proc1");
    proc.pc = 0;
    proc.pid = 1;
    proc.ppid = 1;
    proc.prioridade = 3;
    strcpy(proc.programa,"proc1.prog");
    proc.tempoInicio = 0;
    proc.tempoUsado = 6;
    proc.valor = 16;

    Linsere(&listaProc,proc,-1);

    proc.estado = 0;
    strcpy(proc.nomeArquivo,"proc2");
    proc.pc = 0;
    proc.pid = 2;
    proc.ppid = 2;
    proc.prioridade = 2;
    strcpy(proc.programa,"proc2.prog");
    proc.tempoInicio = 0;
    proc.tempoUsado = 6;
    proc.valor = 97;

    Linsere(&listaProc,proc,-1);
    Lremove(&listaProc);
    Limprime(&listaProc);


    return 0;
}
