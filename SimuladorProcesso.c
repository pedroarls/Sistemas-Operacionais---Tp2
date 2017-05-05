#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "SimuladorProcesso.h"
#include <string.h>


Processo criarProcesso(int pid, int ppid, int prioridade, int pc, int valor,int tempoInicio, int tempoUsado, char *nomeArquivo)
{
    Processo processo;
    processo.pid = pid;
    processo.ppid = ppid;
    processo.prioridade = prioridade;
    processo.pc = pc;
    processo.valor = valor;
    processo.tempoInicio = tempoInicio;
    processo.tempoUsado = tempoUsado;
    strcpy(processo.nomeArquivo, nomeArquivo);
    LePrograma(processo.nomeArquivo, &processo);
    return processo;
}

Processo duplicaProcesso(Processo processoPai, int pidFilho, int tempoAtual)
{
    Processo copia;
    copia.pid = pidFilho;
    copia.ppid = processoPai.pid;
    copia.prioridade = processoPai.prioridade;
    copia.pc = processoPai.pc;  // Execute the instruction immediately after F instruction.
    copia.valor = processoPai.valor;
    copia.tempoInicio = tempoAtual;
    copia.tempoUsado = 0;
    copia.tamanhoPrograma = processoPai.tamanhoPrograma;
    strcpy(copia.nomeArquivo, processoPai.nomeArquivo);
    LePrograma(copia.nomeArquivo,&copia);
    return copia;
}

void mostra(Tlista* listaProcessos, Processo TabelaProcessos[])
{
    Processo proc;
    Tcelula* aux;

    if (LEhVazia(listaProcessos))
    {
        printf("Lista vazia\n");
        return ;
    }
    aux = listaProcessos->pprimeiro->pprox;
    while (aux!=NULL)
    {
        proc = TabelaProcessos[aux->pid];
        printf("pc, pid, ppid, priority, value, start time, CPU time used so far\n");
        printf("%2d, %3d,  %3d, %8d, %5d, %10d, %3d\n",
               proc.pc, proc.pid, proc.ppid, proc.prioridade,
               proc.valor, proc.tempoInicio, proc.tempoUsado);
        aux = aux->pprox;
    }
    printf("\n");
    return;
}

