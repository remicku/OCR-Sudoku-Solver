#include "detection.h"
#include <stdio.h>
#include <math.h>
#define pi 3.1415926535
#define maxTheta 360



int minAverage = 200;

float ratio = 0.33;

size_t Msize;
size_t maxDist;
unsigned int minPeak;

double cosArray[maxTheta];
double sinArray[maxTheta];


int exist(Line *lines, Line line, size_t len, int maxGap,
                 unsigned int *M) {

    for (size_t i = 0; i < len; i++) {
        Line iLine = lines[i];

        int dTheta = abs((int)(iLine.theta % 180) - (int)(line.theta % 180));
        int dRho = abs((int)(iLine.rho) - (int)(line.rho));

        if ((dTheta <= 10 || dTheta >= 170) && dRho <= maxGap) {
            if (line.value < iLine.value) {
                M[iLine.accuPos] = 0;
            } 
	    else 
	    {
		lines[i] = line;
                M[line.accuPos] = 0;
            }
            return 1;
        }
    }
    return 0;
}

int check_peak(unsigned int *M, size_t i, unsigned int val) {
    if (val < minPeak)
        return 0;
    if (i % maxTheta != 0 && M[i - 1] > val)
        return 0;
    if (i % maxTheta != maxTheta - 1 && M[i + 1] > val)
        return 0;
    if (i >= maxTheta && M[i - maxTheta] > val)
        return 0;
    if (i < Msize - maxTheta && M[i + maxTheta] > val)
        return 0;
    return 1;
}



Line * detect_lines(unsigned int *M) {

    Line *lines = malloc(500 * sizeof(Line));
    size_t len = 0;

    size_t maxGap = sqrt(maxDist);

    int rho = 0;
    int theta = 0;
    for (size_t i = 0; i < Msize; i++) {

        unsigned int val = M[i];
        if (check_peak(M, i, val)) {
            Line line = {theta, rho, val, i};

            if (!exist(lines, line, len, maxGap, M)) {
                lines[len] = line;
                len++;
            }
        }
       	else 
	{
            M[i] = 0;
        }

        theta++;
        if (theta == maxTheta)
       	{
            theta = 0;
            rho++;
        }
    }
    lines[len].accuPos = Msize + 1;
    return lines;
}



unsigned int FindMinPeak(unsigned int *M) {
    unsigned int maxPeak = 0;
    size_t i = 0;
    while(i<Msize)
    {
    	if(M[i] > maxPeak)
	{
		maxPeak = M[i];
	}
	i++;
    }
    //return maxPeak * ratio;
    return 255;
}



