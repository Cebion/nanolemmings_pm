#ifndef MENU_H
#define MENU_H
#include <stdlib.h>

#include "SDL.h"
#include "SDL_gfx/SDL_gfxPrimitives.h"

#include "util.h"
#include "levels.h"
#include "text.h"
#include "draw.h"
#include "game.h"

#define M_MAINMENU 0
#define M_LEVELSELECT 1
#define M_OPTIONS 2
#define M_HELP 3
#define M_CREDITS 4
#define M_GAMELOST 5
#define M_GAMEWON 6

#define MAXHELPSCREEN 5

#define GHA 0
#define GHB 1
#define GHX 2
#define GHY 3
#define GHDL 4
#define GHDR 5
#define GHDLR 6



class Menu {
public:
    LevelLoader *l;
    SDL_Surface *screen;
    SDL_Surface *lemming;
    SDL_Surface *lemmingfl;
    SDL_Surface *lemmingfli;
    SDL_Surface *twolemmings;
    SDL_Surface *twolemmingsi;
    Text * text;
    Draw * dr;
    Game * game;
    Sound * sound;
    bool playing;
    int pos;
    int intro;
    int flx;
    int fly;
    int fldx;
    int tlx;
    int tly;
    int tldx;
    int menuscreen;
    int sel;
    int helpscreen;
    SDL_Surface *guihints[8];


    Menu(LevelLoader * ll, SDL_Surface * scr, Text * t, Sound * s);
    void draw_options(SDL_Surface *surf, int yoffset);
    void draw_obstacles();
    void draw();
    void step();
    void do_action();
    void go_back();
    void input(int k);
    void change_screen(int s);
};

#endif