#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#define ORDEM 64/4 // tamanho do bloco registro / tamanho da chave
#define N_CHAVES 2*ORDEM

struct aluno {
    long long cpf;
    char nome[50];
    int nota;
};typedef struct aluno Aluno;

struct reg {
    long long cpf;
    char nome[50];
    int nota;
}; typedef struct reg Registro;

struct noInterno { // no arquivo, a ordem da informação é [m, pai, apontaFolha, chaves[], ponteiros[]]
    long long chaves[N_CHAVES]; //vetor de chaves; chave é os 9 primeiros digitos do cpf
    int pont[N_CHAVES+1]; // vetor de ponteiros
    int m; //num elementos no nó
    int pai, apontaFolha; 
}; typedef struct noInterno NoInterno;

struct noFolha { // no arquivo, a ordem da informação é [m, pai, prox, ordem, registros[]]
    // long long chaves[N_CHAVES]; //vetor de chaves; chave é os 9 primeiros digitos do cpf
    Registro reg[N_CHAVES]; //vetor de registros; 
    int m, pai, prox;
}; typedef struct noFolha NoFolha;

struct metadados { // primeiro tem a flag raizfolha, depois tem o ponteiro pra raiz, entao o n.
    int raizfolha; // 1 se raiz é folha, 0 se não
    int pontraiz; // ponteiro para a raiz no arquivo de indices
    int n; // numero de elementos na árvore
}; typedef struct metadados Meta;

void limparBufferEntrada() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

//pausa a execução do programa até que o usuário pressione Enter.
 
void pressioneEnterParaContinuar() {
    printf("\nPressione ENTER para continuar...");
    getchar();
}

long long geraRand(long long lim) {
    /*Gera inteiro aleatório [0, lim)*/
    long long a=1, r=1;
    a *= RAND_MAX;
    a *= RAND_MAX;
    a *= RAND_MAX;
    a /= lim + 1;
    r *= rand();
    r *= rand();
    r *= rand();
    while (1) {
        if (r < a*lim) {
            return r/a;
        } else {
            r=1;
            r *= rand();
            r *= rand();
            r *= rand();
        }
    }
}

void criaNomeAleatorio(FILE *arqNomes, char nome[50]) {
    int r, tamLista=201, i=0;
    char c;
    // char nome[50], palavra[20];

    for (int j=0; j<4; j++) {
        r = rand() % tamLista;

        fseek(arqNomes, r, SEEK_SET);
        fscanf(arqNomes, "%c", &c);
        while (c != ';') {
            fscanf(arqNomes, "%c", &c);
        }
        fscanf(arqNomes, "%c", &c);
        while (c != ';') {
            // fscanf(arqNomes, "%[^;]s", &c);
            if (c != '\n') {
                nome[i] = c;
                i++;
            }
            fscanf(arqNomes, "%c", &c);
        }
        if (j != 3) {
            nome[i] = ' ';
            i++;
        }
    }
    nome[i] = '\000';
}

// void carregaNoArquivo(FILE *fp, long long cpf, char nome[], int nota, char zeroscpf[]) {
//     fprintf(fp, "%s%lld;%s;%d;\n", zeroscpf, cpf, nome, nota);
// }

void carregaNoArquivo(FILE *fp, FILE *arqTexto, long long cpf, char nome[50], int nota) {
    fwrite(&cpf, sizeof(long long), 1, fp);
    fwrite(nome, sizeof(char), 50, fp);
    fwrite(&nota, sizeof(int), 1, fp);
    fprintf(arqTexto, "%lld;%s;%d;\n", cpf, nome, nota);
}

void escreveNoArquivo(FILE *fp, int i, Registro *reg) {
    fseek(fp, i*(sizeof(long long)+(sizeof(char)*50)+sizeof(int)), SEEK_SET);
    fwrite(&reg->cpf, sizeof(long long), 1, fp);
    fwrite(reg->nome, sizeof(char), 50, fp);
    fwrite(&reg->nota, sizeof(int), 1, fp);
}

void criaReg(FILE *fp, FILE *arqNomes, FILE *arqTexto, int i) {
    long long cpf, nota, limCPF, teste;
    char nome[50], zeroscpf[10], zerosMat[5];

    limCPF = 100000000000;

    cpf = geraRand(limCPF);

    nota = rand() % 101;

    criaNomeAleatorio(arqNomes, nome);

    carregaNoArquivo(fp, arqTexto, cpf, nome, nota);
}

void copiaArquivo(char nomeArqAntigo[], char nomeNovoArq[]) {
    FILE *fp, *fnovo;

    fp = fopen(nomeArqAntigo, "rb");
    
    fnovo = fopen(nomeNovoArq, "wb");

    int n, m;
    unsigned char buff[256];
    do {
        n = fread(buff, 1, sizeof(buff), fp);
        if (n) {
            m = fwrite(buff, 1, n, fnovo);
        } else {
            m = 0;
        } 
    } while ((n > 0) && (n == m));

    fclose(fp);
    fclose(fnovo);
}

