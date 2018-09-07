#include <SDL2/SDL.h>
#define OUTPUTDIR "./output/"
#define INPUTADRN "./adrn_138.bin"
#define INPUTREALBIN "./real_138.bin"
#define SPRDBIN "./sprd_116.bin"
#define SPRADRNBIN "./spradrn116.bin"
#include <stdio.h>
#include "unpack.h"
#include "types.h"
#include "surface.h"

FILE *Realbinfp;
FILE *Addrbinfp;

PALETTEENTRY	Palette[256];
SDL_Color palColors[256];
SDL_Color highColor32Palette[256];
SDL_Surface* screenSurface = NULL;
SDL_Color highSDLColor[256];

int RealBinWidth = 0;
int RealBinHeight = 0;
char *pRealBinBits;
int maxBitmapNo = 0;

char cmdBuffer[255];

#define REALGETIMAGEMAXSIZE 3200*3200
unsigned char g_realgetimagebuf[REALGETIMAGEMAXSIZE];
unsigned char g_realgetimagebuf2[REALGETIMAGEMAXSIZE];
#define MAX_GRAPHICS 550000

ADRNBIN adrnbuff[MAX_GRAPHICS];
int currentShowImageNo = 0;

void appendCharToCmdBuf(char* c)
{
	strcat(cmdBuffer, c);
}

void parseCmd()
{
	SDL_Log("Jerry Cmd is %s", cmdBuffer);
	char *p;
	p = strtok(cmdBuffer, " ");
	while(p){
   		 SDL_Log("%s ", p);
   		 if(strcmp(p, "g") == 0)
   		 {
   		 	p = strtok(NULL, " ");
   		 	int no = atoi(p);
   		 	no = no <= 0 ? 1 : no;
   		 	no = no >= maxBitmapNo ? maxBitmapNo : no;
   		 	currentShowImageNo = no;
   		 }
   		 else if(strcmp(p, "b") == 0)
   		 {
   		 	p = strtok(NULL, " ");
   		 	int no = currentShowImageNo - atoi(p);
   		 	no = no <= 0 ? 1:no;
   		 	currentShowImageNo = no;
   		 }
   		 else if(strcmp(p, "f") == 0)
   		 {
   		 	p = strtok(NULL, " ");
   		 	int no = currentShowImageNo + atoi(p);
   		 	no = no >= maxBitmapNo ? maxBitmapNo : no;
   		 	currentShowImageNo = no;
   		 }
   		 
		 p = strtok(NULL, " ");
	}
	memset(cmdBuffer, 0, 255);
}

void clearSurface(SDL_Surface* surface)
{
	SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0,0,0));
}

bool realGetImage( int graphicNo, unsigned char **bmpdata, int *width, int *height)
{
	ADRNBIN adrdata;
	if(graphicNo<0 || graphicNo>=MAX_GRAPHICS)return false;
	adrdata=adrnbuff[graphicNo];

	fseek(Realbinfp, adrdata.adder, SEEK_SET);
	if( fread(&g_realgetimagebuf, adrdata.size, 1, Realbinfp ) != 1 )
		return false;

	unsigned int len;
	*bmpdata = g_realgetimagebuf2;
	if( decoder( g_realgetimagebuf, bmpdata,
			(unsigned int*)width, (unsigned int*)height, &len ) == NULL ){
		return false;
	}
	return true;
}

SDL_Surface* createSDLSurface(int w, int h, SDL_Color* pixels = 0)
{
    Uint32 rmask, gmask, bmask, amask;
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;

    int depth, pitch;
    depth = 32;
    pitch = w * 4;

    SDL_Color* pixelData = pixels;
    if (pixels == 0)
    {
        pixelData = (SDL_Color*)malloc(w*h*sizeof(SDL_Color));
        memset(pixelData, 0, w * h * sizeof(SDL_Color));
    }
    
    SDL_Surface* surface = 0;
    surface = SDL_CreateRGBSurfaceFrom((void*)pixelData, w, h, depth, pitch,
                                             rmask, gmask, bmask, amask);
    char error[355];
    sprintf(error, "%s", SDL_GetError());
    return surface;
}

void PaletteForColors(PALETTEENTRY* pal)
{
    for(int i = 0 ; i < 256; i++)
    {
        palColors[i].r = pal[i].peRed;
        palColors[i].g = pal[i].peGreen;
        palColors[i].b = pal[i].peBlue;
    }
}

