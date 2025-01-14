#include "game.h"


//#define MASK      0x00FFFFFF

// used on image only
#define C_DUG       0x000000
#define C_EXPLODED  0x550000
//0x00FFFF00

#define build (action==1)
#define dig (action==2)

// these terrains can be digged
inline bool Game::lmv_dig(int x, int y) {
//    if (!lm(x,y)) return false;
    unsigned int v = lmv(x,y);
    return v == C_WALL || v == C_STAIR || v == C_WANTSTAIR; // not C_STAIRT, no need
}

// what is air from the point of view of a lemming
inline bool Game::lmv_air(int x, int y) {
    unsigned int v = lmv(x,y);
    return v == C_NOTHING || v == C_TARGET || v == C_LAVA || v == C_WATER || v == C_WANTDIG || v == C_WANTSTAIR;
}
// what is air from the point of view of a water particle
inline bool Game::lmv_realair(int x, int y) {
    unsigned int v = lmv(x,y);
    return v == C_NOTHING || v == C_LEMMING || v == C_LAVA || v == C_WANTSTAIR;
}
// what is air from the point of view of a lava particle
inline bool Game::lmv_realairl(int x, int y) {
    unsigned int v = lmv(x,y);
    return v == C_NOTHING || v == C_LEMMING || v == C_WATER || v == C_WANTSTAIR;
}
inline bool Game::lmv_softlanding(int x, int y) {
    unsigned int v = lmv(x,y);
    return v == C_WATER || v == C_LEMMINGW || v == C_LEMMING;
}
inline bool Game::lmv_stair(int x, int y) {
    unsigned int v = lmv(x,y);
    return v == C_STAIR || v == C_STAIRT;
}
inline bool Game::lmv_support(int x, int y) {
//    if (!lm(x,y)) return false;
    Uint8 v = lmv(x,y);
    return v == C_WALL || v == C_STAIR || v == C_STAIRT || v == C_HARDWALL || v == C_VERYHARDWALL;
//    return v != C_NOTHING && v != C_TARGET && v != C_WANTSTAIR && v != C_WANTDIG
//        && v != C_STAIRMAYBE && v != C_LEMMING && v != C_LAVA && v != C_WATER && v != C_LEMMINGW;
//        && v != C_LEMMINGTR && v != C_LEMMINGTL && v != C_LEMMINGF;
}

inline Uint8 * Game::lm(int x, int y) {
    if (x<0) return NULL;
    if (y<0) return NULL;
    if (x>=levelmap->w) return NULL;
    if (y>=levelmap->h) return NULL;
    return (Uint8*) ((Uint8 *)levelmap->pixels + levelmap->pitch * y + x);
}

inline unsigned int * Game::li(int x, int y) {
    if (x<0) return NULL;
    if (y<0) return NULL;
    if (x>=levelimg->w) return NULL;
    if (y>=levelimg->h) return NULL;
    return (unsigned int*) ((unsigned int *)levelimg->pixels + levelimg->pitch / 4 * y + x);
}
inline unsigned int * Game::lio(int x, int y) {
    if (x<0) return NULL;
    if (y<0) return NULL;
    if (x>=levelimg_orig->w) return NULL;
    if (y>=levelimg_orig->h) return NULL;
    return (unsigned int*) ((unsigned int *)levelimg_orig->pixels + levelimg_orig->pitch / 4 * y + x);
}
inline Uint8 Game::lmv(int x, int y) {
    if (lm(x,y)) return ((*lm(x,y))); //  & MASK);
    else return C_NOTHING;
}


void Game::support_neighbors(int x, int y) {
      *(lm(x, y)) = C_STAIR;
      if (lmv(x-1,y)==C_STAIRMAYBE) support_neighbors(x-1,y);
      if (lmv(x-1,y-1)==C_STAIRMAYBE) support_neighbors(x-1,y-1);
      if (lmv(x-1,y+1)==C_STAIRMAYBE) support_neighbors(x-1,y+1);
      if (lmv(x+1,y)==C_STAIRMAYBE) support_neighbors(x+1,y);
      if (lmv(x+1,y-1)==C_STAIRMAYBE) support_neighbors(x+1,y-1);
      if (lmv(x+1,y+1)==C_STAIRMAYBE) support_neighbors(x+1,y+1);
      if (lmv(x,y+1)==C_STAIRMAYBE) support_neighbors(x,y+1);
      if (lmv(x,y-1)==C_STAIRMAYBE) support_neighbors(x,y-1);
}


void Game::check_bridges() {
    bridge_check=false;
    for (int i=0; i<nb; i++) {
      if (l[i].hp != 0) {
        if (l[i].hp == -1) {
           Sint16 &x = l[i].x;
           Sint16 &y = l[i].y;
           if (lmv(x,y) == C_WANTDIG) { // || lmv(x,y-1) == C_WANTDIG) {
             bridge_check=true;
             l[i].hp=1000;
             *(lm(x, y)) = C_LEMMING;
             *(lm(x, y-1)) = C_LEMMING;
//             l[i].terrain = 0;
             *(li(x, y)) = *(lio(x, y));
             *(li(x, y-1)) = *(lio(x, y-1));
           } else {
             *(lm(x, y)) = C_STAIRMAYBE;
             *(lm(x, y-1)) = C_STAIRMAYBE;
           }
        }
      }
    }

    for (int i=0; i<nb; i++) {
      if (l[i].hp == -1) {
        Sint16 &x = l[i].x;
        Sint16 &y = l[i].y;
        if (lmv_support(x-1,y) || lmv_support(x+1,y) ||
            lmv_support(x-1,y-1) || lmv_support(x+1,y-1) ||
            lmv_support(x-1,y-2) || lmv_support(x+1,y-2) ||
            lmv_support(x-1,y+1) || lmv_support(x+1,y+1) ||
            lmv_support(x,y+1) || lmv_support(x,y-2)) {
            support_neighbors(x,y);
        }
      }
    }
}


