#include <err.h>
#include "grayscale.h"
#include "detection.h"
#define THRESHOLD 80
#define BLUR_RADIUS 3

// Updates the display.
//
// renderer: Renderer to draw on.
// texture: Texture that contains the image.
void draw(SDL_Renderer* renderer, SDL_Texture* texture)
{
	SDL_RenderCopy( renderer,  texture, NULL, NULL);
	SDL_RenderPresent(renderer);

}



// Event loop that calls the relevant event handler.
//
// renderer: Renderer to draw on.
// colored: Texture that contains the colored image.
// grayscale: Texture that contains the grayscale image.
// Event loop that calls the relevant event handler.
//
// renderer: Renderer to draw on.
// texture: Texture to display.
void event_loop(SDL_Renderer* renderer, SDL_Texture* texture)
{
	draw(renderer,texture);
	SDL_Event event;
	int nbRota = 0;
	const double a = 20;
	while (1)
	{
	    SDL_WaitEvent(&event);
	    switch (event.type)
	    {
	    	case SDL_WINDOWEVENT :
		    if (event.window.event == SDL_WINDOWEVENT_RESIZED)
		    	draw(renderer,texture);
		    break;

		case SDL_QUIT:
		    return;
		case SDL_KEYDOWN :
		{
		    switch( event.key.keysym.sym )
		    {
		    	case SDLK_r: //turn to the right
			    nbRota -=1;
                	    SDL_RenderCopyExF(renderer,texture,NULL,NULL,a*nbRota,NULL,SDL_FLIP_NONE);
		            SDL_RenderPresent(renderer);
			    break;
							
			case SDLK_l: //turn to the left
			    nbRota +=1;
                            SDL_RenderCopyExF(renderer,texture,NULL,NULL,a*nbRota,NULL,SDL_FLIP_NONE);
                            SDL_RenderPresent(renderer);					
			    break;
		    }
		}
	   }
		
	}
}

Uint32 get_pixel(SDL_Surface* surface, int x, int y) {
    int bpp = surface->format->BytesPerPixel;
    Uint8* pixel = (Uint8*) surface->pixels + y * surface->pitch + x * bpp;

    switch (bpp) {
        case 1:
            return *pixel;
        case 2:
            return *(Uint16*) pixel;
        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
                return pixel[0] << 16 | pixel[1] << 8 | pixel[2];
            else
                return pixel[0] | pixel[1] << 8 | pixel[2] << 16;
        case 4:
            return *(Uint32*) pixel;
        default:
            return 0;
    }
}

void put_pixel(SDL_Surface* surface, int x, int y, Uint32 color) {
    int bpp = surface->format->BytesPerPixel;
    Uint8* pixel = (Uint8*) surface->pixels + y * surface->pitch + x * bpp;

    switch (bpp) {
        case 1:
            *pixel = color;
            break;
        case 2:
            *(Uint16*) pixel = color;
            break;
        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                pixel[0] = (color >> 16) & 0xff;
                pixel[1] = (color >> 8) & 0xff;
                pixel[2] = color & 0xff;
            } else {
                pixel[0] = color & 0xff;
                pixel[1] = (color >> 8) & 0xff;
                pixel[2] = (color >> 16) & 0xff;
            }
            break;
        case 4:
            *(Uint32*) pixel = color;
            break;
        default:
            break;
    }
}

SDL_Surface* rotate_surface(SDL_Surface* surface, guint angle){
    if (angle > 0){
        angle -= 360;
    }

    // Calculate the sine and cosine of the angle
    float angle_radians = -angle * M_PI / 180.0f;
    float cos_theta = cos(angle_radians);
    float sin_theta = sin(angle_radians);

    // Get the width and height of the original surface
    int width = surface->w;
    int height = surface->h;

    // Create a new surface to hold the rotated image
    SDL_Surface* rotated_image = SDL_CreateRGBSurface(surface->flags, width, height, surface->format->BitsPerPixel, surface->format->Rmask, surface->format->Gmask, surface->format->Bmask, surface->format->Amask);

    // Loop over each pixel in the rotated surface and set its color based on the corresponding pixel in the original surface
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Calculate the coordinates of the original pixel relative to the center of rotation
            float x_rot = (x - width / 2) * cos_theta - (y - height / 2) * sin_theta + width / 2;
            float y_rot = (x - width / 2) * sin_theta + (y - height / 2) * cos_theta + height / 2;

            // If the rotated pixel is within the bounds of the original surface, set its color
            if (x_rot >= 0 && x_rot < width && y_rot >= 0 && y_rot < height) {
                Uint32 pixel = get_pixel(surface, x_rot, y_rot);
                put_pixel(rotated_image, x, y, pixel);
            }
        }
    }
    return rotated_image;
}


Uint32 pixel_to_grayscale(Uint32 pixel_color, SDL_PixelFormat* format)
{
        Uint8 r, g, b;
        SDL_GetRGB(pixel_color, format, &r, &g, &b);
        Uint32 average = 0.3*r +0.59*g+0.11*b;
        r = average;
        g = average;
        b = average;
        Uint32 color = SDL_MapRGB(format, r, g, b);
        return color;
}


