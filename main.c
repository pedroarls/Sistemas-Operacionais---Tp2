#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Lista.h"
#include "SimuladorProcesso.h"

int main()
{
    int opcao, opcaoEscalonamento;
    char nomeArquivo[MAX_TAM_STRING];
    printf("Digite o nome do arquivo com o programa inicial a ser executado:");
    scanf("%s",nomeArquivo);
    char arqComandos[MAX_TAM_STRING];

    FILE* arq = fopen(nomeArquivo,"r");

    if(arq==NULL)
    {
        strcpy(nomeArquivo,"init.prog");//Define arquivo de leitura padrão
    }

    printf("MENU\n");
    printf("1 - Inserir comandos manualmente\n");
    printf("2 - Ler a partir de arquivo.\n");
    printf("Para sair pressione qualquer outra tecla...\n\n");
    printf("Opcao: ");
    scanf("%d",&opcao);
    fflush(stdin);

    printf("\n\nSELECIONE UM DOS ALGORITMOS ESCALONAMENTOS\n");
    printf("1 - Escalamento proposto.\n");
    printf("2 - Escalonamento do grupo.\n");
    printf("Opcao: ");
    scanf("%d",&opcaoEscalonamento);

    switch (opcao)
    {
    case 1:
        //chama commander para inserir manualmente

        ProcessCommander(nomeArquivo,arqComandos,1,opcaoEscalonamento);
        break;

    case 2:
        //chama commander para arquivo
        printf("Digite o nome do arquivo com comandos: ");
        scanf("%s",arqComandos);
        ProcessCommander(nomeArquivo,arqComandos,0,opcaoEscalonamento);
        break;

    default:
        printf("Encerrando...\n");
        break;
    }

    return 0;
}
