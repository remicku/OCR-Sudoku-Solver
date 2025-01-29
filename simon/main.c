#include <stdio.h>
#include "csv.h"
#include "matrix.h"
#include "trainXor.h"
#include "train.h"
#include "img.h"


int main() {


    //char* path = "mnist_train.csv";
    //matrix** m = csvToMatrix(path);
    matrix** dataset = csvToMatrix("mnist_train.csv");
    matrix* imgx = *(dataset + 0);
    matrix* imgy = *(dataset + 1);
    imgx->row = 50000;
    imgy->row = 50000;

    double* dimgtestx = imgx->data + (imgx->col*51000);
    double* dimgtesty = imgy->data + (imgy->col*51000);
    matrix* imgtestx = init(1000, imgx->col, dimgtestx);
    matrix* imgtesty = init(1000, imgy->col, dimgtesty);
    imgtestx = transpose(imgtestx);
    imgtesty = transpose(imgtesty);
    imgtestx = sclalar(imgtestx, (double) 1 / 255);
    matrix* one_hot_test = one_hot_y(imgtesty);

    imgx = transpose(imgx);
    imgy = transpose(imgy);
    imgx = sclalar(imgx, (double) 1 / 255);
    matrix* one_hot = one_hot_y(imgy);

    matrix** params = descent(imgx, one_hot, 0.1, 520, imgtestx, imgtesty, imgy);

    char* testpath = "handwritten.model";

    save_params(params, testpath);
    matrix** test_params = load_params(testpath);

    printf("Loaded\n");
    test_training(test_params, imgx, imgy);
    test_training(test_params, imgtestx, imgtesty);

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        fprintf(stderr, "IMG_Init Error: %s\n", IMG_GetError());
        SDL_Quit();
        return 0;
    }

    char* path = "imgst/Capture.PNG";
    SDL_Surface** surfaces = paths_to_surface(&path, 1);
    surfaces = scale_surfaces(surfaces, 1);
    to_blacks_and_whites(surfaces, 1);
    matrix* testx = imgss_to_matrix(surfaces, 1);
    testx = transpose(testx);
    IMG_SavePNG(*surfaces, "imgst/Captureyoyo.PNG");

    foward(testx, params);


    return 0;
}
