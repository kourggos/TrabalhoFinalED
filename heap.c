#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>

struct reg {
    long long cpf;
    char nome[50];
    int nota;
}; typedef struct reg Registro;

Registro *leRegistro(FILE *fp) {
    // assumindo que o fp está na posição do registro que eu quero ler
    Registro *reg = (Registro*)malloc(sizeof(Registro));

    fread(&reg->cpf, sizeof(long long), 1, fp);
    fread(reg->nome, sizeof(char), 50, fp);
    fread(&reg->nota, sizeof(int), 1, fp);
    return reg;
}

Registro *buscaRegistro(FILE *fp, int i){
    // busca registro pelo Índice
    fseek(fp, i*(sizeof(long long)+(sizeof(char)*50)+sizeof(int)), SEEK_SET);
    return leRegistro(fp);
}

void escreveNoArquivo(FILE *fp, int i, Registro *reg) {
    fseek(fp, i*(sizeof(long long)+(sizeof(char)*50)+sizeof(int)), SEEK_SET);
    fwrite(&reg->cpf, sizeof(long long), 1, fp);
    fwrite(reg->nome, sizeof(char), 50, fp);
    fwrite(&reg->nota, sizeof(int), 1, fp);
}

void sobeHeap(FILE *fp, int i) {
    int pai = (i-1)/2;
    Registro *regPai, *regI;

    if (i != 0) {
        regI = buscaRegistro(fp, i);
        regPai = buscaRegistro(fp, pai);
        if (regI->nota > regPai->nota) {
            escreveNoArquivo(fp, i, regPai);
            escreveNoArquivo(fp, pai, regI);
            free(regI);
            free(regPai);
            sobeHeap(fp, pai);
        }
    }
}

void desceHeap(FILE *fp, int i, int n) {
    int filho1 = 2*i+1, filho2 = 2*i+2;
    Registro *regI, *regF1, *regF2;

    if (filho1 <= n-1) {
        if (filho2 < n-1) {
            regI = buscaRegistro(fp, i);
            regF1 = buscaRegistro(fp, filho1);
            regF2 = buscaRegistro(fp, filho2);
            if (regI->nota < regF1->nota || regI->nota < regF2->nota) {
                if (regF1->nota >= regF2->nota) {
                    escreveNoArquivo(fp, i, regF1);
                    escreveNoArquivo(fp, filho1, regI);
                    free(regI);
                    free(regF1);
                    free(regF2);
                    desceHeap(fp, filho1, n);
                } else {
                    escreveNoArquivo(fp, i, regF2);
                    escreveNoArquivo(fp, filho2, regI);
                    free(regI);
                    free(regF1);
                    free(regF2);
                    desceHeap(fp, filho2, n);
                }
            }
        } else {
            regI = buscaRegistro(fp, i);
            regF1 = buscaRegistro(fp, filho1);
            if (regI->nota < regF1->nota) {
                escreveNoArquivo(fp, i, regF1);
                escreveNoArquivo(fp, filho1, regI);
                free(regI);
                free(regF1);
                desceHeap(fp, filho1, n);
            }
        }
    }

}

void constroiHeap(FILE *fp, int n) {
    // heap sort de um arquivo
    int comeco = n/2 - 1;
    for (int i = comeco; i>=0; i--) {
        desceHeap(fp, i, n);
    }
}

int removeHeap(FILE *fp, int n) {
    escreveNoArquivo(fp, 0, buscaRegistro(fp, n-1));
    desceHeap(fp, 0, n-1);
    return n-1;
}

int insereHeap(FILE *fp, int n) {
    Registro *reg = (Registro*)malloc(sizeof(Registro));
    int c;
    printf("Digite os dados do registro que quer inserir: \n");
    printf("CPF (apenas dígitos): ");
    scanf("%lld", &reg->cpf);

    while ((c = getchar()) != '\n' && c != EOF);

    printf("Nome: ");
    scanf("%[^\n]", reg->nome);
    printf("Nota: ");
    scanf("%d", &reg->nota);

    escreveNoArquivo(fp, n, reg);
    sobeHeap(fp, n);
    free(reg);
    return n+1;
}

Registro *buscaHeap(FILE *fp, int n) {
    // retorna o elemento de maior prioridade
    return buscaRegistro(fp, 0);
}

void imprimeArquivo(FILE *fp, int n) {
    /* imprime o arquivo sequencialmente */
    Registro *reg = (Registro*)malloc(sizeof(Registro));

    fseek(fp, 0, SEEK_SET);

    printf("Vetor: \n");
    for (int i=0; i<n; i++) {
        fread(&reg->cpf, sizeof(long long), 1, fp);
        fread(reg->nome, sizeof(char), 50, fp);
        fread(&reg->nota, sizeof(int), 1, fp);
        printf("CPF: %lld; Nome: %s; Nota: %d\n", reg->cpf, reg->nome, reg->nota);
    }
    free(reg);
}

void imprimeHeap(FILE *fp, int n) {
    int h = 1;
    fseek(fp, 0, SEEK_SET);
    Registro *reg = (Registro*)malloc(sizeof(Registro));
    for (int i=0; i<n; i++) {
        if (i == h) {
            h = 2*h+1;
            printf("\n");
        }
        fread(&reg->cpf, sizeof(long long), 1, fp);
        fread(reg->nome, sizeof(char), 50, fp);
        fread(&reg->nota, sizeof(int), 1, fp);
        printf("CPF: %lld; Nome: %s; Nota: %d | ", reg->cpf, reg->nome, reg->nota);
    }
    free(reg);
}

