#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

struct reg {
    long long cpf;
    char nome[50];
    int nota;
}; typedef struct reg Registro;

int geraRand(int lim) {
    /*Gera inteiro aleatório [0, lim)*/
    int a = (RAND_MAX*RAND_MAX)/lim, r;
    r = rand()*rand();
    while (1) {
        if (r < a*lim) {
            return r/a;
        } else {
            r = rand();
        }
    }
}

int hash(long long cpf, int k) {
    int r;

    srand(cpf);

    for (int i=0; i<=k; i++) {
        r = geraRand(100000);
    }

    return r;
}

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

int verificaOcupado(FILE *fp, int i) {
    // retorna verdadeiro se estiver ocupado
    Registro *reg;
    reg = buscaRegistro(fp, i);
    return (reg->cpf == -1);
}

Registro *buscaHash(FILE *fp, long long cpf) {
    int n, i=0;
    Registro *reg;

    do {
        n = hash(cpf / 100, i);
        i++;
        reg = buscaRegistro(fp, n);
        if (reg->cpf == -1) {
            return NULL;
        }
    } while (reg->cpf != cpf);
    return reg;
}

void escreveNoArquivo(FILE *fp, int i, Registro *reg) {
    fseek(fp, i*(sizeof(long long)+(sizeof(char)*50)+sizeof(int)), SEEK_SET);
    fwrite(&reg->cpf, sizeof(long long), 1, fp);
    fwrite(reg->nome, sizeof(char), 50, fp);
    fwrite(&reg->nota, sizeof(int), 1, fp);
}

int insereHash(FILE *fp, Registro *novo, int n) {
    int idx, i=0, c;
    Registro *aux;

    do {
        idx = hash(novo->cpf, i);
        i++;
        aux = buscaRegistro(fp, idx);
        if (aux->cpf == -1) {
            escreveNoArquivo(fp, idx, novo);
            free(aux);
            return n+1;
        }
    } while (aux->cpf != novo->cpf);
    free(aux);

    return n+1;
}

int removeHash(FILE *fp, long long cpf, int n) {
    int idx, i=0;
    Registro *aux, *vazio;

    for (i=0; i<100000; i++) {
        idx = hash(cpf / 100, i);
        aux = buscaRegistro(fp, idx);
        if (aux->cpf == cpf) {
            vazio = (Registro*)malloc(sizeof(Registro));
            vazio->cpf = -1;
            vazio->nota = -1;
            escreveNoArquivo(fp, idx, vazio);
            free(vazio);
            return n-1;
        } else if (aux->cpf == -1) {
            printf("\nO registro não está na tabela.\n");
            return n;
        }
    }

    printf("\nO registro não está na tabela.\n");
    return n;
}

void constroiHash(FILE *fregistros, FILE *fhash, int n) {
    Registro *reg, *vazio = (Registro*)malloc(sizeof(Registro));
    vazio->cpf = -1;
    vazio->nome[0] = '\0';
    vazio->nota = -1;
    for (int i=0; i<100000; i++) {
        escreveNoArquivo(fhash, i, vazio);
    }
    free(vazio);
    for (int i=0; i<n; i++) {
        reg = buscaRegistro(fregistros, i);
        insereHash(fhash, reg, n);
        free(reg);
    }
}

void imprimeHash(FILE *fhash) {
    Registro *reg;
    int i=0;
    fseek(fhash, 0, SEEK_SET);
    for (i=0; i<100000; i++) {
        reg = leRegistro(fhash);
        if (feof(fhash)) return;
        if (reg->cpf != -1) {
            printf("Indice: %d | CPF: %lld | Nome: %s | Nota: %d\n", i, reg->cpf, reg->nome, reg->nota);
        }
    }
}

