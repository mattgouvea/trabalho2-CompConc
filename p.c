#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

//FCLOSE MATHEEEEEEEEEEUUUS!!!

int *fila;
int numero_fila=0, numero_atendimento=0, lendo=0;
int global, n_leitores, n_escritores, n_leitura, n_escrita, tam_fila;
pthread_mutex_t mutex_numero, mutex_atendimento;
pthread_cond_t cond_leitura, cond_escrita;

int pega_numero() {
    int n;
    pthread_mutex_lock(&mutex_numero);
    n = numero_fila;
    numero_fila = (numero_fila+1)%tam_fila;
    pthread_mutex_unlock(&mutex_numero);
    return n;
}

void *le (void *args) {
    int id = *(int*) args;
    int meu_numero, leitura_local;
    char nome_arquivo[10];
    sprintf(nome_arquivo, "%d.txt", id);
    FILE *f = fopen(nome_arquivo, "w");

    pthread_mutex_lock(&mutex_atendimento);
    leitura_local = n_leitura;
    n_leitura--;
    pthread_mutex_unlock(&mutex_atendimento);

    while(leitura_local>0) {
        meu_numero = pega_numero();
        pthread_mutex_lock(&mutex_atendimento);
        while(meu_numero!=numero_atendimento) {
            pthread_cond_wait(&cond_leitura, &mutex_atendimento);
        }
        numero_atendimento=(numero_atendimento+1)%tam_fila;
        pthread_cond_broadcast(&cond_leitura);
        lendo++;
        pthread_mutex_unlock(&mutex_atendimento);

        /* lendo... */
        fprintf(f, "%d\n", global);

        pthread_mutex_lock(&mutex_atendimento);
        lendo--;
        if (lendo==0) {
            pthread_cond_broadcast(&cond_escrita);
        }
        leitura_local = n_leitura;
        n_leitura--;
        pthread_mutex_unlock(&mutex_atendimento);       
    }
    pthread_exit(NULL);
}

void *escreve (void *args) {
    int id = *(int*) args;
    int meu_numero, escrita_local;

    pthread_mutex_lock(&mutex_atendimento);
    escrita_local = n_escrita;
    n_escrita--;
    pthread_mutex_unlock(&mutex_atendimento);

    while(escrita_local>0) {
        meu_numero = pega_numero();
        pthread_mutex_lock(&mutex_atendimento);
        while (meu_numero != numero_atendimento) {
            pthread_cond_wait(&cond_escrita, &mutex_atendimento);
        }

        /* escrevendo... */
        global = id;
        printf("Thread %d escreveu\n", id);

        numero_atendimento = (numero_atendimento+1)%tam_fila;
        escrita_local = n_escrita;
        n_escrita--;
        pthread_cond_broadcast(&cond_escrita);
        pthread_cond_broadcast(&cond_leitura);
        pthread_mutex_unlock(&mutex_atendimento);
    }
    pthread_exit(NULL);
}

void main (int argc, char *argv[]) {
    pthread_t leitores[n_leitores];
    pthread_t escritores[n_escritores];
    int *tid_sistema;

    if(argc < 5) {
        printf("use: <número de leitores> <número de escritores> <quantidade de leituras> ");
        printf("<quantidade de escritas> <nome do arquivo de log>\n ");
        exit(1);
    }

    n_leitores = atoi(argv[1]);
    n_escritores = atoi(argv[2]);
    n_leitura = atoi(argv[3]);
    n_escrita = atoi(argv[4]);
    tam_fila = n_leitores+n_escritores;

    for(int i=0; i<n_escritores; i++) {
        tid_sistema = malloc(sizeof(int)); if(tid_sistema==NULL) { printf("Erro no malloc.\n"); exit(1); }
        *tid_sistema = i;
        if (pthread_create(&escritores[i], NULL, escreve, (void *) tid_sistema)) {
            printf("Erro na criação da thread.\n");
            exit(1);
        }        
    }

    for(int i=0; i<n_leitores; i++) {
        tid_sistema = malloc(sizeof(int)); if(tid_sistema==NULL) { printf("Erro no malloc.\n"); exit(1); }
        *tid_sistema = i;
        if (pthread_create(&leitores[i], NULL, le, (void *) tid_sistema)) {
            printf("Erro na criação da thread.\n");
            exit(1);
        }        
    }

    for(int i=0; i<n_escritores; i++) {
        if(pthread_join(escritores[i], NULL)) {
            printf("Erro no join escritores.\n");
            exit(1);
        }
    }

    /* for(int i=0; i<n_leitores; i++) {
        if(pthread_join(leitores[i], NULL)) {
            printf("Erro no join leitores %d.\n", i);
            exit(1);
        }
    } */

    exit(1);
}