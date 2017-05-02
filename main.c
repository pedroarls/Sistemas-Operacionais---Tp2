#include <stdio.h>
#include <stdlib.h>
#include "Lista.h"
#include "SimuladorProcesso.h"

int main()
{
    int opcao;
    char nomeArquivo[MAX_TAM_STRING];
    printf("Digite o nome do arquivo com o programa inicial a ser executado:");
    scanf("%s",nomeArquivo);

    if(nomeArquivo==NULL){
        strcpy(nomeArquivo,"init.prog");//Define arquivo de leitura padrão
    }

    printf("MENU\n");
    printf("1 - Inserir comandos manualmente\n");
    printf("2 - Ler a partir de arquivo.\n");
    printf("Para sair pressione qualquer outra tecla...\n\n");
    printf("Opcao: ");
    scanf("%d",&opcao);

    switch (opcao)
    {
        case 1:
            //chama commander para inserir manualmente
            ProcessCommander(nomeArquivo);
            break;

        case 2:
            //chama commander para arquivo
            break;

        default:
            printf("Encerrando...\n");
            break;
    }

    return 0;
}