void exportToFile(ADRNBIN data, unsigned char** bmpdata, int width, int height)
{
	printf("Export");
	int sizeX = width;
	int sizeY = height;
	unsigned char *pSource;
	int offsetY = height - 1;
	int offsetX = 0;
    pSource = *bmpdata + offsetY * width + offsetX;
    static SDL_Color* pixelDataSurface = (SDL_Color*)malloc(1600* 1600* sizeof(SDL_Color));
	int currentPixelPos = 0;
	for(int i = 0; i < sizeY; i++)
	{
		for(int j = 0; j < sizeX ; j++)
		{
			pixelDataSurface[currentPixelPos + j ]  = highColor32Palette[pSource[j]];
		}
		pSource -= width;
		currentPixelPos += sizeX;
	}
	SDL_Surface* surf = createSDLSurface(sizeX, sizeY, pixelDataSurface);
	char buff[255];
	sprintf(buff, "./output/%d.bmp", data.attr.bmpnumber);
	printf("file: buff:%s", buff);
    if (data.attr.bmpnumber ==29027) {
            SDL_SaveBMP(surf, buff);
    }
	
    SDL_FreeSurface(surf);
}

bool initRealbinFileOpen(const char *realbinfilename, const char *addrbinfilename)
{
	SDL_Log("Jerry Start to Init Realbin file....");
	ADRNBIN tmpadrnbuff;
    memset(&tmpadrnbuff, 0, sizeof(tmpadrnbuff));
	if ((Addrbinfp = fopen(addrbinfilename, "rb"))==NULL)
	{
		SDL_Log("Open Adrn File Failed");
		return false;
	}
	if ((Realbinfp = fopen(realbinfilename, "rb"))==NULL)
	{
		SDL_Log("Open Realbin file failed");
		return false;
	}

	//adrn.bin
	unsigned long adrnIndex = 0;
	while(!feof(Addrbinfp)){
		fread(&tmpadrnbuff, sizeof(tmpadrnbuff), 1, Addrbinfp);
		adrnbuff[tmpadrnbuff.bitmapno] = tmpadrnbuff;
		maxBitmapNo = tmpadrnbuff.bitmapno;
		//if (tmpadrnbuff.bitmapno == 437719)
		//{
		//	int debug = 0;
		//}
        unsigned int width = 0;
        unsigned int height = 0;
		fseek(Realbinfp, tmpadrnbuff.adder, SEEK_SET);//real.bin??????????? 
		if( fread(g_realgetimagebuf, tmpadrnbuff.size, 1, Realbinfp ) == 1 )
		{
			unsigned char** bmpdata = (unsigned char**)&g_realgetimagebuf2;		
			unsigned int len;
			if( decoder( g_realgetimagebuf, bmpdata,
					(unsigned int*)&width, (unsigned int*)&height, &len ) == NULL ){
					unsigned int bmpNo = tmpadrnbuff.bitmapno;
					SDL_Log("Jerry Should Export to file?");
			}
			else
			{
//				exportToFile(tmpadrnbuff, bmpdata, width, height);
			}			
		}
		
		if( tmpadrnbuff.attr.bmpnumber != 0 ){
			if( (12802 <= tmpadrnbuff.attr.bmpnumber && tmpadrnbuff.attr.bmpnumber <= 12811)
			 || (10132 <= tmpadrnbuff.attr.bmpnumber && tmpadrnbuff.attr.bmpnumber <= 10136) ){
			}
			if( tmpadrnbuff.attr.bmpnumber<=33 && tmpadrnbuff.bitmapno>230000){//∑¿∂¬ƒß∑®Õº∫≈∏≤∏«…˘“Ùµƒbug
			}
			else
			{
				unsigned short bmpNo = tmpadrnbuff.bitmapno;
				if(tmpadrnbuff.attr.bmpnumber == 26301){
					SDL_Log("Jerry attrno is %d", bmpNo);
				}
				//SDL_Log("%d, ---bmpNo: %d", tmpadrnbuff.attr.bmpnumber, bmpNo);
//				printf("%d\n", bmpNo);
			}
		}
		else
		{
//			printf("not bmp:%d\n", tmpadrnbuff.bitmapno);
		}
	}
	fclose(Addrbinfp);
	return true;
}

