#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

/*
  Disciplina: Computação Concorrente
  Professora: Silvana
  Aluno: Matheus Abrantes Gouvêa da Silva
  Codigo: Programa que simula o problema dos leitores e escritores, e gera um arquivo
  .c com código compilável que verifica se o mesmo foi executado corretamente.
*/

/* definição das variáveis globais, de exclusão mútua e de condição */
int global;                                                             //variável compartilhada para leitura e escrita
int numero_fila=0, numero_atendimento=0, numero_operacao=0, lendo=0; 
int n_leitores, n_escritores, n_leitura, n_escrita, tam_fila;           //variáveis para as entradas do usuário
pthread_mutex_t mutex_numero, mutex_atendimento;
pthread_cond_t cond_leitura, cond_escrita;
FILE *log_file;

/* função que retorna a próxima senha para atendimento futuro */
int pega_numero() {
    int n;
    pthread_mutex_lock(&mutex_numero);
    n = numero_fila;
    numero_fila = (numero_fila+1)%tam_fila;
    pthread_mutex_unlock(&mutex_numero);
    return n;
}

/* função chamada pelas threads leitoras */
void *le (void *args) {
    int id = *(int*) args;
    int meu_numero, leitura_local = n_leitura;
    char nome_arquivo[10];
    sprintf(nome_arquivo, "%d.txt", id);
    FILE *f = fopen(nome_arquivo, "w");

    if (f == NULL) {
        printf("arquivo de saida da thread leitora não abriu.\n");
        exit(1);
    }
    

    while(leitura_local>0) {
        meu_numero = pega_numero();
        pthread_mutex_lock(&mutex_atendimento);
        while(meu_numero!=numero_atendimento) {
            pthread_cond_wait(&cond_leitura, &mutex_atendimento);
        }
        numero_atendimento=(numero_atendimento+1)%tam_fila;
        pthread_cond_broadcast(&cond_leitura);
        lendo++; numero_operacao++;
        fprintf(log_file, "entra_leitura(%d); \t\t //operação n° %d\n", id, numero_operacao);
        pthread_mutex_unlock(&mutex_atendimento);

        /* lendo... */
        fprintf(f, "%d\n", global);
        //usleep(1);

        pthread_mutex_lock(&mutex_atendimento);
        lendo--; numero_operacao++;
        fprintf(log_file, "sai_leitura(); \t\t\t //operação n° %d\n", numero_operacao);
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
        while (meu_numero!=numero_atendimento || lendo>0) {
            pthread_cond_wait(&cond_escrita, &mutex_atendimento);
        }
        
        numero_operacao++;
        fprintf(log_file, "entra_escrita(%d);\t\t //operação n° %d\n", id, numero_operacao);
        /* escrevendo... */
        global = id;
        printf("Thread %d escreveu\n", id);
        //usleep(1);

        numero_atendimento = (numero_atendimento+1)%tam_fila;
        numero_operacao++;
        fprintf(log_file, "sai_escrita(); \t\t\t //operação n° %d\n", numero_operacao);
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
    char c, nome_log[100];
    FILE *modelo = fopen("modelo.c", "r");
    if (modelo==NULL) { printf("modelo não abriu.\n"); exit(1); }


    pthread_cond_init(&cond_escrita, NULL);
    pthread_cond_init(&cond_leitura, NULL);
    pthread_mutex_init(&mutex_atendimento, NULL);
    pthread_mutex_init(&mutex_numero, NULL);

    if(argc < 6) {
        printf("use: <número de leitores> <número de escritores> <quantidade de leituras> ");
        printf("<quantidade de escritas> <nome do arquivo de log>\n");
        exit(1);
    }

    n_leitores = atoi(argv[1]);
    n_escritores = atoi(argv[2]);
    n_leitura = atoi(argv[3]);
    n_escrita = atoi(argv[4]);
    sprintf(nome_log, "%s.c", argv[5]);
    tam_fila = n_leitores+n_escritores;
    log_file = fopen(nome_log, "w");
    if (log_file==NULL) { printf("log_file não abriu.\n"); exit(1); }

    while ((c=fgetc(modelo)) != EOF) {
        fputc(c, log_file);
    }
    fclose(modelo);

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

    fprintf(log_file, "printf(\"O programa executou corretamente\\n\");\n}");
    fclose(log_file);
    printf("Finalizou com sucesso. Arquivo de log gerado: %s\n", nome_log);
    pthread_exit(NULL);
}