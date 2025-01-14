#ifndef LEVELS_H
#define LEVELS_H
#include <stdlib.h>

#include "SDL_image.h"

#include "util.h"
#include "game.h"

class Level {
public:
    int id;
    int saved;
    char name[100];
    char map[100];
    char img[100];
    int nbframes;
    char frame[10][100];
    int framedelay;
    int nboframes;
    char oframe[10][100];
    int oframedelay;
    char bgmusic[100];
    int rescue;
    int actions[6];
};

class LevelLoader {
public:
    Level * level[50];
    int current;
    int nb;
    Game * game;
    Level * cl() { return level[current]; }
    void start();
    void next();
    void prev();
    void save();
    LevelLoader(Game *g);
};

#endif