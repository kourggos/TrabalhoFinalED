#include <stdio.h>
#include <stdlib.h>
#define MAX 40

void sobeHeap(int heap[], int i) {
    int pai = (i-1)/2, temp;
    if (i != 0) {
        if (heap[i] > heap[pai]) {
            temp = heap[i];
            heap[i] = heap[pai];
            heap[pai] = temp;
            sobeHeap(heap, pai);
        }
    }
}

void desceHeap(int heap[], int i, int n) {
    int filho1 = 2*i+1, filho2 = 2*i+2, temp;
    if (filho1 <= n-1) {
        if (filho2 < n-1) {
            if (heap[i] < heap[filho1] || heap[i] < heap[filho2]) {
                if (heap[filho1] >= heap[filho2]) {
                    temp = heap[i];
                    heap[i] = heap[filho1];
                    heap[filho1] = temp;
                    desceHeap(heap, filho1, n);
                } else {
                    temp = heap[i];
                    heap[i] = heap[filho2];
                    heap[filho2] = temp;
                    desceHeap(heap, filho2, n);
                }
            }
        } else {
            if (heap[i] < heap[filho1]) {
                temp = heap[i];
                heap[i] = heap[filho1];
                heap[filho1] = temp;
                desceHeap(heap, filho1, n);
            }
        }
    }
}

void constroiHeap(int heap[], int n) {
    int comeco = n/2 - 1;
    for (int i = comeco; i>=0; i--) {
        desceHeap(heap, i, n);
    }
}

int removeHeap(int heap[], int n) {
    heap[0] = heap[n-1];
    desceHeap(heap, 0, n-1);
    return n-1;
}

int insereHeap(int heap[], int n, int add) {
    if (n+1 < MAX) {
        heap[n] = add;
        sobeHeap(heap, n);
    }
    return n+1;
}

int buscaHeap(int heap[], int n) {
    return heap[0];
}

void imprimeVetor(int vetor[], int n) {
    for (int i=0; i<n; i++) {
        printf("%d ", vetor[i]);
    }
}

void imprimeHeap(int heap[], int n) {
    int h = 1;
    for (int i=0; i<n; i++) {
        if (i == h) {
            h = 2*h+1;
            printf("\n");
        }
        printf("%d ", heap[i]);
    }
}

int main(void) {
    int heap[MAX], n=0;

    for (int i=0;i<10;i++) {
        heap[i] = 3*i;
        n+=1;
    }

    printf("Vetor: ");
    imprimeVetor(heap, n);
    printf("\tn: %d\n", n);

    constroiHeap(heap, n);
    printf("Heap: \n");
    imprimeHeap(heap, n);
    printf("\n");
    printf("Vetor: ");
    imprimeVetor(heap, n);
    printf("\n");

    printf("Heap apos insercoes: \n");
    n = insereHeap(heap, n, 7);
    n = insereHeap(heap, n, 7);
    n = insereHeap(heap, n, 7);
    n = insereHeap(heap, n, 7);
    n = insereHeap(heap, n, 7);
    n = insereHeap(heap, n, 7);
    n = insereHeap(heap, n, 7);
    n = insereHeap(heap, n, 7);
    n = insereHeap(heap, n, 7);
    n = insereHeap(heap, n, 7);
    n = insereHeap(heap, n, 7);
    n = insereHeap(heap, n, 29);
    n = insereHeap(heap, n, 29);
    n = insereHeap(heap, n, 29);
    n = insereHeap(heap, n, 29);
    n = insereHeap(heap, n, 717);
    n = insereHeap(heap, n, 29);
    n = insereHeap(heap, n, 100);
    n = insereHeap(heap, n, 76);
    n = insereHeap(heap, n, 44);
    n = insereHeap(heap, n, 13);
    n = insereHeap(heap, n, 29);
    
    imprimeHeap(heap, n);
    printf("\n");
    printf("Vetor após inserções: ");
    imprimeVetor(heap, n);
}