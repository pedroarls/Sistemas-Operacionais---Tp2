#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>ř
#include <sys/types.h>
#include <sys/wait.h>
#include "Lista.h"
#include "SimuladorProcesso.h"

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
    //readProgram(processo.nomeArquivo, processo.programa);
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
    strcpy(copia.nomeArquivo, processoPai.nomeArquivo);
    LePrograma(copia.nomeArquivo, copia.programa);
    return copia;
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
int LePrograma(char *nomeArquivo, char programa[][MAX_TAM_STRING])
{
    FILE *arq;
    char buff[MAX_TAM_STRING];
    int linha, coluna, i, j;

    for(linha=0; linha<MAX_QTD_LINHAS; linha++)
    {
        for(coluna=0; coluna<MAX_TAM_STRING; coluna++)
        {
            programa[linha][coluna] = '\0';
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

        strcpy(programa[i], buff);
        i++;
    }

    fclose(arq);
    return(0);
}

/* Quebra a string por espaços e retorna um array*/
char **quebraPrograma(int *n, char *programa)
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
void ProcessCommander(char* nomeArquivo)
{
    //FILE *fp = fdopen(wfd, "w");
    int status,pid;
    int fd[2]; ///Descritores de arquivo do pipe, posição 0 leitura, posição 1 escrita
    char cmd[MAX_TAM_STRING];
    FILE *arq;

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
        copiaArquivo(stdin, arq);//Copia dados da entrada padrão para o arquivo de escrita do pipe
        fclose(arq);

        if(wait(&status) == -1)//Se ocorreu um erro no processo filho
        {
            printf("Erro no processo filho.\nAbortando execução...\n");
            exit(1);
        }
    }
    else //Está executando o processo filho
    {
        close(fd[1]);
        ProcessManager(fd[0],nomeArquivo);
    }
}
//
///* Report the current status of system. */
//void reporterProcess(int wfd, Processo pcbTable[], int time, struct TA_TIME ta,
//                     QUE *s_run, QUE *s_ready, QUE *s_block)
//{
//    int i;
//    printf("*********************************************\n");
//    printf("The current system state is as follows:\n");
//    printf("*********************************************\n");
//    printf("CURRENT TIME: %d\n", time);
//    printf("AVERAGE TURN AROUND TIME: %f.\n", calc_ta_time_avg(ta));
//    printf("\n");
//    printf("RUNNING PROCESS:\n");
//    show(s_run, pcbTable);
//
//    printf("\n");
//    printf("BLOCKED PROCESSES:\n");
//    printf("Queue of blocked processes:\n");
//    show(s_block, pcbTable);
//
//    printf("\n");
//    printf("PROCESSES READY TO EXECUTE:\n");
//
//    for(i=0; i<MAX_PRIORITY; i++)
//    {
//        printf("Queue of processes with priority %d:\n", i);
//        show_by_priority(s_ready, pcbTable, i);
//    }
//
//    close(wfd); // Pipe Synchronized.
//    exit(3);
//}
//

