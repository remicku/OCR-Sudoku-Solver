//
// Created by simon on 10/31/2023.
//

#include "matrix.h"

#ifndef NNXOR_TRAINXOR_H
#define NNXOR_TRAINXOR_H
matrix** trainXor(double alpha, double iteration);
matrix** forwardXor(matrix* x, matrix** params);
#endif //NNXOR_TRAINXOR_H
