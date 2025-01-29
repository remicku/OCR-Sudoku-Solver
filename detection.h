#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <gtk/gtk.h>
#include "grayscale.h"

typedef struct Output {
    SDL_Surface *surface;
    Intersection *intersections;
    size_t len;
} Output;


Output* cyril (SDL_Surface* bin_surface);
void draw_intersections(SDL_Surface *surface, Intersection *coords, size_t len);
typedef struct Line {
    unsigned int theta;
    unsigned int rho;
    unsigned int value;
    size_t accuPos;
} Line;









