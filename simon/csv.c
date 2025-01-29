//
// Created by simon on 10/30/2023.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "matrix.h"

char* getField(char* line, int i) {
    char* tok;
    char* copy = strdup(line);

    tok = strtok(copy, ",");
    int curr = 0;
    while(curr != i && tok != NULL) {
        tok = strtok(NULL, ",");
        curr++;
    }
    return tok;
}

int getNumField(char* line) {
    int res = 0;
    char* tok;
    for(tok = strtok(line, ",\n"); tok != NULL; tok = strtok(NULL, ",\n")) {
        res++;
    }
    return res;
}

matrix** csvToMatrix(char* path) {
    FILE* file = fopen(path, "r");


    char line[32164];

    fgets(line, 8192, file);
    int nfield = getNumField(line);

    int nline  = 0;
    while(fgets(line, 4096, file)) {
        nline++;
    }

    file = fopen(path, "r");
    double *dataX = malloc(sizeof(double) * nline * (nfield - 1));
    double *dataY = malloc(sizeof(double ) * nline);
    fgets(line, 8192, file);
    int curr = 0;
    while(fgets(line, 4096, file)) {
        *(dataY + curr) = strtod(getField(line, 0), NULL);
        for(int i = 0; i < 28*28; i++) {
            *(dataX + (28*28*curr + i)) = strtod(strtok(NULL, ",\n"), NULL);
        }
        curr++;
    }
    matrix** res = malloc(sizeof(matrix) * 2);
    *(res + 0) = init(nline, nfield-1, dataX);
    *(res + 1) = init(nline, 1, dataY);
    return res;
}
