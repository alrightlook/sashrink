#ifndef _SURFACE_HEADER_
#define _SURFACE_HEADER_

#include "SDL.h"

int DrawBitmap2Surface(int x,int y,int w, int h, unsigned char** data, SDL_Surface* surface);
int SaveSurface2File(SDL_Surface* surface, char* filename);

#endif// _SURFACE_HEADER_