#define DEL_WATER(x,y)  {*(lm(x, y)) &= C_NOWATER;}
#define ADD_WATER(x,y)  {*(lm(x, y)) |= C_WATER;}
#define DEL_LAVA(x,y)  {*(lm(x, y)) &= C_NOLAVA;}
#define ADD_LAVA(x,y)  {*(lm(x, y)) |= C_LAVA;}
#define DEL_LEMMING(x,y)  {*(lm(x, y)) &= C_NOLEMMING ; *(lm(x, y-1)) &= C_NOLEMMING;}
//#define DEL_LEMMING(x,y)  {*(lm(x, y)) ^= C_LEMMING; *(lm(x, y-1)) ^= C_LEMMING;}
//#define DEL_LEMMING(x,y)  {*(lm(x, y)) = l[i].terrain >> 16; *(lm(x, y-1)) = l[i].terrain & 0x0000FFFF;}
//#define DEL_LEMMING(x,y)  {*(lm(x, y)) = C_NOTHING; *(lm(x, y-1)) = C_NOTHING;}
// *(li(x, y)) = C_NOTHING; *(li(x, y-1)) = C_NOTHING;}
// rlookup(x,y) = -1;}
#define ADD_LEMMING(x,y)  {*(lm(x, y)) |= C_LEMMING; *(lm(x, y-1)) |= C_LEMMING;}
//#define ADD_LEMMING(x,y)  {l[i].terrain = (lmv(x,y) << 16) | (lmv(x, y-1) & 0x0000FFFF) ; *(lm(x, y)) = C_LEMMING; *(lm(x, y-1)) = C_LEMMING;}
//#define ADD_LEMMING(x,y)  {l[i].terrain = (lmv(x,y) << 16) | (lmv(x, y-1) & 0x0000FFFF) ; *(lm(x, y)) = ((l[i].floater && l[i].hp==1000-FALL_DISTANCE) ? C_LEMMINGF : C_LEMMING); *(lm(x, y-1)) = (l[i].dir ? C_LEMMINGTR : C_LEMMINGTL);}
//#define ADD_LEMMING(x,y)  {*(lm(x, y)) = C_LEMMING; *(lm(x, y-1)) = C_LEMMING;}
// rlookup(x,y) = i;}
void Game::step_lemmings() {
    int building=0;
    int lemming_in=0;
    int lava_death=0;
    int fall_death=0;
    int cliff_death=0;
    int exploding=0;
    int floating_rain=0;
    for (int i=0; i<nb; i++) {
      if (l[i].hp == 0) { dead++; continue; }
      alive++;
      if (l[i].selected()) selected++;
      if (l[i].hp < -1) {
        l[i].hp--;
        if (l[i].hp < -110) l[i].hp=0;
        continue;
      }
      Sint16 &x = l[i].x;
      Sint16 &y = l[i].y;
      if (l[i].hp <= FPS*6 && l[i].hp > 0) {
        l[i].hp--;

	if (l[i].hp == 60) exploding=1;
	if (l[i].hp == EXPLOSION_SIZE) exploding=2;
        if (l[i].hp < 60) {
//	    filledCircleRGBA(levelimg,l[i].x,l[i].y,60-l[i].hp,0,0,0,255);
//	    filledCircleRGBA(levelmap,l[i].x,l[i].y,60-l[i].hp,0,0,0,255);
	    if (l[i].hp == 0) {
		died_explode++;
                bridge_check=true;
		for (int i=x-EXPLOSION_SIZE; i <= x+EXPLOSION_SIZE; i++) {
		    for (int j=y-EXPLOSION_SIZE; j <= y+EXPLOSION_SIZE; j++) {
			if (lmv(i,j) != C_VERYHARDWALL) {
			  if (lm(i,j)) *lm(i,j) = C_NOTHING;
//			if (li(i,j)) *li(i,j) = C_EXPLODED;
			  pixelRGBA(levelimg,i,j,80,20,0,150);
			  pixelRGBA(levelimg_orig,i,j,80,20,0,150);
			}
		    }
		}
	    }
	    continue;
        }
      }

      if (l[i].hp == -1) {
        bridge++;
        if (lmv(x,y) == C_STAIRMAYBE) {
           // lemming turned into stair has nothing to hang on to, falls a bit
//           *li(x,y-1) = C_NOTHING;
//           *li(x,y) = C_NOTHING;
           *(li(x, y-1)) = *(lio(x, y-1));
           *(li(x, y)) = *(lio(x, y));
           bridge_check=true;
//           DEL_LEMMING(x,y);
//           *lm(x,y-1) = C_NOTHING;
//           *lm(x,y) = C_NOTHING;
        } else {
          // produce the desired thickness of stair
          if (lmv(x,y+1) == C_WANTSTAIR && lmv(x,y+2)==C_WANTSTAIR) {
           *lm(x,y-1) = C_WANTSTAIR;
           *lm(x,y) = C_WANTSTAIR;
           *lm(x,y+1) = C_STAIRT;
           *lm(x,y+2) = C_STAIR;
//           *li(x,y-1) = C_WANTSTAIR;
//           *li(x,y) = C_WANTSTAIR;
//           *li(x,y+1) = C_NOTHING;
//           *li(x,y+2) = C_NOTHING;
//           *(li(x, y-1)) = C_WANTSTAIR;
//           *(li(x, y)) = C_WANTSTAIR;
           *(li(x, y-1)) = *(lio(x, y-1));
            pixelRGBA(levelimg,x,y-1,50,150,250,180);
           *(li(x, y)) = *(lio(x, y));
            pixelRGBA(levelimg,x,y,50,150,250,180);
           *(li(x, y+1)) = *(lio(x, y+1));
           *(li(x, y+2)) = *(lio(x, y+2));
           y+=2;
           bridge_check=true;
          } else if (lmv(x,y+1) == C_WANTSTAIR) {
           *lm(x,y-1) = C_WANTSTAIR;
           *lm(x,y) = C_STAIRT;
           *lm(x,y+1) = C_STAIR;
//           *(li(x, y-1)) = C_WANTSTAIR;
           *(li(x, y-1)) = *(lio(x, y-1));
            pixelRGBA(levelimg,x,y-1,50,150,250,180);

           *(li(x, y)) = *(lio(x, y));
           *(li(x, y+1)) = *(lio(x, y+1));
//           *li(x,y-1) = C_WANTSTAIR;
//           *li(x,y+1) = C_NOTHING;
           y+=1;
//          } else {
//           *lm(x,y-1) = C_STAIRT;
//           *lm(x,y) = C_STAIR;
           bridge_check=true;
          }
          continue;
        }
      } else if (lmv(x, y) == C_WANTSTAIR || lmv(x,y-1) == C_WANTSTAIR) {
           if (lmv(x,y-1) != C_WANTSTAIR && lmv(x,y+1) == C_WANTSTAIR) {
                 y++;
           }
           if (lmv(x,y) != C_WANTSTAIR && lmv(x,y-2) == C_WANTSTAIR) {
                 y--;
           }
           l[i].hp = -1;
           building++;
           bridge_check=true;
           if (lmv(x,y-1) == C_WANTSTAIR)*(lm(x,y-1)) = C_STAIRT;
           if (lmv(x,y) == C_WANTSTAIR) *(lm(x,y)) = C_STAIR;
           *(li(x, y-1)) = *(lio(x, y-1));
           *(li(x, y)) = *(lio(x, y));
           continue;
      }


      if (l[i].selected() && dx) l[i].setdir(dx==1);
      if (lmv_air(x, y+1) && lmv_air(x, y+2)  && (!l[i].selected() || dy > -1)
          && (!l[i].floater() || l[i].hp>1000-FALL_DISTANCE)
          && (lmv(x,y+1) != C_WATER) // || (l[i].selected && dy>0))
          && (!l[i].climber() || !(lmv_support(x+l[i].dx(),y+1) || lmv_support(x+l[i].dx(),y)
                            || lmv_support(x+l[i].dx(),y-1) || lmv_support(x+l[i].dx(),y-2)) ) ) {
        // lemming falls
        DEL_LEMMING(x,y);
        y+=2;
        if (l[i].hp > 1000-FALL_DISTANCE) l[i].hp--;
        if (l[i].hp > 1000-FALL_DISTANCE && l[i].floater()) l[i].hp--;
      } else if (lmv_air(x, y+1)
          && (lmv(x,y+1) != C_WATER || !l[i].selected() || dy>0 || (!dx && dy > -1 && rand()%100 < 90))
          && (!l[i].climber() || !(lmv_support(x+l[i].dx(),y+1) || lmv_support(x+l[i].dx(),y)
                                || lmv_support(x+l[i].dx(),y-1) || lmv_support(x+l[i].dx(),y-2))
                            || (l[i].selected() && dy > 0)
                            || (l[i].floater() && l[i].hp==1000-FALL_DISTANCE)
                            )) {
        if (l[i].selected() && dy < 0 && (rand()&255) < 200) {
	  if (dx==0) {
	     if (lmv_support(x-1,y+1) || lmv(x-1,y+1) == C_LEMMING) continue;
	     if (lmv_support(x+1,y+1) || lmv(x+1,y+1) == C_LEMMING) continue;
	  }
	  if (lmv_support(x-dx,y+1) || lmv(x-dx,y+1) == C_LEMMING) continue;
        }
        // lemming falls a bit
        DEL_LEMMING(x,y);
        if (l[i].floater() && l[i].hp==1000-FALL_DISTANCE) {
          if (lmv_air(x+l[i].dx(),y+1) && lmv_air(x+l[i].dx(),y)) {
            // floater has some horizontal movement
            x += l[i].dx();
          }
          // randomly change direction
          if (rand()%100 < 8)
          l[i].setdir(!l[i].dir());
          floating_rain++;
        }
        if (l[i].floater() && l[i].hp==1000-FALL_DISTANCE) {
           if (rand()%100 < 70) y++;
        } else if (lmv(x,y+1) == C_WATER) {
           if (rand()%100 < 40 || dy>0) y++;
           if (l[i].hp > FPS*6) l[i].hp=1000;
        } else {
           y++;
           if (l[i].hp > 1000-FALL_DISTANCE && (time&1)) l[i].hp--;
        }
      } else {
        if (l[i].hp > 1000-FALL_DISTANCE || (l[i].hp==1000-FALL_DISTANCE && (l[i].floater() || lmv_softlanding(x,y+1)))) {
            l[i].hp = 1000;
        } else if (l[i].hp == 1000-FALL_DISTANCE) {
            // lemming falls to death
            DEL_LEMMING(x,y);
            l[i].hp = -100;
            fall_death++;
            died_fall++;
            continue;
        }
        if (dy < 0 && lmv(x+dx,y-2) == C_WATER && (dx==0 || lmv_air(x+dx,y-1)) && l[i].selected()) {
           // lemming swims up
           DEL_LEMMING(x,y);
           x += dx;
           y--;
        } else
        if (lmv_air(x+l[i].dx(),y) && lmv_air(x+l[i].dx(),y-1) ) {
           // lemming walks
           DEL_LEMMING(x,y);
           x += l[i].dx();
        } else
        if (lmv_air(x+l[i].dx(),y-1) && lmv_air(x+l[i].dx(),y-2) && y>1 && (!l[i].selected() || dy < 1)) {
           // lemming climbs
           DEL_LEMMING(x,y);
           x += l[i].dx();
           y -= 1;
        } else 
        if (lmv_air(x+l[i].dx(),y-2) && lmv_air(x+l[i].dx(),y-3) && y>2 && (!l[i].selected() || dy < 1)) {
           // lemming climbs high
           DEL_LEMMING(x,y);
           x += l[i].dx();
           y -= 2;
        } else if (lmv_stair(x+l[i].dx(),l[i].y-2) && lmv_air(x+l[i].dx(),y-3)&& lmv_air(x+l[i].dx(),y-4) && y>3 && (!l[i].selected() || dy < 1)) {
           // lemming climbs stairs
           DEL_LEMMING(x,y);
           x += l[i].dx();
           y -= 3;
        } else if (lmv_stair(x+l[i].dx(),y-3) && lmv_air(x+l[i].dx(),y-4)&& lmv_air(x+l[i].dx(),y-5) && y>4 && (!l[i].selected() || dy < 1)) {
           // lemming climbs stairs
           DEL_LEMMING(x,y);
           x += l[i].dx();
           y -= 4;
        } else if (lmv_stair(x+2*l[i].dx(),l[i].y-2) && lmv_air(x+2*l[i].dx(),y-3)&& lmv_air(x+2*l[i].dx(),y-4) && y>3 && (!l[i].selected() || dy < 1)) {
           // lemming climbs through stairs
           DEL_LEMMING(x,y);
           x += 2*l[i].dx();
           y -= 3;
        } else if (l[i].climber() && lmv_air(x,y-2)) {
           // lemming climbs whatever
           DEL_LEMMING(x,y);
           y -= 1;
        } else {
           // lemming changes direction
           if (!l[i].selected() || !dx)
             l[i].setdir(!l[i].dir());
             l[i].flags |= 16;
//           if (l[i].selected && dx) l[i].dx=dx;
//           if (l[i].hp>0 && l[i].hp<100) l[i].hp++;
           continue;
        }
      }
      l[i].flags &= 15; // remove turn flag
      if (loopy) {
          if (y>=levelmap->h) y=1;
          if (y<1) y=levelmap->h-1;
      }
      if (loopx) {
          if (x<0) x=levelmap->w-1;
          if (x>=levelmap->w) x=0;
      }
      if (y >= levelmap->h || y<1 || x<0 || x >= levelmap->w) {
          // lemming died because out of map
          l[i].hp = 0;
          cliff_death++;
          if (y>=levelmap->h && lmv(x,levelmap->h-1) == C_WATER) died_drown++;
          else died_cliff++;

          continue;
      }
      if (lmv(x,y) == C_TARGET || lmv(x,y-1) == C_TARGET) {
        l[i].hp = 0;
        reached_target++;
        lemming_in++;
        continue;
      }
      if (lmv(x, y) == C_LAVA || lmv(x,y-1) == C_LAVA) {
           // died because of lava
           l[i].hp = 0;
           lava_death++;
           died_lava++;
           continue;
      }
      if ((lmv(x, y) == C_WANTSTAIR || lmv(x,y-1) == C_WANTSTAIR) && l[i].hp > 6*FPS) {
           if (lmv(x,y-1) != C_WANTSTAIR && lmv(x,y+1) == C_WANTSTAIR) {
                 y++;
           }
           if (lmv(x,y) != C_WANTSTAIR && lmv(x,y-2) == C_WANTSTAIR) {
                 y--;
           }
           l[i].hp = -1;
           building++;
           bridge_check=true;
           //if (lmv(x,y) == C_WANTSTAIR) 
           *(li(x, y)) = *(lio(x, y));
           //if (lmv(x,y-1) == C_WANTSTAIR) 
           *(li(x, y-1)) = *(lio(x, y-1));
      }
      if (l[i].hp == -1) {
//           if (li(x, y)) *(li(x, y)) = C_NOTHING;
//           if (li(x, y-1)) *(li(x, y-1)) = C_NOTHING;
           *(li(x, y-1)) = *(lio(x, y-1));
           *(li(x, y)) = *(lio(x, y));
           if (lmv(x, y-1) != C_WATER && lmv(x, y) != C_WATER) {
             *(lm(x, y-1)) = C_STAIRT;*(lm(x, y)) = C_STAIR;
             continue;
           } else {
             l[i].hp = 1000;
           }
      }
      if (lmv(x, y) == C_WANTDIG || lmv(x,y-1) == C_WANTDIG) {
//         if (lmv(x, y) == C_WANTDIG) *(li(x, y)) = *(lio(x, y)) = C_NOTHING;
//         if (lmv(x, y-1) == C_WANTDIG) *(li(x, y-1)) = *(lio(x, y-1)) = C_NOTHING;
           if (lmv(x, y) == C_WANTDIG) {
	     pixelRGBA(levelimg,x,y,0,0,0,120);
	     pixelRGBA(levelimg_orig,x,y,0,0,0,120);
	   }
           if (lmv(x, y-1) == C_WANTDIG) {
	     pixelRGBA(levelimg,x,y-1,0,0,0,120);
	     pixelRGBA(levelimg_orig,x,y-1,0,0,0,120);
	   }
           *(lm(x, y)) = C_NOTHING;
           *(lm(x, y-1)) = C_NOTHING;
           digging++;
      }
      ADD_LEMMING(x,y);
    }


    if (sound) {
       if (digging) sound->safeloopsound(1,127);
       else sound->safestoploopsound(1);
       if (building) sound->safeloopsound(2,127);
       else sound->safestoploopsound(2);
       if (sound->loopduration[2] == FPS*3 && (rand()&255) < 100) sound->playsoundchan(19,4); // bridge over troubled water
       if (lemming_in) sound->playsoundchan(3+rand()%3,0);
       if (cliff_death) sound->playsoundchan(21+rand()%3,1);
       if (fall_death) sound->playsoundchan(9+rand()%2,2);
       if (lava_death) sound->playsoundchan(11+rand()%2,3);
       if (exploding == 1) sound->playsound(0);
       if (exploding == 2) sound->playsound(18);
       if (floating_rain > 50 && (rand()&255) < 1) sound->playsoundchan(20,4); // singing in the rain
    }
}
void Game::step_water() {
    first_dead_water=nb_water;
    for (int i=0; i<nb_water; i++) {
      Sint16 &x = w[i].x;
      Sint16 &y = w[i].y;
      if (y<0) {
        if (first_dead_water==nb_water) first_dead_water=i;
        continue;
      }
      Sint16 dir=1;
      if (x<0) {x = -x; dir=-1;}
      if (lmv(x,y) == C_NOTHING) {y = -500; continue;}
      if (lmv_realair(x, y+1) && lmv_realair(x, y+2) && y < levelmap->h-2) {
        // water falls
        DEL_WATER(x,y);
        y+=2;
        if (lmv_realair(x+dir,y) && (rand()&255)<20 && x+dir >= 0 && x+dir < levelmap->w) x += dir;
      } else if (lmv_realair(x, y+1) && y < levelmap->h-1) {
        // water falls a bit
        DEL_WATER(x,y);
        y++;
      } else if (dir==1 && lmv_realair(x+1, y) && x < levelmap->w-1) {
        // water goes right
        DEL_WATER(x,y);
        x++;
        if (lmv_realair(x+1, y) && x < levelmap->w-1) x++;
      } else if (dir==-1 && lmv_realair(x-1, y) && x > 0) {
        // water goes left
        DEL_WATER(x,y);
        x--;
        if (lmv_realair(x-1, y) && x > 0) x--;
//      } else if (y>0 && lmv_realair(x, y-1) && rand()%100<30) {
        // jump up sometimes
//        DEL_WATER(x,y);
//        y--;
      } else {
        dir = dir * (-1);
        x = x*dir;
        continue;
      }
      if (lmv(x,y) == C_LAVA) {
        // water evaporates because of lava
        *(lm(x,y)) = C_NOTHING;
           // adjust image if needed
            if (*li(x,y) == OC_LAVA || *li(x,y) == OC_WATER) {
               *li(x,y) = OC_NOTHING;
               *lio(x,y) = OC_NOTHING;
            }
        y = -1000;
        continue;
      }
      // change direction randomly sometimes
      if ((rand()&255) < 4) dir = dir * (-1);

      ADD_WATER(x,y);
      x = x*dir;
    }
}
void Game::step_lava() {
    first_dead_lava=nb_lava;
    for (int i=0; i<nb_lava; i++) {
      Sint16 &x = lava[i].x;
      Sint16 &y = lava[i].y;
      if (y<0) {
        if (first_dead_lava==nb_lava) first_dead_lava=i;
        continue;
      }
      Sint16 dir=1;
      if (x<0) {x = -x; dir=-1;}
      if (lmv(x,y) == C_NOTHING) {y = -500; continue;}
//      if (lmv_realairl(x, y+1) && lmv_realairl(x, y+2) && y < levelmap->h-2) {
        // lava falls
//        DEL_LAVA(x,y);
//        y+=2;
//      } else 
      if (lmv_realairl(x, y+1) && y < levelmap->h-1) {
        // lava falls a bit
        DEL_LAVA(x,y);
        y++;
      } else if (dir==1 && lmv_realairl(x+1, y) && x < levelmap->w-1) {
        // lava goes right
        DEL_LAVA(x,y);
        x++;
      } else if (dir==-1 && lmv_realairl(x-1, y) && x > 0) {
        // lava goes left
        DEL_LAVA(x,y);
        x--;
      } else {
        dir = dir * (-1);
        x = x*dir;
        continue;
      }
      if (lmv(x,y) == C_WATER) {
        // lava gets extinguished because of water
        *(lm(x,y)) = C_NOTHING;
           // adjust image if needed
            if (*li(x,y) == OC_LAVA || *li(x,y) == OC_WATER) {
               *li(x,y) = OC_NOTHING;
               *lio(x,y) = OC_NOTHING;
            }
        y = -1000;
        continue;
      }
      // change direction randomly sometimes
      if ((rand()&255) < 10) dir = dir * (-1);

      ADD_LAVA(x,y);
      x = x*dir;
    }
}