void surface_to_grayscale(SDL_Surface *surface)
{
    Uint32* pixels = surface->pixels;
    int len = surface->w * surface->h;
    SDL_PixelFormat* format = surface->format;
    int lock = SDL_LockSurface(surface);
    if (lock != 0)
    {
        errx(EXIT_FAILURE,"%s", SDL_GetError());
    }
    for (int i =0 ; i<len ;i++)
    {
        pixels[i] = pixel_to_grayscale(pixels[i],format);
    }

    SDL_UnlockSurface(surface);
}

void histo_init (SDL_Surface *surface, double h[],int hm , int hM ,int wm , int wM , int w)
{
        Uint32 *pixels = surface->pixels;
        for (size_t i = 0; i<256;i++)
        {
                h[i] = 0;
        }
        for (int x = wm ; x < wM ; x++)
        {
                for(int y = hm; y<hM ; y++)
                {
                        Uint32 p = pixels[x + w * y];
                        Uint8 r,g,b;
                        SDL_GetRGB(p, surface->format , &r,&g,&b);
                        h[r] += 1;
                }
        }

}


int otsu (SDL_Surface *surface, int hm , int hM , int wm , int wM , int w)
{
        double h[256];
        histo_init(surface , h , hm,hM,wm,wM,w);
	double q = 0;
	int i = 0;
    while (256>i)
	{
		q += ((int)h[i])*i;
		i++;
	}

	double m1 , m2 ,var ;
	double maxv = 0;
	double w2 = 0;
	long n1 = 0;
	long n2;
	int nbp = (hM - hm) * (wM - wm);
    	int threshold = 0;

	for  (int i = 0; i < 255 ; i++)
	{
		n1 += h[i];
        	n2 = nbp - n1;
        	w2 += (double)(i * ((int)h[i]));
		m1 = w2 / n1;
        	m2 = (q - w2) / n2;
		var = (double)n1 * (double)n2 * (m1 - m2) * (m1 - m2);
		if (maxv < var )
		{
			maxv = var;
			threshold = i;
		}
	}

	return threshold - 15;
}


void bin_square(SDL_Surface *surface , int threshold, int len  )
{
	Uint32 *pixels = surface->pixels;
	 SDL_PixelFormat *format = surface->format;
	 for (int i = 0 ; i< len;i++)
	 {
		Uint8  r,g,b;
		SDL_GetRGB(pixels[i],format,&r,&g,&b);

	 	if (threshold > r)
			pixels[i] = SDL_MapRGB (format , 255 , 255 ,255);
		else 
			 pixels[i] = SDL_MapRGB (format , 0 , 0 ,0);
	 }
}

void binarisation(SDL_Surface *surface)
{
        //Uint32* pixels = surface->pixels;
        int longueur = surface->w;
        int largeur = surface->h;
        //SDL_PixelFormat* format = surface->format;
        int len = longueur*largeur;

	if(SDL_LockSurface(surface)!=0){
                return errx(EXIT_FAILURE,"%s",SDL_GetError());
        }
	int threshold = otsu( surface , 0 , largeur , 0 , longueur,longueur);
	bin_square(surface,threshold,len);
}







// Loads an image in a surface.
// The format of the surface is SDL_PIXELFORMAT_RGB888.
//
// path: Path of the image.
SDL_Surface* load_image(const char* path)
{
	SDL_Surface* surface = IMG_Load(path);
    if (surface == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());
    SDL_Surface* res  = SDL_ConvertSurfaceFormat (surface,SDL_PIXELFORMAT_RGB888,0 );
	if (res == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());
    SDL_FreeSurface(surface);
	return res;
}





void extract_region(SDL_Surface* surface, int x, int y, int length)
{
    int longueur = length / 9;
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            SDL_Rect src_rect = { x + j * longueur, y + i * longueur, longueur, longueur };

            SDL_Surface* extracted_surface = SDL_CreateRGBSurface(0, longueur, longueur, 32, 0, 0, 0, 0);
            if (extracted_surface == NULL) {
                fprintf(stderr, "Erreur : impossible de crÃ©er la surface extraite.\n");
                exit(1);
            }

            // Convert the pixel format of the source surface to match the extracted surface
            SDL_Surface* converted_surface = SDL_ConvertSurface(surface, extracted_surface->format, 0);
            if (converted_surface == NULL) {
                fprintf(stderr, "Erreur : impossible de convertir la surface source.\n");
                exit(1);
            }

            // Clear the extracted surface with a transparent color
            SDL_FillRect(extracted_surface, NULL, SDL_MapRGBA(extracted_surface->format, 0, 0, 0, 0));

            // Extract the portion from the converted surface and copy it to the extracted surface
            SDL_BlitSurface(converted_surface, &src_rect, extracted_surface, NULL);

            char filename[50];
            sprintf(filename, "cases/line_%d-column_%d.png", i, j);
            IMG_SavePNG(extracted_surface, filename);

            SDL_FreeSurface(converted_surface);
            SDL_FreeSurface(extracted_surface);
        }
    }
}





SDL_Surface* color_to_grayscale(gchar* argv)
{
	// - Initialize the SDL.

	if (SDL_Init(SDL_INIT_VIDEO) != 0)
		errx(EXIT_FAILURE, "%s", SDL_GetError());

	// - Create a surface from the colored image.
	SDL_Surface* surface = load_image(argv);

    // - Convert the surface into grayscale.
    surface_to_grayscale(surface);
    binarisation(surface);

	// - Dispatch the events.

	// - Destroy the objects.
	
	return surface;
}
