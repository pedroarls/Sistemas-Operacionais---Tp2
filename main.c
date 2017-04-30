#include <stdio.h>
#include <stdlib.h>
#include "Lista.h"

int main()
{
    Tlista listaProc;
    Processo proc;

    FLvazia(&listaProc);
    proc.estado = 0;
    strcpy(proc.fname,"proc1");
    proc.pc = 0;
    proc.pid = 1;
    proc.ppid = 1;
    proc.prioridade = 3;
    strcpy(proc.prog,"proc1.prog");
    proc.tempoInicio = 0;
    proc.tempoUsado = 6;
    proc.valor = 16;

    Linsere(&listaProc,proc,0);

    proc.estado = 0;
    strcpy(proc.fname,"proc2");
    proc.pc = 0;
    proc.pid = 2;
    proc.ppid = 2;
    proc.prioridade = 2;
    strcpy(proc.prog,"proc2.prog");
    proc.tempoInicio = 0;
    proc.tempoUsado = 6;
    proc.valor = 97;

    Linsere(&listaProc,proc,0);

    Limprime(&listaProc);
    return 0;
}