void Game::step_sources() {
    for (int i=0; i<nb_lavasource; i++) {
      Sint16 &x = lavasource[i].x;
      Sint16 &y = lavasource[i].y;
      if (lmv(x,y+1) == C_NOTHING && first_dead_lava<MAXLAVA-1) {
          *lm(x,y) = C_LAVA;
          lava[first_dead_lava].x=x;
          lava[first_dead_lava].y=y;
          if (first_dead_lava == nb_lava) { nb_lava++; first_dead_lava++; }
          else {
            while (first_dead_lava<nb_lava) {
               first_dead_lava++;
               if (lava[first_dead_lava].y<0) break;
            }
          }
      }
    }
    for (int i=0; i<nb_watersource; i++) {
      Sint16 &x = wsource[i].x;
      Sint16 &y = wsource[i].y;
      if (lmv(x,y+1) == C_NOTHING && first_dead_water<MAXWATER-1) {
          *lm(x,y) = C_WATER;
          w[first_dead_water].x=x;
          w[first_dead_water].y=y;
          if (first_dead_water == nb_water) { nb_water++; first_dead_water++; }
          else {
            while (first_dead_water<nb_water) {
               first_dead_water++;
               if (w[first_dead_water].y<0) break;
            }
          }      }
    }
}

int Game::step() {

    time++;
    if (nbframes > 0) curframe = (time/framedelay) % nbframes;
    if (nboframes > 0) curoframe = (time/oframedelay) % nboframes;

    alive=0;
    bridge=0;
    dead=0;
    selected=0;

    digging=0;

//    if (bridge_check) printf("X"); else printf(".");
    if (bridge_check) check_bridges();
    step_lemmings();
    step_water();
    step_lava();
    step_sources();

    if (alive + reached_target >= min_reached_target) {
	if (alive>0) return GAME_PLAYING;
	if (sound) {
	  char sfn[50];
	  if (reached_target < min_reached_target+dead/10)
	       snprintf(sfn,50,"sound/won-barely.ogg");
	  else snprintf(sfn,50,"sound/won0%i.ogg",1+rand()%9);
	  sound->playsoundfile(sfn);
	}
	return GAME_WON;
    } else {
	if (sound) {
	  char sfn[50];
	  if (reached_target > min_reached_target*9/10)
	       snprintf(sfn,50,"sound/lost-close.ogg");
	  else snprintf(sfn,50,"sound/lost0%i.ogg",1+rand()%3);
	  sound->playsoundfile(sfn);
	}
	return GAME_LOST;
    }
}

