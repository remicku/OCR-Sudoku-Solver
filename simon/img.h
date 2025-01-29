//
// Created by simon on 11/25/2023.
//

#ifndef NNXOR_IMG_H
#define NNXOR_IMG_H

#include <stddef.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "matrix.h"

SDL_Surface* paths_to_surface(char* paths, size_t len);

SDL_Surface* scale_surfaces(SDL_Surface* surfaces, size_t len);

void to_blacks_and_whites(SDL_Surface* surfaces, size_t len);

matrix* imgs_to_matrix(SDL_Surface* surfaces, size_t len);
matrix* imgss_to_matrix(SDL_Surface* surfaces, size_t len);

matrix** tagged_imgs_to_matrix(char** paths, size_t len);


#endif //NNXOR_IMG_H