bool InitPalette( void )
{
	int i;
	FILE *fp;
	
	PALETTEENTRY pal[ 32 ]={
		{0x00 ,0x00, 0x00, 0 | 2}, // 0:?
		{0x80 ,0x00, 0x00, 0 | 2}, // 1:??��
		{0x00 ,0x80, 0x00, 0 | 2}, // 2:???
		{0x80 ,0x80, 0x00, 0 | 2}, // 3:???
		{0x00 ,0x00, 0x80, 0 | 2}, // 4:??��
		{0x80 ,0x00, 0x80, 0 | 2}, // 5:???
		{0x00 ,0x80, 0x80, 0 | 2}, // 6:???��
		{0xc0 ,0xc0, 0xc0, 0 | 2}, // 7:???
		{0xc0 ,0xdc, 0xc0, 0 | 2}, // 8:?
		{0xa6 ,0xca, 0xf0, 0 | 2}, // 9:?
		
		{ 0xde, 0x00, 0x00, 0 | 2 },
		{ 0xff, 0x5f, 0x00, 0 | 2 },
		{ 0xff, 0xff, 0xa0, 0 | 2 },
		{ 0x00, 0x5f, 0xd2, 0 | 2 },
		{ 0x50, 0xd2, 0xff, 0 | 2 },
		{ 0x28, 0xe1, 0x28, 0 | 2 },
		
		{ 0xf5, 0xc3, 0x96, 0 | 2 },
		{ 0xe1, 0xa0, 0x5f, 0 | 2 },
		{ 0xc3, 0x7d, 0x46, 0 | 2 },
		{ 0x9b, 0x55, 0x1e, 0 | 2 },
		{ 0x46, 0x41, 0x37, 0 | 2 },
		{ 0x28, 0x23, 0x1e, 0 | 2 },
		
		{0xff ,0xfb, 0xf0, 0 | 2}, // 246:?
		{0xa0 ,0xa0, 0xa4, 0 | 2}, // 247:?
		{0x80 ,0x80, 0x80, 0 | 2}, // 248:?��
		{0xff ,0x00, 0x00, 0 | 2}, // 249:��
		{0x00 ,0xff, 0x00, 0 | 2}, // 250:?
		{0xff ,0xff, 0x00, 0 | 2}, // 251:?
		{0x00 ,0x00, 0xff, 0 | 2}, // 252:��
		{0xff ,0x00, 0xff, 0 | 2}, // 253:?
		{0x00 ,0xff, 0xff, 0 | 2}, // 254:?��
		{0xff ,0xff, 0xff, 0 | 2}  // 255:?
	};


	for( i = 0; i < 10; i++ ){
		Palette[i].peBlue  	= pal[i].peBlue;
		Palette[i].peGreen 	= pal[i].peGreen;
		Palette[i].peRed 	= pal[i].peRed;
		Palette[i].peFlags 	= 3;
		
		Palette[i+246].peBlue  	= pal[i+22].peBlue;
		Palette[i+246].peGreen 	= pal[i+22].peGreen;
		Palette[i+246].peRed 	= pal[i+22].peRed;
		Palette[i+246].peFlags 	= 3;
	}
	
	for( i = 0; i < 6; i++ ){
		Palette[i+10].peBlue  	= pal[i+10].peBlue;
		Palette[i+10].peGreen 	= pal[i+10].peGreen;
		Palette[i+10].peRed 	= pal[i+10].peRed;
		Palette[i+10].peFlags 	= 0 | 2;
		
		Palette[i+240].peBlue  	= pal[i+16].peBlue;
		Palette[i+240].peGreen 	= pal[i+16].peGreen;
		Palette[i+240].peRed 	= pal[i+16].peRed;
		Palette[i+240].peFlags 	= 0 | 2;
	}
	
    fp = fopen("./pal/Palet_1.sap", "rb" );
    if(fp == NULL){
        return false;
    }
    else{
        for( i = 16; i < 240; i++ ){
            Palette[i].peBlue  	= fgetc( fp );
            Palette[i].peGreen 	= fgetc( fp );
            Palette[i].peRed 	= fgetc( fp );
            Palette[i].peFlags = 0 | 2;
        }
        fclose( fp );
    }
    Palette[168].peBlue  	= 0;
	Palette[168].peGreen 	= 0;
	Palette[168].peRed 		= 0;

    PaletteForColors(Palette);
    highColor32Palette[0].r = 0;
    highColor32Palette[0].g = 0;
    highColor32Palette[0].b = 0;
    highColor32Palette[0].a = 0;

    for( i = 1; i < 256; i++ ){
        highColor32Palette[i].r = Palette[i].peRed;
        highColor32Palette[i].g = Palette[i].peGreen;
        highColor32Palette[i].b = Palette[i].peBlue;
        highColor32Palette[i].a = 1;
    }

    for (i = 1; i < 256; i++)
    {
    	highSDLColor[i].r = Palette[i].peRed;
	highSDLColor[i].g = Palette[i].peGreen;
	highSDLColor[i].b = Palette[i].peBlue;
	highSDLColor[i].a = 255;
    }    
	return true;
};

