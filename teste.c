#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void main() {
    int i;
    i = 608;
    char name[10];
    sprintf(name, "%d.txt", i);
    FILE *a = fopen(name, "w");
    fprintf(a, "hello file!\npulei linha");
    exit(1);
}