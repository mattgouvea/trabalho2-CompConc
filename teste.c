#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void arquivo() {
    int i;
    i = 608;
    char name[10];
    sprintf(name, "%d.txt", i);
    FILE *a = fopen(name, "w");
    if(a == NULL) {
        printf("arquivo não abriu.\n");
        exit(1);
    }
    fprintf(a, "hello file!\npulei linha");
    exit(1);
}

void hello() {
    printf("printei heim\n");
    exit(1);
}

void main() {
    FILE *f = fopen("t.c", "w");
    FILE *temp = fopen("template.c", "r");
    char c;

    while ((c=fgetc(temp)) != EOF) 
    {
        fputc(c, f);
        printf("%c", c);
    }

    fclose(f);
    fclose(temp);
    
    exit(1);
}