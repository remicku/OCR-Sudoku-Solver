//
// Created by simon on 10/31/2023.
//

#include "trainXor.h"
#include "matrix.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

double sous(double x, double y) {
    return x - y;
}

double mul(double x, double y) {
    return x * y;
}

double addOne(double x) {
    return x + 1;
}

double oppo(double x) {
    return -x;
}

double inverse(double x) {
    return 1 / x;
}

matrix* sigmoid(matrix* m) {
    matrix* oppom = fn(m, &oppo);
    matrix* logm = fn(oppom, &exp);
    matrix* deno = fn(logm, &addOne);
    matrix* res = fn(deno, &inverse);
    return res;

}

matrix** initXor() {
    matrix* w1 = initRandom(2, 2);
    matrix* w2 = initRandom(1, 2);
    matrix** res = malloc(sizeof(double) * 2);
    *(res + 0) = w1;
    *(res + 1) = w2;


    return res;
}

matrix** forwardXor(matrix* x, matrix** params) {

    matrix* w1 = *(params + 0);
    matrix* w2 = *(params + 1);
    matrix* z1 = dot(w1, x);
    matrix* a1 = sigmoid(z1);
    matrix* z2 = dot(w2, a1);
    matrix* a2 = sigmoid(z2);
    matrix** res = malloc(sizeof(double) * 4);
    *(res + 0) = z1;
    *(res + 1) = a1;
    *(res + 2) = z2;
    *(res + 3) = a2;
    return res;
}

matrix** backwardXor(matrix* y, matrix* x, matrix** params, matrix** layer) {
    //matrix* w1 = *(params + 0);
    matrix* w2 = *(params + 1);
    //matrix* z1 = *(layer + 0);
    matrix* a1 = *(layer + 1);
    //matrix* z2 = *(layer + 2);
    matrix* a2 = *(layer + 3);

    matrix* dz2 = fnm(a2, y, &sous);
    matrix* dw2 = sclalar(dot(dz2, transpose(a1)),(float) 1/4); //check

    matrix* dz1i1 = dot(transpose(w2), dz2);

    matrix* dz1i2 = fn(fn(a1, &oppo), &addOne);

    matrix* dz1i3 = fnm(a1, dz1i2, &mul);

    matrix* dz1 = fnm(dz1i1, dz1i3, &mul);

    matrix* dw1 = sclalar(dot(dz1, transpose(x)),(float) 1/4);

    matrix** res = malloc(sizeof(double) * 4);
    *(res + 0) = dz2;
    *(res + 1) = dw2;
    *(res + 2) = dz1;
    *(res + 3) = dw1;

    return res;
}

matrix** updateXor(matrix** params, matrix** descent, double alpha) {
    matrix* w1 = *(params + 0);
    matrix* w2 = *(params + 1);

    //matrix* dz2 = *(descent + 0);
    matrix* dw2 = *(descent + 1);
    //matrix* dz1 = *(descent + 2);
    matrix* dw1 = *(descent + 3);

    matrix* lrdw2 = sclalar(dw2, alpha);
    matrix* lrdw1 = sclalar(dw1, alpha);
    matrix* nw2 = fnm(w2, lrdw2, &sous);
    matrix* nw1 = fnm(w1, lrdw1, &sous);

    matrix** nparams = malloc(sizeof(matrix*) * 2);
    *(nparams + 0) = nw1;
    *(nparams + 1) = nw2;
    return nparams;
}

matrix** trainXor(double alpha, double iteration) {
    double datax[] = {1, 0, 0, 0, 1, 1, 1, 0};
    matrix* x = init(2, 4, datax);
    double datay[] = {1, 0, 0, 1};
    matrix* y = init(1, 4, datay);

    matrix** params = initXor();
    for(int i = 0; i < iteration; i++) {
        matrix** layer = forwardXor(x, params);
        matrix** gradient = backwardXor(y, x, params, layer);
        matrix** nparams = updateXor(params, gradient, alpha);
        *(params + 0) = *(nparams + 0);
        *(params + 1) = *(nparams + 1);
    }
    return params;
}