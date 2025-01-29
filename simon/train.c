//
// Created by simon on 10/30/2023.
//
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <float.h>
#include "matrix.h"
#include "train.h"
#include "img.h"

double addScalar(double x, double y) {
    return x + y;
}

double substractScalar(double x, double y) {
    return x - y;
}

double divScalar(double x, double y) {
    return x / y;
}

double mulScalar(double x, double y) {
    return x * y;
}



double ReluScalar(double d) {
    if(d > 0) {
        return d;
    }
    return 0;
}

double dReluScalar(double d) {
    return d > 0;
}

matrix* Relu(matrix* m) {
    return fn(m, &ReluScalar);
}

matrix* dRelu(matrix* m) {
    return fn(m, &dReluScalar);
}


matrix* expM(matrix* m) {
    matrix* res = fn(m, exp);
    return res;
}

matrix* sum(matrix* m) {
    double* data = malloc(sizeof(double) * m->col);
    for(int i = 0; i < m->col; i++) {
        double curr = 0;
        for(int j = 0; j < m->row; j++) {
            curr += *(m->data + (m->col * j + i));
        }
        *(data + i) = curr;
    }
    return init(1, m->col, data);
}

double sumNoAxis(matrix* m) {
    double res = 0;
    for(int i = 0; i < m->row * m->col; i++) {
        res += *(m->data + i);
    }
    return res;
}

double addone (double x) {
    return x + 1;
}

matrix* softmax(matrix* m) {

    double* data = malloc(sizeof(double) * m->row * m->col);
    for(int i = 0; i < m->col*m->row; i++) {
        *(data + i) = *(m->data + i);
    }
    matrix* mm = init(m->row, m->col, data);
    matrix* mmm = mm; //sclalar(mm, (double) 1 / 1);
    matrix* expm = expM(mmm);
    matrix* summ = sum(expm);
    return fnmBroadcast(expm, summ, &divScalar);
}

matrix* one_hot_y(matrix* m) {
    if(m->row != 1) {
        errx(1, "row must be equal to 1");
    }
    matrix* res = zeros(10, m->col);
    for(int i = 0; i < m->col; i++) {
        int value = (int) *(m->data + i);
        *(res->data + (res->col * value + i)) = 1;
    }
    return res;
}

matrix* get_prediction(matrix* a2) {
    double* data = malloc(sizeof(double) * a2->col);
    for(int i = 0; i < a2->col; i++) {
        int max = 0;
        double maxval = 0;
        for(int j = 0; j < a2->row; j++) {
            if (*(a2->data + (a2->col * j + i)) > maxval) {
                maxval = *(a2->data + (a2->col * j + i));
                max = j;
            }
        }
        *(data + i) = max;
    }
    return init(1, a2->col, data);
}

double get_accuracy(matrix* prediction, matrix* y) {
    int sum = y->col;
    int goodres = 0;
    for(int i = 0; i < prediction->col; i++) {
        if(*(prediction->data+i) == *(y->data + i)) {
            goodres++;
        }
    }
    return (double) goodres / sum;
}

matrix** initParams() {
    matrix* w1 = initRandom(10, 784);
    matrix* b1 = initRandom(10, 1);
    matrix* w2 = initRandom(10, 10);
    matrix* b2 = initRandom(10, 1);
    matrix** res = malloc(sizeof(matrix*) * 4);
    *(res + 0) = w1;
    *(res + 1) = b1;
    *(res + 2) = w2;
    *(res + 3) = b2;
    return res;
}



matrix** foward(matrix* x, matrix** params) {
    matrix* w1 = *(params + 0);
    matrix* b1 = *(params + 1);
    matrix* w2 = *(params + 2);
    matrix* b2 = *(params + 3);

    matrix* intedot1 = dot(w1, x);
    matrix* z1 = fnmBroadcast(intedot1, b1, &addScalar);
    freem(intedot1);
    matrix* a1 = Relu(z1);

    matrix* inteddot2 = dot(w2, a1);
    matrix* z2 = fnmBroadcast(inteddot2, b2, &addScalar);
    freem(inteddot2);
    matrix* a2 = softmax(z2);
    matrix** res = malloc(sizeof(matrix*) * 4);
    *(res + 0) = z1;
    *(res + 1) = a1;
    *(res + 2) = z2;
    *(res + 3) = a2;
    return res;

}