void ProcessManager(int descritorLeitura, char *programa)
{
    FILE *arq = fdopen(descritorLeitura, "r");
    int fd[2];
    char buffer[MAX_TAM_STRING];


    char **instrucoes;
    int c, n;
    int pid_count;
    int arg;
    int i,x,y;
    int esperaDesbloquear = 0;
    TabelaTempos tabTempos; //Para calcula a média do tempo de resposta
    Processo temp_proc;
    int valorTemp;
    int pidTemp;
    char nomeArqTemp[MAX_TAM_STRING];
    int temp_index;


    int tempoAtual;
    CPU cpu;
    Processo TabelaDeProcessos[MAX_QTD_PROCESSOS];

    Tlista Prontos;
    Tlista Bloqueados;
    Tlista Executando;

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


    while (fgets(buffer, BUFSIZ, arq) != NULL)
    {

        printf("Comando = %s",buffer);
        if(!strcmp(buffer, "Q\n") || !strcmp(buffer, "q\n")) //Aceita maiúscula e minúscula
        {
            if(esperaDesbloquear == 1)
            {
                printf("Ha somente processos bloqueados, esperandos desbloquear.\n");
                printf("\n");
                printf("> ");
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
                instrucoes = quebraPrograma(&n, TabelaDeProcessos[cpu.pid].programa[cpu.pc]);
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
                printf("Create %d new simulated process(es).\n", atoi(instrucoes[1]));
                // arg = atoi(instrucoes[1]);

                cpu2proc(&cpu, &TabelaDeProcessos[cpu.pid]);
                /* Duplicate a proc and enqueue it into Ready states queue. */
                TabelaDeProcessos[pid_count] = duplicaProcesso(TabelaDeProcessos[cpu.pid], pid_count,tempoAtual);
                pid_count++;
                Linsere(&Prontos, pid_count-1,-1);
                printf("Created a process(pid=%d).\n", pid_count-1);

                cpu.pc += arg; // Execute N instructions after the next instruction.
                // Not necessary to schdule processes.

            }
            else if(!strcmp(instrucoes[0],"R"))
            {
                printf("Substittuindo o programa do processo simulado pelo programa no arquivo '%s'.\n", instrucoes[1]);
                strcpy(nomeArqTemp, instrucoes[1]);
                cpu.pc = 0;
                cpu.valor = 0;

                LePrograma(nomeArqTemp, TabelaDeProcessos[cpu.pid].programa);
            }
            else
            {
                printf("Instrução desconhecida\n");
                printf("Erro!Saindo...\n");
                return;
            }
//
//            /*** Do scheduling ***/
//            if(Prontos == NULL)  // No processes in the Ready queue, so skipped.
//            {
//                if(Executando != NULL)
//                {
//                    printf("No ready processes, so continue to run the current process.\n");
//                }
//                else if(Bloqueados != NULL)
//                {
//                    printf("Only blocked processes remain, so waiting for unblocking.\n");
//                    esperaDesbloquear = true;
//                }
//                else
//                {
//                    // All processes were finished -> finished execution.
//                    if(err_flg == false) printf("Program was successfully executed.\n");
//                    printf("\n");
//                    printf("=== RESULT ===\n");
//                    if (pipe(fd))
//                    {
//                        perror("pipe");
//                    }
//                    else if ((pidTemp = fork()) == -1)
//                    {
//                        perror("fork");
//                    }
//                    else if (temp_pid == 0)
//                    {
//                        close(fd[0]);
//                        if(DEBUG) cpu2proc(&cpu, &pcbTable[cpu.pid]);
//                        reporterProcess(fd[1], pcbTable, tempoAtual, tabTempos,
//                                        Executando, Prontos, Bloqueados);
//                    }
//                    else
//                    {
//                        close(fd[1]);
//                        while(i=(read(fd[0],&c,1)) > 0); // Pipe Synchronization
//                    }
//                    printf("=== END OF SYSTEM ===\n");
//                    return;
//                }
//
//
//            }
//            else if(Executando == NULL)   // When process was blocked or terminated.
//            {
//                printf("There are no process running, so assign the first process in the queue to CPU.\n");
//                temp_pid = dequeue(&Prontos);
//                proc2cpu(&pcbTable[temp_pid], &cpu);
//                enqueue(&Executando, temp_pid);
//                printf("Assigned: cpu <--- pcbTable[%d]\n", temp_pid);
//
//            }
//            else if(cpu.t_remain <= 0)    // When quantum expired
//            {
//                printf("Quantum was expired, so assign the first process in the que to CPU.\n");
//                set_next_priority(&pcbTable[cpu.pid]);
//                printf("Pid(%d)'s priority class was raised to %d.\n",
//                       cpu.pid, pcbTable[cpu.pid].priority);
//                cpu2proc(&cpu, &pcbTable[cpu.pid]);
//                enqueue(&Prontos, cpu.pid);
//                temp_pid = dequeue(&Executando);
//
//                proc2cpu(&pcbTable[dequeue(&Prontos)], &cpu);
//                enqueue(&Executando, cpu.pid);
//                printf("Swithed: cpu(%d) <--> pid(%d)\n", temp_pid, cpu.pid);
//
//            }
//            else if(cpu.t_remain > 0)
//            {
//                printf("CPU Time is still remained, so continue to run the current process.\n");
//
//            }
//            else
//            {
//                printf("Unknown condition to schedule.\n");
//            }
//            /*** End of Scheduling ***/
//            free(cmd);
//            /* End of One Unit of Time*/
//
//        }
//        else if(!strcmp(buffer, "U\n") || !strcmp(buffer, "u\n"))
//        {
//            printf("Unblock the first simulated process in blocked queue.\n");
//            temp_index = dequeue(&Bloqueados);
//            if(temp_index == -1)
//            {
//                printf("There are no states in blocked queue.\n");
//            }
//            else
//            {
//                printf("pid=%d moves from blocked queue to ready queue.\n", temp_index);
//                enqueue(&Prontos, temp_index);
//                esperaDesbloquear = false;
//            }
//
//        }
//        else if(!strcmp(buffer, "P\n") || !strcmp(buffer, "p\n"))
//        {
//            printf("Print the current state of the system.\n");
//            if (pipe(fd))
//            {
//                perror("pipe");
//            }
//            else if ((temp_pid = fork()) == -1)
//            {
//                perror("fork");
//            }
//            else if (temp_pid == 0)
//            {
//                close(fd[0]);
//                if(DEBUG) cpu2proc(&cpu, &pcbTable[cpu.pid]);
//                reporterProcess(fd[1], pcbTable, tempoAtual, ta,
//                                Executando, Prontos, Bloqueados);
//            }
//            else
//            {
//                close(fd[1]);
//                while(i=(read(fd[0],&c,1)) > 0); // Pipe Synchronization
//            }
//
//        }
//        else if(!strcmp(buffer, "help\n"))
//        {
//            printf("\
//     The following commands are accepted:\n\
//     Q:End of one unitof time\n\
//     - CPU consumes 1 instruction from programs, andexecuteit.\n\
//     U: Unblockthe first simulated process in blocked queue\n\
//     - If there is ablockedprocess, move its statefrom Blocked toReady.\n\
//     P: Print the current state of the system.\n \
//        - The state include PC, PID, PPID, Priority, Value, Time, etc.\n\
//     T: Terminate the system after printing the current state.\n\
//        - The printing is same as 'P' command.\n");
//
//        }
//        else if(!strcmp(buffer, "T\n") || !strcmp(buffer, "t\n"))
//        {
//            printf("Print the average turnaround time, and terminate the system.\n");
//            // Calculating average turnaround time.
//            printf("Average turn around time is %f.\n", calc_ta_time_avg(ta));
//            if (pipe(fd))
//            {
//                perror("pipe");
//            }
//            else if ((temp_pid = fork()) == -1)
//            {
//                perror("fork");
//            }
//            else if (temp_pid == 0)
//            {
//                close(fd[0]);
//                if(DEBUG) cpu2proc(&cpu, &pcbTable[cpu.pid]);
//                reporterProcess(fd[1], pcbTable, tempoAtual, ta,
//                                Executando, Prontos, Bloqueados);
//            }
//            else
//            {
//                close(fd[1]);
//                while(i=(read(fd[0],&c,1)) > 0); // Pipe Synchronization
//            }
//            printf("\n");
//            printf("Terminate the system.\n");
//            return;
//
//        }
//        else
//        {
//            printf("Unknown command.\n");
//        }
        fputs(buffer, stdout);
        printf("\n");
        printf("> ");
        fflush(stdout);
        }
        fclose(arq);
    }

}
