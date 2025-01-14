#ifndef TEXT_H
#define TEXT_H
#include "SDL_ttf.h"
#include "SDL.h"
#include "SDL_image.h"
#include "util.h"

#define A_LEFT 0
#define A_RIGHT 1
#define A_CENTER 2
#define A_TOP 0
#define A_BOTTOM 1

#define CACHE_SIZE 20
#define MAX_STRING_SIZE 500

class Text {
public:
    TTF_Font *huge;
    TTF_Font *big;
    TTF_Font *large;
    TTF_Font *normal;
    TTF_Font *small;
    TTF_Font *verysmall;
    TTF_Font *tiny;
    SDL_Surface *cache[CACHE_SIZE];
    char cache_string[CACHE_SIZE][MAX_STRING_SIZE];

    Text();
    ~Text() {
	TTF_CloseFont(huge);
	TTF_CloseFont(big);
	TTF_CloseFont(normal);
	TTF_CloseFont(small);
	TTF_CloseFont(verysmall);
	TTF_CloseFont(tiny);
    }
    void resize(float f);
    void reset_cache();

    int drawtext_cache(const char * t, SDL_Surface *surf, int x, int y, SDL_Color c, TTF_Font *f, int halign, int valign, int cid);
    inline int drawtext(const char * t, SDL_Surface *surf, int x, int y, SDL_Color c, TTF_Font *f, int halign, int valign);
    void drawtext(const char * t, SDL_Surface *surf, int x, int y, SDL_Color c, TTF_Font *f);
    void drawtext_c(const char * t, SDL_Surface *surf, int x, int y, SDL_Color c, TTF_Font *f);
    int drawtext_cx(const char * t, SDL_Surface *surf, int x, int y, SDL_Color c, TTF_Font *f);
    void drawtext_r(const char * t, SDL_Surface *surf, int x, int y, SDL_Color c, TTF_Font *f);
    void drawtext_cc(const char * t, SDL_Surface *surf, int x, int y, SDL_Color c, TTF_Font *f);
    SDL_Surface* rendertext(const char * t, SDL_Color c, TTF_Font *f);
    inline int drawrenderedtext(SDL_Surface *text, SDL_Surface *surf, int x, int y, int halign, int valign);
    void drawrenderedtext_cc(SDL_Surface *text, SDL_Surface *surf, int x, int y);
    void drawrenderedtext_lc(SDL_Surface *text, SDL_Surface *surf, int x, int y);
    void drawrenderedtext_r(SDL_Surface *text, SDL_Surface *surf, int x, int y);
};

#endif