matrix** backward(matrix* x, matrix* y, matrix** params, matrix** layer) {
    matrix* w1 = *(params + 0);
    matrix* b1 = *(params + 1);
    matrix* w2 = *(params + 2);
    matrix* b2 = *(params + 3);
    matrix* z1 = *(layer + 0);
    matrix* a1 = *(layer + 1);
    matrix* z2 = *(layer + 2);
    matrix* a2 = *(layer + 3);

    matrix* dz2 = fnm(a2, y, &substractScalar);
    matrix* a1T= transpose(a1);
    matrix* dw2 = sclalar(dot(dz2, a1T), (double)  1/ x->col);
    freem(a1T);

    double db2n = sumNoAxis(dz2) / x->col;
    matrix* db2 = zeros(b2->row, b2->col);
    for(int i = 0; i < b2->row * b2->col; i++) {
        *(db2->data + i) = db2n;
    }

    matrix* w2T = transpose(w2);
    matrix* intedot1 = dot(w2T, dz2);
    matrix* intedot2 = dRelu(z1);
    matrix* dz1 = fnm(intedot1, intedot2, &mulScalar);
    freem(w2T);
    freem(intedot1);
    freem(intedot2);

    matrix* xT = transpose(x);
    matrix* intedot3 = dot(dz1, xT);
    matrix* dw1 = sclalar(intedot3, (double) 1 / x->col);
    freem(xT);
    freem(intedot3);

    double db1n = sumNoAxis(dz1) / x->col;
    matrix* db1 = zeros(b1->row, b1->col);
    for(int i = 0; i < b1->row * b1->col; i++) {
        *(db1->data + i) = db1n;
    }
    matrix** res = malloc(sizeof(matrix*) * 4);
    *(res + 3) = db2;
    *(res + 2) = dw2;
    *(res + 1) = db1;
    *(res + 0) = dw1;

    return res;
}

matrix** update(matrix** params, matrix** gradient, double alpha) {
    matrix* intedot1 = sclalar(*(gradient + 0), alpha);
    matrix* nw1 = fnm(*(params + 0), intedot1, &substractScalar);
    freem(intedot1);

    matrix* intedot2 = sclalar(*(gradient + 1), alpha);
    matrix* nb1 = fnm(*(params + 1), intedot2, &substractScalar);
    freem(intedot2);

    matrix* intedot3 = sclalar(*(gradient + 2), alpha);
    matrix* nw2 = fnm(*(params + 2), intedot3, &substractScalar);
    freem(intedot3);

    matrix* intedot4 = sclalar(*(gradient + 3), alpha);
    matrix* nb2 = fnm(*(params + 3), intedot4, &substractScalar);
    freem(intedot4);

    matrix** res = malloc(sizeof(matrix*) * 4);
    *(res + 0) = nw1;
    *(res + 1) = nb1;
    *(res + 2) = nw2;
    *(res + 3) = nb2;
    return res;
}

void test_training(matrix** params, matrix* x, matrix* y) {
    matrix** layer = foward(x, params);
    matrix* a2 = *(layer + 3);
    matrix* pred = get_prediction(a2);
    double  accuracy = get_accuracy(pred, y);
    freem(*(layer + 0));
    freem(*(layer + 1));
    freem(*(layer + 2));
    freem(*(layer + 3));
    free(layer);
    freem(pred);
    printf("Accuracy: %.2f%%\n", accuracy * 100);

}

matrix** descent(matrix* x, matrix* y, double alpha, int iteration, matrix* test_trainx, matrix* test_trainy, matrix* y_no_hot) {
    matrix** params = initParams();
    for(int i = 0; i < iteration; i++) {
        matrix** layer = foward(x, params);
        if(i % 10 == 0) {
            matrix* a2 = *(layer + 3);
            /*matrix* pred = get_prediction(a2);
            double accuracy = get_accuracy(pred, y);
            printf("Step: %d Accuracy: %.2f%%\n", i, accuracy * 100);*/
            printf("Step %d\n", i);
            test_training(params, x, y_no_hot);
            test_training(params, test_trainx, test_trainy);

        }
        if(i == 1200) {
            test_training(params, x, y_no_hot);
        }
        matrix** gradient = backward(x, y, params, layer);
        matrix** nparams = update(params, gradient, alpha);

        freem(*(params + 0));
        freem(*(params + 1));
        freem(*(params + 2));
        freem(*(params + 3));
        freem(*(gradient + 0));
        freem(*(gradient + 1));
        freem(*(gradient + 2));
        freem(*(gradient + 3));
        freem(*(layer + 0));
        freem(*(layer + 1));
        freem(*(layer + 2));
        freem(*(layer + 3));
        free(params);
        free(gradient);
        free(layer);

        params = nparams;

    }
    return params;
}

