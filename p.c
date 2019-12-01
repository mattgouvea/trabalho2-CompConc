#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

//FCLOSE MATHEEEEEEEEEEUUUS!!!

int *fila;
int numero_fila=0, numero_atendimento=0, lendo=0;
int global, n_leitores, n_escritores, n_leitura, n_escrita, tam_fila;
pthread_mutex_t mutex_numero, mutex_atendimento;
pthread_cond_t cond_leitura, cond_escrita;
FILE *log_file;

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
    int meu_numero, leitura_local = n_leitura;
    char nome_arquivo[10];
    sprintf(nome_arquivo, "%d.txt", id);
    FILE *f = fopen(nome_arquivo, "w");

    while(leitura_local>0) {
        meu_numero = pega_numero();
        pthread_mutex_lock(&mutex_atendimento);
        while(meu_numero!=numero_atendimento) {
            pthread_cond_wait(&cond_leitura, &mutex_atendimento);
        }
        numero_atendimento=(numero_atendimento+1)%tam_fila;
        pthread_cond_broadcast(&cond_leitura);
        lendo++;
        fprintf(log_file, "entra_leitura(%d)\n", id);
        //printf("Thread %d lendo.\n", id); //print do log dentro do mutex
        pthread_mutex_unlock(&mutex_atendimento);

        /* lendo... */
        fprintf(f, "%d\n", global);
        //usleep(1000);

        pthread_mutex_lock(&mutex_atendimento);
        fprintf(log_file, "sai_leitura(%d)\n", id);
        lendo--;
        if (lendo==0) {
            pthread_cond_broadcast(&cond_escrita);
        }
        pthread_mutex_unlock(&mutex_atendimento);       
        leitura_local--;
    }
    fclose(f);
    pthread_exit(NULL);
}

void *escreve (void *args) {
    int id = *(int*) args;
    int meu_numero, escrita_local = n_escrita;

    while(escrita_local>0) {
        meu_numero = pega_numero();
        pthread_mutex_lock(&mutex_atendimento);
        while (meu_numero != numero_atendimento) {
            pthread_cond_wait(&cond_escrita, &mutex_atendimento);
        }

        /* escrevendo... */
        fprintf(log_file, "entra_escrita(%d)\n", id);
        global = id;
        printf("Thread %d escreveu\n", id);
        //usleep(1000);

        numero_atendimento = (numero_atendimento+1)%tam_fila;
        fprintf(log_file, "sai_escrita(%d)\n", id);
        pthread_cond_broadcast(&cond_escrita);
        pthread_cond_broadcast(&cond_leitura);
        pthread_mutex_unlock(&mutex_atendimento);
        escrita_local--;
    }
    pthread_exit(NULL);
}

void main (int argc, char *argv[]) {
    pthread_t *leitores;
    pthread_t *escritores;
    int *tid_sistema;
    int i;
    char nome_log[100];


    pthread_cond_init(&cond_escrita, NULL);
    pthread_cond_init(&cond_leitura, NULL);
    pthread_mutex_init(&mutex_atendimento, NULL);
    pthread_mutex_init(&mutex_numero, NULL);

    if(argc < 6) {
        printf("use: <número de leitores> <número de escritores> <quantidade de leituras> ");
        printf("<quantidade de escritas> <nome do arquivo de log>\n ");
        exit(1);
    }

    n_leitores = atoi(argv[1]);
    n_escritores = atoi(argv[2]);
    n_leitura = atoi(argv[3]);
    n_escrita = atoi(argv[4]);
    sprintf(nome_log, "%s.txt", argv[5]);
    tam_fila = n_leitores+n_escritores;
    log_file = fopen(nome_log, "w");

    escritores = malloc(sizeof(pthread_t)*n_escritores);
    if (escritores == NULL) { printf("falha no malloc.\n"); exit(1); }
    leitores = malloc(sizeof(pthread_t)*n_leitores);
    if (leitores == NULL) { printf("falha no malloc.\n"); exit(1); }

    for(i=0; i<n_escritores; i++) {
        tid_sistema = malloc(sizeof(int)); if(tid_sistema==NULL) { printf("Erro no malloc.\n"); exit(1); }
        *tid_sistema = i;
        if (pthread_create(&escritores[i], NULL, escreve, (void *) tid_sistema)) {
            printf("Erro na criação da thread.\n");
            exit(1);
        }        
    }

    for(i=0; i<n_leitores; i++) {
        tid_sistema = malloc(sizeof(int)); if(tid_sistema==NULL) { printf("Erro no malloc.\n"); exit(1); }
        *tid_sistema = i;
        if (pthread_create(&leitores[i], NULL, le, (void *) tid_sistema)) {
            printf("Erro na criação da thread.\n");
            exit(1);
        }        
    }

    /* Fazendo o join */

    for(i=0; i<n_escritores; i++) {
        if(pthread_join(escritores[i], NULL)) {
            printf("Erro no join escritores %d.\n", i);
            exit(1);
        }
    }

    for(i=0; i<n_leitores; i++) {
        if(pthread_join(leitores[i], NULL)) {
            printf("Erro no join leitores %d.\n", i);
            exit(1);
        }
    }

    printf("terminou\n");
    pthread_exit(NULL);
}