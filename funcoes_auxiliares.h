#ifndef funcoes
#define funcoes

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdlib.h>

int lendo=0, escrevendo=0, operacao=0;


void entra_leitura(int id) {
    operacao++;
    if (escrevendo>0) {
        printf("Erro na operação número %d.\n", operacao);
        printf("O leitor %d tentou ler enquanto tinha um escritor escrevendo.\n", id);
        exit(1);
    }
    lendo++;
}

void sai_leitura() {
    lendo--; operacao++;
}

void entra_escrita(int id) {
    operacao++;
    if (escrevendo>0) {
        printf("Erro na operação número %d.\n", operacao);
        printf("O escritor %d tentou escrever enquanto tinha outro escritor escrevendo.\n", id);
        exit(1);
    }
    else if (lendo>0) {
        printf("Erro na operação número %d.\n", operacao);
        printf("O escritor %d tentou escrever enquanto tinha um leitor lendo.\n", id);
        exit(1);
    }
    escrevendo++;
}

void sai_escrita() {
    escrevendo--; operacao++;
}

#endif