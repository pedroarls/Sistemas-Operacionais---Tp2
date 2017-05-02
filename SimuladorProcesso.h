#ifndef PROCESSO_H_INCLUDED
#define PROCESSO_H_INCLUDED

#define MAX_TAM_STRING 50
#define MAX_QTD_LINHAS 200
#define MAX_QTD_PROCESSOS 200
#define TAM_BUFFER 20

#define QTD_PRIORIDADE 4


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

void copiaArquivo(FILE *arquivoEntrada, FILE *arquivoSaida);
char **quebraPrograma(int *n, char *programa);
void cpu2proc(CPU *cpu,Processo *proc);
int calculaTempoResposta(int tempoAtual, Processo p);

Processo criarProcesso(int pid, int ppid, int prioridade, int pc, int valor,int tempoInicio, int tempoUsado, char *nomeArquivo);
Processo duplicaProcesso(Processo processoPai, int pidFilho, int tempoAtual);

void ProcessCommander(char* nomeArquivo);
void ProcessManager(int descritorLeitura, char *programa);

#endif // PROCESSO_H_INCLUDED
