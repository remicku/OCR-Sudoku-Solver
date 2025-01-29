//
// Created by simon on 11/25/2023.
//

#include <stddef.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "matrix.h"


SDL_Surface* path_to_surface(char* path) {
    SDL_Surface* res = IMG_Load(path);
    if(!res) {
        fprintf(stderr, "IMG_Load Error: %s\n", IMG_GetError());
        IMG_Quit();
        SDL_Quit();
        return NULL;
    }
    return  res;
}

SDL_Surface* paths_to_surface(char* paths, size_t len) {
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        fprintf(stderr, "IMG_Init Error: %s\n", IMG_GetError());
        SDL_Quit();
        return NULL;
    }
    SDL_Surface* res;
    res = path_to_surface(paths);


    return res;
}

SDL_Surface* scale_surfaces(SDL_Surface* surfaces, size_t len) {
    SDL_Surface* curr = surfaces;
    SDL_Surface* res  = SDL_CreateRGBSurfaceWithFormat(0, 28, 28, curr->format->BitsPerPixel, curr->format->format);
    SDL_Rect rect = {0, 0, 28, 28};
    SDL_BlitScaled(curr, NULL, res, &rect);


    return res;
}

void to_blacks_and_whites(SDL_Surface* surfaces, size_t len) {
        //Uint32* pixels = (Uint32*) ((*(surfaces + i))->pixels);
        int pixelCount = surfaces->w * surfaces->h;
        for(int j = 0; j < pixelCount; j++) {
            Uint8 r, g, b, a;
            Uint32 pixel = ((Uint32*) surfaces->pixels)[j];
            SDL_GetRGBA(pixel, surfaces->format, &r, &g, &b, &a);
            printf("%d %d %d\n", pixel, g, b);
            Uint8 gray = (Uint8)(0.3 * r + 0.59 * g + 0.11 * b);
            Uint32 grayPixel = SDL_MapRGBA(surfaces->format, gray, gray, gray, a);
            ((Uint32*) surfaces->pixels)[j] = grayPixel;

        }


}

matrix* imgs_to_matrix(SDL_Surface* surfaces, size_t len) {
    Uint32* pixels = (Uint32*) (surfaces)->pixels;
    int pixelCount = (surfaces)->w * surfaces->h;
    double* data = malloc(sizeof(double) * pixelCount);
    for(int j = 0; j < pixelCount; j++) {
        Uint32 pixel = ((Uint32*) surfaces->pixels)[j];
        Uint8 r, g, b, a;
        SDL_GetRGBA(pixel, surfaces->format, &r, &g, &b, &a);
        *(data + j) = (double) r;
	if((j % 28) == 0) {
		printf("\n", j);
	}
	printf("%.0f%.0f%.0f", r, g, b);
    }
    matrix* res = init(28*28, 1, data);

    return res;
}


matrix* imgss_to_matrix(SDL_Surface** surfaces, size_t len) {
    double* data = malloc(sizeof(double) * 28 * 28 * len);
    for(size_t i = 0; i < len; i++) {
        for(size_t j = 0; j < 28 * 28; j++) {
            Uint32 pixel = ((Uint32*) ((*(surfaces + i))->pixels))[j];
            Uint8 r, g, b, a;
            SDL_GetRGBA(pixel, (*(surfaces + i))->format, &r, &g, &b, &a);
            *(data + ((i * 28 * 28) * i + j)) = (double) r;
        }
    }
    return init((int) len, 28*28, data);
}

double* append_surface(SDL_Surface** surfaces, size_t len, double* data) {
    for(size_t i = 0; i < len; i++) {
        for(size_t j = 0; j < 28*28; j++) {
            Uint32 pixel = ((Uint32*) ((*(surfaces + i))->pixels))[j];
            Uint8 r, g, b, a;
            SDL_GetRGBA(pixel, (*(surfaces + i))->format, &r, &g, &b, &a);
            *(data + ((i * 28 * 28) * i + j)) = (double) r;
        }
    }
    return data + (len * 28 * 28);
}

int read_file(char* path) {
    FILE* file = fopen(path, "r");
    char line[8];
    fgets(line, 8, file);
    return (int) *line;
}

matrix** tagged_imgs_to_matrix(char** paths, size_t len) {
    matrix** res = malloc(sizeof(matrix*) * 2);
    double* datax = malloc(sizeof(double) * 28 * 28 * len);
    double* datay = malloc(sizeof(double) * len);
    double* currdatax = datax;
    double* currdatay = datay;
    for(size_t i = 0; i < len; i++) {
        char* img_path = realloc(paths[i], (strlen(paths[i]) + 5) * sizeof(char));
        strcat(img_path, ".png");
        char* txt_path = realloc(paths[i], (strlen(paths[i]) + 5) * sizeof(char));
        strcat(txt_path, ".txt");
        SDL_Surface* surface = path_to_surface(img_path);
        SDL_Surface** scaled = scale_surfaces(&surface, 1);
        to_blacks_and_whites(scaled, 1);
        currdatax = append_surface(scaled, 1, currdatax);
        *currdatay = read_file(txt_path);
        currdatay++;

    }
    matrix* resx = init((int) len, 28 * 28, datax);
    matrix* resy = init((int) len, 1, datay);
    *(res + 0) = resx;
    *(res + 1) = resy;
}