void Game::paint(int x1, int y1) {
        for (int x=x1-brush_size*2-dig; x <= x1+brush_size*2; x++)
        for (int y=y1-brush_size*2-dig; y <= y1+brush_size*2; y++) {
         if (build) {
	  if (lmv(x,y)==C_WANTDIG) {
	   if (lm(x, y)) *(lm(x, y)) = C_WALL;
	   if (li(x, y)) *(li(x, y)) = *(lio(x, y));
	  } else if (lmv(x,y)==C_NOTHING || lmv(x,y)==C_LEMMING) {
	   if (lm(x, y)) *(lm(x, y)) = C_WANTSTAIR;
	   //if (li(x, y)) *(li(x, y)) = C_WANTSTAIR;
	   pixelRGBA(levelimg,x,y,50,150,250,180);
	  }
	 } else if (dig) {
	  if (lmv(x,y)==C_WANTSTAIR) {
	   if (lm(x, y)) *(lm(x, y)) = C_NOTHING;
	   if (li(x, y)) *(li(x, y)) = *(lio(x, y));
	  } else if (lmv_dig(x,y)) {
	   if (lmv(x,y)==C_STAIR) bridge_check=true;
	   if (lm(x, y)) *(lm(x, y)) = C_WANTDIG;
	   //if (li(x, y)) *(li(x, y)) = C_WANTDIG;
	   pixelRGBA(levelimg,x,y,60,120,30,180);
	  }
	 }
	}
}

