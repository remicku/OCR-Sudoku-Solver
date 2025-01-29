#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <gtk/gtk.h>

// header of the pretty_print
//
 SDL_Surface* color_to_grayscale(gchar* argv);
 SDL_Surface* load_image(const char* path);
 SDL_Surface* rotate_surface(SDL_Surface* surface, guint angle);
 void extract_region(SDL_Surface* surface, int x, int y, int length);

 typedef struct Intersection {
    unsigned int x;
    unsigned int y;
} Intersection;