Uint32 get_pixels(const SDL_Surface* surface, int x, int y)
{
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

void GetPixelColor(const SDL_Surface *surface, int x, int y, Uint8 *r, Uint8 *g, Uint8 *b) 
{

    SDL_GetRGB(get_pixels(surface, x, y), surface->format, (Uint8 *)r,
               (Uint8 *)g, (Uint8 *)b);
}

void fill_acumu(unsigned int *M ,SDL_Surface *surface) {

    Uint8 r, g, b;
    for (size_t y = 0; y < (size_t)surface->h; y++) {
        for (size_t x = 0; x < (size_t)surface->w; x++) {

            GetPixelColor(surface, x, y, &r, &g, &b);
            if ((b+ r + g) / 3 >= minAverage) {

                for (size_t theta = 0; theta < maxTheta; theta++) {
                    int rho = x * cosArray[theta] + y * sinArray[theta];
                    if (rho < (int)maxDist && rho >= 0)
		    {
                        M[rho * maxTheta + theta]++;
                    }
                }
            }
        }
    }
}


unsigned int * edge(SDL_Surface *surface) {

    int w = surface->w;
    int h = surface->h;

    maxDist = (size_t)sqrt((double)w * w + h * h) + 1;
    Msize = maxTheta * maxDist;
    unsigned int *M = calloc(Msize, sizeof(unsigned int));
	
    size_t i = 0;
    while (i<maxTheta)
    {
    	 cosArray[i] = cos(i * pi / 180);
        sinArray[i] = sin(i * pi / 180);
	i++;
    }

    fill_acumu(M,surface);
    minPeak = FindMinPeak(M);

    return M;
}



void compute(unsigned int *normalSpace, long int w, long int h, long int x1,
                 long int y1, long int x2, long int y2) {

    int i, dx, dy, maxmove;
    int d, dinc1, dinc2;
    int x, xinc1, xinc2;
    int y, yinc1, yinc2;

    dx = x1 > x2 ? x1 - x2 : x2 - x1;
    dy = y1 > y2 ? y1 - y2 : y2 - y1;

    if (dy <= dx) {
        maxmove = dx + 1;
        d = (2 * dy) - dx;
        dinc1 = 2 * dy;
        dinc2 = (dy - dx) * 2;
        xinc1 = 1;
        xinc2 = 1;
        yinc1 = 0;
        yinc2 = 1;
    } else {
        maxmove = dy + 1;
        d = (2 * dx) - dy;
        dinc1 = 2 * dx;
        dinc2 = (dx - dy) * 2;
        xinc1 = 0;
        xinc2 = 1;
        yinc1 = 1;
        yinc2 = 1;
    }

    if (x2 < x1) {
        xinc1 = -xinc1;
        xinc2 = -xinc2;
    }

    if (y1 > y2) {
        yinc1 = -yinc1;
        yinc2 = -yinc2;
    }

    x = x1;
    y = y1;

    for (i = 0; i < maxmove; ++i) {
        if (x >= 0 && x < w && y >= 0 && y < h)
            normalSpace[y * w + x] += 1;
        if (d < 0) {
            d += dinc1;
            x += xinc1;
            y += yinc1;
        } else {
            d += dinc2;
            x += xinc2;
            y += yinc2;
        }
    }
}

unsigned int * create_space( Line *lines,SDL_Surface *surface) {

    int w = surface->w;
    int h = surface->h;

    unsigned int *normalSpace = calloc(w * h, sizeof(unsigned int));

    Line *current = lines;
    while (current->value != 0) {
        double thetaRad = current->theta * pi / 180;
        double a = cos(thetaRad);
        double b = sin(thetaRad);
        int x0 = a * current->rho;
        int y0 = b * current->rho;
        int x1 = x0 + 3000 * (-b);
        int y1 = y0 + 3000 * a;
        int x2 = x0 - 3000 * (-b);
        int y2 = y0 - 3000 * a;

        compute(normalSpace, w, h, x1, y1, x2, y2);
        current += 1;
    }
    return normalSpace;
}

void Remove(unsigned int *accumulator, Line *lines, size_t capacity,
            size_t *size, size_t gap) {
    size_t dGap = gap / 2;
    while (dGap > 0 && *size > 10) {
        char inSudoku = 0;
        Line *prev = &lines[0];
        for (size_t i = 1; i < capacity; i++) {
            Line *curr = &lines[i];
            if (prev->value == 0 || curr->value == 0 ) {
                continue;
            }

            size_t dRho = (curr->rho - prev->rho) % gap;
            if (dRho > dGap && dRho < gap - dGap) {
                if (!inSudoku) {
                    prev->value = 0;
                    accumulator[prev->accuPos] = 0;
                } else {
                    curr->value = 0;
                    accumulator[curr->accuPos] = 0;
                }
                (*size)--;
            } else {
                inSudoku = 1;
            }
            prev = curr;
        }
        dGap--;
    }
}


size_t find_gap(unsigned int *histo, size_t histoSize, size_t range) {
    size_t current = 0;
    size_t i = 0;
    while (i< 2*range)
    {
    	current += histo[i];
	i++;
    }

    size_t gap = range;
    size_t max = current;
    for (size_t i = 2+range;  histoSize - range > i ; i++) {
        current -= histo[i - range - 1];
        current += histo[i + range];
        if ( max < current )
       	{
            gap = i;
            max = current;
        }
    }
    return gap;
}


Line *filter_line(unsigned int *M, Line *lines, size_t *vertLen,
                  size_t *horiLen) {

    size_t len = 0;
    while (lines[len].accuPos != Msize + 1)
        len++;

    size_t histoSize = Msize / maxTheta;
    unsigned int *histoVert = calloc(histoSize, sizeof(unsigned int));
    unsigned int *histoHori = calloc(histoSize, sizeof(unsigned int));
    Line *horiLines = calloc(len, sizeof(Line));	
    Line *vertLines = calloc(len, sizeof(Line));

    for (size_t i = 1; i < len; i++) {
        size_t j = i;
        while (j >=1 && lines[j - 1].rho > lines[j].rho) {
            Line tmp = lines[j - 1];
            lines[j - 1] = lines[j];
            lines[j] = tmp;
            j-=1;
        }
    }

    unsigned int rho1 = -1, rho2 = -1;
    for (size_t i = 0; i < len; i++) {
        Line line = lines[i];

        if (line.theta % 90 >= 2 && line.theta % 90 <= 88)
            M[line.accuPos] = 0;
        else {
            if ( line.theta > 178 || line.theta < 2) {
                vertLines[*vertLen] = line;
                (*vertLen)+=1;
                if (*vertLen == 1)
                    rho1 = line.rho;
                else {
                    histoVert[line.rho - rho1] += 1;
                    rho1 = line.rho;
                }
            } else {
                horiLines[*horiLen] = line;
                (*horiLen)++;
                if (*horiLen == 1)
                    rho2 = line.rho;
                else 
		{
                    histoHori[line.rho - rho2] += 1;
                    rho2 = line.rho;
                }
            }
        }
    }

    size_t range = 5;
    size_t vertCapacity = *vertLen, horiCapacity = *horiLen;
    while (range > 0 && (*horiLen > 10 || *vertLen > 10)) {
        size_t gap = find_gap(histoVert, histoSize, range);
        Remove(M, vertLines, vertCapacity, vertLen, gap);
        gap = find_gap(histoHori, histoSize, range);
        Remove(M, horiLines, horiCapacity, horiLen, gap);
        range-=1;
    }

    size_t mallocLen = *(vertLen) * (*horiLen) + 1;
    if (mallocLen == 2)
        mallocLen++;

    Line *newLines = malloc(mallocLen * sizeof(Line));
    size_t i = 0, j = 0;
    while (i < vertCapacity) 
    {
        if (vertLines[i].value != 0) {
            if (j >= 10) 
	    {
                (*vertLen)-=1;
                M[vertLines[i].accuPos] = 0;
            }
	    else 
	    {
                newLines[j] = vertLines[i];
                j+=1;
            }
        }
        i++;
    }
    i = 0;
    while (i < horiCapacity) 
    {
        if (horiLines[i].value > 0) 
	{
            if ( *vertLen + 10>j) 
	    {
		 newLines[j] = horiLines[i];
                j+=1;
            }
	    else 
	    {
		     (*horiLen)-=1;
                M[horiLines[i].accuPos] = 0;
            }
        }
        i+=1;
    }
    newLines[j].value = 0;

    free(histoVert);
    free(histoHori);
    free(vertLines);
    free(horiLines);
    free(lines);
    return newLines;
}


int dont_add(Intersection *coords, size_t x, size_t y, size_t nbinter) 
{
	size_t i = 0;
	while ( i <nbinter)
	{
		Intersection iCoord = coords[i];

        	int dx = abs((int)(iCoord.x) - (int)x);
        	int dy = abs((int)(iCoord.y) - (int)y);
	
        	if (dy <= 30 && dx <= 30) 
		{
        	    return 1;
        	}
		i++;
	}	
    
    return 0;
}


Intersection *find_intersections(SDL_Surface *surface, unsigned int *normalSpace,size_t vertLen, size_t horiLen) 
{
    unsigned int w = surface->w, h = surface->h;

    size_t len = vertLen * horiLen;
    Intersection *coords = calloc(len, sizeof(Intersection));
	

    size_t j = 0;
    unsigned int x = 0, y = 0;
    for (size_t i = 0; i < w * h; i++) 
    {
        if (!dont_add(coords, x, y, j) && normalSpace[i] >= 2 ) 
	{
            coords[j].x = x;
            coords[j].y = y;
            j++;
        }
        x++;
        if (x == w) 
	{
            x = 0;
            y++;
        }
    }

    size_t start = 0;
    for (size_t i = 1; i < len; i++) {
        size_t j = i;
        if (0 == i % vertLen ) {
            start = i;
        }
        while (j > start && coords[j - 1].x > coords[j].x)
       	{
            Intersection tmp = coords[j - 1];
            coords[j - 1] = coords[j];
            coords[j] = tmp;
            j-=1;
        }
    }

    vertLen = vertLen > 10 ? 10 : vertLen;
    horiLen = horiLen > 10 ? 10 : horiLen;

    return coords;
}

void put_pixels(SDL_Surface* surface, int x, int y, Uint32 color) 
{
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

void draw_line(int *pixels, long int w, long int h, long int x1, long int y1,long int x2, long int y2, Uint32 color) 
{
    int i, dx, dy, maxmove;
    int d, dinc1, dinc2;
    int x, xinc1, xinc2;
    int y, yinc1, yinc2;

    dx = x1 > x2 ? x1 - x2 : x2 - x1;
    dy = y1 > y2 ? y1 - y2 : y2 - y1;

    if (dy <= dx) {
        maxmove = dx + 1;
        d = (2 * dy) - dx;
        dinc1 = 2 * dy;
        dinc2 = (dy - dx) * 2;
        xinc1 = 1;
        xinc2 = 1;
        yinc1 = 0;
        yinc2 = 1;
    } else {
        maxmove = dy + 1;
        d = (2 * dx) - dy;
        dinc1 = 2 * dx;
        dinc2 = (dx - dy) * 2;
        xinc1 = 0;
        xinc2 = 1;
        yinc1 = 1;
        yinc2 = 1;
    }

    if (x1 > x2) {
        xinc1 = -xinc1;
        xinc2 = -xinc2;
    }

    if (y1 > y2) {
        yinc1 = -yinc1;
        yinc2 = -yinc2;
    }

    x = x1;
    y = y1;

    for (i = 0; i < maxmove; ++i) {
        if (x >= 0 && x < w && y >= 0 && y < h)
            pixels[y * w + x] = color;
        if (d < 0) {
            d += dinc1;
            x += xinc1;
            y += yinc1;
        } else {
            d += dinc2;
            x += xinc2;
            y += yinc2;
        }
    }
}



void draw_lines(SDL_Surface *surface, int *pixels, Line *lines, size_t len) 
{
    Uint32 color = SDL_MapRGB(surface->format, 255, 0, 0);
	size_t i = 0;	
    while (i<len)
    {
	    Line line = lines[i];
    	double thetaRad = line.theta * pi / 180;
        double a = cos(thetaRad);
        double b = sin(thetaRad);
        int x0 = a * line.rho;
        int y0 = b * line.rho;
        int x1 = x0 + 3000 * (-b);
        int y1 = y0 + 3000 * a;
        int x2 = x0 - 3000 * (-b);
        int y2 = y0 - 3000 * a;

        draw_line(pixels, surface->w, surface->h, x1, y1, x2, y2, color);
	i++;
    }
}


void draw_intersections(SDL_Surface *surface, Intersection *coords, size_t len) {
    int w = surface->w;
    int h = surface->h;

    Uint32 color = SDL_MapRGB(surface->format, 0, 0, 255);
    for (size_t i = 0; i < len; i++) {
        int x = (int)coords[i].x;
        int y = (int)coords[i].y;
        for (int dx = -5; dx <= 5; dx++) {
            for (int dy = -5; dy <= 5; dy++) {
                if (x + dx < w && y + dy >= 0 && y + dy < h && x + dx >= 0) {
                    put_pixels(surface, x + dx, y + dy, color);
                }
            }
        }
    }
}

// fct d'appel


Output* cyril(SDL_Surface* bin_surface)
{
	Output* output = malloc(sizeof(Output));
		unsigned int* M = edge(bin_surface);
	size_t vertLen = 0, horiLen = 0;
	Line *lines = detect_lines(M);
        lines = filter_line(M, lines, &vertLen, &horiLen);
	 unsigned int *space = create_space(lines,bin_surface);	
        Intersection *intersections = find_intersections(bin_surface, space, vertLen, horiLen);

    output->surface = bin_surface;
    output->intersections = intersections;
    output->len = vertLen * horiLen;


     draw_lines(bin_surface, bin_surface->pixels, lines, vertLen * horiLen);
        draw_intersections(bin_surface,intersections,vertLen*horiLen);
	//free(M);
    	
	return output;
}
