#include "text.h"
const SDL_Color black = {0,0,0};

void Text::resize(float f) {
//	if (huge) TTF_CloseFont(huge);
//	if (normal) TTF_CloseFont(normal);
//	if (small) TTF_CloseFont(small);
//	if (verysmall) TTF_CloseFont(verysmall);
//	if (tiny) TTF_CloseFont(tiny);
	if (!huge) huge=TTF_OpenFont("./fonts/truetype/LiberationSans-Bold.ttf",72);
	if (!big) big=TTF_OpenFont("./fonts/truetype/LiberationSans-Bold.ttf",54);
	if (!large) large=TTF_OpenFont("./fonts/truetype/LiberationSans-Bold.ttf",42);
	if (!normal) normal=TTF_OpenFont("./fonts/truetype/LiberationSans-Regular.ttf",36);
	if (!small) small=TTF_OpenFont("./fonts/truetype/LiberationSans-Regular.ttf",28);
//	verysmall=TTF_OpenFont("/usr/share/fonts/truetype/LiberationSans-Regular.ttf",26*f);
	if (!tiny) tiny=TTF_OpenFont("./fonts/truetype/LiberationSans-Regular.ttf",12);
	reset_cache();
}
void Text::reset_cache() {
	for (int i=0; i<CACHE_SIZE; i++) {
	    cache_string[i][0]=0;
	}
}


Text::Text() {
	if(TTF_Init()) throw Exiter(TTF_GetError());
	huge=big=large=normal=small=verysmall=tiny=NULL;
	resize(1.0);
	for (int i=0; i<CACHE_SIZE; i++) {
	    cache[i]=NULL;
	    cache_string[i][0]=0;
	}
}

int Text::drawtext_cache(const char * t, SDL_Surface *surf, int x, int y, SDL_Color c, TTF_Font *f, int halign, int valign, int cid) {
    if (cid<0 || cid>CACHE_SIZE) return -1;
    if (strcmp(t,cache_string[cid])) {
      if (cache[cid]) SDL_FreeSurface(cache[cid]);
      cache[cid]=rendertext(t,c,f);
      strcpy(cache_string[cid],t);
//      printf("rendering string:'%s' @ %i\n",cache_string[cid],cid);
    }
    return drawrenderedtext(cache[cid], surf, x, y, halign, valign);
}
inline int Text::drawtext(const char * t, SDL_Surface *surf, int x, int y, SDL_Color c, TTF_Font *f, int halign, int valign) {
//    if (y < 30) return -1;
    SDL_Surface *text;
    text = rendertext(t, c, f);
    int rx = drawrenderedtext(text, surf, x, y, halign, valign);
    SDL_FreeSurface(text);
    return rx;
}
void Text::drawtext(const char * t, SDL_Surface *surf, int x, int y, SDL_Color c, TTF_Font *f) {
    drawtext(t, surf, x, y, c, f, A_LEFT, A_TOP);
}
void Text::drawtext_c(const char * t, SDL_Surface *surf, int x, int y, SDL_Color c, TTF_Font *f) {
    drawtext(t, surf, x, y, c, f, A_CENTER, A_TOP);
}
int Text::drawtext_cx(const char * t, SDL_Surface *surf, int x, int y, SDL_Color c, TTF_Font *f) {
    return drawtext(t, surf, x, y, c, f, A_CENTER, A_TOP);
}
void Text::drawtext_r(const char * t, SDL_Surface *surf, int x, int y, SDL_Color c, TTF_Font *f) {
    drawtext(t, surf, x, y, c, f, A_RIGHT, A_TOP);
}
void Text::drawtext_cc(const char * t, SDL_Surface *surf, int x, int y, SDL_Color c, TTF_Font *f) {
    drawtext(t, surf, x, y, c, f, A_CENTER, A_CENTER);
}

SDL_Surface* Text::rendertext(const char * t, SDL_Color c, TTF_Font *f) {
//    return TTF_RenderText_Shaded(f, t, c, black);
    return TTF_RenderText_Blended(f, t, c);
}
inline int Text::drawrenderedtext(SDL_Surface *text, SDL_Surface *surf, int x, int y, int halign, int valign) {
    if (!text) throw Exiter("No text surface!");
    SDL_Rect re;
    if (halign == A_LEFT) { re.x = x; }
    else if (halign == A_RIGHT) { re.x = x - text->w; }
    else { re.x = x - text->w/2; }

    if (valign == A_TOP) { re.y = y; }
    else if (valign == A_BOTTOM) { re.y = y - text->h; }
    else { re.y = y - text->h/2; }

//    re.w = text->w;
//    re.h = text->h;
//    printf("%ix%i  at %i,%i\n",text->w,text->h,re.x,re.y);
    SDL_BlitSurface(text,NULL,surf,&re);
//    printf("ok\n");
    return re.x;
}
void Text::drawrenderedtext_cc(SDL_Surface *text, SDL_Surface *surf, int x, int y) {
    drawrenderedtext(text, surf, x, y, A_CENTER, A_CENTER);
}
void Text::drawrenderedtext_r(SDL_Surface *text, SDL_Surface *surf, int x, int y) {
    drawrenderedtext(text, surf, x, y, A_RIGHT, A_TOP);
}
void Text::drawrenderedtext_lc(SDL_Surface *text, SDL_Surface *surf, int x, int y) {
    drawrenderedtext(text, surf, x, y, A_LEFT, A_CENTER);
}




