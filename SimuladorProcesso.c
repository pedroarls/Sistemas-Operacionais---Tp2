#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Lista.h"
#include "SimuladorProcesso.h"

Processo criarProcesso(int pid, int ppid, int prioridade, int pc, int valor,
			int tempoInicio, int tempoUsado, char *nomeArquivo){
  Processo processo;
  processo.pid = pid;
  processo.ppid = ppid;
  processo.prioridade = prioridade;
  processo.pc = pc;
  processo.valor = valor;
  processo.tempoInicio = tempoInicio;
  processo.tempoUsado = tempoUsado;
  strcpy(processo.nomeArquivo, nomeArquivo);
  //readProgram(processo.nomeArquivo, processo.programa);
  return processo;
}

Processo CriarFilho(Processo processoPai, int pidFilho, int tempoAtual){
  Processo copia;
  copia.pid = pidFilho;
  copia.ppid = processoPai.pid;
  copia.prioridade = processoPai.prioridade;
  copia.pc = processoPai.pc;  // Execute the instruction immediately after F instruction.
  copia.valor = processoPai.valor;
  copia.tempoInicio = tempoAtual;
  copia.tempoUsado = 0;
  strcpy(copia.nomeArquivo, processoPai.nomeArquivo);
  //readProgram(cp.fname, cp.prog);
  return copia;
}

void mostrarPorPrioridade(Tlista* lista,Processo pcbTable[], int prioridade){
    Processo proc;
    int count = 0;
    Tcelula* auxCelula = lista->pprimeiro->pprox;

    if(LEhVazia(lista)){
        printf("A fila esta vazia;");
        return;
    }

    while (auxCelula != NULL){
        if(pcbTable[auxCelula->processo.pid].prioridade == prioridade){
          proc = pcbTable[auxCelula->processo.pid];
          printf("pc, pid, ppid, prioridade, valor, tempo de incio, tempo de CPU usado at agra\n");
          printf("%2d, %3d,  %3d, %8d, %5d, %10d, %3d\n",
             proc.pc, proc.pid, proc.ppid, proc.prioridade,
             proc.valor, proc.tempoInicio, proc.tempoUsado);
          count++;
        }
    auxCelula = auxCelula->pprox;
    }

    printf("\n");
    return;
}


/* Store a given cpu to a given process. */
void cpu2proc(CPU *cpu,Processo *proc){
  proc->pc = cpu->pc;
  proc->pid = cpu->pid;
  proc->valor = cpu->valor;
  proc->tempoUsado = cpu->tempoCPU;
  return;
}

void proc2cpu(Processo *proc, CPU*cpu){
  cpu->pc =  proc->pc;
  cpu->pid = proc->pid;
  cpu->valor = proc->valor;
  cpu->fatiaTempo = quantum[proc->prioridade];
  if(proc->tempoUsado > 0){ // When blocked process is assigned.
    printf("Blocked process was assigned to CPU.\n");
    cpu->tempoCPU = proc->tempoUsado;
  }else{ // When new process is assigned.
    printf("New process was assigned to CPU.\n");
    cpu->tempoCPU = cpu->fatiaTempo; //???
  }
  return;
}

/* Set next priority based on a give priority. */
void incrementaPrioridade(Processo *p){
    if(p->prioridade != CLASSE_3){
        p->prioridade += 1;
    }
    return;
}

/*Calcula o tempo de resposta para um processo de acordo tempo atual*/
int calculaTempoResposta(int tempoAtual, Processo p){
  return tempoAtual - p.tempoInicio;
}

/*Calcula o tempo médio de resposta para todos os processos.*/
double calculaTempoMedioResposta(TabelaTempos tempoProcessos){
  int i;
  int total = 0;
  for(i=0; i<tempoProcessos.quantidadeProcessos; i++){
    total += tempoProcessos.tempos[i];
  }
  if(tempoProcessos.count == 0){
    return 0;
  }else{
    return total/tempoProcessos.quantidadeProcessos;
  }
}

/*Lê um programa por um determinado nome de arquivo e o armazena em uma matriz*/
int LePrograma(char *nomeArquivo, char programa[][MAX_TAM_STRING]){
  FILE *arq;
  char buff[MAX_TAM_STRING],;
  int linha, coluna, i, j;

  for(linha=0;linha<MAX_QTD_LINHAS;linha++){
    for(coluna=0;coluna<MAX_TAM_STRING;coluna++){
      programa[linha][coluna] = '\0';
    }
  }

  arq = fopen(nomeArquivo, "r");
  if(arq == NULL){
    printf("Can't open the file: '%s'\n", nomeArquivo);
    exit(1);
  }

  i=0;
  while((fgets(buff, MAX_TAM_STRING, fp)!= NULL){

    j=0;
    //deleta qebra de linhas
    while(buff[j] != '\0'){
      if(buff[j] == '\n') buff[j] = '\0';
      j++;
    }

    strcpy(programa[i], buff);
    i++;
  }

  fclose(arq);
  return(0);
}

/* Quebra a string por espaços e retorna um array*/
char **quebra(int *n, char *string)
{
  char **array=NULL;
  char *p=string;
  char *s;

  for(*n=0; (s = strtok(p, " ")) != NULL; (*n)++) {
    array = (char**)realloc(array, sizeof(char*) * (*n+1));
    array[*n] = s;
    p = NULL;
  }

  return array;
}





