//
// Created by simon on 10/29/2023.
//

#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include "matrix.h"


matrix* init(int row, int col, double* data) {
    matrix* res = malloc(sizeof(matrix));
    res->row = row;
    res->col = col;
    res->data = data;
    return res;
}

matrix* initRandom(int row, int col) {
    double *data = malloc(sizeof(double) * row * col);
    for (int i = 0; i < row * col; i++) {
        *(data + i) = drand48() - 0.5;
    }
    return init(row, col, data);
}


matrix* zeros(int row, int col) {
    double *data = malloc(sizeof(double) * (row * col));
    for(int i = 0; i < row * col; i++) {
        *(data + i) = 0;
    }
    matrix* res = malloc(sizeof(matrix));
    res->row = row;
    res->col = col;
    res-> data = data;
    return res;
}

matrix* dot(matrix* m1, matrix* m2) {
    if (m1->col != m2->row) {
        err(1, "Wrong dim\n");
    }
    double *data = malloc(sizeof(double) * m1->row * m2->col);
    for (int i = 0; i < m1->row; i++) {
        for(int j = 0; j < m2->col; j++) {
            double curr = 0.0;
            for(int s = 0; s < m1->col; s++) {
                curr += (*(m1->data + m1->col * i + s)) * (*(m2->data + m2->col * s + j));
            }
            *(data + (m2->col) * i + j) = curr;
        }
    }
    matrix* res = malloc(sizeof(matrix));
    res->row = m1->row;
    res->col  = m2->col;

    res->data = data;
    return res;
}

matrix* sclalar(matrix* m, double x) {
    double* data = malloc(sizeof(double) * m->row * m->col);
    for(int i = 0; i < m->row * m->col; i++) {
        *(data + i) = *(m->data + i) * x;
    }
    return init (m->row, m->col, data);
}

matrix* transpose(matrix* m) {
    double *data = malloc(sizeof(double) * m->row * m->col);
    for(int i = 0; i < m->row; i++) {
        for(int j = 0; j < m->col; j++) {
            *(data + (m->row * j + i)) = *(m->data + (m->col * i + j));
        }
    }
    /*matrix* res = malloc(sizeof(matrix));
    res->row = m->col;
    res->col = m->row;
    res->data = data;*/
    matrix* res = init(m->col, m->row, data);
    return res;
}

matrix* fn(matrix* m, double (*f)(double)) {
    double *data = malloc(sizeof(double) * (m->row * m->col));
    matrix* res = malloc(sizeof(matrix));
    res->row = m->row;
    res->col = m->col;
    for(int i = 0; i < m->row * m->col; i++) {
        *(data + i) = (*f)(*(m->data + i));
    }
    res->data = data;
    return res;
}

matrix* fnm(matrix* m1, matrix* m2, double (*f)(double, double)) {
    if (m1->row != m2->row || m1->col != m2->col) {
        errx(1, "m1 and m2 must have the same dim");
    }
    double *data = malloc((sizeof(double ) * m1->col * m2->row));
    for(int i = 0; i < m1->col * m1->row; i++) {
        *(data + i) = (*f)(*(m1->data + i), *(m2->data + i));
    }
    matrix* res = malloc(sizeof(matrix));
    res->row = m1->row;
    res->col = m1->col;
    res->data = data;
    return res;
}

matrix* fnmBroadcast(matrix* m1, matrix* m2, double(*f)(double, double)) {
    if((m1->row != m2->row) || m2->col != 1) {
        if (m1->col != m2->col || m2->row != 1) {
            err(1, "Wrong dim");
        }
    }
    if(m1->row == m2->row) {
        double *data = malloc(sizeof(double) * m1->row * m1->col);
        for (int i = 0; i < m1->row; i++) {
            for (int j = 0; j < m1->col; j++) {
                *(data + (m1->col * i + j)) = (*f)(*(m1->data + (m1->col * i + j)), *(m2->data + i));
            }
        }
        return init(m1->row, m1->col, data);
    } else if(m1->col == m2->col) {
        double *data = malloc(sizeof(double) * m1->row * m1->col);

        for(int i = 0; i < m1->row; i++) {
            for(int j = 0; j < m1->col; j++) {
                *(data + (m1->col * i + j)) = (*f)(*(m1->data + (m1->col * i + j)), *(m2->data + j));
            }
        }
        return init(m1->row, m1->col, data);
    }
}

void freem(matrix* m) {
    free(m->data);
    free(m);
}

void printMatrix(matrix* mat) {
    for (int i = 0; i < mat->row; i++) {
        for (int j = 0; j < mat->col; j++) {
            printf("%lf ", mat->data[i * mat->col + j]);
        }
        printf("\n");
    }
    printf("\n");

}