#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Lista.h"
#include "SimuladorProcesso.h"

Processo criarProcesso(int pid, int ppid, int prioridade, int pc, int valor,
                       int tempoInicio, int tempoUsado, char *nomeArquivo)
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

Processo CriarFilho(Processo processoPai, int pidFilho, int tempoAtual)
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
    //readProgram(cp.fname, cp.prog);
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
        if(pcbTable[auxCelula->processo.pid].prioridade == prioridade)
        {
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
void cpu2proc(CPU *cpu,Processo *proc)
{
    proc->pc = cpu->pc;
    proc->pid = cpu->pid;
    proc->valor = cpu->valor;
    proc->tempoUsado = cpu->tempoCPU;
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
        cpu->tempoCPU = proc->tempoUsado;
    }
    else   // When new process is assigned.
    {
        printf("New process was assigned to CPU.\n");
        cpu->tempoCPU = cpu->fatiaTempo; //???
    }
    return;
}

/* Set next priority based on a give priority. */
void incrementaPrioridade(Processo *p)
{
    if(p->prioridade != CLASSE_3)
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
    if(tempoProcessos.count == 0)
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
    char buff[MAX_TAM_STRING],;
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
    while((fgets(buff, MAX_TAM_STRING, fp)!= NULL)
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
char **quebra(int *n, char *string)
{
    char **array=NULL;
    char *p=string;
    char *s;

    for(*n=0; (s = strtok(p, " ")) != NULL; (*n)++)
    {
        array = (char**)realloc(array, sizeof(char*) * (*n+1));
        array[*n] = s;
        p = NULL;
    }

    return array;
}

//  Copiando arquivo descritor de entrada pro de saída

void copy(FILE *fin, FILE *fout)
{
    while (fgets(buffer, BUFSIZ, fin) != NULL)
    {
        fputs(buffer, fout);
        fflush(fout); // arquivo seja salvo

    }
}


/* Commander process that manipulates inputs from users,
   and send them to Process Manager process. */
void commanderProcess(int wfd)
{
    FILE *fp = fdopen(wfd, "w");
    int status;
    char cmd[MAX_STR];

    if (fp == NULL)
    {
        perror("parent: fdopen");
        exit(3);
    }
    copy(stdin, fp);
    fclose(fp);

    if(wait(&status) == -1)
    {
        perror("wait");
        exit(4);
    }
}

/* Report the current status of system. */
void reporterProcess(int wfd, struct Proc pcbTable[], int time, struct TA_TIME ta,
                     QUE *s_run, QUE *s_ready, QUE *s_block)
{
    int i;
    printf("*********************************************\n");
    printf("The current system state is as follows:\n");
    printf("*********************************************\n");
    printf("CURRENT TIME: %d\n", time);
    printf("AVERAGE TURN AROUND TIME: %f.\n", calc_ta_time_avg(ta));
    printf("\n");
    printf("RUNNING PROCESS:\n");
    show(s_run, pcbTable);

    printf("\n");
    printf("BLOCKED PROCESSES:\n");
    printf("Queue of blocked processes:\n");
    show(s_block, pcbTable);

    printf("\n");
    printf("PROCESSES READY TO EXECUTE:\n");

    for(i=0; i<MAX_PRIORITY; i++)
    {
        printf("Queue of processes with priority %d:\n", i);
        show_by_priority(s_ready, pcbTable, i);
    }

    close(wfd); // Pipe Synchronized.
    exit(3);
}

/* Based on inputs from Commander process,
   manage all processes and share a CPU between them. */
void processManagerProcess(int rfd, char *init_program)
{
    FILE *fp = fdopen(rfd, "r");
    int fd[2];

    char **cmd;
    int c, n;
    int pid_count;
    int arg;
    int i,x,y; // iterators
    int err_flg = false;
    // For skipping instructions when only blocked processes are remained.
    int wait4unblocking = false;

    // For calculating average turn around time
    struct TA_TIME ta;

    struct Proc temp_proc;
    int temp_value;
    int temp_pid;
    char temp_fname[MAX_STR];
    int temp_index;

    /* ProcessManager's 6 Data Structures*/
    int current_time;
    struct Cpu cpu;
    struct Proc pcbTable[MAX_PROCS];

    QUE *ready_states;
    QUE *blocked_states;
    QUE *running_states;
    /**************************************/

    /* Initializing */
    quantum[CLASS_0] = 1;
    quantum[CLASS_1] = 2;
    quantum[CLASS_2] = 4;
    quantum[CLASS_3] = 8;

    pid_count = 0;
    ta.count = 0;
    ta.times[0] = 0;
    current_time = 0;
    cpu.pc = 0;
    cpu.pid = 0;
    cpu.value = 0;
    cpu.t_slice = quantum[CLASS_0];
    cpu.t_remain = cpu.t_slice;

    pcbTable[cpu.pid] = create_proc(pid_count, -1, CLASS_0, cpu.pc, cpu.value,
                                    current_time, quantum[CLASS_0]-current_time,
                                    init_program);
    pid_count++;

    ready_states = NULL;
    blocked_states = NULL;
    running_states = NULL; // TODO: Re-thinking of this structure needed.
    enqueue(&running_states, cpu.pid);
    /****************/
    //if(DEBUG) show(running_states, pcbTable);

    while (fgets(buffer, BUFSIZ, fp) != NULL)
    {

        printf("Command = %s",buffer);
        if(!strcmp(buffer, "Q\n") || !strcmp(buffer, "q\n"))
        {
            if(wait4unblocking == true)
            {
                printf("Only blocked processes remain, so waiting for unblocking.\n");
                printf("\n");
                printf("> ");
                fflush(stdout);
                continue;
            }

            printf("End of one unit of time.\n");
            printf("Instruction = '%s'\n", pcbTable[cpu.pid].prog[cpu.pc]);
            if(!strcmp(pcbTable[cpu.pid].prog[cpu.pc], ""))
            {
                printf("Instructions unexpectedy finished, so exit forcedlly with printing.\n");
                sprintf(cmd[0], "E");
                err_flg = true;
            }
            else
            {
                cmd = split(&n, pcbTable[cpu.pid].prog[cpu.pc]);
            }

            current_time++;
            cpu.pc++;
            cpu.t_remain--;

            if(!strcmp(cmd[0], "S"))
            {
                printf("Set the value of the integer variable to %d.\n", atoi(cmd[1]));
                temp_value = cpu.value;
                cpu.value = atoi(cmd[1]);
                printf("CPU value: %d -> %d\n", temp_value, cpu.value);

            }
            else if(!strcmp(cmd[0], "A"))
            {
                printf("Add %d to the value of the integer variable.\n", atoi(cmd[1]));
                temp_value = cpu.value;
                cpu.value += atoi(cmd[1]);
                printf("CPU value: %d -> %d\n", temp_value, cpu.value);

            }
            else if(!strcmp(cmd[0], "D"))
            {
                printf("Substract %d from the value of the integer variable.\n", atoi(cmd[1]));
                temp_value = cpu.value;
                cpu.value -= atoi(cmd[1]);
                printf("CPU value: %d -> %d\n", temp_value, cpu.value);

            }
            else if(!strcmp(cmd[0], "B"))
            {
                printf("Block this simulated process.\n");
                // Store CPU data to proc
                dequeue(&running_states);
                cpu2proc(&cpu, &pcbTable[cpu.pid]);
                printf("Running Process(pid=%d) was blocked.\n", cpu.pid);
                enqueue(&blocked_states, cpu.pid);
                // Scheduling required.

            }
            else if(!strcmp(cmd[0], "E"))
            {
                printf("Terminate this simulated process.\n");
                dequeue(&running_states);
                printf("pid=%d is Terminated.\n", cpu.pid);
                ta.times[ta.count++] = calc_ta_time(current_time, &pcbTable[cpu.pid]);
                // Scheduling required.

            }
            else if(!strcmp(cmd[0], "F"))
            {
                printf("Create %d new simulated process(es).\n", atoi(cmd[1]));
                arg = atoi(cmd[1]);

                cpu2proc(&cpu, &pcbTable[cpu.pid]);
                /* Duplicate a proc and enqueue it into Ready states queue. */
                pcbTable[pid_count] = dup_proc(&pcbTable[cpu.pid], pid_count,
                                               arg, current_time);
                pid_count++;
                enqueue(&ready_states, pid_count-1);
                printf("Created a process(pid=%d).\n", pid_count-1);

                cpu.pc += arg; // Execute N instructions after the next instruction.
                // Not necessary to schdule processes.

            }
            else if(!strcmp(cmd[0],"R"))
            {
                printf("Replace the program of the simulated process with the program in the file '%s'.\n", cmd[1]);
                strcpy(temp_fname, cmd[1]);
                cpu.pc = 0;
                cpu.value = 0;

                readProgram(temp_fname, pcbTable[cpu.pid].prog);
                //printf("Replaced the current program with the program in '%s' file.\n", temp_fname);

            }
            else
            {
                printf("Unknown Instruction.\n");
                printf("Exited by error.\n");
                return;
            }

            /*** Do scheduling ***/
            if(ready_states == NULL)  // No processes in the Ready queue, so skipped.
            {
                if(running_states != NULL)
                {
                    printf("No ready processes, so continue to run the current process.\n");
                }
                else if(blocked_states != NULL)
                {
                    printf("Only blocked processes remain, so waiting for unblocking.\n");
                    wait4unblocking = true;
                }
                else
                {
                    // All processes were finished -> finished execution.
                    if(err_flg == false) printf("Program was successfully executed.\n");
                    printf("\n");
                    printf("=== RESULT ===\n");
                    if (pipe(fd))
                    {
                        perror("pipe");
                    }
                    else if ((temp_pid = fork()) == -1)
                    {
                        perror("fork");
                    }
                    else if (temp_pid == 0)
                    {
                        close(fd[0]);
                        if(DEBUG) cpu2proc(&cpu, &pcbTable[cpu.pid]);
                        reporterProcess(fd[1], pcbTable, current_time, ta,
                                        running_states, ready_states, blocked_states);
                    }
                    else
                    {
                        close(fd[1]);
                        while(i=(read(fd[0],&c,1)) > 0); // Pipe Synchronization
                    }
                    printf("=== END OF SYSTEM ===\n");
                    return;
                }


            }
            else if(running_states == NULL)   // When process was blocked or terminated.
            {
                printf("There are no process running, so assign the first process in the queue to CPU.\n");
                temp_pid = dequeue(&ready_states);
                proc2cpu(&pcbTable[temp_pid], &cpu);
                enqueue(&running_states, temp_pid);
                printf("Assigned: cpu <--- pcbTable[%d]\n", temp_pid);

            }
            else if(cpu.t_remain <= 0)    // When quantum expired
            {
                printf("Quantum was expired, so assign the first process in the que to CPU.\n");
                set_next_priority(&pcbTable[cpu.pid]);
                printf("Pid(%d)'s priority class was raised to %d.\n",
                       cpu.pid, pcbTable[cpu.pid].priority);
                cpu2proc(&cpu, &pcbTable[cpu.pid]);
                enqueue(&ready_states, cpu.pid);
                temp_pid = dequeue(&running_states);

                proc2cpu(&pcbTable[dequeue(&ready_states)], &cpu);
                enqueue(&running_states, cpu.pid);
                printf("Swithed: cpu(%d) <--> pid(%d)\n", temp_pid, cpu.pid);

            }
            else if(cpu.t_remain > 0)
            {
                printf("CPU Time is still remained, so continue to run the current process.\n");

            }
            else
            {
                printf("Unknown condition to schedule.\n");
            }
            /*** End of Scheduling ***/
            free(cmd);
            /* End of One Unit of Time*/

        }
        else if(!strcmp(buffer, "U\n") || !strcmp(buffer, "u\n"))
        {
            printf("Unblock the first simulated process in blocked queue.\n");
            temp_index = dequeue(&blocked_states);
            if(temp_index == -1)
            {
                printf("There are no states in blocked queue.\n");
            }
            else
            {
                printf("pid=%d moves from blocked queue to ready queue.\n", temp_index);
                enqueue(&ready_states, temp_index);
                wait4unblocking = false;
            }

        }
        else if(!strcmp(buffer, "P\n") || !strcmp(buffer, "p\n"))
        {
            printf("Print the current state of the system.\n");
            if (pipe(fd))
            {
                perror("pipe");
            }
            else if ((temp_pid = fork()) == -1)
            {
                perror("fork");
            }
            else if (temp_pid == 0)
            {
                close(fd[0]);
                if(DEBUG) cpu2proc(&cpu, &pcbTable[cpu.pid]);
                reporterProcess(fd[1], pcbTable, current_time, ta,
                                running_states, ready_states, blocked_states);
            }
            else
            {
                close(fd[1]);
                while(i=(read(fd[0],&c,1)) > 0); // Pipe Synchronization
            }

        }
        else if(!strcmp(buffer, "help\n"))
        {
            printf("\
     The following commands are accepted:\n\
     Q:End of one unitof time\n\
     - CPU consumes 1 instruction from programs, andexecuteit.\n\
     U: Unblockthe first simulated process in blocked queue\n\
     - If there is ablockedprocess, move its statefrom Blocked toReady.\n\
     P: Print the current state of the system.\n \
        - The state include PC, PID, PPID, Priority, Value, Time, etc.\n\
     T: Terminate the system after printing the current state.\n\
        - The printing is same as 'P' command.\n");

        }
        else if(!strcmp(buffer, "T\n") || !strcmp(buffer, "t\n"))
        {
            printf("Print the average turnaround time, and terminate the system.\n");
            // Calculating average turnaround time.
            printf("Average turn around time is %f.\n", calc_ta_time_avg(ta));
            if (pipe(fd))
            {
                perror("pipe");
            }
            else if ((temp_pid = fork()) == -1)
            {
                perror("fork");
            }
            else if (temp_pid == 0)
            {
                close(fd[0]);
                if(DEBUG) cpu2proc(&cpu, &pcbTable[cpu.pid]);
                reporterProcess(fd[1], pcbTable, current_time, ta,
                                running_states, ready_states, blocked_states);
            }
            else
            {
                close(fd[1]);
                while(i=(read(fd[0],&c,1)) > 0); // Pipe Synchronization
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
        printf("\n");
        printf("> ");
        fflush(stdout);
    }
    fclose(fp);
}

