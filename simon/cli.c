//
// Created by simon on 12/7/2023.
//
#include <stdio.h>
#include "matrix.h"
#include "csv.h"
#include "train.h"
#include "img.h"

int mainCli() {
	if (SDL_Init(SDL_INIT_VIDEO) != 0)            
		errx(EXIT_FAILURE, "%s", SDL_GetError());
	char* testpath = "handwritten.model";


    printf("1) Entrainer le réseau\n");
    printf("2) Tester le réseau\n");
    printf("3) Quitter\n");

    int res;
    scanf("%d", &res);
    if(res == 1) {
        matrix** dataset = csvToMatrix("smalltrain2.csv");
        matrix* imgx = *(dataset + 0);
        matrix* imgy = *(dataset + 1);
        imgx->row = 900;
        imgy->row = 900;

        double* dimgtestx = imgx->data + (imgx->col*900);
        double* dimgtesty = imgy->data + (imgy->col*900);
        matrix* imgtestx = init(100, imgx->col, dimgtestx);
        matrix* imgtesty = init(100, imgy->col, dimgtesty);
        imgtestx = transpose(imgtestx);
        imgtesty = transpose(imgtesty);
        imgtestx = sclalar(imgtestx, (double) 1 / 255);
        matrix* one_hot_test = one_hot_y(imgtesty);

        imgx = transpose(imgx);
        imgy = transpose(imgy);
        imgx = sclalar(imgx, (double) 1 / 255);
        matrix* one_hot = one_hot_y(imgy);

        matrix** params = descent(imgx, one_hot, 0.1, 300, imgtestx, imgtesty, imgy);



        save_params(params, testpath);
        matrix** test_params = load_params(testpath);

        printf("Test du modèle enregistré\n");
        test_training(test_params, imgx, imgy);
        test_training(test_params, imgtestx, imgtesty);
        return mainCli();
    } else if(res == 2) {
        char path[256];
        printf("renter un chemin vers une image à prédire:\n");
        scanf("%s", path);
        matrix** params = load_params(testpath);
	SDL_Surface* s = IMG_Load(path);
	SDL_Surface* ss = SDL_ConvertSurfaceFormat(s, SDL_PIXELFORMAT_RGB888 , 0);
        Uint32* pixels = ss->pixels;
 	int len = ss->w * ss->h;
 	SDL_PixelFormat* format = ss->format;
 	SDL_LockSurface(ss);
	double* data = malloc(sizeof(double) * 784);
 	for(int i = 0; i < len; i++) {
		Uint8 r, g, b;
		SDL_GetRGB(pixels[i], format, &r, &g, &b);
		*(data + i) = (double) r;
	}
	matrix* x = init(28*28, 1, data);
	x = sclalar(x, (double) 1 / 255);
        matrix** layer = foward(x, params);
	
        matrix* a2 = *(layer+3);
        for (int i = 0; i < 10; i++) {
            printf("%d:  %.2f%%\n", i, *(a2->data + i) * 100);
        }
        return mainCli();

    }
    else if(res == 3) {
        return 0;
    } else {
        return mainCli();
    }
}

int main() {
    printf("    __    ______   ____  ___________ _________   __  __   ____  ______   _   __________  ______  ____  _   _____________\n"
           "   / /   / ____/  / __ \\/ ____/ ___// ____/   | / / / /  / __ \\/ ____/  / | / / ____/ / / / __ \\/ __ \\/ | / / ____/ ___/\n"
           "  / /   / __/    / /_/ / __/  \\__ \\/ __/ / /| |/ / / /  / / / / __/    /  |/ / __/ / / / / /_/ / / / /  |/ / __/  \\__ \\ \n"
           " / /___/ /___   / _, _/ /___ ___/ / /___/ ___ / /_/ /  / /_/ / /___   / /|  / /___/ /_/ / _, _/ /_/ / /|  / /___ ___/ / \n"
           "/_____/_____/  /_/ |_/_____//____/_____/_/  |_\\____/  /_____/_____/  /_/ |_/_____/\\____/_/ |_|\\____/_/ |_/_____//____/\n\n");
    return mainCli();
}
