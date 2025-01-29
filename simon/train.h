//
// Created by simon on 10/30/2023.
//

#include "matrix.h"

#ifndef NNXOR_TRAIN_H
#define NNXOR_TRAIN_H


matrix* Relu(matrix* m);
matrix* dRelu(matrix* m);
matrix* expM(matrix* m);
matrix* sum(matrix* m);

matrix* one_hot_y(matrix* m);

double get_accuracy(matrix* prediction, matrix* y);
matrix* get_prediction(matrix* a2);

matrix** foward(matrix* x, matrix** params);
matrix** descent(matrix* x, matrix* y, double alpha, int iteration, matrix* test_trainx, matrix* test_trainy, matrix* y_no_hot);

void save_params(matrix** params, char* path);

matrix** load_params(char* path);

void test_training(matrix** params, matrix* x, matrix* y);
int predict(char* path, matrix** params);

#endif //NNXOR_TRAIN_H