#define ABS(a) (((a)<0) ? -(a) : (a))
void Game::select(int ix1, int iy1, int ix2, int iy2) {
  if (ix1>ix2) { x1=ix2; x2=ix1; }
  else { x1=ix1; x2=ix2; }
  if (iy1>iy2) { y1=iy2; y2=iy1; }
  else { y1=iy1; y2=iy2; }
  if (action>0) {
//        for (int x=x1-brush_size*2-1; x <= x2+brush_size*2; x++)
//        for (int y=y1-brush_size*2-1; y <= y2+brush_size*2; y++) paint(x,y);
		int deltax=ix2-ix1;
		int deltay=iy2-iy1;
		int sx = (deltax >= 0) ? 1 : -1;
		int sy = (deltay >= 0) ? 1 : -1;
		int ax = ABS(deltax) << 1;
		int ay = ABS(deltay) << 1;
		int x = ix1;
		int y = iy1;
		if (ax > ay) {
			int d = ay - (ax >> 1);
			while (x != ix2) {
				paint(x,y);
				if (d > 0 || (d == 0 && sx == 1)) { y += sy; d -= ax;}
				x += sx; d += ay;
			}
		} else {
			int d = ax - (ay >> 1);
			while (y != iy2) {
				paint(x,y);
				if (d > 0 || (d == 0 && sy == 1)) { x += sx; d -= ay;}
				y += sy; d += ax;
			}
		}
		paint(x,y);
        x1=x2=y1=y2=-1;
  } else if (action==0) {
    selected=0;
    for (int i=0; i<nb; i++) {
	if (l[i].hp != 0 && l[i].x >= x1 && l[i].x <= x2
	    && l[i].y >= y1 && l[i].y <= y2) {
//	    l[i].selected=1;
	    l[i].flags |= 8;
	    selected++;
	} else {
//	    l[i].selected=0;
	    l[i].flags &= 7;
	}
    }
  }
}
void Game::select_all() {
    selected=0;
    for (int i=0; i<nb; i++) {
     if (l[i].hp != 0) {
//      l[i].selected=1;
      l[i].flags |= 8;
      selected++;
     }
    }
    sound->playsound(14+rand()%4);
}

