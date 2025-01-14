#ifndef DRAW_H
#define DRAW_H
#include <vector>
#include <queue>
#include <math.h>
#include <stdlib.h>

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_gfx/SDL_gfxPrimitives.h"

#include "util.h"
#include "text.h"
#include "game.h"


class Draw {
public:
    Game *game;
    Text *text;
    SDL_Surface *screen;
    SDL_Surface *bar[MAXSCALE];
    SDL_Surface *animbar[MAXSCALE];
    SDL_Surface *numbers[MAXSCALE];
    int xoffset;
    int yoffset;
    int xo,yo;
    int cx, cy;
    int SCREENWIDTH, SCREENHEIGHT;
    int BARHEIGHT;
    int scale;
    bool cursor;

    void draw_digit(int nb, int color, int x, int y);
    void draw_number(int nb, int color, int x, int y);
    void draw();
    void draw_bare();
    void draw_stats();
    Draw(Game *g, SDL_Surface *s, Text *t) {
	game=g;
	screen=s;
	text=t;
	xoffset=0;
	yoffset=0;
	cursor=false;
	scale=DEFAULTSCALE;
	SCREENWIDTH=MSCREENWIDTH*DEFAULTSCALE;
	SCREENHEIGHT=MSCREENHEIGHT*DEFAULTSCALE;
	cx=SCREENWIDTH/2;
	cy=SCREENHEIGHT/2;
	BARHEIGHT=MBARHEIGHT*DEFAULTSCALE;
	SDL_Surface *img = NULL;
	char imgfn[50];
	for (int i=1; i <= MAXSCALE; i++) {
	    snprintf(imgfn,50,"gfx/actions%i.png",i*MBARHEIGHT);
	    img=IMG_Load(imgfn);
	    if (img) bar[i-1]=SDL_DisplayFormat(img);
	    else {
	      bar[i-1]=0;
	      printf("Warning: could not load action bar at scale %i",i);
	    }
	    SDL_FreeSurface(img);
	    snprintf(imgfn,50,"gfx/actions_anim%i.png",i*MBARHEIGHT);
	    img=IMG_Load(imgfn);
	    if (img) animbar[i-1]=SDL_DisplayFormat(img);
	    else {
	      animbar[i-1]=0;
	      printf("Warning: could not load action bar animations at scale %i",i);
	    }
	    SDL_FreeSurface(img);
	    snprintf(imgfn,50,"gfx/numbers%i.png",i);
	    img=IMG_Load(imgfn);
	    if (img) numbers[i-1]=SDL_DisplayFormatAlpha(img);
	    else {
	      numbers[i-1]=0;
	      printf("Warning: could not load number map at scale %i",i);
	    }
	    SDL_FreeSurface(img);
	    img=NULL;
	}
    }
};

#endif