void save_params(matrix** params, char* path) {
    matrix* w1 = *(params + 0);
    matrix* b1 = *(params + 1);
    matrix* w2 = *(params + 2);
    matrix* b2 = *(params + 3);

    FILE* file = fopen(path, "w");

    fwrite(&w1->row, sizeof(int), 1, file);
    fwrite(&w1->col, sizeof(int), 1, file);
    fwrite(w1->data, sizeof(double), w1->col * w1->row, file);

    fwrite(&b1->row, sizeof(int), 1, file);
    fwrite(&b1->col, sizeof(int), 1, file);
    fwrite(b1->data, sizeof(double), b1->col * b1->row, file);

    fwrite(&w2->row, sizeof(int), 1, file);
    fwrite(&w2->col, sizeof(int), 1, file);
    fwrite(w2->data, sizeof(double), w2->col * w2->row, file);

    fwrite(&b2->row, sizeof(int), 1, file);
    fwrite(&b2->col, sizeof(int), 1, file);
    fwrite(b2->data, sizeof(double), b2->col * b2->row, file);

    fclose(file);

}

matrix** load_params(char* path) {

    FILE* file = fopen(path, "r");

    matrix* w1 = malloc(sizeof(matrix));
    matrix* b1 = malloc(sizeof(matrix));
    matrix* w2 = malloc(sizeof(matrix));
    matrix* b2 = malloc(sizeof(matrix));

    fread(&w1->row, sizeof(int), 1, file);
    fread(&w1->col, sizeof(int), 1, file);
    double* w1data = malloc(sizeof(double) * w1->row * w1->col);
    fread(w1data, sizeof(double), w1->row * w1->col, file);
    w1->data = w1data;

    fread(&b1->row, sizeof(int), 1, file);
    fread(&b1->col, sizeof(int), 1, file);
    double* b1data = malloc(sizeof(double) * b1->row * b1->col);
    fread(b1data, sizeof(double), b1->row * b1->col, file);
    b1->data = b1data;

    fread(&w2->row, sizeof(int), 1, file);
    fread(&w2->col, sizeof(int), 1, file);
    double* w2data = malloc(sizeof(double) * w2->row * w2->col);
    fread(w2data, sizeof(double), w2->row * w2->col, file);
    w2->data = w2data;

    fread(&b2->row, sizeof(int), 1, file);
    fread(&b2->col, sizeof(int), 1, file);
    double* b2data = malloc(sizeof(double) * b2->row * b2->col);
    fread(b2data, sizeof(double), b2->row * b2->col, file);
    b2->data = b2data;

    fclose(file);

    matrix** res = malloc(sizeof(matrix*) * 4);
    *(res + 0) = w1;
    *(res + 1) = b1;
    *(res + 2) = w2;
    *(res + 3) = b2;
    return res;

}

int predict(char* path, matrix** params) {
    SDL_Surface** surface = paths_to_surface(&path, 1);
    SDL_Surface** scaled = scale_surfaces(surface, 1);
    to_blacks_and_whites(scaled, 1);
    matrix** x = imgs_to_matrix(scaled, 1);
    matrix** layer = foward(*x, params);
    matrix* a2 = *(layer+3);
    int max = -1;
    double maxval = -1;
    for(int i = 0; i < 10; i++) {
        if(*(a2->data + i) > maxval) {
            max = i;
            maxval = *(a2->data + i);

        }
    }
    freem(*(layer + 0));
    freem(*(layer + 1));
    freem(*(layer + 2));
    freem(*(layer + 3));
    freem(*x);
    return max;

}