void Game::set_action(int a) {
    if (a<3 && a>=0) {
	if (action_allowed[a]) {
	    if (action == a) {
		if (a==0) select_all();
	    } else action=a;
	    if (sound) sound->playsound(14+rand()%4);
	} else {
            if (sound) sound->playsound(24+rand()%5);
	}
    }
}
int Game::set_action(int a, int b) {
/*
    if (a < 6 && action_allowed[a/2]) action=a/2;
    else if (a>8 && a <12 && b<1) brush_size=a-9;
    else if (a==12 && b==0) faster();
    else if (a==12 && b==1) slower();
    else if (a==6 && b==1) explode();
    else if (a==7 && b==1) climb();
    else if (a==8 && b==1) floater();
    */
    int action = -1;
    for (int i=0; i<13; i++) {
	if (a >= b_x[i] && a < b_x[i]+b_w[i]
	 && b >= b_y[i] && b < b_y[i]+b_h[i]) {
	    action=i;
	    break;
	}
    }
    if (action<0) return -1;
    switch (action) {
	case 0:
	case 1:
	case 2:
	    set_action(action); break;
	case 3: explode(); break;
	case 4: climb(); break;
	case 5: floater(); break;
	case 6: brush_size=0; break;
	case 7: brush_size=1; break;
	case 8: brush_size=2; break;
	case 9: slower(); break;
	case 10: faster(); break;
	case 11: return 1;
	case 12: return 2;
    }
    return 0;
}

