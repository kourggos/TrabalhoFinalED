#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

struct aluno {
    long long cpf;
    char nome[50];
    int nota;
};typedef struct aluno Aluno;

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

void criaReg(FILE *fp, FILE *arqNomes, FILE *arqTexto, int i) {
    long long cpf, nota, limCPF, teste;
    char nome[50], zeroscpf[10], zerosMat[5];

    limCPF = 100000000000;

    // cpf = 10000 + i*100;

    cpf = geraRand(limCPF);
    // cpf = 92099189;
    // teste = cpf;

    // for (int i=0; i<11; i++) {
    //     teste = teste*(pow(10, i));
    //     // printf("%lld ", teste);
    //     if (teste < limCPF/10) {
    //         zeroscpf[i] = 48;
    //     } else {
    //         zeroscpf[i] = 0;
    //         break;
    //     }
    // }

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

int main(void) {
    char nomeArqRegs[50], nomeArqTxtRegs[50], nomeArvRegs[50], nomeHeapRegs[50], nomeHashRegs[50];
    FILE *fpreg, *fpnomes, *fptxt;
    srand(time(NULL));

    for (int j=0; j<20; j++) {
        sprintf(nomeArqRegs, "files/registros%d.bin", j);
        fpreg = fopen(nomeArqRegs, "wb");
        fpnomes = fopen("files/nomes.txt", "r");
        sprintf(nomeArqTxtRegs, "files/registros%d.txt", j);
        fptxt = fopen(nomeArqTxtRegs, "w");

        for (int i=0; i<1000; i++) {
            criaReg(fpreg, fpnomes, fptxt, i);
        }
        
        fflush(fpreg);

        fclose(fpreg);
        fclose(fpnomes);

        sprintf(nomeHeapRegs, "files/heapRegistros%d.bin", j);
        sprintf(nomeHashRegs, "files/hashRegistros%d.bin", j);
        sprintf(nomeArvRegs, "files/bplusRegistros%d.bin", j);
        copiaArquivo(nomeArqRegs, nomeHeapRegs);
        copiaArquivo(nomeArqRegs, nomeHashRegs);
        copiaArquivo(nomeArqRegs, nomeArvRegs);
    }
}