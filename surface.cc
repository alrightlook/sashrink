#include "surface.h"
#include "FreeImage.h"

extern SDL_Color* highColor32Palette;
extern SDL_Color highSDLColor[256];
int DrawBitmap2Surface(int x,int y,int w, int h, unsigned char** data, SDL_Surface *surface)
{
    unsigned char* pSource = *data + (h - 1) * w;
//    SDL_Log("Jerry :Draw Bitmap 2 surface with %d, %d, %d, %d", x, y, w, h);
    SDL_LockSurface(surface);
    Uint32* pDest =  (Uint32*)surface->pixels;
//    pDest = pDest + x + y * 800;
    for(int i = 0 ; i < h; i++)
    {
        for(int j = 0; j < w; j++)
        {
            if(pSource[j] == 0)
            {
                pDest[j] = 0;
            }
            else {
                SDL_Color c = highSDLColor[(unsigned char)pSource[j]];
                pDest[j] = SDL_MapRGBA(surface->format, c.b, c.g, c.r, c.a);
            }
        }
        pDest += 600;
        pSource -= w;
        
    }
    SDL_UnlockSurface(surface);
    return 0;
}

int DrawBitmapToPNG(int x, int y, int w, int h, unsigned char** data, const char* filename)
{
    unsigned char* pSource = *data + (h - 1) * w;
    SDL_Log("Jerry Create Png :%d, %d", w, h);
    FIBITMAP* bitmap = FreeImage_Allocate(w, h, 32,8,8,8);
    RGBQUAD   color;
    for (int i = h-1 ; i >= 0; i--)
    {
        for(int j = 0; j < w; j++)
        {
                SDL_Color c = highSDLColor[(unsigned char)pSource[j]];
                color.rgbRed = c.r;
                color.rgbBlue = c.b;
                color.rgbGreen = c.g;
              color.rgbReserved = c.a;
            FreeImage_SetPixelColor(bitmap, j, i, &color);
        }
        pSource-=w;
    }
    
    FreeImage_Save(FIF_PNG, bitmap, filename, 0);
    return 0;
}

int SaveSurface2File(SDL_Surface* surface, char* filename)
{
    SDL_SaveBMP(surface, filename);
    return 0;
}