void Game::explode() {
    // nuke-all is always allowed
    if (action_allowed[3]==0 && selected==alive) action_allowed[3]=alive;

    if (sound) {
      if (action_allowed[3]) {
        sound->playsound(14+rand()%4);
        action_performed[3]=1;
      } else sound->playsound(24+rand()%5);
    }

    for (int i=0; i<nb; i++) {
     if (l[i].hp != 0 && l[i].selected() && (l[i].hp>FPS*6 || l[i].hp==-1)) {
        if (action_allowed[3]) {
          l[i].hp = FPS*6;
          if (action_allowed[3]>0) action_allowed[3]--;
        } else return;
     }
    }
}
void Game::climb() {
    if (sound) {
      if (action_allowed[4]) {
        sound->playsound(14+rand()%4);
        action_performed[4]=1;
      } else sound->playsound(24+rand()%5);
    }
    for (int i=0; i<nb; i++) {
     if (l[i].hp != 0 && l[i].selected() && l[i].hp>FPS*6 && !l[i].climber()) {
        if (action_allowed[4]) {
//          l[i].climber=true;
	    l[i].flags |= 4;
          if (action_allowed[4]>0) action_allowed[4]--;
        } else return;
     }
    }
}
void Game::floater() {
    if (sound) {
      if (action_allowed[5]) {
        sound->playsound(14+rand()%4);
        action_performed[5]=1;
      } else sound->playsound(24+rand()%5);
    }
    for (int i=0; i<nb; i++) {
     if (l[i].hp != 0 && l[i].selected() && l[i].hp>FPS*6 && !l[i].floater()) {
        if (action_allowed[5]) {
//          l[i].floater=true;
	    l[i].flags |= 2;
          if (action_allowed[5]>0) action_allowed[5]--;
        } else return;
     }
    }
}

void Game::slower() {
    if (speed > 1) {
       speed /= 2;
       if (sound) sound->playsound(14+rand()%4);
    } else {
       if (sound) sound->playsound(24+rand()%5);
    }
}
void Game::faster() {
    if (speed < 32) {
       speed *= 2;
       if (sound) sound->playsound(14+rand()%4);
    } else {
       if (sound) sound->playsound(24+rand()%5);
    }
}
void Game::init() {
    nb=0;
    nb_water=0;
    nb_lava=0;
    nb_watersource=0;
    nb_lavasource=0;
    loopx=loopy=false;
    bridge_check=true;
    reached_target=0;
    x1=x2=y1=y2=-1;
    dx=dy=action=0;
    brush_size=1;
    alive=bridge=dead=0;
    died_cliff = died_fall = died_drown = died_lava = died_explode = 0;

    action=-1;
    speed = 8;
    action_allowed[0]=true;
    action_allowed[1]=true;
    action_allowed[2]=true;
    for (int i=0; i<6; i++) action_performed[i]=0;

    time=0;
    curframe=0;
    nbframes=0;
    curoframe=0;
    nboframes=0;
    for (Sint16 y=0; y<levelmap->h; y++) {
      for (Sint16 x=0; x<levelmap->w; x++) {
//        printf("[%x]",lmv(x,y));
//         if (lmv(x,y) == C_TARGET) {
//            printf("target at %i,%i (%x)\n",x,y,lmv(x,y));
//         }
         if (lmv(x,y) == C_LEMMING) {
//            printf("lemming at %i,%i (%x)\n",x,y,lmv(x,y));
            l[nb].hp=1000;
            l[nb].x=x;
            l[nb].y=y;
            l[nb].flags=8+rand()%2; // selected and random direction
//            l[nb].dir= (rand()%2==0);
//            l[nb].selected=true;
//            l[nb].floater=false;
//            l[nb].climber=false;
//            l[nb].terrain=0;

            // if the map and the img are the same, we don't want a red background where
            // the lemmings started...
            if (*li(x,y) == OC_LEMMING) {
               *li(x,y) = OC_NOTHING;
               *lio(x,y) = OC_NOTHING;
            }
            nb++;
            alive++;
            selected++;
            if (nb>=MAXLEMMINGS) throw Exiter("Too many lemmings!");
         } else if (lmv(x,y) == C_REALWATER) {
            w[nb_water].x=x;
            w[nb_water].y=y;
            *lm(x,y) = C_WATER;
            nb_water++;
            // if the map and the img are the same, we don't want a blue background where
            // the water started...
            if (*li(x,y) == OC_REALWATER) {
               *li(x,y) = OC_NOTHING;
               *lio(x,y) = OC_NOTHING;
            }
            if (nb_water>=MAXWATER) throw Exiter("Too much real water!");
         } else if (lmv(x,y) == C_WATER_SOURCE) {
            wsource[nb_watersource].x=x;
            wsource[nb_watersource].y=y;
            *lm(x,y) = C_WATER;
            nb_watersource++;
            if (nb_watersource>=MAXWATERSOURCE) throw Exiter("Too many water sources!");
         } else if (lmv(x,y) == C_REALLAVA) {
            lava[nb_lava].x=x;
            lava[nb_lava].y=y;
            *lm(x,y) = C_LAVA;
            nb_lava++;
            // if the map and the img are the same, we don't want a yellow background where
            // the lava started...
            if (*li(x,y) == OC_REALLAVA) {
               *li(x,y) = OC_NOTHING;
               *lio(x,y) = OC_NOTHING;
            }
            if (nb_lava>=MAXLAVA) throw Exiter("Too much real lava!");
         } else if (lmv(x,y) == C_LAVA_SOURCE) {
            lavasource[nb_lavasource].x=x;
            lavasource[nb_lavasource].y=y;
            *lm(x,y) = C_LAVA;
            nb_lavasource++;
            if (nb_lavasource>=MAXLAVASOURCE) throw Exiter("Too many lava sources!");
         }
      }
    }
    printf("%i lemmings initialized\n",nb);
    if (nb_water>0) printf("%i water particles initialized\n",nb_water);
    if (nb_lava>0) printf("%i lava particles initialized\n",nb_lava);
    if (sound) {
	char sfn[50];
	snprintf(sfn,50,"sound/begin0%i.ogg",1+rand()%4);
	sound->playsoundfile(sfn);
    }
}

