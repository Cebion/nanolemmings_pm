#ifndef GAME_H
#define GAME_H
//#include <math.h>
//#include <stdlib.h>


#include "SDL.h"
#include "SDL_image.h"
#include "SDL_gfx/SDL_gfxPrimitives.h"

#include "util.h"
#include "sound.h"

#define MAXLEMMINGS 50000
#define MAXWATER 100000
#define MAXLAVA 100000
#define MAXWATERSOURCE 1000
#define MAXLAVASOURCE 1000
#define EXPLOSION_SIZE 25
#define FALL_DISTANCE 40
#define GAME_PLAYING 0
#define GAME_LOST 1
#define GAME_WON 2

#define MAXFRAMES 10


#define OC_LEMMING    0xFF0000
#define OC_LEMMINGW   0xFF00FF
#define OC_NOTHING    0x000000
#define OC_TARGET     0x00FF00
#define OC_WATER      0x000088
#define OC_WATER_SOURCE      0x8888FF
#define OC_REALWATER  0x0000FF
#define OC_LAVA       0xAAAA00
#define OC_LAVA_SOURCE       0xFFFF88
#define OC_REALLAVA   0xFFFF00
#define OC_WALL       0xCC9933
#define OC_HARDWALL   0x888888
#define OC_VERYHARDWALL 0xDDDDDD
#define OC_STAIR      0x880000
#define OC_STAIRT     0x890000
#define OC_STAIRMAYBE 0x8AF000
#define OC_WANTSTAIR  0x0088FF
#define OC_WANTDIG    0x448822

//   8   4   2   1   8   4   2   1
//   |               |   |
//lemming         water lava
#define C_LEMMING    ((Uint8) 0x80)
#define C_LEMMINGW   ((Uint8) 0x88)
#define C_NOTHING    ((Uint8) 0x00)
#define C_TARGET     ((Uint8) 0x10)
#define C_REALWATER  ((Uint8) 0x09)
#define C_WATER      ((Uint8) 0x08)
#define C_LAVA       ((Uint8) 0x04)
#define C_WALL       ((Uint8) 0x30)
#define C_HARDWALL   ((Uint8) 0x31)
#define C_VERYHARDWALL ((Uint8) 0x32)
#define C_STAIR      ((Uint8) 0x81)
#define C_STAIRT     ((Uint8) 0x82)
#define C_STAIRMAYBE ((Uint8) 0x83)
#define C_WANTSTAIR  ((Uint8) 0x40)
#define C_WANTDIG    ((Uint8) 0x20)
#define C_REALLAVA   ((Uint8) 0x05)
#define C_WATER_SOURCE ((Uint8) 0x0A)
#define C_LAVA_SOURCE  ((Uint8) 0x06)

#define C_NOWATER    ((Uint8) 0xC0)
#define C_NOLEMMING  ((Uint8) 0x08)
#define C_NOLAVA     ((Uint8) 0x40)

class Lemming {
public:
    Sint16 x;
    Sint16 y;
    Sint16 hp;  // normal: 1000,  falling: 900-1000,   exploding: 1-360,  dead: 0,   stair: -1,
                // [-100,-150] splashing
    Uint8 flags;

    bool dir() {return flags & 1;}
    // true = walking to the right, false = left
    bool floater() {return flags & 2;}
    bool climber() {return flags & 4;}
    bool selected() {return flags & 8;}
    bool turning() {return flags & 16;}
    void setdir(bool d) {flags &= 14; if (d) { flags |= 1;}}
    int dx() {return (dir()?1:-1);}
};

class Water {
public:
    Sint16 x;
    Sint16 y;
};

class Game {
public:
    int dx,dy;
    int action;
    int brush_size;
    int x1,x2,y1,y2;
    int action_allowed[6];
    // 0=select, 1=build, 2=dig, 3=explode, 4=climb, 5=float

    int action_performed[6];

    int speed;
    SDL_Surface *levelimg;
    SDL_Surface *levelimg_orig;
    SDL_Surface *levelmap;
    int nbframes;
    int nboframes;
    int framedelay;
    int curframe;
    int oframedelay;
    int curoframe;
    int time;
    SDL_Surface *levelimg_frame[MAXFRAMES];
    SDL_Surface *levelimg_oframe[MAXFRAMES];

    Lemming l[MAXLEMMINGS];
    Water w[MAXWATER];
    Water lava[MAXLAVA];
    Water wsource[MAXWATERSOURCE];
    Water lavasource[MAXLAVASOURCE];

    int nb;
    int nb_water;
    int nb_lava;
    int nb_watersource;
    int nb_lavasource;
    int first_dead_water;
    int first_dead_lava;
    int reached_target;
    int alive;
    int bridge;
    int digging;
    int dead;
    int selected;
    int min_reached_target;

    int died_cliff;
    int died_fall;
    int died_drown;
    int died_lava;
    int died_explode;

    bool loopx;
    bool loopy;
    bool bridge_check;

    Sound *sound;
    int step();
    void init();
    void begin(const char * mapfn, const char * imgfn);
    void select(int ix1, int iy1, int ix2, int iy2);
    void select_all();
    void explode();
    void climb();
    void floater();
    void set_action(int a);
    int set_action(int a, int b);
    void slower();
    void faster();
    Game(Sound *s);

private:
    inline unsigned int * li(int x, int y);
    inline unsigned int * lio(int x, int y);
    inline Uint8 * lm(int x, int y);
    inline Uint8 lmv(int x, int y);
    inline bool lmv_dig(int x, int y);
    inline bool lmv_air(int x, int y);
    inline bool lmv_realair(int x, int y);
    inline bool lmv_realairl(int x, int y);
    inline bool lmv_softlanding(int x, int y);
    inline bool lmv_stair(int x, int y);
    inline bool lmv_support(int x, int y);
    void support_neighbors(int x, int y);
    void paint(int x, int y);
    void check_bridges();
    void step_lemmings();
    void step_water();
    void step_lava();
    void step_sources();
};

#endif