int main(void) { // tratar tabela nao inicializada
    FILE *fregs, *fhash;
    int tamArqReg, n=0, continua=0;
    long long cpf;
    Registro *reg;
    char c, nomeArqRegs[30] = "files/registros0.bin", nomeArqHash[30] = "files/hashRegistros0.bin";

    // Inicialização do arquivo e da tabela Hash
    fregs = fopen(nomeArqRegs, "rb");
    if (fregs == NULL) {
        perror("Não foi possível abrir o arquivo.");
        return 1;
    }
    fseek(fregs, 0, SEEK_END);
    tamArqReg = ftell(fregs)/(sizeof(long long)+(sizeof(char)*50)+sizeof(int)); // numero de registros
    fclose(fregs);
    
    fregs = fopen(nomeArqRegs, "rb+");
    if (fregs == NULL) {
        perror("Não foi possível abrir o arquivo.");
        return 1;
    }
    fseek(fregs, 0, SEEK_SET);

    fhash = fopen(nomeArqHash, "rb+");
    if (fhash ==NULL) {
        perror("Não foi possível abrir o arquivo.");
        return 1;
    }

    do {
        printf("Digite a operação à fazer: \n\n");

        printf("Atualmente, o arquivo de registros é: %s e o da tabela Hash (destino) é: %s\n\n", nomeArqRegs, nomeArqHash);
        printf("b - Buscar um valor pelo índice na Hash.\n");
        printf("i - Inserir registro na tabela Hash.\n");
        printf("r - Remover registro na tabela Hash.\n");
        printf("a - Trocar o arquivo de registros.\n");
        printf("d - Trocar o arquivo de destino.\n");
        printf("c - Construir tabela Hash baseado no arquivo de registros (deleta o arquivo destino atual).\n");
        // opção e a tabela precisa estar alocada.
        printf("e - Adiciona registros do arquivo de registros na tabela Hash (preserva os dados do arquivo destino)\n");
        printf("m - Mostra a Hash sem espaços vazios.\n");
        printf("s - Sair do programa.\n");
        scanf("%c", &c);

        switch (c) {
            case 'b':
                if (n>0) {
                    printf("\nDigite o CPF a procurar (apenas dígitos): ");
                    scanf("%lld", &cpf);
                    reg = buscaHash(fhash, cpf);
                    if (reg != NULL) {
                        printf("\nCPF: %lld | Nome: %s | Nota: %d\n\n", reg->cpf, reg->nome, reg->nota);
                    } else {
                        printf("\nEsse registro não está na tabela.\n");
                    }
                    free(reg);
                } else {
                    printf("\nA tabela Hash está vazia.\n");
                }
                break;
            case 'i':
                if (n < 100000) {
                    reg = (Registro*)malloc(sizeof(Registro));

                    printf("Digite os dados do registro que quer inserir: \n");
                    printf("CPF (apenas dígitos): ");
                    scanf("%lld", &reg->cpf);

                    while ((c = getchar()) != '\n' && c != EOF);

                    printf("Nome: ");
                    scanf("%[^\n]", reg->nome);
                    printf("Nota: ");
                    scanf("%d", &reg->nota);

                    n = insereHash(fhash, reg, n);
                    free(reg);
                    fflush(fhash);
                } else {
                    printf("\nA tabela está cheia.\n");
                }
                break;
            case 'r':
                if (n>0) {
                    printf("\nDigite o CPF a remover (apenas dígitos): ");
                    scanf("%lld", &cpf);

                    reg = buscaHash(fhash, cpf);
                    printf("\nA remover {CPF: %lld | Nome: %s | Nota: %d}\n\n", reg->cpf, reg->nome, reg->nota);
                    n = removeHash(fhash, cpf, n);
                    free(reg);
                    fflush(fhash);
                } else {
                    printf("\nA tabela Hash está vazia.\n");
                }
                break;
            case 'm':
                printf("\n");
                imprimeHash(fhash);
                break;
            case 'a':

                fclose(fregs);

                printf("\nDigite o nome do novo arquivo de registros: \n");
                scanf("%s", nomeArqRegs);

                fregs = fopen(nomeArqRegs, "rb");
                if (fregs == NULL) {
                    perror("Não foi possível abrir o arquivo.");
                    return 1;
                }
                fseek(fregs, 0, SEEK_END);
                tamArqReg = ftell(fregs)/(sizeof(long long)+(sizeof(char)*50)+sizeof(int)); // numero de registros
                fclose(fregs);

                fregs = fopen(nomeArqRegs, "rb+");
                if (fregs == NULL) {
                    perror("Não foi possível abrir o arquivo.");
                    return 1;
                }
                fseek(fregs, 0, SEEK_SET);

                break;
            case 'd':

                fclose(fhash);

                printf("\nDigite o nome do novo arquivo de destino (tabela Hash): \n");
                scanf("%s", nomeArqHash);

                fregs = fopen(nomeArqHash, "rb+");
                if (fregs == NULL) {
                    perror("Não foi possível abrir o arquivo.");
                    return 1;
                }
                fseek(fregs, 0, SEEK_SET);

                break;
            case 'c':
                constroiHash(fregs, fhash, tamArqReg);
                n = tamArqReg;
            case 'e':
                Registro *reg;
                for (int i=0; i<tamArqReg; i++) {
                    reg = buscaRegistro(fregs, i);
                    insereHash(fhash, reg, n);
                    free(reg);
                }
                break;
            case 's':
                fclose(fhash);
                fclose(fregs);
                exit(0);
                break;
        }
        do {
            printf("\nAperte enter para continuar.\n\n");
            scanf("%c", &continua);
        } while (continua != '\n');
        
    } while (c != 's');

    return 0;
}