void init()
{
    if(InitPalette()) {
     printf("Init Palette OK!\n");
    }
    else
    {
     printf("Init Palette Failed!\n");
    }
    
    Uint32 rmask, gmask, bmask, amask;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
#endif

    screenSurface = SDL_CreateRGBSurface(0, 800, 600, 32,
                                   rmask, gmask, bmask, amask);
}

void unpacking()
{
    printf("Unpacking...\n");
    initRealbinFileOpen("./real_136.bin", "./adrn_136.bin");
//    initRealbinFileOpen("./real_15.bin", "./adrn_15.bin");
//    initRealbinFileOpen("./real_2.bin", "./adrn_2.bin");
}

void paint(unsigned int bmpNo, int x, int y)
{
	static int lastPaintImage = -1;
	if(lastPaintImage == bmpNo)
	{
		return ;
	}
	lastPaintImage = bmpNo;
	clearSurface(screenSurface);
	bool readImageRes = realGetImage(bmpNo,(unsigned char **)&pRealBinBits, &RealBinWidth,&RealBinHeight);
	if(false == readImageRes)
	{
		SDL_Log("Jerry : Read Image Failed: %d", bmpNo);
		return ;
	}
	int res = DrawBitmap2Surface(x, y, RealBinWidth, RealBinHeight, (unsigned char**)&pRealBinBits, screenSurface);
	char buffer[255];
	sprintf(buffer, "%d.bmp", bmpNo);
//	SaveSurface2File(screenSurface, buffer);
}


int main(int argc, char* argv[])
{
    if(argc < 2) {
        printf("Usage : sashrink <command> commands are : unpack, pack\n");
        //return 1;
    }
    SDL_Init(SDL_INIT_EVERYTHING);
    init();
    unpacking();
    bool quit = false;
//    SDL_Window* window = SDL_CreateWindow("PocketSATools", 100, 100, 800, 600, 0);
  //  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    //SDL_Texture* screenTexture = SDL_CreateTextureFromSurface(renderer, screenSurface);
    int screenPitch = 600 * sizeof(int);
    void* pixels;
    while(adrnbuff[currentShowImageNo].bitmapno == 0)
    {
    	currentShowImageNo++;
    }
    SDL_Log("Jerry : First bitmapNo is %d, %d", adrnbuff[currentShowImageNo].bitmapno, currentShowImageNo);

/*    while(!quit)
    {
    	SDL_RenderClear(renderer);
    	SDL_Event e;
    	if(SDL_PollEvent(&e)) {
    		if(e.type == SDL_QUIT) {
    			quit = true;
    		}
    		else if(e.type == SDL_KEYDOWN) {
			if (e.key.keysym.sym == SDLK_ESCAPE)
			{
				quit = true;
			}
			else if(e.key.keysym.sym == SDLK_LEFT)
			{
				currentShowImageNo = currentShowImageNo - 1 <= 0 ? 1 : currentShowImageNo - 1;
				char buffer[255];
				sprintf(buffer, "PocketSaTools: %d, %d", currentShowImageNo, adrnbuff[currentShowImageNo].attr.bmpnumber);
				SDL_SetWindowTitle(window, buffer);
			}
			else if(e.key.keysym.sym == SDLK_RIGHT)
			{
				currentShowImageNo = currentShowImageNo + 1 >= maxBitmapNo ? maxBitmapNo : currentShowImageNo + 1;
				char buffer[255];
				sprintf(buffer, "PocketSaTools: %d", currentShowImageNo);
				SDL_SetWindowTitle(window, buffer);
			}
			else if(e.key.keysym.sym == SDLK_RETURN)
			{
				parseCmd();
			}
			
    		}
    		else if(SDL_TEXTINPUT == e.type)
    		{
    			appendCharToCmdBuf(e.text.text);
    		}
    	}
    	paint(currentShowImageNo, 10, 0);
  	SDL_UpdateTexture(screenTexture, NULL, screenSurface->pixels, 600 * 4);
	SDL_RenderCopy(renderer, screenTexture, NULL, NULL);
	SDL_RenderPresent(renderer);
    }
    SDL_DestroyRenderer(renderer);
    SDL_FreeSurface(screenSurface);
    SDL_DestroyTexture(screenTexture);*/
    SDL_Quit();
    return 0;
}