int gerarArquivosDeRegistros(void) {
    char nomeArqRegs[50], nomeArqTxtRegs[50], nomeArvRegs[50], nomeHeapRegs[50], nomeHashRegs[50], nomeArqPlaceholders[50];
    FILE *fpreg, *fpnomes, *fptxt, *fp;
    int n, numregs;
    srand(time(NULL));

    printf("Digite quantos arquivos criar (1 cria no arquivo principal não enumerado.):\n");
    scanf("%d", &n);

    if (n == 1) {
        sprintf(nomeArqRegs, "files/registros.bin", n);
        fpreg = fopen(nomeArqRegs, "wb");
        fpnomes = fopen("nomes.txt", "r");
        sprintf(nomeArqTxtRegs, "files/registros.txt", n);
        fptxt = fopen(nomeArqTxtRegs, "w");

        printf("Digite quantos registros quer criar no arquivo principal:\n");
        scanf("%d", &numregs);
        
        for (int i=0; i<numregs; i++) {
            criaReg(fpreg, fpnomes, fptxt, i);
        }
    
        
        fflush(fpreg);

        fclose(fpreg);
        fclose(fpnomes);

        sprintf(nomeHeapRegs, "files/heapRegistros.bin", n);
        sprintf(nomeHashRegs, "files/hashRegistros.bin", n);
        sprintf(nomeArvRegs, "files/bplusRegistros.bin", n);

        sprintf(nomeArqPlaceholders, "files/arquivoDadosArvore.bin", n);
        fp = fopen(nomeArqPlaceholders, "wb");
        fclose(fp);
        sprintf(nomeArqPlaceholders, "files/arquivoIndicesArvore.bin", n);
        fp = fopen(nomeArqPlaceholders, "wb");
        fclose(fp);
        sprintf(nomeArqPlaceholders, "files/arquivoMetadadosArvore.txt", n);
        fp = fopen(nomeArqPlaceholders, "w");
        fclose(fp);

        copiaArquivo(nomeArqRegs, nomeHeapRegs);
        copiaArquivo(nomeArqRegs, nomeHashRegs);
        copiaArquivo(nomeArqRegs, nomeArvRegs);
    } else {

    for (int j=0; j<n; j++) {
        sprintf(nomeArqRegs, "files/registros%d.bin", j);
        fpreg = fopen(nomeArqRegs, "wb");
        fpnomes = fopen("nomes.txt", "r");
        sprintf(nomeArqTxtRegs, "files/registros%d.txt", j);
        fptxt = fopen(nomeArqTxtRegs, "w");

        printf("Digite quantos registros quer criar no arquivo enumerado %d:\n", j);
        scanf("%d", &numregs);
        
        for (int i=0; i<numregs; i++) {
            criaReg(fpreg, fpnomes, fptxt, i);
        }
    
        
        fflush(fpreg);

        fclose(fpreg);
        fclose(fpnomes);

        sprintf(nomeHeapRegs, "files/heapRegistros%d.bin", j);
        sprintf(nomeHashRegs, "files/hashRegistros%d.bin", j);
        sprintf(nomeArvRegs, "files/bplusRegistros%d.bin", j);

        sprintf(nomeArqPlaceholders, "files/arquivoDadosArvore%d.bin", j);
        fp = fopen(nomeArqPlaceholders, "wb");
        fclose(fp);
        sprintf(nomeArqPlaceholders, "files/arquivoIndicesArvore%d.bin", j);
        fp = fopen(nomeArqPlaceholders, "wb");
        fclose(fp);
        sprintf(nomeArqPlaceholders, "files/arquivoMetadadosArvore%d.txt", j);
        fp = fopen(nomeArqPlaceholders, "w");
        fclose(fp);

        copiaArquivo(nomeArqRegs, nomeHeapRegs);
        copiaArquivo(nomeArqRegs, nomeHashRegs);
        copiaArquivo(nomeArqRegs, nomeArvRegs);
    }
    }
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

void escreveHeapArq(FILE *fp, int n) {
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
        fprintf(fp, "CPF: %lld; Nome: %s; Nota: %d | ", reg->cpf, reg->nome, reg->nota);
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

int menuHeap(void) {
    FILE *fp;
    int n, continua=0;
    char c, b, nomeArq[50] = "files/heapRegistros.bin";
    Registro *reg = (Registro*)malloc(sizeof(Registro));

    // Inicialização do arquivo e do Heap
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

    do {
        printf("\nDigite a operação à fazer: \n\n");

        printf("Atualmente, o arquivo sendo acessado é: %s\n\n", nomeArq);
        printf("b - Mostrar o registro de maior prioridade no Heap.\n");
        printf("i - Inserir registro no Heap.\n");
        printf("r - Remover registro de maior prioridade no Heap.\n");
        printf("a - Trocar o arquivo acessado (será automaticamente transformado em Heap).\n");
        printf("m - Mostra o vetor de registros do Heap.\n");
        printf("t - Carrega o vetor de registros em um arquivo texto.\n");
        printf("n - Mostra as notas(chaves) em níveis de altura.\n");
        printf("s - Sair do programa.\n");
        scanf("%c", &c);

        switch (c) {
            case 't':
                printf("\nDigite o nome do novo arquivo: \n");
                scanf("%s", nomeArq);
                // sprintf(nomeArq, "files/%s", nomeArqPlaceholder);

                fp = fopen(nomeArq, "w");
                if (fp == NULL) {
                    perror("Não foi possível abrir o arquivo.");
                    return 1;
                }

                escreveHeapArq(fp, n);
                break;
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

        pressioneEnterParaContinuar();
        
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

int hash(long long cpf, int k) {
    int r;

    srand(cpf);

    for (int i=0; i<=k; i++) {
        r = geraRand(100000);
    }

    return r;
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

void escreveHashArq(FILE *fhash, FILE *fp) {
    Registro *reg;
    int i=0;
    fseek(fhash, 0, SEEK_SET);
    for (i=0; i<100000; i++) {
        reg = leRegistro(fhash);
        if (feof(fhash)) return;
        if (reg->cpf != -1) {
            fprintf(fp, "Indice: %d | CPF: %lld | Nome: %s | Nota: %d\n", i, reg->cpf, reg->nome, reg->nota);
        }
    }
    free(reg);
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
    free(reg);
}

int menuHash(void) { // tratar tabela nao inicializada
    FILE *fregs, *fhash, *fp;
    int tamArqReg, n=0, continua=0;
    long long cpf;
    Registro *reg;
    char c, nomeArqRegs[30] = "files/registros.bin", nomeArqHash[30] = "files/hashRegistros.bin", nomeArq[50];

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
        printf("t - Escreve a Hash em um arquivo texto sem espaços vazios.\n");
        printf("s - Sair do programa.\n");
        scanf("%c", &c);

        switch (c) {
            case 't':
                printf("\nDigite o nome do novo arquivo: \n");
                scanf("%s", nomeArq);
                // sprintf(nomeArq, "files/%s", nomeArqPlaceholder);

                fp = fopen(nomeArq, "w");
                if (fp == NULL) {
                    perror("Não foi possível abrir o arquivo.");
                    return 1;
                }

                escreveHeapArq(fp, n);
                break;
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
        pressioneEnterParaContinuar();
        
    } while (c != 's');

    return 0;
}


// -----------------------------------------------------------------------------------

Meta *leMetadados(FILE *fp) {
    Meta *meta = (Meta*)malloc(sizeof(Meta));

    fseek(fp, 0, SEEK_SET);

    fscanf(fp, "%d;%d;%d", &meta->raizfolha, &meta->pontraiz, &meta->n);

    return meta;
}

void escreveMetadados(FILE *fp, Meta *meta) {
    fseek(fp, 0, SEEK_SET);

    fprintf(fp, "%d;%d;%d", meta->raizfolha, meta->pontraiz, meta->n);
    fflush(fp);
}


// ------------------------------------------------

NoFolha *buscaNoFolha(FILE *fp, int k) {
    NoFolha *no = (NoFolha*)malloc(sizeof(NoFolha));

    fseek(fp, k*sizeof(NoFolha), SEEK_SET);
    fread(no, sizeof(NoFolha), 1, fp);

    return no;
}

void escreveNoFolhaFimArquivo(FILE *fp, NoFolha *no) {
    fseek(fp, 0, SEEK_END);

    fwrite(no, sizeof(NoFolha), 1, fp);
    fflush(fp);
}

void escreveNoFolha(FILE *fp, int k, NoFolha *no) {
    fseek(fp, k*sizeof(NoFolha), SEEK_SET);

    fwrite(no, sizeof(NoFolha), 1, fp);
    fflush(fp);
}

// -------------------------------------------------------------

NoInterno *buscaNoInterno(FILE *fp, int k) {
    NoInterno *no = (NoInterno*)malloc(sizeof(NoInterno));

    fseek(fp, k*sizeof(NoInterno), SEEK_SET);
    fread(no, sizeof(NoInterno), 1, fp);

    return no;
}

void escreveNoInterno(FILE *fp, int k, NoInterno *no) {
    fseek(fp, k*sizeof(NoInterno), SEEK_SET);

    fwrite(no, sizeof(NoInterno), 1, fp);
    fflush(fp);
}

void escreveNoInternoNoFinal(FILE *fp, NoInterno *no) {
    fseek(fp, 0, SEEK_END);

    fwrite(no, sizeof(NoInterno), 1, fp);
    fflush(fp);
}

// -------------------------------------------------------------

void escreveRegistro(Registro *reg, FILE*fp){

    fprintf(fp, "CPF: %lld | Nome: %s | Nota: %d\n", reg->cpf, reg->nome, reg->nota);
}

void imprimeRegistro(Registro *reg){

    printf("CPF: %lld | Nome: %s | Nota: %d\n", reg->cpf, reg->nome, reg->nota);
}

void imprimeFolha(FILE *fp, int k){
    fseek(fp, k*sizeof(NoFolha), SEEK_SET);
    NoFolha *no = buscaNoFolha(fp, k);

    printf("Chaves na folha: %d,\t Endereço do Pai: %d,\t Endereço do prox: %d\n", no->m, no->pai, no->prox);
    for (int i=0; i<no->m; i++) {
        imprimeRegistro(&no->reg[i]);
    }
}

void imprimeTodasAsFolhas(FILE *fp){
    fseek(fp, 0, SEEK_SET);
    int k = 0;
    int eof = !feof(fp);
    while (eof) {
        NoFolha *no = buscaNoFolha(fp, k);
        eof = !feof(fp);
            if (eof) {
            printf("Chaves na folha: %d,\t Endereço do Pai: %d,\t Endereço do prox: %d\n", no->m, no->pai, no->prox);
            for (int i=0; i<no->m; i++) {
                imprimeRegistro(&no->reg[i]);
            }
            printf("\n");
            k++;
        }
    }
}

void escreveTodasAsFolhasNaOrdemCrescente(FILE *fp, FILE *output){
    fseek(fp, 0, SEEK_SET);
    int k = 0;

    int eof = !feof(fp);
    while (eof) {
        NoFolha *no = buscaNoFolha(fp, k);
        k = no->prox;
        eof = !feof(fp);
            if (eof) {
            fprintf(output, "Chaves na folha: %d,\t Endereço do Pai: %d,\t Endereço do prox: %d\n", no->m, no->pai, no->prox);
            for (int i=0; i<no->m; i++) {
                escreveRegistro(&no->reg[i], output);
            }
            printf("\n");
        }
        if (k == -1) {
            return;
        }
        // k++;
    }
}

void imprimeTodasAsFolhasNaOrdemCrescente(FILE *fp){
    fseek(fp, 0, SEEK_SET);
    int k = 0;

    int eof = !feof(fp);
    while (eof) {
        NoFolha *no = buscaNoFolha(fp, k);
        k = no->prox;
        eof = !feof(fp);
            if (eof) {
            printf("Chaves na folha: %d,\t Endereço do Pai: %d,\t Endereço do prox: %d\n", no->m, no->pai, no->prox);
            for (int i=0; i<no->m; i++) {
                imprimeRegistro(&no->reg[i]);
            }
            printf("\n");
        }
        if (k == -1) {
            return;
        }
        // k++;
    }
}

void imprimeTodosNosInternos(FILE *fp) {
    fseek(fp, 0, SEEK_SET);
    int k = 0;
    int eof = !feof(fp);
    while (eof) {
        NoInterno *no = buscaNoInterno(fp, k);
        eof = !feof(fp);
            if (eof) {
            printf("Chaves no No Interno: %d,\t Endereço do Pai: %d,\t Aponta pra folha: %d\n", no->m, no->pai, no->apontaFolha);
            for (int i=0; i<no->m; i++) {
                printf("|%d|  %d  ", no->pont[i], no->chaves[i]);
            }
            printf("|%d|", no->pont[no->m]);
            printf("\n");
            k++;
        }
    }
}


void imprimeMetadados(FILE *fp) {
    fseek(fp, 0, SEEK_SET);
    Meta *meta = leMetadados(fp);

    printf("Metadados | Raiz é folha: %d,\tPonteiro para raiz: %d,\tNúmero de Elementos: %d\n", meta->raizfolha, meta->pontraiz, meta->n);
}

void escreveRegistroNoArquivo(FILE *fp, int i, Registro *reg) {
    fseek(fp, i*sizeof(Registro), SEEK_SET);
    fwrite(reg, sizeof(Registro), 1, fp);
    fflush(fp);
}

FILE *abreArquivo(char nomeArq[], char operation[]) {
    FILE *fp;

    fp = fopen(nomeArq, operation);
    if (fp == NULL) {
        perror("Não foi possível abrir o arquivo.");
        exit(1);
    }
    fseek(fp, 0, SEEK_SET);

    return fp;
}

// ------------------------ BUSCA ------------------------------

int buscaNaArvoreFolhas(FILE *fdados, int ponteiroRaiz, long long chave) { // retorna ponteiro para a folha se a chave está, retorna -1 se não está na folha
    NoFolha *raiz = buscaNoFolha(fdados, ponteiroRaiz);
    
    for (int i=0; i<raiz->m; i++) {
        if (raiz->reg[i].cpf / 100 == chave) { // procura a chave
            return ponteiroRaiz; // retorna o nó que está sendo procurado
        }
    }
    free(raiz);
    return -1;
}

int buscaNaArvoreInterna(FILE *findice, FILE *fdados, int ponteiroRaiz, long long chave) {
    NoInterno *raiz = buscaNoInterno(findice, ponteiroRaiz);
    int ponteiroParaFolhaBuscada;

    for (int i=0; i<raiz->m; i++) {
        if (raiz->chaves[i] > chave) { // procura a chave
            if (raiz->apontaFolha) {
                ponteiroParaFolhaBuscada = buscaNaArvoreFolhas(fdados, raiz->pont[i], chave);

                return ponteiroParaFolhaBuscada;
            } else {
                ponteiroParaFolhaBuscada = buscaNaArvoreInterna(findice, fdados, raiz->pont[i], chave);

                return ponteiroParaFolhaBuscada;
            }
        } else if (raiz->chaves[i] == chave) {
            if (raiz->apontaFolha) {
                ponteiroParaFolhaBuscada = buscaNaArvoreFolhas(fdados, raiz->pont[i+1], chave);

                return ponteiroParaFolhaBuscada;
            } else {
                ponteiroParaFolhaBuscada = buscaNaArvoreInterna(findice, fdados, raiz->pont[i+1], chave);

                return ponteiroParaFolhaBuscada;
            }
        }
    }

    if (raiz->apontaFolha) {
        ponteiroParaFolhaBuscada = buscaNaArvoreFolhas(fdados, raiz->pont[raiz->m], chave);

        return ponteiroParaFolhaBuscada;
    } else {
        ponteiroParaFolhaBuscada = buscaNaArvoreInterna(findice, fdados, raiz->pont[raiz->m], chave);

        return ponteiroParaFolhaBuscada;
    }
}


int buscaNaArvore(FILE *findice, FILE *fdados, int flagFolhaRaiz, int ponteiroRaiz, long long cpf) {
    long long chave = cpf / 100;
    int ponteiroParaFolhaBuscada;

    if (flagFolhaRaiz) { // se a raiz for folha:
        ponteiroParaFolhaBuscada = buscaNaArvoreFolhas(fdados, ponteiroRaiz, chave);

        return ponteiroParaFolhaBuscada;
    } else {
        ponteiroParaFolhaBuscada = buscaNaArvoreInterna(findice, fdados, ponteiroRaiz, chave);

        return ponteiroParaFolhaBuscada;
    }

}

// ---------------------------- Busca pra inserção ------------------------------------

int buscaNaArvoreFolhasInsercao(FILE *fdados, int ponteiroRaiz, long long chave) { // retorna ponteiro para a folha se a chave está, retorna -1 se não está na folha
    NoFolha *raiz = buscaNoFolha(fdados, ponteiroRaiz);
    
    for (int i=0; i<raiz->m; i++) {
        if (raiz->reg[i].cpf / 100 == chave) { // procura a chave
            return -1; // retorna o nó que está sendo procurado
        }
    }
    free(raiz);
    return ponteiroRaiz;
}

int buscaNaArvoreInternaInsercao(FILE *findice, FILE *fdados, int ponteiroRaiz, long long chave) {
    NoInterno *raiz = buscaNoInterno(findice, ponteiroRaiz);
    int ponteiroParaFolhaBuscada;

    for (int i=0; i<raiz->m; i++) {
        if (raiz->chaves[i] > chave) { // procura a chave
            if (raiz->apontaFolha) {
                ponteiroParaFolhaBuscada = buscaNaArvoreFolhasInsercao(fdados, raiz->pont[i], chave);

                return ponteiroParaFolhaBuscada;
            } else {
                ponteiroParaFolhaBuscada = buscaNaArvoreInternaInsercao(findice, fdados, raiz->pont[i], chave);

                return ponteiroParaFolhaBuscada;
            }
        } else if (raiz->chaves[i] == chave) {
            if (raiz->apontaFolha) {
                ponteiroParaFolhaBuscada = buscaNaArvoreFolhasInsercao(fdados, raiz->pont[i+1], chave);

                return ponteiroParaFolhaBuscada;
            } else {
                ponteiroParaFolhaBuscada = buscaNaArvoreInternaInsercao(findice, fdados, raiz->pont[i+1], chave);

                return ponteiroParaFolhaBuscada;
            }
        }
    }

    if (raiz->apontaFolha) {
        ponteiroParaFolhaBuscada = buscaNaArvoreFolhasInsercao(fdados, raiz->pont[raiz->m], chave);

        return ponteiroParaFolhaBuscada;
    } else {
        ponteiroParaFolhaBuscada = buscaNaArvoreInternaInsercao(findice, fdados, raiz->pont[raiz->m], chave);

        return ponteiroParaFolhaBuscada;
    }
}


int buscaNaArvoreInsercao(FILE *findice, FILE *fdados, int flagFolhaRaiz, int ponteiroRaiz, long long cpf) {
    long long chave = cpf / 100;
    int ponteiroParaFolhaBuscada;

    if (flagFolhaRaiz) { // se a raiz for folha:
        ponteiroParaFolhaBuscada = buscaNaArvoreFolhasInsercao(fdados, ponteiroRaiz, chave);

        return ponteiroParaFolhaBuscada;
    } else {
        ponteiroParaFolhaBuscada = buscaNaArvoreInternaInsercao(findice, fdados, ponteiroRaiz, chave);

        return ponteiroParaFolhaBuscada;
    }

}

// --------------------- INICIALIZAR ÁRVORE --------------------

void inicializaArvoreVazia(FILE *fdados, FILE *fmeta) {
    NoFolha *novazio = (NoFolha*)malloc(sizeof(NoFolha));
    novazio->m = 0;
    novazio->pai = -1;
    novazio->prox = -1;

    fseek(fmeta, 0, SEEK_SET);
    fseek(fdados, 0, SEEK_SET);

    fprintf(fmeta, "%d;%d;%d", 1, 0, 0);

    fwrite(novazio, sizeof(NoFolha), 1, fdados);
}

// ----------------------- INSERÇÃO ----------------------------

NoInterno *criaNoInternoVazio(FILE *findice, int posicao, int apontaPraFolha) {
    NoInterno *no = (NoInterno*)malloc(sizeof(NoInterno));

    no->m = 0;
    no->pai = -1;
    no->apontaFolha = apontaPraFolha;

    return no;
}

int sobeChaveProPai(FILE *findice, FILE *fdados, int posicaoPai, int posicaoFilhoEsq, int posicaoFilhoDir, long long chave, int apontaPraFolha, Meta *meta) { // retorna o endereço do pai
    NoInterno *raiz;
    int chaves[N_CHAVES+1], inddomeio, chavedomeio, offset;
    int ponteiros[N_CHAVES+2];

    if (posicaoPai == -1) { // se nao tiver pai, cria o pai
        raiz = criaNoInternoVazio(findice, fseek(findice, 0, SEEK_END), apontaPraFolha);

        raiz->m = 1;
        raiz->pai = -1;
        raiz->chaves[0] = chave;
        raiz->pont[0] = posicaoFilhoEsq;
        raiz->pont[1] = posicaoFilhoDir;

        fseek(findice, 0, SEEK_END);
        meta->pontraiz = ftell(findice) / sizeof(NoInterno); 

        escreveNoInternoNoFinal(findice, raiz);
        return meta->pontraiz;
    } else {
        raiz = buscaNoInterno(findice, posicaoPai);

        if (raiz->m == N_CHAVES) { // se estiver cheia
            for (int i=0; i<raiz->m; i++) {
                if (raiz->chaves[i] > chave) { // procura onde o registro seria inserido
                    inddomeio = N_CHAVES / 2;
                    if (i != inddomeio) { // se o que eu quero adicionar nao é o do meio, pega o do meio do registro
                        if (i > N_CHAVES/2) {chavedomeio = raiz->chaves[inddomeio];}
                        else {chavedomeio = raiz->chaves[inddomeio-1];}

                        NoInterno *novoNo = (NoInterno*)malloc(sizeof(NoInterno));
                        fseek(findice, 0, SEEK_END);
                        int posicaoNovoNo = ftell(findice) / sizeof(NoInterno);
                        novoNo->m = (N_CHAVES/2);
                        novoNo->pai = raiz->pai;
                        novoNo->apontaFolha = raiz->apontaFolha;
                        // raiz->prox = posicaoNovoNo;
                        raiz->m = N_CHAVES/2;

                        // printf("\n%d\n", raiz->pai);

                        // --------Ajeita chaves na nova ordem----------
                        
                        for (int j=0; j<N_CHAVES+1; j++) {
                            if (j == i){
                                chaves[j] = chave;
                                offset = 1;
                            } else {
                                chaves[j] = raiz->chaves[j - offset];
                            }
                        }

                        for (int j=0; j<(N_CHAVES/2); j++) {
                            novoNo->chaves[j] = chaves[j+(N_CHAVES/2)+1];
                        }

                        for (int j=0; j<(N_CHAVES/2); j++) {
                            raiz->chaves[j] = chaves[j];
                        }

                        // -------------- Ajeita ponteiros na nova ordem ------------------

                        offset = 0;
                        for (int j=0; j<N_CHAVES+2; j++) {
                            if (j == i+1){
                                ponteiros[j] = posicaoFilhoDir;
                                offset = 1;
                            } else {
                                ponteiros[j] = raiz->pont[j - offset];
                            }
                        }

                        for (int j=0; j<(N_CHAVES/2) + 1; j++) {
                            novoNo->pont[j] = ponteiros[j+(N_CHAVES/2)+1];
                        }

                        for (int j=0; j<(N_CHAVES/2)+1; j++) {
                            raiz->pont[j] = ponteiros[j];
                        }

                        // -------------- Atualiza Pais --------------------

                        if (apontaPraFolha) {
                            for (int j=0; j<(N_CHAVES/2)+1; j++) {
                                NoFolha *filho = buscaNoFolha(fdados, ponteiros[j+(N_CHAVES/2)+1]);

                                filho->pai = posicaoNovoNo;

                                escreveNoFolha(fdados, ponteiros[j+(N_CHAVES/2)+1], filho);
                            }
                        } else {
                            for (int j=0; j<(N_CHAVES/2)+1; j++) {
                                NoInterno *filho = buscaNoInterno(findice, ponteiros[j+(N_CHAVES/2)+1]);

                                filho->pai = posicaoNovoNo;

                                escreveNoInterno(findice, ponteiros[j+(N_CHAVES/2)+1], filho);
                            }
                        }

                        // ---------------------------------------------------------------

                        if (raiz->pai == -1) {
                            raiz->pai = posicaoNovoNo+1;
                            novoNo->pai = posicaoNovoNo+1;

                            escreveNoInterno(findice, posicaoNovoNo, novoNo);
                            escreveNoInterno(findice, posicaoPai, raiz);

                            sobeChaveProPai(findice, fdados, -1, posicaoPai, posicaoNovoNo, chaves[N_CHAVES/2], 0, meta);
                        } else {
                            escreveNoInterno(findice, posicaoNovoNo, novoNo);
                            escreveNoInterno(findice, posicaoPai, raiz);

                            sobeChaveProPai(findice, fdados, novoNo->pai, posicaoPai, posicaoNovoNo, chaves[N_CHAVES/2], 0, meta);
                        }

                        // escreveNoInterno(findice, posicaoNovoNo, novoNo);
                        // escreveNoInterno(findice, posicaoPai, raiz);

                        // raiz->pai = sobeChaveProPai(findice, fdados, novoNo->pai, posicaoPai, posicaoNovoNo, chaves[N_CHAVES/2], 0, meta);
                        // novoNo->pai = raiz->pai;

                        // escreveNoInterno(findice, posicaoNovoNo, novoNo);
                        // escreveNoInterno(findice, posicaoPai, raiz);
                        free(raiz);
                        free(novoNo);
                        return posicaoPai;
                        
                    } else {

                        
                        chavedomeio = chave;

                        NoInterno *novoNo = (NoInterno*)malloc(sizeof(NoInterno));
                        fseek(findice, 0, SEEK_END);
                        int posicaoNovoNo = ftell(findice) / sizeof(NoInterno);
                        novoNo->m = (N_CHAVES/2);
                        novoNo->pai = raiz->pai;
                        novoNo->apontaFolha = raiz->apontaFolha;
                        raiz->apontaFolha = posicaoNovoNo;
                        raiz->m = N_CHAVES/2;

                        // printf("\n%d\n", raiz->pai);

                        // ---------- Ajeita chaves na nova ordem -----------

                        for (int j=0; j<N_CHAVES+1; j++) {
                            if (j == i){
                                chaves[j] = chave;
                                offset = 1;
                            } else {
                                chaves[j] = raiz->chaves[j - offset];
                            }
                        }

                        for (int j=0; j<(N_CHAVES/2); j++) {
                            novoNo->chaves[j] = chaves[j+(N_CHAVES/2)];
                        }

                        for (int j=0; j<(N_CHAVES/2); j++) {
                            raiz->chaves[j] = chaves[j];
                        }

                        // -------------- Ajeita ponteiros na nova ordem -----------------

                        offset = 0;
                        for (int j=0; j<N_CHAVES+2; j++) {
                            if (j == i+1){
                                ponteiros[j] = posicaoFilhoDir;
                                offset = 1;
                            } else {
                                ponteiros[j] = raiz->pont[j - offset];
                            }
                        }


                        for (int j=0; j<(N_CHAVES/2) + 1; j++) {
                            novoNo->pont[j] = ponteiros[j+(N_CHAVES/2)+1];
                        }

                        for (int j=0; j<(N_CHAVES/2)+1; j++) {
                            raiz->pont[j] = ponteiros[j];
                        }


                        // -------------- Atualiza Pais --------------------

                        if (apontaPraFolha) {
                            for (int j=0; j<(N_CHAVES/2)+1; j++) {
                                NoFolha *filho = buscaNoFolha(fdados, ponteiros[j+(N_CHAVES/2)+1]);

                                filho->pai = posicaoNovoNo;

                                escreveNoFolha(fdados, ponteiros[j+(N_CHAVES/2)+1], filho);
                            }
                        } else {
                            for (int j=0; j<(N_CHAVES/2)+1; j++) {
                                NoInterno *filho = buscaNoInterno(findice, ponteiros[j+(N_CHAVES/2)+1]);

                                filho->pai = posicaoNovoNo;

                                escreveNoInterno(findice, ponteiros[j+(N_CHAVES/2)+1], filho);
                            }
                        }
                        

                        // ---------------------------------------------------------------

                        if (raiz->pai == -1) {
                            raiz->pai = posicaoNovoNo+1;
                            novoNo->pai = posicaoNovoNo+1;

                            escreveNoInterno(findice, posicaoNovoNo, novoNo);
                            escreveNoInterno(findice, posicaoPai, raiz);

                            sobeChaveProPai(findice, fdados, -1, posicaoPai, posicaoNovoNo, chaves[N_CHAVES/2], 0, meta);
                        } else {
                            escreveNoInterno(findice, posicaoNovoNo, novoNo);
                            escreveNoInterno(findice, posicaoPai, raiz);

                            sobeChaveProPai(findice, fdados, novoNo->pai, posicaoPai, posicaoNovoNo, chaves[N_CHAVES/2], 0, meta);
                        }

                        // escreveNoInterno(findice, posicaoNovoNo, novoNo);
                        // escreveNoInterno(findice, posicaoPai, raiz);

                        // raiz->pai = sobeChaveProPai(findice, fdados, novoNo->pai, posicaoPai, posicaoNovoNo, chaves[N_CHAVES/2], 0, meta);
                        // novoNo->pai = raiz->pai;

                        // escreveNoInterno(findice, posicaoNovoNo, novoNo);
                        // escreveNoInterno(findice, posicaoPai, raiz);
                        free(raiz);
                        free(novoNo);
                        return posicaoPai;

                    }
                }
            }
            chavedomeio = raiz->chaves[N_CHAVES/2];
            int i = raiz->m;

            NoInterno *novoNo = (NoInterno*)malloc(sizeof(NoInterno));
            fseek(findice, 0, SEEK_END);
            int posicaoNovoNo = ftell(findice) / sizeof(NoInterno);
            novoNo->m = (N_CHAVES/2);
            novoNo->pai = raiz->pai;
            novoNo->apontaFolha = raiz->apontaFolha;
            // raiz->prox = posicaoNovoNo;
            raiz->m = N_CHAVES/2;

            // printf("\n%d\n", raiz->pai);

            // --------Ajeita chaves na nova ordem----------
            
            for (int j=0; j<N_CHAVES+1; j++) {
                if (j == i){
                    chaves[j] = chave;
                    offset = 1;
                } else {
                    chaves[j] = raiz->chaves[j - offset];
                }
            }

            for (int j=0; j<(N_CHAVES/2); j++) {
                novoNo->chaves[j] = chaves[j+(N_CHAVES/2)+1];
            }

            for (int j=0; j<(N_CHAVES/2); j++) {
                raiz->chaves[j] = chaves[j];
            }

            // -------------- Ajeita ponteiros na nova ordem ------------------

            offset = 0;
            for (int j=0; j<N_CHAVES+2; j++) {
                if (j == i+1){
                    ponteiros[j] = posicaoFilhoDir;
                    offset = 1;
                } else {
                    ponteiros[j] = raiz->pont[j - offset];
                }
            }

            for (int j=0; j<(N_CHAVES/2) + 1; j++) {
                novoNo->pont[j] = ponteiros[j+(N_CHAVES/2)+1];
            }

            for (int j=0; j<(N_CHAVES/2)+1; j++) {
                raiz->pont[j] = ponteiros[j];
            }

            // -------------- Atualiza Pais --------------------

            if (apontaPraFolha) {
                for (int j=0; j<(N_CHAVES/2)+1; j++) {
                    NoFolha *filho = buscaNoFolha(fdados, ponteiros[j+(N_CHAVES/2)+1]);

                    filho->pai = posicaoNovoNo;

                    escreveNoFolha(fdados, ponteiros[j+(N_CHAVES/2)+1], filho);
                }
            } else {
                for (int j=0; j<(N_CHAVES/2)+1; j++) {
                    NoInterno *filho = buscaNoInterno(findice, ponteiros[j+(N_CHAVES/2)+1]);

                    filho->pai = posicaoNovoNo;

                    escreveNoInterno(findice, ponteiros[j+(N_CHAVES/2)+1], filho);
                }
            }

            // ---------------------------------------------------------------

            if (raiz->pai == -1) {
                raiz->pai = posicaoNovoNo+1;
                novoNo->pai = posicaoNovoNo+1;

                escreveNoInterno(findice, posicaoNovoNo, novoNo);
                escreveNoInterno(findice, posicaoPai, raiz);

                sobeChaveProPai(findice, fdados, -1, posicaoPai, posicaoNovoNo, chaves[N_CHAVES/2], 0, meta);
            } else {
                escreveNoInterno(findice, posicaoNovoNo, novoNo);
                escreveNoInterno(findice, posicaoPai, raiz);

                sobeChaveProPai(findice, fdados, novoNo->pai, posicaoPai, posicaoNovoNo, chaves[N_CHAVES/2], 0, meta);
            }

            // escreveNoInterno(findice, posicaoNovoNo, novoNo);
            // escreveNoInterno(findice, posicaoPai, raiz);

            // raiz->pai = sobeChaveProPai(findice, fdados, novoNo->pai, posicaoPai, posicaoNovoNo, chaves[N_CHAVES/2], 0, meta);
            // novoNo->pai = raiz->pai;

            // escreveNoInterno(findice, posicaoNovoNo, novoNo);
            // escreveNoInterno(findice, posicaoPai, raiz);
            free(raiz);
            free(novoNo);
            return posicaoPai;
            
        } else { // se tiver espaço
            for (int i=0; i<raiz->m; i++) { // procura onde a chave seria inserida
                if (raiz->chaves[i] > chave) { 
                    int j = raiz->m; // coloca a nova chave no lugar certo
                    while ( j > i ) {
                        raiz->chaves[j] = raiz->chaves[j-1];
                        raiz->pont[j+1] = raiz->pont[j];
                        j--;
                    }
                    raiz->chaves[j] = chave;
                    raiz->pont[j+1] = posicaoFilhoDir;

                    raiz->m++;
                    escreveNoInterno(findice, posicaoPai, raiz);
                    free(raiz);
                    return posicaoPai;
                }
            }
            raiz->chaves[raiz->m] = chave;
            raiz->pont[raiz->m+1] = posicaoFilhoDir;
            raiz->m++;
            escreveNoInterno(findice, posicaoPai, raiz);
            free(raiz);
            return posicaoPai;
        }

    }
    free(raiz);
    return posicaoPai;

}

int insereNaFolha(FILE *findice, FILE *fdados, Registro *reg, int posicao, Meta *meta) {
    int chavedomeio, inddomeio, offset = 0;
    NoFolha *raiz = buscaNoFolha(fdados, posicao);
    Registro regs[N_CHAVES+1];

    if (raiz->m == 0) { // folha vazia
        raiz->reg[0] = *reg;
        raiz->m++;
    } else if (raiz->m == N_CHAVES) { // folha cheia -> fazer split
        for (int i=0; i<raiz->m; i++) {
                if (raiz->reg[i].cpf / 100 > reg->cpf / 100) { // procura onde o registro seria inserido
                    inddomeio = N_CHAVES / 2;
                    if (i != inddomeio) { // se o que eu quero adicionar nao é o do meio, pega o do meio do registro
                        if (i > N_CHAVES/2) {chavedomeio = raiz->reg[inddomeio].cpf / 100;}
                        else {chavedomeio = raiz->reg[inddomeio-1].cpf / 100;}

                        NoFolha *novoNo = (NoFolha*)malloc(sizeof(NoFolha));
                        fseek(fdados, 0, SEEK_END);
                        int posicaoNovoNo = ftell(fdados) / sizeof(NoFolha);
                        novoNo->m = (N_CHAVES/2) + 1;
                        novoNo->pai = raiz->pai;
                        novoNo->prox = raiz->prox;
                        raiz->prox = posicaoNovoNo;
                        raiz->m = N_CHAVES/2;

                        for (int j=0; j<N_CHAVES+1; j++) {
                            if (j == i){
                                regs[j] = *reg;
                                offset = 1;
                            } else {
                                regs[j] = raiz->reg[j - offset];
                            }
                        }

                        for (int j=0; j<(N_CHAVES/2) + 1; j++) {
                            novoNo->reg[j] = regs[j+(N_CHAVES/2)];
                        }

                        for (int j=0; j<(N_CHAVES/2); j++) {
                            raiz->reg[j] = regs[j];
                        }

                        if (raiz->pai == -1) {
                            raiz->pai = 0;
                            novoNo->pai = 0;

                            escreveNoFolhaFimArquivo(fdados, novoNo);
                            escreveNoFolha(fdados, posicao, raiz);

                            sobeChaveProPai(findice, fdados, -1, posicao, posicaoNovoNo, regs[N_CHAVES/2].cpf / 100, 1, meta);
                        } else {
                            escreveNoFolhaFimArquivo(fdados, novoNo);
                            escreveNoFolha(fdados, posicao, raiz);

                            sobeChaveProPai(findice, fdados, novoNo->pai, posicao, posicaoNovoNo, regs[N_CHAVES/2].cpf / 100, 1, meta);
                        }

                        // escreveNoFolhaFimArquivo(fdados, novoNo);
                        // escreveNoFolha(fdados, posicao, raiz);

                        // sobeChaveProPai(findice, fdados, novoNo->pai, posicao, posicaoNovoNo, novoNo->reg[0]->cpf / 100, 1, meta);
                        // if (novoNo->pai == -1) {
                        //     novoNo->pai = raiz->pai;
                        // }
                        // novoNo->pai = raiz->pai;
                        meta->raizfolha = 0;

                        // escreveNoFolhaFimArquivo(fdados, novoNo);
                        // escreveNoFolha(fdados, posicao, raiz);
                        free(raiz);
                        free(novoNo);
                        return 0;
                        
                    } else {
                        chavedomeio = reg->cpf / 100;

                        NoFolha *novoNo = (NoFolha*)malloc(sizeof(NoFolha));
                        fseek(fdados, 0, SEEK_END);
                        int posicaoNovoNo = ftell(fdados) / sizeof(NoFolha);
                        novoNo->m = (N_CHAVES/2) + 1;
                        novoNo->pai = raiz->pai;
                        novoNo->prox = raiz->prox;
                        raiz->prox = posicaoNovoNo;
                        raiz->m = N_CHAVES/2;

                        for (int j=0; j<N_CHAVES+1; j++) {
                            if (j == i){
                                regs[j] = *reg;
                                offset = 1;
                            } else {
                                regs[j] = raiz->reg[j - offset];
                            }
                        }

                        for (int j=0; j<(N_CHAVES/2) + 1; j++) {
                            novoNo->reg[j] = regs[j+(N_CHAVES/2)];
                        }

                        for (int j=0; j<(N_CHAVES/2); j++) {
                            raiz->reg[j] = regs[j];
                        }


                        if (raiz->pai == -1) {
                            raiz->pai = 0;
                            novoNo->pai = 0;

                            escreveNoFolhaFimArquivo(fdados, novoNo);
                            escreveNoFolha(fdados, posicao, raiz);

                            sobeChaveProPai(findice, fdados, -1, posicao, posicaoNovoNo, regs[N_CHAVES/2].cpf / 100, 1, meta);
                        } else {
                            escreveNoFolhaFimArquivo(fdados, novoNo);
                            escreveNoFolha(fdados, posicao, raiz);

                            sobeChaveProPai(findice, fdados, novoNo->pai, posicao, posicaoNovoNo, regs[N_CHAVES/2].cpf / 100, 1, meta);
                        }


                        // escreveNoFolhaFimArquivo(fdados, novoNo);
                        // escreveNoFolha(fdados, posicao, raiz);

                        

                        // sobeChaveProPai(findice, fdados, novoNo->pai, posicao, posicaoNovoNo, novoNo->reg[0]->cpf / 100, 1, meta);
                        // if (novoNo->pai == -1) {
                        //     novoNo->pai = raiz->pai;
                        // } 
                        // novoNo->pai = raiz->pai;
                        meta->raizfolha = 0;

                        // escreveNoFolhaFimArquivo(fdados, novoNo);
                        // escreveNoFolha(fdados, posicao, raiz);
                        free(raiz);
                        free(novoNo);
                        return 0;

                    }
                }
            }
        chavedomeio = raiz->reg[N_CHAVES/2].cpf / 100;
        int i = raiz->m;

        NoFolha *novoNo = (NoFolha*)malloc(sizeof(NoFolha));
        fseek(fdados, 0, SEEK_END);
        int posicaoNovoNo = ftell(fdados) / sizeof(NoFolha);
        novoNo->m = (N_CHAVES/2) + 1;
        novoNo->pai = raiz->pai;
        novoNo->prox = raiz->prox;
        raiz->prox = posicaoNovoNo;
        raiz->m = N_CHAVES/2;

        for (int j=0; j<N_CHAVES+1; j++) {
            if (j == i){
                regs[j] = *reg;
                offset = 1;
            } else {
                regs[j] = raiz->reg[j - offset];
            }
        }

        for (int j=0; j<(N_CHAVES/2) + 1; j++) {
            novoNo->reg[j] = regs[j+(N_CHAVES/2)];
        }

        for (int j=0; j<(N_CHAVES/2); j++) {
            raiz->reg[j] = regs[j];
        }

        if (raiz->pai == -1) {
            raiz->pai = 0;
            novoNo->pai = 0;

            escreveNoFolhaFimArquivo(fdados, novoNo);
            escreveNoFolha(fdados, posicao, raiz);

            sobeChaveProPai(findice, fdados, -1, posicao, posicaoNovoNo, regs[N_CHAVES/2].cpf / 100, 1, meta);
        } else {
            escreveNoFolhaFimArquivo(fdados, novoNo);
            escreveNoFolha(fdados, posicao, raiz);

            sobeChaveProPai(findice, fdados, novoNo->pai, posicao, posicaoNovoNo, regs[N_CHAVES/2].cpf / 100, 1, meta);
        }

        // escreveNoFolhaFimArquivo(fdados, novoNo);
        // escreveNoFolha(fdados, posicao, raiz);

        // sobeChaveProPai(findice, fdados, novoNo->pai, posicao, posicaoNovoNo, novoNo->reg[0]->cpf / 100, 1, meta);
        // if (novoNo->pai == -1) {
        //     novoNo->pai = raiz->pai;
        // }
        // novoNo->pai = raiz->pai;
        meta->raizfolha = 0;

        // escreveNoFolhaFimArquivo(fdados, novoNo);
        // escreveNoFolha(fdados, posicao, raiz);
        free(raiz);
        free(novoNo);
        return 0;
        


    } else {
        // NoFolha *novoNo = (NoFolha*)malloc(sizeof(NoFolha));
        // fseek(fdados, 0, SEEK_END);
        // novoNo->m = N_CHAVES/2;
        // novoNo->pai = raiz->pai;
        // novoNo->prox = raiz->prox;
        // raiz->prox = ftell(fdados) / sizeof(NoFolha);
        // raiz->m = N_CHAVES/2;

        // for (int i=0; i<raiz->m; i++) {
        //         if (raiz->reg[i]->cpf / 100 > reg->cpf / 100) { // procura onde a chave seria inserida
        //             inddomeio = N_CHAVES / 2;
        //             if (i != inddomeio) { // se o que eu quero adicionar nao é o do meio, pega o do meio do registro
        //                 chavedomeio = raiz->reg[inddomeio]->cpf / 100;
        //             } else {
        //                 chavedomeio = reg->cpf / 100;
        //             }
        //         }
        //     }

        // for (int i=0; i<N_CHAVES+1;i++) {
        //     if (i == inddomeio) {
        //         novoNo->reg[0] = reg;
        //     } else if (i < N_CHAVES/2){
        //         raiz->
        //     }
        // }

        for (int i=0; i<raiz->m; i++) { // tem espaço pra adicionar na folha
            if (raiz->reg[i].cpf / 100 > reg->cpf / 100) { // procura onde a chave deve ser inserida
                int j = raiz->m;
                    while ( j > i ) {
                    raiz->reg[j] = raiz->reg[j-1];
                    j--;
                    }
                    raiz->reg[j] = *reg;
                    raiz->m++;
                    escreveNoFolha(fdados, posicao, raiz);
                    free(raiz);
                    return 1;
            }
        }
        raiz->reg[raiz->m] = *reg;
        raiz->m++;
    }

    escreveNoFolha(fdados, posicao, raiz);
    free(raiz);
    return 1;
}

int insereNaArvore(FILE *findice, FILE *fdados, FILE *fmeta, Registro *reg) {
    Meta *meta = leMetadados(fmeta);
    int ponteiroParaFolhaBuscada = buscaNaArvoreInsercao(findice, fdados, meta->raizfolha, meta->pontraiz, reg->cpf);

    if (ponteiroParaFolhaBuscada == -1) {
        printf("Inserção inválida, elemento já está na árvore.\n");
        return 0;
    } else {
        if (meta->raizfolha) { // se a raiz é folha, insere na folha.
            insereNaFolha(findice, fdados, reg, meta->pontraiz, meta);
        } else { // se a raiz não é folha, busca a folha onde a raiz seria inserida.
            insereNaFolha(findice, fdados, reg, ponteiroParaFolhaBuscada, meta);
        }
        meta->n++;
    }
    escreveMetadados(fmeta, meta);
}

// --------- CONSTRUIR ÁRVORE DO ARQUIVO DE REGISTROS ----------

void constroiArvoreDoZeroRegistros(FILE *fregs, FILE *findice, FILE *fdados, FILE *fmeta, int numRegs) {
    Registro *reg;
    Meta *meta = leMetadados(fmeta);

    inicializaArvoreVazia(fdados, fmeta);

    for (int i=0; i<numRegs; i++) {
        reg = buscaRegistro(fregs, i);
        insereNaArvore(findice, fdados, fmeta, reg);
        
        // free(reg);
    }
}

// ------------------------- Remoção ---------------------------

void del(Registro arr[], int *n, long long key) {
  
  	// Find the element
  	int i = 0;
  	while (arr[i].cpf / 100 != key) i++;
  	
    // Shifting the right side elements one
  	// position towards left
    for (int j = i; j < *n - 1; j++) {
        arr[j] = arr[j + 1];
    }
  
  	// Decrease the size
    (*n)--; 
}

// Função auxiliar para encontrar o índice do ponteiro para um filho no nó pai.
int achaIndiceDoPonteiro(NoInterno *pai, int ponteiroFilho) {
    for (int i = 0; i <= pai->m; i++) {
        if (pai->pont[i] == ponteiroFilho) {
            return i;
        }
    }
    return -1; // Não encontrado
}

// Função recursiva para remover de um nó interno e rebalancear se necessário.
void removeDoInterno(FILE *findice, FILE *fdados, int posNoAtual, int posPai, Meta *meta) {
    NoInterno *noAtual = buscaNoInterno(findice, posNoAtual);

    // Se o nó atual tem menos chaves que o mínimo permitido, precisa rebalancear.
    if (noAtual->m < ORDEM) {
        // Se o nó atual é a raiz
        if (posPai == -1) {
            // Se a raiz ficou sem chaves, seu único filho se torna a nova raiz.
            if (noAtual->m == 0) {
                meta->pontraiz = noAtual->pont[0];
                
                // Se o novo nó raiz aponta para folhas, atualiza metadados.
                NoInterno* novaRaizTeste = buscaNoInterno(findice, meta->pontraiz);
                if (novaRaizTeste->apontaFolha) {
                     // Este caso é mais complexo, a árvore pode ter diminuido de altura
                     // e a nova raiz pode ser uma folha
                } else {
                    NoInterno* novaRaiz = buscaNoInterno(findice, meta->pontraiz);
                    novaRaiz->pai = -1;
                    escreveNoInterno(findice, meta->pontraiz, novaRaiz);
                    free(novaRaiz);
                }
                free(novaRaizTeste);
            }
            // Se a raiz ainda tem chaves, ela é válida.
            escreveNoInterno(findice, posNoAtual, noAtual);
            free(noAtual);
            return;
        }

        NoInterno *pai = buscaNoInterno(findice, posPai);
        int indiceMeuPonteiroNoPai = achaIndiceDoPonteiro(pai, posNoAtual);

        // tenta redistribuir com o irmão da direita
        if (indiceMeuPonteiroNoPai < pai->m) {
            NoInterno *irmaoDir = buscaNoInterno(findice, pai->pont[indiceMeuPonteiroNoPai + 1]);
            if (irmaoDir->m > ORDEM) {
                // pega a chave do pai
                noAtual->chaves[noAtual->m] = pai->chaves[indiceMeuPonteiroNoPai];
                noAtual->m++;

                // pega o primeiro ponteiro e chave do irmão da direita
                noAtual->pont[noAtual->m] = irmaoDir->pont[0];
                
                pai->chaves[indiceMeuPonteiroNoPai] = irmaoDir->chaves[0];
                

                for (int i = 0; i < irmaoDir->m - 1; i++) {
                    irmaoDir->chaves[i] = irmaoDir->chaves[i + 1];
                    irmaoDir->pont[i] = irmaoDir->pont[i + 1];
                }
                irmaoDir->pont[irmaoDir->m - 1] = irmaoDir->pont[irmaoDir->m];
                irmaoDir->m--;

                escreveNoInterno(findice, posNoAtual, noAtual);
                escreveNoInterno(findice, pai->pont[indiceMeuPonteiroNoPai + 1], irmaoDir);
                escreveNoInterno(findice, posPai, pai);

                free(irmaoDir);
                free(pai);
                free(noAtual);
                return;
            }
            free(irmaoDir);
        }

        // tenta redistribuir com o da esquerda
        if (indiceMeuPonteiroNoPai > 0) {
            NoInterno *irmaoEsq = buscaNoInterno(findice, pai->pont[indiceMeuPonteiroNoPai - 1]);
            if (irmaoEsq->m > ORDEM) {
                // desloca as chaves
                for (int i = noAtual->m; i > 0; i--) {
                    noAtual->chaves[i] = noAtual->chaves[i-1];
                    noAtual->pont[i+1] = noAtual->pont[i];
                }
                noAtual->pont[1] = noAtual->pont[0];

                noAtual->chaves[0] = pai->chaves[indiceMeuPonteiroNoPai - 1];
                noAtual->m++;

                noAtual->pont[0] = irmaoEsq->pont[irmaoEsq->m];
                
                pai->chaves[indiceMeuPonteiroNoPai - 1] = irmaoEsq->chaves[irmaoEsq->m - 1];
                irmaoEsq->m--;

                escreveNoInterno(findice, posNoAtual, noAtual);
                escreveNoInterno(findice, pai->pont[indiceMeuPonteiroNoPai - 1], irmaoEsq);
                escreveNoInterno(findice, posPai, pai);

                free(irmaoEsq);
                free(pai);
                free(noAtual);
                return;
            }
            free(irmaoEsq);
        }


        if (indiceMeuPonteiroNoPai < pai->m) {
            NoInterno *irmaoDir = buscaNoInterno(findice, pai->pont[indiceMeuPonteiroNoPai + 1]);
            

            noAtual->chaves[noAtual->m] = pai->chaves[indiceMeuPonteiroNoPai];
            noAtual->m++;
            

            for(int i = 0; i < irmaoDir->m; i++){
                noAtual->chaves[noAtual->m] = irmaoDir->chaves[i];
                noAtual->pont[noAtual->m] = irmaoDir->pont[i];
                noAtual->m++;
            }
            noAtual->pont[noAtual->m] = irmaoDir->pont[irmaoDir->m];
            
            for (int i = indiceMeuPonteiroNoPai; i < pai->m - 1; i++) {
                pai->chaves[i] = pai->chaves[i + 1];
                pai->pont[i + 1] = pai->pont[i + 2];
            }
            pai->m--;

            escreveNoInterno(findice, posNoAtual, noAtual);
            escreveNoInterno(findice, posPai, pai);
            

            free(irmaoDir);

            removeDoInterno(findice, fdados, posPai, pai->pai, meta);

        } else { 
            NoInterno *irmaoEsq = buscaNoInterno(findice, pai->pont[indiceMeuPonteiroNoPai - 1]);
            

            irmaoEsq->chaves[irmaoEsq->m] = pai->chaves[indiceMeuPonteiroNoPai - 1];
            irmaoEsq->m++;

            for (int i = 0; i < noAtual->m; i++) {
                irmaoEsq->chaves[irmaoEsq->m] = noAtual->chaves[i];
                irmaoEsq->pont[irmaoEsq->m] = noAtual->pont[i];
                irmaoEsq->m++;
            }
            irmaoEsq->pont[irmaoEsq->m] = noAtual->pont[noAtual->m];

            for (int i = indiceMeuPonteiroNoPai - 1; i < pai->m - 1; i++) {
                pai->chaves[i] = pai->chaves[i + 1];
                pai->pont[i + 1] = pai->pont[i + 2];
            }
            pai->m--;

            escreveNoInterno(findice, pai->pont[indiceMeuPonteiroNoPai - 1], irmaoEsq);
            escreveNoInterno(findice, posPai, pai);

            free(irmaoEsq);
            removeDoInterno(findice, fdados, posPai, pai->pai, meta);
        }

        free(pai);
    }
    escreveNoInterno(findice, posNoAtual, noAtual);
    free(noAtual);
}


int removeDaFolha(FILE *findice, FILE *fdados, long long cpf, int posicaoDaFolha, Meta *meta) {
    NoFolha *folha = buscaNoFolha(fdados, posicaoDaFolha);
    long long chave = cpf / 100;
    int posPai = folha->pai;

    // remove o registro da folha
    del(folha->reg, &folha->m, chave);

    // se a raiz é uma folha, dá pra ter de 0 a N_CHAVES. Se ficar com 0, a árvore ta vazia.
    if (meta->raizfolha) {
        escreveNoFolha(fdados, posicaoDaFolha, folha);
        free(folha);
        return 1;
    }

    // se a folha ficou com menos chaves que o mínimo, tenta rebalancear.
    if (folha->m < ORDEM) {
        NoInterno *pai = buscaNoInterno(findice, posPai);
        int indiceMeuPonteiroNoPai = achaIndiceDoPonteiro(pai, posicaoDaFolha);

        // redistribuir com da direita
        if (indiceMeuPonteiroNoPai < pai->m) {
            int posIrmaoDir = pai->pont[indiceMeuPonteiroNoPai + 1];
            NoFolha *irmaoDir = buscaNoFolha(fdados, posIrmaoDir);

            if (irmaoDir->m > ORDEM) {
                folha->reg[folha->m] = irmaoDir->reg[0];// pega o primeiro registro do irmão direito
                folha->m++;
                
                // tira o registro do irmão direito
                del(irmaoDir->reg, &irmaoDir->m, irmaoDir->reg[0].cpf / 100);

                // atualiza a chave no pai
                pai->chaves[indiceMeuPonteiroNoPai] = irmaoDir->reg[0].cpf / 100;
                
                escreveNoFolha(fdados, posicaoDaFolha, folha);
                escreveNoFolha(fdados, posIrmaoDir, irmaoDir);
                escreveNoInterno(findice, posPai, pai);

                free(irmaoDir);
                free(pai);
                free(folha);
                return 1;
            }
            free(irmaoDir);
        }

        // tenta redistribuir com da direita
        if (indiceMeuPonteiroNoPai > 0) {
            int posIrmaoEsq = pai->pont[indiceMeuPonteiroNoPai - 1];
            NoFolha *irmaoEsq = buscaNoFolha(fdados, posIrmaoEsq);

            if (irmaoEsq->m > ORDEM) {
                // joga os registros da folha pra direita
                for (int i = folha->m; i > 0; i--) {
                    folha->reg[i] = folha->reg[i - 1];
                }
                // pega o ultimo da folha esqeuerda
                folha->reg[0] = irmaoEsq->reg[irmaoEsq->m - 1];
                folha->m++;
                irmaoEsq->m--;

                // troca a chave do pai pra nova
                pai->chaves[indiceMeuPonteiroNoPai - 1] = folha->reg[0].cpf / 100;

                escreveNoFolha(fdados, posicaoDaFolha, folha);
                escreveNoFolha(fdados, posIrmaoEsq, irmaoEsq);
                escreveNoInterno(findice, posPai, pai);
                
                free(irmaoEsq);
                free(pai);
                free(folha);
                return 1;
            }
            free(irmaoEsq);
        }

        // concatena com o da direita
        if (indiceMeuPonteiroNoPai < pai->m) {
            int posIrmaoDir = pai->pont[indiceMeuPonteiroNoPai + 1];
            NoFolha *irmaoDir = buscaNoFolha(fdados, posIrmaoDir);

            for (int i = 0; i < irmaoDir->m; i++) {
                folha->reg[folha->m] = irmaoDir->reg[i];
                folha->m++;
            }
            // aualiza o ponteiro de próximo
            folha->prox = irmaoDir->prox;
            
            escreveNoFolha(fdados, posicaoDaFolha, folha);

            int chaveRemovidaDoPai = pai->chaves[indiceMeuPonteiroNoPai];
            for (int i = indiceMeuPonteiroNoPai; i < pai->m - 1; i++) {
                pai->chaves[i] = pai->chaves[i + 1];
                pai->pont[i + 1] = pai->pont[i + 2];
            }
            pai->m--;
            escreveNoInterno(findice, posPai, pai);

            free(irmaoDir);
            removeDoInterno(findice, fdados, posPai, pai->pai, meta);
        } 
        else {
            int posIrmaoEsq = pai->pont[indiceMeuPonteiroNoPai - 1];
            NoFolha *irmaoEsq = buscaNoFolha(fdados, posIrmaoEsq);
            for (int i = 0; i < folha->m; i++) {
                irmaoEsq->reg[irmaoEsq->m] = folha->reg[i];
                irmaoEsq->m++;
            }
            irmaoEsq->prox = folha->prox;

            escreveNoFolha(fdados, posIrmaoEsq, irmaoEsq);
            
            int chaveRemovidaDoPai = pai->chaves[indiceMeuPonteiroNoPai-1];
            for (int i = indiceMeuPonteiroNoPai-1; i < pai->m - 1; i++) {
                pai->chaves[i] = pai->chaves[i + 1];
                pai->pont[i+1] = pai->pont[i + 2];
            }
            pai->m--;
            escreveNoInterno(findice, posPai, pai);

            free(irmaoEsq);
            removeDoInterno(findice, fdados, posPai, pai->pai, meta);
        }
        free(pai);
    } else {
        //se nao deu problema, so escreve a folhq
        escreveNoFolha(fdados, posicaoDaFolha, folha);
    }

    free(folha);
    return 1;
}

int removeDaArvore(FILE *findice, FILE *fdados, FILE *fmeta, long long cpf) {
    Meta *meta = leMetadados(fmeta);
    int ponteiroParaFolhaBuscada = buscaNaArvore(findice, fdados, meta->raizfolha, meta->pontraiz, cpf);

    if (ponteiroParaFolhaBuscada == -1) {
        printf("Remoção inválida, elemento não está na árvore.\n");
        free(meta);
        return 0;
    } else {
        removeDaFolha(findice, fdados, cpf, ponteiroParaFolhaBuscada, meta);
        meta->n--;
    }
    escreveMetadados(fmeta, meta);
    free(meta);
    return 1;
}

// -----------------------------------------------------------

int menuArvoreBMais(void) {
    FILE *fregs, *findice, *fmeta, *fdados, *fp;
    int tamArqReg, construiu = 0;
    char c, continua=0, b, nomeArqRegs[50] = "files/bplusRegistros.bin", nomeArqInd[50] = "files/arquivoIndicesArvore.bin";
    char nomeArqDados[50] = "files/arquivoDadosArvore.bin", nomeArqMetadados[50] = "files/arquivoMetadadosArvore.txt", nomeArq[50];


    fregs = abreArquivo(nomeArqRegs, "rb");
    fseek(fregs, 0, SEEK_END);
    tamArqReg = ftell(fregs)/(sizeof(long long)+(sizeof(char)*50)+sizeof(int)); // numero de registros
    fseek(fregs, 0, SEEK_SET);

    fmeta = abreArquivo(nomeArqMetadados, "rb+");

    findice = abreArquivo(nomeArqInd, "wb+");

    fdados = abreArquivo(nomeArqDados, "wb+");
    fseek(fdados, 0, SEEK_END);
    if (ftell(fdados) != 0){
        construiu = 1;
    }

    Meta *meta = leMetadados(fmeta);

    do {
        printf("Digite a operação à fazer: \n\n");

        printf("Atualmente, o arquivo de registros é: %s, o de Dados é: %s e o de Índices é: %s\n\n", nomeArqRegs, nomeArqDados, nomeArqInd);
        printf("b - Buscar um valor pelo cpf na Árvore.\n");
        printf("i - Inserir registro na Árvore.\n");
        printf("r - Remover registro da Árvore.\n");
        printf("a - Trocar o arquivo de registros.\n");
        printf("d - Trocar o arquivo de dados.\n");
        printf("f - Trocar o arquivo de índices.\n");
        printf("g - Trocar o arquivo de metadados.\n");
        printf("c - Construir ÁrvoreB+ baseado no arquivo de registros (deleta o arquivo dados e índices atuais).\n");
        // opção e a tabela precisa estar alocada.
        printf("e - Adiciona registros do arquivo de registros na tabela Hash (preserva os dados do arquivo destino)\n");
        printf("m - Mostra as folhas da ÁrvoreB+ ordenadas pelos registros dentro delas.\n");
        printf("t - Escreve as folhas da ÁrvoreB+ ordenadas pelos registros dentro delas (txt).\n");
        printf("s - Sair do programa.\n");
        scanf("%c", &c);

        switch(c) {
            case 't':
                printf("\nDigite o nome do novo arquivo: \n");
                scanf("%s", nomeArq);
                // sprintf(nomeArq, "files/%s", nomeArqPlaceholder);

                fp = fopen(nomeArq, "w");
                if (fp == NULL) {
                    perror("Não foi possível abrir o arquivo.");
                    return 1;
                }

                escreveTodasAsFolhasNaOrdemCrescente(fdados, fp);
                break;
            case 'b':
                if (!construiu) {
                    printf("\nVocê ainda não construiu nenhuma árvore!\n");
                } else {
                    long long cpf;
                    printf("Digite o CPF à procurar (apenas dígitos): ");
                    scanf("%lld", &cpf);
                    int ponteiroFolha = buscaNaArvore(findice, fdados, meta->raizfolha, meta->pontraiz, cpf);
                    if (ponteiroFolha != -1) {
                        imprimeFolha(fdados, buscaNaArvore(findice, fdados, meta->raizfolha, meta->pontraiz, cpf));
                    } else {
                        printf("\nO registro não está na árvore.\n");
                    }
                }
                break;
            case 'i':
                if (!construiu) {
                    printf("\nVocê ainda não construiu nenhuma árvore!\n");
                } else {
                
                Registro *reg = (Registro*)malloc(sizeof(Registro));

                printf("Digite os dados do registro que quer inserir: \n");
                printf("CPF (apenas dígitos): ");
                scanf("%lld", &reg->cpf);

                while ((c = getchar()) != '\n' && c != EOF);

                printf("Nome: ");
                scanf("%[^\n]", reg->nome);
                printf("Nota: ");
                scanf("%d", &reg->nota);

                insereNaArvore(findice, fdados, fmeta, reg);
                }
                break;
            case 'r':

                if (!construiu) {
                    printf("\nVocê ainda não construiu nenhuma árvore!\n");
                } else {
                long long cpf;
                printf("\nDigite o CPF a remover (apenas dígitos): ");
                scanf("%lld", &cpf);

                removeDaArvore(findice, fdados, fmeta, cpf);
                }
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
                fseek(fregs, 0, SEEK_SET);
                break;
            case 'd':
                fclose(fdados);

                printf("\nDigite o nome do novo arquivo de dados: \n");
                scanf("%s", nomeArqDados);

                fdados = fopen(nomeArqDados, "rb");
                if (fdados == NULL) {
                    perror("Não foi possível abrir o arquivo.");
                    return 1;
                }
                break;
            case 'f':
                fclose(findice);

                printf("\nDigite o nome do novo arquivo de índices: \n");
                scanf("%s", nomeArqInd);

                findice = fopen(nomeArqInd, "rb");
                if (findice == NULL) {
                    perror("Não foi possível abrir o arquivo.");
                    return 1;
                }
                break;  
            case 'g':
                fclose(fmeta);

                printf("\nDigite o nome do novo arquivo de metadados: \n");
                scanf("%s", nomeArqMetadados);

                fmeta = fopen(nomeArqMetadados, "rb");
                if (fmeta == NULL) {
                    perror("Não foi possível abrir o arquivo.");
                    return 1;
                }
                break;  
            case 'c':

                int n;
                printf("\nDigite com quantos registros deseja construir a árvore (-1 para todos do arquivo): \n");
                scanf("%d", &n);

                if (n == -1) {
                    constroiArvoreDoZeroRegistros(fregs, findice, fdados, fmeta, tamArqReg);
                } else {
                    constroiArvoreDoZeroRegistros(fregs, findice, fdados, fmeta, n);
                }
                construiu = 1;
                
                break;
            case 'e':
                if (!construiu) {
                    printf("\nVocê ainda não construiu nenhuma árvore!\n");
                } else {
                    Registro *reg;
                    Meta *meta = leMetadados(fmeta);

                    int n;
                    printf("\nDigite com quantos registros deseja construir a árvore (-1 para todos do arquivo): \n");
                    scanf("%d", &n);

                    if (n == -1) {
                        for (int i=0; i<tamArqReg; i++) {
                            reg = buscaRegistro(fregs, i);
                            insereNaArvore(findice, fdados, fmeta, reg);
                        }
                    } else {
                        for (int i=0; i<n; i++) {
                        reg = buscaRegistro(fregs, i);
                        insereNaArvore(findice, fdados, fmeta, reg);
                        }
                    }
                }
                break;
            case 'm':
                imprimeTodasAsFolhasNaOrdemCrescente(fdados);
                break;
            case 's':
                exit(0);
                break;
        }
        
        pressioneEnterParaContinuar();
    } while (c != 's');


    constroiArvoreDoZeroRegistros(fregs, findice, fdados, fmeta, 10);
    
    imprimeTodasAsFolhasNaOrdemCrescente(fdados);

    imprimeTodosNosInternos(findice);

    imprimeMetadados(fmeta);


    fclose(fregs);
    fclose(findice);
}


int main(void) {
    char opcao;
    srand(time(NULL)); 

    do {
        // menu principal
        printf("\n\n==================== MENU PRINCIPAL ====================\n");
        printf("Escolha uma operacao ou estrutura:\n");
        printf("1. Gerar Arquivos de Registros\n");
        printf("2. Heap\n");
        printf("3. Tabela Hash\n");
        printf("4. Árvore B+\n");
        printf("5. Sair do Programa\n");
        printf("========================================================\n");
        printf("Opcão: ");
        
        scanf(" %c", &opcao);
        limparBufferEntrada();

        switch (opcao) {
            case '1':
                gerarArquivosDeRegistros();
                pressioneEnterParaContinuar();
                break;
            case '2':
                menuHeap();
                break;
            case '3':
                menuHash();
                break;
            case '4':
                menuArvoreBMais(); // Chamaria a interface da Árvore B+
                pressioneEnterParaContinuar();
                break;
            case '5':
                printf("Encerrando o programa.\n");
                break;
            default:
                printf("Opção inválida. Tente novamente.\n");
                pressioneEnterParaContinuar();
                break;
        }

    } while (opcao != '5');

    return 0;
}