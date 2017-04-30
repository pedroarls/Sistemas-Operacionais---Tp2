#ifndef PROCESSO_H_INCLUDED
#define PROCESSO_H_INCLUDED

#define MAX_TAM_STRING 20
#define MAX_QTD_LINHAS 200
#define MAX_QTD_PROCESSOS 200
#define TAM_BUFFER 20

#define QTD_PRIORIDADE 4
#define CLASSE_0 0
#define CLASSE_1 1
#define CLASSE_2 2
#define CLASSE_3 3

typedef struct TipoProcesso{
    int pid;
    int ppid;
    int pc;
    int valor;
    int prioridade;
    int estado;
    int tempoInicio;
    int tempoUsado;
    char nomeArquivo[MAX_TAM_STRING]; // Nome do arquivo para ler um programa
    char programa[MAX_QTD_LINHAS][MAX_TAM_STRING]; // Programa do processo simulado.
}Processo;

typedef struct  TipoCPU{
    int pc;
    int pid;
    int valor;
    int fatiaTempo;
    int tempoCPU;
}CPU;

typedef struct tempos{
  int tempos[MAX_QTD_PROCESSOS];
  int quantidadeProcessos;
}TabelaTempos;

char buffer[TAM_BUFFER];
int quantum[4];

#endif // PROCESSO_H_INCLUDED
