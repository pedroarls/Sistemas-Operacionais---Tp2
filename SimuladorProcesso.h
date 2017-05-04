#ifndef PROCESSO_H_INCLUDED
#define PROCESSO_H_INCLUDED

#include "Lista.h"

#define MAX_TAM_STRING 50
#define MAX_QTD_LINHAS 200
#define MAX_QTD_PROCESSOS 200
#define TAM_BUFFER 20
#define DEBUG 1

#define MAX_PRIORIDADE 4


int quantum[4];

typedef struct TipoProcesso
{
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
} Processo;

typedef struct  TipoCPU
{
    int pc;
    int pid;
    int valor;
    int fatiaTempo;
    int tempoRestante;
} CPU;

typedef struct tempos
{
    int tempos[MAX_QTD_PROCESSOS];
    int quantidadeProcessos;
} TabelaTempos;


    int tempoAtual;
    CPU cpu;
    Processo TabelaDeProcessos[MAX_QTD_PROCESSOS];
    TabelaTempos tabTempos; //Para calcular a média do tempo de resposta

    Tlista Prontos;
    Tlista Bloqueados;
    Tlista Executando;

void copiaArquivo(FILE *arquivoEntrada, FILE *arquivoSaida);
char **quebraInstrucao(int *n, char *programa);
void cpu2proc(CPU *cpu,Processo *proc);
int calculaTempoResposta(int tempoAtual, Processo p);

Processo criarProcesso(int pid, int ppid, int prioridade, int pc, int valor,int tempoInicio, int tempoUsado, char *nomeArquivo);
Processo duplicaProcesso(Processo processoPai, int pidFilho, int tempoAtual);

void ProcessCommander(char* nomeArquivo, char*  arqComandos, int flag);
void ProcessManager(int descritorLeitura, char *programa);
void reporterProcess(int descritorLeitura, Processo tabelaProcessos[], int tempo, TabelaTempos tabTempos,Tlista estadoExecucao, Tlista estadoPronto, Tlista estadoBloqueado);
void Escalonamento(int* esperaDesbloquear, int fd[], int* pidTemp);

void proc2cpu(Processo *proc, CPU* cpu);

#endif // PROCESSO_H_INCLUDED
