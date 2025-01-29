//
// Created by simon on 10/29/2023.
//

#ifndef NNXOR_MATRIX_H
#define NNXOR_MATRIX_H


struct matrix {
    int row;
    int col;
    double *data;
};

typedef struct matrix matrix;

matrix* zeros(int, int);
matrix* init(int row, int col, double* data);
matrix* initRandom(int row, int col);

matrix* dot(matrix*, matrix*);
matrix* transpose(matrix*);
matrix* sclalar(matrix* m, double x);

matrix* fn(matrix*, double (*f)(double));
matrix* fnm(matrix*, matrix*, double (*f)(double, double ));
matrix* fnmBroadcast(matrix* m1, matrix* m2, double(*f)(double, double));

void freem(matrix *m);

void printMatrix(matrix* mat);

#endif //NNXOR_MATRIX_H