void mostrarPorPrioridade(Tlista* lista,Processo pcbTable[], int prioridade)
{
    Processo proc;
    int count = 0;
    Tcelula* auxCelula = lista->pprimeiro->pprox;

    if(LEhVazia(lista))
    {
        printf("A fila esta vazia;");
        return;
    }

    while (auxCelula != NULL)
    {
        if(pcbTable[auxCelula->pid].prioridade == prioridade)
        {
            proc = pcbTable[auxCelula->pid];
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

void cpu2proc(CPU *cpu,Processo *proc)
{
    proc->pc = cpu->pc;
    proc->pid = cpu->pid;
    proc->valor = cpu->valor;
    proc->tempoUsado = cpu->tempoRestante;
    return;
}

void proc2cpu(Processo *proc, CPU*cpu)
{
    cpu->pc =  proc->pc;
    cpu->pid = proc->pid;
    cpu->valor = proc->valor;
    cpu->fatiaTempo = quantum[proc->prioridade];
    if(proc->tempoUsado > 0)  // When blocked process is assigned.
    {
        printf("Blocked process was assigned to CPU.\n");
        cpu->tempoRestante= proc->tempoUsado;
    }
    else   // When new process is assigned.
    {
        printf("New process was assigned to CPU.\n");
        cpu->tempoRestante = cpu->fatiaTempo; //???
    }
    return;
}

/* Set next priority based on a give priority. */
void incrementaPrioridade(Processo *p)
{
    if(p->prioridade != 3)
    {
        p->prioridade += 1;
    }
    return;
}

/*Calcula o tempo de resposta para um processo de acordo tempo atual*/
int calculaTempoResposta(int tempoAtual, Processo p)
{
    return tempoAtual - p.tempoInicio;
}

/*Calcula o tempo médio de resposta para todos os processos.*/
double calculaTempoMedioResposta(TabelaTempos tempoProcessos)
{
    int i;
    int total = 0;
    for(i=0; i<tempoProcessos.quantidadeProcessos; i++)
    {
        total += tempoProcessos.tempos[i];
    }
    if(tempoProcessos.quantidadeProcessos == 0)
    {
        return 0;
    }
    else
    {
        return total/tempoProcessos.quantidadeProcessos;
    }
}

/*Lê um programa por um determinado nome de arquivo e o armazena em uma matriz*/
int LePrograma(char *nomeArquivo, Processo* proc)
{
    FILE *arq;
    char buff[MAX_TAM_STRING];
    int linha, coluna, i, j;

    for(linha=0; linha<MAX_QTD_LINHAS; linha++) //Limpa o espaço para armazenar o programa
    {
        for(coluna=0; coluna<MAX_TAM_STRING; coluna++)
        {
            proc->programa[linha][coluna] = '\0';
        }
    }

    arq = fopen(nomeArquivo, "r");
    if(arq == NULL)
    {
        printf("Can't open the file: '%s'\n", nomeArquivo);
        exit(1);
    }

    i=0;

    while((fgets(buff, MAX_TAM_STRING, arq)!= NULL))
    {

        j=0;
        //deleta qebra de linhas
        while(buff[j] != '\0')
        {
            if(buff[j] == '\n') buff[j] = '\0';
            j++;
        }
        //printf("%s\n",buff);
        strcpy(proc->programa[i], buff);
        i++;
    }
    proc->tamanhoPrograma = i;
    fclose(arq);
    return(0);
}

/* Quebra a string por espaços e retorna um array*/
char **quebraInstrucao(int *n, char *programa)
{
    char **array=NULL;
    char *p=programa;
    char *s;

    for(*n=0; (s = strtok(p, " ")) != NULL; (*n)++)
    {
        array = (char**)realloc(array, sizeof(char*) * (*n+1));
        array[*n] = s;
        p = NULL;
    }

    return array;
}
//
////  Copiando arquivo descritor de entrada pro de saída
//
void copiaArquivo(FILE *arquivoEntrada, FILE *arquivoSaida)
{
    char bufferEntrada[TAM_BUFFER];
    while (fgets(bufferEntrada, TAM_BUFFER, arquivoEntrada) != NULL)
    {

        fputs(bufferEntrada, arquivoSaida);
        fflush(arquivoSaida); //garantindo que o arquivo seja salvo, força a salvar alterações em disco
    }
}


/* Commander process that manipulates inputs from users,
   and send them to Process Manager process. */
void ProcessCommander(char* nomeArquivo, char*  arqComandos, int flagComandos, int flagEscalonamento)
{
    //FILE *fp = fdopen(wfd, "w");
    int status,pid;
    int fd[2]; ///Descritores de arquivo do pipe, posição 0 leitura, posição 1 escrita
    //char cmd[MAX_TAM_STRING];
    FILE *arq,*arq1;
    FLvazia(&Prontos);
    FLvazia(&Executando);
    FLvazia(&Bloqueados);

    if(pipe(fd)<0)
    {
        printf("Erro no pipe!\n");
        exit(1);
    }

    pid=fork();

    if(pid<0)
    {
        printf("Erro na criação do processo filho.\n");
        exit(1);
    }

    else if (pid>0)  //Está executando o processo pai
    {
        close(fd[0]);//Fecha o arquivo de leitura do pipe, com base no descritor

        arq=fdopen(fd[1], "w"); //Abre o arquivo de leitura do pipe, com base no descrito
        if (arq == NULL)
        {
            printf("Erro na leitura do arquivo do descritor.\n");
            exit(1);
        }


        if(flagComandos ==  1)
        {
            printf("Comando >");
            copiaArquivo(stdin, arq);//Copia dados da entrada padrão para o arquivo de escrita do pipe
            fflush(stdin);
            fclose(arq);
        }
        else
        {
            arq1 = fopen(arqComandos,"r");
            copiaArquivo(arq1, arq);//Copia dados do arquivo
            fclose(arq);
        }

        if(wait(&status) == -1)//Se ocorreu um erro no processo filho
        {
            printf("Erro no processo filho.\nAbortando execução...\n");
            exit(1);
        }
    }
    else //Está executando o processo filho
    {
        close(fd[1]);
        ProcessManager(fd[0],nomeArquivo,flagEscalonamento);
    }
}
//
///* Report the current status of system. */
void reporterProcess(int descritorEscrita, Processo tabelaProcessos[], int tempo, TabelaTempos tabTempos,
                     Tlista estadoExecucao, Tlista estadoPronto, Tlista estadoBloqueado)
{
    int i;
    printf("-------------ESTADO ATUAL DO SISTEMA------------\n");
    printf("-> Tempo atual: %d\n", tempo);
    printf("-> Tempo médio de resposta: %f.\n", calculaTempoMedioResposta(tabTempos));
    printf("\n");
    printf("PROCESSOS EM EXECUCCAO\n");
    mostra(&estadoExecucao, tabelaProcessos);

    printf("\n");
    printf("PROCESSOS BLOQUEADOS:\n");
    printf("Lista de processos bloqueados\n");
    mostra(&estadoBloqueado, tabelaProcessos);

    printf("\n");
    printf("PROCESSOS PRONTOS PARA EXECUTAR\n");

    for(i=0; i<MAX_PRIORIDADE; i++)
    {
        printf("Lista de processos com prioridade %d:\n", i);
        mostrarPorPrioridade(&estadoPronto, tabelaProcessos, i);
    }

    close(descritorEscrita); // Pipe Synchronized.
    exit(1);
}


void ProcessManager(int descritorLeitura, char *programa,int flagEscalonamento)
{
    FILE *arq = fdopen(descritorLeitura, "r");
    int fd[2];
    char buffer[MAX_TAM_STRING];


    char **instrucoes;
    int c, n;
    int pid_count;
    int arg;
    int i;
    int esperaDesbloquear = 0;

    //Processo temp_proc;
    int valorTemp;
    int pidTemp;
    char nomeArqTemp[MAX_TAM_STRING];
    int temp_index;



    // Inicializando fatias de tempo
    quantum[0] = 1;
    quantum[1] = 2;
    quantum[2] = 4;
    quantum[3] = 8;

    pid_count = 0;
    tabTempos.quantidadeProcessos = 0;
    tabTempos.tempos[0] = 0;
    tempoAtual = 0;
    cpu.pc = 0;
    cpu.pid = 0;
    cpu.valor = 0;
    cpu.fatiaTempo = quantum[0];
    cpu.tempoRestante = cpu.fatiaTempo;

    TabelaDeProcessos[cpu.pid] = criarProcesso(pid_count, -1, 0, cpu.pc, cpu.valor,tempoAtual, quantum[0]-tempoAtual,programa);
    pid_count++;

    FLvazia(&Prontos);
    FLvazia(&Bloqueados);
    FLvazia(&Executando); // TODO: Re-thinking of this structure needed.

    Linsere(&Executando, cpu.pid,-1);//guarda somente o pid do processo,
    //Para acessar um processo tem que ir na tabela de processos

    if((fgets(buffer,MAX_TAM_STRING, arq) == NULL))//Conferir depois
    {
        printf("Error.");
    }

    while (fgets(buffer, MAX_TAM_STRING, arq)!= NULL)
    {
        printf("Comando = %s",buffer);
        if(!strcmp(buffer, "Q\n") || !strcmp(buffer, "q\n")) //Aceita maiúscula e minúscula
        {
            if(esperaDesbloquear == 1)
            {
                printf("Ha somente processos bloqueados, esperandos desbloquear.\n");
                printf("\n");
                printf("\nComando > ");
                fflush(stdout);
                continue;
            }

            printf("Fim de uma unidade de tempo\n");
            printf("Instrucao = '%s'\n", TabelaDeProcessos[cpu.pid].programa[cpu.pc]);
            if(!strcmp(TabelaDeProcessos[cpu.pid].programa[cpu.pc], ""))
            {
                printf("Instrucoes acabaram inesperadamente. Forcando saida\n");
                sprintf(instrucoes[0], "E");
            }
            else
            {
                instrucoes = quebraInstrucao(&n, TabelaDeProcessos[cpu.pid].programa[cpu.pc]);
            }

            tempoAtual++;
            cpu.pc++;
            cpu.tempoRestante--;

            if(!strcmp(instrucoes[0], "S"))
            {
                printf("Colocando o valor da variavel em %d.\n", atoi(instrucoes[1]));
                valorTemp = cpu.valor;
                cpu.valor = atoi(instrucoes[1]);
                printf("Valor atual na CPU: %d -> %d\n", valorTemp, cpu.valor);

            }
            else if(!strcmp(instrucoes[0], "A"))
            {
                printf("Somando %d ao valor da variável.\n", atoi(instrucoes[1]));
                valorTemp = cpu.valor;
                cpu.valor += atoi(instrucoes[1]);
                printf("Valor da CPU: %d -> %d\n", valorTemp, cpu.valor);

            }
            else if(!strcmp(instrucoes[0], "D"))
            {
                printf("Subtraindo %d do valor da variável\n", atoi(instrucoes[1]));
                valorTemp = cpu.valor;
                cpu.valor -= atoi(instrucoes[1]);
                printf("Valor da CPU: %d -> %d\n", valorTemp, cpu.valor);

            }
            else if(!strcmp(instrucoes[0], "B"))
            {
                printf("Bloqueando o processo simulado atual.\n");
                Lremove(&Executando);
                cpu2proc(&cpu, &TabelaDeProcessos[cpu.pid]);
                printf("Processo(pid=%d) foi bloqueado.\n", cpu.pid);
                Linsere(&Bloqueados, cpu.pid,-1);

            }
            else if(!strcmp(instrucoes[0], "E"))
            {
                printf("Terminando o processo simulado\n");
                Lremove(&Executando);
                printf("pid=%d está terminado.\n", cpu.pid);
                tabTempos.tempos[tabTempos.quantidadeProcessos] = calculaTempoResposta(tempoAtual, TabelaDeProcessos[cpu.pid]);
                tabTempos.quantidadeProcessos++;
            }
            else if(!strcmp(instrucoes[0], "F"))
            {
                printf("Criando processo filho e pulando %d instrucoes do pai.\n", atoi(instrucoes[1]));
                arg = atoi(instrucoes[1]);

                cpu2proc(&cpu, &TabelaDeProcessos[cpu.pid]);
                /* Duplica o processo e o coloca na lista de Prontos */
                TabelaDeProcessos[pid_count] = duplicaProcesso(TabelaDeProcessos[cpu.pid], pid_count,tempoAtual);
                pid_count++;
                Linsere(&Prontos, pid_count-1,-1);
                printf("Criou um processo(pid=%d).\n", pid_count-1);

                cpu.pc += arg; // Executa n instruções depois da próxima instrução.

            }
            else if(!strcmp(instrucoes[0],"R"))
            {
                printf("Substituindo o programa do processo simulado pelo programa no arquivo '%s'.\n", instrucoes[1]);
                strcpy(nomeArqTemp, instrucoes[1]);
                cpu.pc = 0;
                cpu.valor = 0;

                LePrograma(nomeArqTemp, &TabelaDeProcessos[cpu.pid]);
            }

            else
            {
                printf("Instrução desconhecida\n");
                printf("Erro!Saindo...\n");
                return;
            }
            if(flagEscalonamento==1)
            {
                Escalonamento(&esperaDesbloquear,fd,&pidTemp);
            }
            else if(flagEscalonamento==2)
            {
                EscalonamentoGrupo(&esperaDesbloquear,fd,&pidTemp);
            }
            else //Se digitar uma opção inválida
            {
                Escalonamento(&esperaDesbloquear,fd,&pidTemp);
            }

            free(instrucoes);

        }
        else if(!strcmp(buffer, "U\n") || !strcmp(buffer, "u\n"))
        {
            printf("Unblock the first simulated process in blocked queue.\n");
            temp_index = Lremove(&Bloqueados);
            if(temp_index == -1)
            {
                printf("There are no states in blocked queue.\n");
            }
            else
            {
                printf("pid=%d moves from blocked queue to ready queue.\n", temp_index);
                Linsere(&Prontos, temp_index,-1);
                esperaDesbloquear = 0;
            }

        }
        else if(!strcmp(buffer, "P\n") || !strcmp(buffer, "p\n"))
        {
            printf("Print the current state of the system.\n");
            if (pipe(fd))
            {
                perror("pipe");
            }
            else if ((pidTemp = fork()) == -1)
            {
                perror("fork");
            }
            else if (pidTemp == 0)
            {
                close(fd[0]);
                if(DEBUG) cpu2proc(&cpu, &TabelaDeProcessos[cpu.pid]);
                reporterProcess(fd[1], TabelaDeProcessos, tempoAtual, tabTempos,
                                Executando, Prontos, Bloqueados);
            }
            else
            {
                close(fd[1]);
                while((i=(read(fd[0],&c,1))) > 0); // Pipe Synchronization
            }

        }
        else if(!strcmp(buffer, "T\n") || !strcmp(buffer, "t\n"))
        {
            printf("Print the average turnaround time, and terminate the system.\n");
            // Calculating average turnaround time.
            printf("Average turn around time is %f.\n", calculaTempoMedioResposta(tabTempos));
            if (pipe(fd))
            {
                perror("pipe");
            }
            else if ((pidTemp = fork()) == -1)
            {
                perror("fork");
            }
            else if (pidTemp == 0)
            {
                close(fd[0]);
                if(DEBUG)
                    cpu2proc(&cpu, &TabelaDeProcessos[cpu.pid]);

                reporterProcess(fd[1], TabelaDeProcessos, tempoAtual, tabTempos,
                                Executando, Prontos, Bloqueados);
            }
            else
            {
                close(fd[1]);
                while((i=(read(fd[0],&c,1))) > 0); // Pipe Synchronization
            }
            printf("\n");
            printf("Terminate the system.\n");
            return;

        }
        else
        {
            printf("Unknown command.\n");
        }
        //fputs(buffer, stdout);
        //printf("\n");
        printf("\nComando > ");
        fflush(stdout);
    }
    fclose(arq);
}

void Escalonamento(int* esperaDesbloquear, int fd[], int* pidTemp)
{
    int i;
    char c;

    if(LEhVazia(&Prontos))  // No processes in the Ready queue, so skipped.
    {
        if(!LEhVazia(&Executando))
        {
            printf("No ready processes, so continue to run the current process.\n");
        }
        else if(!LEhVazia(&Bloqueados))
        {
            printf("Only blocked processes remain, so waiting for unblocking.\n");
            *esperaDesbloquear = 1;
        }
        else
        {
            // All processes were finished -> finished execution.
            printf("=== RESULT ===\n");
            if (pipe(fd))
            {
                perror("pipe");
            }
            else if ((*pidTemp = fork()) == -1)
            {
                perror("fork");
            }
            else if (*pidTemp == 0)
            {
                close(fd[0]);
                if(DEBUG) cpu2proc(&cpu, &TabelaDeProcessos[cpu.pid]);
                reporterProcess(fd[1], TabelaDeProcessos, tempoAtual, tabTempos,
                                Executando, Prontos, Bloqueados);
            }
            else
            {
                close(fd[1]);
                while((i=(read(fd[0],&c,1))) > 0); // Pipe Synchronization
            }
            printf("\n=== END OF SYSTEM ===\n");
            exit(0);
            return;
        }


    }
    else if(LEhVazia(&Executando))   // When process was blocked or terminated.
    {
        printf("There are no process running, so assign the first process in the queue to CPU.\n");
        *pidTemp = Lremove(&Prontos);
        proc2cpu(&TabelaDeProcessos[*pidTemp], &cpu);
        Linsere(&Executando, *pidTemp,-1);
        printf("Assigned: cpu <--- pcbTable[%d]\n", *pidTemp);

    }
    else if(cpu.tempoRestante<= 0)    // When quantum expired
    {
        printf("Quantum was expired, so assign the first process in the que to CPU.\n");
        incrementaPrioridade(&TabelaDeProcessos[cpu.pid]);
        printf("Pid(%d)'s priority class was raised to %d.\n",
               cpu.pid, TabelaDeProcessos[cpu.pid].prioridade);
        cpu2proc(&cpu, &TabelaDeProcessos[cpu.pid]);
        Linsere(&Prontos, cpu.pid,-1);
        *pidTemp = Lremove(&Executando);

        proc2cpu(&TabelaDeProcessos[Lremove(&Prontos)], &cpu);
        Linsere(&Executando, cpu.pid,-1);
        printf("Trocado: cpu(%d) <--> pid(%d)\n", *pidTemp, cpu.pid);

    }
    else if(cpu.tempoRestante> 0)
    {
        printf("CPU Time is still remained, so continue to run the current process.\n");

    }
    else
    {
        printf("Erro ao escalonar.\n");
    }

}

void EscalonamentoGrupo(int* esperaDesbloquear, int fd[], int* pidTemp)
{
    char c;
    int i,posicao=0;
    int pidMenor, menorTamanho;
    Tcelula* auxMenor;

    Processo proc;
    Tcelula* aux;

    if (LEhVazia(&Prontos))
    {
        if(!LEhVazia(&Executando))
        {
            printf("Continua executando o processo atual...\n");
        }
        else if(!LEhVazia(&Bloqueados))
        {
            printf("Somente processsos bloqueados. Desbloqueie algum (Comando: U ou u).\n");
            *esperaDesbloquear = 1;
        }
        else
        {
            // Todos os processos acabaram
            printf("RESULTADO\n");
            if (pipe(fd))
            {
                perror("pipe");
            }
            else if ((*pidTemp = fork()) == -1)
            {
                perror("fork");
            }
            else if (*pidTemp == 0) //processo filho
            {
                close(fd[0]);//Encerrando a leitura de comandos
                if(DEBUG)
                {
                    cpu2proc(&cpu, &TabelaDeProcessos[cpu.pid]);
                }
                reporterProcess(fd[1], TabelaDeProcessos, tempoAtual, tabTempos,Executando, Prontos, Bloqueados);
            }
            else
            {
                close(fd[1]);
                while((i=(read(fd[0],&c,1))) > 0); // Pipe Synchronization
            }
            printf("\n=== FIM SISTEMA ===\n");
            exit(0);
            return;
        }
    }
    else if(LEhVazia(&Executando))
    {
        aux = Prontos.pprimeiro;
        menorTamanho = TabelaDeProcessos[aux->pprox->pid].tamanhoPrograma;
        auxMenor = aux;
        aux = aux->pprox;

        while (aux->pprox!=NULL)
        {
            if(TabelaDeProcessos[aux->pprox->pid].tamanhoPrograma < menorTamanho )
            {
                pidMenor = TabelaDeProcessos[aux->pprox->pid].pid;
                menorTamanho = TabelaDeProcessos[aux->pprox->pid].tamanhoPrograma;
                auxMenor = aux;
            }
            aux = aux->pprox;
        }

        LRetiraNoMeio(auxMenor,&Prontos,pidTemp);
        proc2cpu(&TabelaDeProcessos[*pidTemp], &cpu);
        Linsere(&Executando, *pidTemp,-1);
        printf("Colocando na CPU processo de menor tamanho...\n");
        printf("Tamanho do programa: %d.\n",TabelaDeProcessos[*pidTemp].tamanhoPrograma);
        printf("Pid do processo: %d.\n", *pidTemp);

    }
    else if(cpu.tempoRestante <= 0)    // When quantum expired
    {
        printf("O tempo do processo acabou, selecionando o processo PRONTO com o menor programa.\n");
        incrementaPrioridade(&TabelaDeProcessos[cpu.pid]);
        printf("A nova prioridade é %d.\n", cpu.pid);
        cpu2proc(&cpu, &TabelaDeProcessos[cpu.pid]);

        *pidTemp = Lremove(&Executando);
        Linsere(&Prontos, cpu.pid,-1);

        aux = Prontos.pprimeiro;
        menorTamanho = TabelaDeProcessos[aux->pprox->pid].tamanhoPrograma;
        auxMenor = aux;

        aux = aux->pprox;

        while (aux->pprox!=NULL)
        {
            if(TabelaDeProcessos[aux->pprox->pid].tamanhoPrograma < menorTamanho )
            {
                pidMenor = TabelaDeProcessos[aux->pprox->pid].pid;
                menorTamanho = TabelaDeProcessos[aux->pprox->pid].tamanhoPrograma;
                auxMenor = aux;
            }
            aux = aux->pprox;
        }
        LRetiraNoMeio(auxMenor,&Prontos,pidTemp);

        printf("\nTrocando: Processo na cpu(%d) <--> Processo tabela de processos pid(%d)\n", cpu.pid, pidMenor);
        proc2cpu(&TabelaDeProcessos[pidMenor], &cpu);
        Linsere(&Executando, cpu.pid,-1);

    }
    else if(cpu.tempoRestante> 0)
    {
        printf("Ainda sobra tempo de execução para o processo atual. Continuando a sua execução...\n");

    }
    else
    {
        printf("Erro ao escalonar.\n");
    }

}


