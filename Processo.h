#ifndef PROCESSO_H_INCLUDED
#define PROCESSO_H_INCLUDED

#define MAX_TAM_STRING 20
#define MAX_QTD_LINHA 200

typedef struct TipoProcesso{
    int pid;
    int ppid;
    int pc;
    int valor;
    int prioridade;
    int estado;
    int tempoInicio;
    int tempoUsado;
    char fname[MAX_TAM_STRING]; // Nome do arquivo para ler um programa
    char prog[MAX_QTD_LINHA][MAX_TAM_STRING]; // Programa do processo simulado.
}Processo;

#endif // PROCESSO_H_INCLUDED