void imprimeHeapSoNotas(FILE *fp, int n) {
    /* imprime só as notas com alturas diferentes */
    int h = 1;
    fseek(fp, 0, SEEK_SET);
    Registro *reg = (Registro*)malloc(sizeof(Registro));
    for (int i=0; i<n; i++) {
        if (i == h) {
            h = 2*h+1;
            printf("\n");
        }
        fread(&reg->cpf, sizeof(long long), 1, fp);
        fread(reg->nome, sizeof(char), 50, fp);
        fread(&reg->nota, sizeof(int), 1, fp);
        // printf("CPF: %lld; Nome: %s; Nota: %d | ", reg->cpf, reg->nome, reg->nota);
        printf("%d ", reg->nota);
    }
    free(reg);
}

int main(void) {
    FILE *fp;
    int n, continua=0;
    char c, b, nomeArq[50] = "files/heapRegistros0.bin";
    Registro *reg = (Registro*)malloc(sizeof(Registro));

    // Inicialização do arquivo e do Heap
    fp = fopen(nomeArq, "rb");
    if (fp == NULL) {
        perror("Não foi possível abrir o arquivo.");
        return 1;
    }
    fseek(fp, 0, SEEK_END);
    n = ftell(fp)/(sizeof(long long)+(sizeof(char)*50)+sizeof(int)); // numero de registros
    printf("%d", ftell(fp));
    fclose(fp);

    fp = fopen(nomeArq, "rb+");
    if (fp == NULL) {
        perror("Não foi possível abrir o arquivo.");
        return 1;
    }
    fseek(fp, 0, SEEK_SET);

    constroiHeap(fp, n);

    do {
        printf("\nDigite a operação à fazer: \n\n");

        printf("Atualmente, o arquivo sendo acessado é: %s\n\n", nomeArq);
        printf("b - Mostrar o registro de maior prioridade no Heap.\n");
        printf("i - Inserir registro no Heap.\n");
        printf("r - Remover registro de maior prioridade no Heap.\n");
        printf("a - Trocar o arquivo acessado (será automaticamente transformado em Heap).\n");
        printf("m - Mostra o vetor de registros do Heap.\n");
        printf("n - Mostra as notas(chaves) em níveis de altura.\n");
        printf("s - Sair do programa.\n");
        scanf("%c", &c);

        switch (c) {
            case 'k':
                reg = buscaRegistro(fp, 4);
                printf("%lld, %s, %d.", reg->cpf, reg->nome, reg->nota);
                break;
            case 'b':
                if (n>0) {
                    reg = buscaHeap(fp, n);
                    printf("\nCPF: %lld | Nome: %s | Nota: %d\n\n", reg->cpf, reg->nome, reg->nota);
                } else {
                    printf("\nA Heap está vazia.\n");
                }
                break;
            case 'i':
                n = insereHeap(fp, n);
                fflush(fp);
                break;
            case 'r':
                if (n>0) {
                    reg = buscaHeap(fp, n);
                    printf("\nA remover {CPF: %lld | Nome: %s | Nota: %d}\n\n", reg->cpf, reg->nome, reg->nota);
                    n = removeHeap(fp, n);
                    fflush(fp);
                } else {
                    printf("\nA Heap está vazia.\n");
                }
                break;
            case 'm':
                printf("\n");
                imprimeArquivo(fp, n);
                break;
            case 'n':
                printf("\n");
                imprimeHeapSoNotas(fp, n);
                break;
            case 'a':
                printf("\nDigite o nome do novo arquivo: \n");
                scanf("%s", nomeArq);
                // sprintf(nomeArq, "files/%s", nomeArqPlaceholder);

                fp = fopen(nomeArq, "rb");
                if (fp == NULL) {
                    perror("Não foi possível abrir o arquivo.");
                    return 1;
                }
                fseek(fp, 0, SEEK_END);
                n = ftell(fp)/(sizeof(long long)+(sizeof(char)*50)+sizeof(int)); // numero de registros
                fclose(fp);

                fp = fopen(nomeArq, "rb+");
                if (fp == NULL) {
                    perror("Não foi possível abrir o arquivo.");
                    return 1;
                }
                fseek(fp, 0, SEEK_SET);

                constroiHeap(fp, n);    

                break;
            case 's':
                exit(0);
                break;
        }

        do {
            printf("\nAperte enter para continuar.\n\n");
            scanf("%c", &continua);
        } while (continua != '\n');
        
    } while (c != 's');
    free(reg);
    fclose(fp);

    // fp = fopen("heapRegistros.bin", "rb");
    // fseek(fp, 0, SEEK_END);
    // n = ftell(fp)/(sizeof(long long)+(sizeof(char)*50)+sizeof(int)); // numero de registros
    // fclose(fp);

    // fp = fopen("heapRegistros.bin", "rb+");
    // fseek(fp, 0, SEEK_SET);

    // constroiHeap(fp, n);

    // fflush(fp);

    // fclose(fp);
    return 0;
}