void Game::begin(const char * mapfn, const char * imgfn) {
    SDL_Surface *img, *img2;
    img2=IMG_Load(mapfn);
    if (!img2) {
	printf("Could not load image \"%s\"\n",mapfn);
	throw Exiter("Level map not found");
    }
    if (levelmap) SDL_FreeSurface(levelmap);
//    levelmap=SDL_DisplayFormat(img);
    img=SDL_DisplayFormat(img2);
    SDL_FreeSurface(img2);
    levelmap= SDL_AllocSurface(SDL_SWSURFACE,
        img->w, img->h, 8,0,0,0,0);
    SDL_Color colors[256];
    for (int i=0; i<256; i++) {
	colors[i].r=(i & C_LEMMING);
	colors[i].g=i;
	colors[i].b=(i & C_WATER)<<4;
    }
    SDL_SetColors(levelmap, colors, 0, 256);
    Uint8 *dp;
    unsigned int *sp;
    int width = img->w;
    for (int y = 0; y < img->h; y++) {
	dp = (Uint8*) ((Uint8 *) levelmap->pixels + levelmap->pitch * y);
	sp = (unsigned int *) ( (unsigned int *) img->pixels + img->pitch / 4 * y);
	for (int x = 0; x < width; x++) {
	    switch (sp[x]) {
		case OC_LEMMING: dp[x] = C_LEMMING; break;
		case OC_LEMMINGW: dp[x] = C_LEMMINGW; break;
		case OC_NOTHING: dp[x] = C_NOTHING; break;
		case OC_TARGET: dp[x] = C_TARGET; break;
		case OC_WATER: dp[x] = C_WATER; break;
		case OC_WATER_SOURCE: dp[x] = C_WATER_SOURCE; break;
		case OC_REALWATER: dp[x] = C_REALWATER; break;
		case OC_LAVA: dp[x] = C_LAVA; break;
		case OC_LAVA_SOURCE: dp[x] = C_LAVA_SOURCE; break;
		case OC_REALLAVA: dp[x] = C_REALLAVA; break;
		case OC_WALL: dp[x] = C_WALL; break;
		case OC_VERYHARDWALL: dp[x] = C_VERYHARDWALL; break;
		case OC_WANTSTAIR: dp[x] = C_WANTSTAIR; break;
		default: dp[x] = C_HARDWALL; break;
	    }
	}
    }
    
    SDL_FreeSurface(img);
    img=IMG_Load(imgfn);
    if (!img) {
	printf("Could not load image \"%s\"\n",imgfn);
	throw Exiter("Level image not found");
    }
    if (levelimg) SDL_FreeSurface(levelimg);
    levelimg=SDL_DisplayFormat(img);
    if (levelimg_orig) SDL_FreeSurface(levelimg_orig);
    levelimg_orig=SDL_DisplayFormat(img);
    SDL_FreeSurface(img);
    init();
}

Game::Game(Sound *s) {
    sound = s;
    printf("Memory per lemming: %i bytes\n",sizeof(Lemming));
    printf("Memory per water particle: %i bytes\n",sizeof(Water));
    levelmap=NULL;
    levelimg=NULL;
    levelimg_orig=NULL;
    for (int i=0; i<MAXFRAMES; i++) levelimg_frame[i]=NULL;
    for (int i=0; i<MAXFRAMES; i++) levelimg_oframe[i]=NULL;
/*    SDL_Surface *img;
    img=IMG_Load("maps/level01-map.png");
//    img=IMG_Load("maps/tutorial/level01.png");
    if (!img) throw Exiter("Map not found");
    levelmap=SDL_DisplayFormat(img);
    SDL_FreeSurface(img);
    img=IMG_Load("maps/level01-img.png");
//    img=IMG_Load("maps/tutorial/level01.png");
    if (!img) throw Exiter("Map not found");
    levelimg=SDL_DisplayFormat(img);
    SDL_FreeSurface(img);
    init();
*/
}
