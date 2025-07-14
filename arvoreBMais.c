#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define ORDEM 64/4 // tamanho do bloco registro / tamanho da chave
#define N_CHAVES 2*ORDEM

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

// -----------------------------------------------------------------------------------------

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

// NoFolha *buscaNaArvoreFolhas(FILE *fdados, int ponteiroRaiz, long long chave) {
//     NoFolha *raiz = buscaNoFolha(fdados, ponteiroRaiz);
    
//     for (int i=0; i<raiz->m; i++) {
//         if (raiz->reg[i]->cpf / 100 == chave) { // procura a chave
//             return raiz; // retorna o nó que está sendo procurado
//         }
//     }
//     free(raiz);
//     return NULL;
// }

// NoFolha *buscaNaArvoreInterna(FILE *findice, FILE *fdados, int ponteiroRaiz, long long chave) {
//     NoInterno *raiz = buscaNoInterno(findice, ponteiroRaiz);
//     NoFolha *nofolha;

//     for (int i=0; i<raiz->m; i++) {
//         if (raiz->chaves[i] > chave) { // procura a chave
//             if (raiz->apontaFolha) {
//                 // nofolha = buscaNoFolha(fdados, raiz->pont[i], chave);

//                 return nofolha;
//             } else {
//                 nofolha = buscaNaArvoreInterna(findice, fdados, raiz->pont[i], chave);

//                 return nofolha;
//             }
//         } else if (raiz->chaves[i] == chave) {
//             if (raiz->apontaFolha) {
//                 nofolha = buscaNaArvoreFolhas(fdados, raiz->pont[i+1], chave);

//                 return nofolha;
//             } else {
//                 nofolha = buscaNaArvoreInterna(findice, fdados, raiz->pont[i+1], chave);

//                 return nofolha;
//             }
//         }
//     }

//     if (raiz->apontaFolha) {
//         nofolha = buscaNaArvoreFolhas(fdados, raiz->pont[raiz->m], chave);

//         return nofolha;
//     } else {
//         nofolha = buscaNaArvoreInterna(findice, fdados, raiz->pont[raiz->m], chave);

//         return nofolha;
//     }
// }


// NoFolha *buscaNaArvore(FILE *findice, FILE *fdados, int flagFolhaRaiz, int ponteiroRaiz, long long cpf) {
//     long long chave = cpf / 100;
//     NoFolha *nofolha;

//     if (flagFolhaRaiz) { // se a raiz for folha:
//         nofolha = buscaNaArvoreFolhas(fdados, ponteiroRaiz, chave);

//         return nofolha;
//     } else {
//         nofolha = buscaNaArvoreInterna(findice, fdados, ponteiroRaiz, chave);

//         return nofolha;
//     }

// }


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

// void ajustaAlturas(FILE *findice, FILE *fdados, int posicaoPai) {

// }

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

int main(void) {
    FILE *fregs, *findice, *fmeta, *fdados;
    int tamArqReg, construiu = 0;
    char c, continua=0, b, nomeArqRegs[50] = "files/bplusRegistros.bin", nomeArqInd[50] = "files/arquivoIndicesArvore.bin";
    char nomeArqDados[50] = "files/arquivoDadosArvore.bin", nomeArqMetadados[50] = "files/arquivoMetadadosArvore.txt";


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
        printf("c - Construir ÁrvoreB+ baseado no arquivo de registros (deleta o arquivo dados e índices atuais).\n");
        // opção e a tabela precisa estar alocada.
        printf("e - Adiciona registros do arquivo de registros na tabela Hash (preserva os dados do arquivo destino)\n");
        printf("m - Mostra as folhas da ÁrvoreB+ ordenadas pelos registros dentro delas.\n");
        printf("s - Sair do programa.\n");
        scanf("%c", &c);

        switch(c) {
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
        
        do {
            printf("\nAperte enter para continuar.\n\n");
            scanf("%c", &continua);
        } while (continua != '\n');
    } while (c != 's');


    constroiArvoreDoZeroRegistros(fregs, findice, fdados, fmeta, 10);
    
    imprimeTodasAsFolhasNaOrdemCrescente(fdados);

    imprimeTodosNosInternos(findice);

    imprimeMetadados(fmeta);


    fclose(fregs);
    fclose(findice);
}
