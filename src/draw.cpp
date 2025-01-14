#include "draw.h"

const SDL_Color black2 = {30,20,10};
const SDL_Color black = {0,0,100};
const SDL_Color white2 = {255,250,240};
const SDL_Color white = {150,200,250};
const SDL_Color realwhite = {250,200,250};
const SDL_Color countercolor = {250,230,150};


inline void myfastPixelColor(SDL_Surface * dst, Sint16 x, Sint16 y, Uint32 color) {
	Uint8 *p;
	p = (Uint8 *) dst->pixels + y * dst->pitch + x * 4;
	*(Uint32 *) p = color;
}
inline void my_putPixelAlpha(SDL_Surface *dst, Sint16 x, Sint16 y, Uint32 color, Uint8 alpha) {
	Uint32 R, G, B;
	Uint32 *pixel = (Uint32 *) dst->pixels + y * dst->pitch / 4 + x;
	Uint32 dc = *pixel;

	const Uint32 Rmask = 16711680;
	const Uint32 Gmask = 65280;
	const Uint32 Bmask = 255;
	const Uint32 Rshift = 16;
	const Uint32 Gshift = 8;
	const Uint32 Bshift = 0;

	R = ((dc & Rmask) + (((((color & Rmask) - (dc & Rmask)) >> Rshift) * alpha >> 8) << Rshift)) & Rmask;
	G = ((dc & Gmask) + (((((color & Gmask) - (dc & Gmask)) >> Gshift) * alpha >> 8) << Gshift)) & Gmask;
	B = ((dc & Bmask) + (((((color & Bmask) - (dc & Bmask)) >> Bshift) * alpha >> 8) << Bshift)) & Bmask;
	*pixel = R | G | B;
}

// fast pixel drawing, hardcoded for omapfb pixel format (XRGB)
inline void mypixelRGBA(SDL_Surface * dst, Sint16 x, Sint16 y, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	Uint32 color = ((Uint32) r << 16) | ((Uint32) g << 8) | ((Uint32) b);
	if (a == 255) {
		myfastPixelColor(dst, x, y, color);
	} else {
		my_putPixelAlpha(dst, x, y, color, a);
	}
}



void Draw::draw_digit(int nb, int color, int x, int y) {
    SDL_Rect re;
    re.x = nb*4*scale;
    re.y = color*5*scale;
    re.w = ((nb==10 || color==3)?8:3)*scale;
    re.h = 5*scale;
    SDL_Rect tre;
    tre.x = x;
    tre.y = y;
//    printf("Digit %i from %i,%i (%ix%i) to %i,%i at scale %i\n",nb,re.x,re.y,re.w,re.h,tre.x,tre.y,scale);
    SDL_BlitSurface(numbers[scale-1],&re,screen,&tre);
}
void Draw::draw_number(int nb, int color, int x, int y) {
    if (nb < 0) {
	draw_digit(10,color,x-11*scale,y);
	return;
    }
    // move large numbers a bit
    if (nb>9999) x += 2*scale;
    int digits=0;
    do {
	x -= 4*scale;
	draw_digit(nb % 10, color, x, y);
	digits++;
	nb /= 10;
    } while (digits<10 && nb > 0);
}

void Draw::draw_bare() {
    clip(xoffset,0,game->levelimg->w-SCREENWIDTH);
    clip(yoffset,0,game->levelimg->h-SCREENHEIGHT+BARHEIGHT);
    SDL_Rect re;
    re.x = xoffset;
    re.y = yoffset;
    re.w = SCREENWIDTH;
    re.h = SCREENHEIGHT-BARHEIGHT;
    xo=-xoffset;
    yo=-yoffset;
    SDL_Rect centerre;
    centerre.x=0;
    centerre.y=0;
    if (re.w > game->levelimg->w) {
      centerre.x = (re.w-game->levelimg->w)/2;
      xo += centerre.x;
      re.w = game->levelimg->w;
    }
    if (re.h > game->levelimg->h) {
      centerre.y = (re.h-game->levelimg->h)/2;
      yo += centerre.y;
      re.h = game->levelimg->h;
    }
    if (centerre.x || centerre.y) {
      SDL_FillRect(screen,NULL,0);
    }


//    SDL_BlitSurface(game->levelmap,&re,screen,&centerre);
//    if (cursor) SDL_BlitSurface(game->levelmap,&re,screen,&centerre); else
    SDL_BlitSurface(game->levelimg,&re,screen,&centerre);
    if (game->nbframes > 0)
    SDL_BlitSurface(game->levelimg_frame[game->curframe],&re,screen,&centerre);


//    int ymax=yoffset+re.h;
//    re.h=1;
//    for (int y=yoffset; y<ymax; y++) {
/*     SDL_BlitSurface(game->levelimg,&re,screen,&centerre);
     if (game->nbframes > 0)
     SDL_BlitSurface(game->levelimg_frame[game->curframe],&re,screen,&centerre);
     centerre.y++;
     re.y++;*/

/*     Uint32 *tp = ((Uint32 *)game->levelmap->pixels + game->levelmap->pitch / 4 * y);
     for (int x=xoffset; x<xoffset+re.w; x++) {
       switch(tp[x]) {
         case C_WANTSTAIR: mypixelRGBA(screen,xo+x,yo+y,  0,127,255,200); break;
         case C_STAIR: mypixelRGBA(screen,xo+x,yo+y,  170,130,20,255); break;
         case C_STAIRT: mypixelRGBA(screen,xo+x,yo+y,  0,179,0,255); break;
         case C_WANTDIG: mypixelRGBA(screen,xo+x,yo+y,  63,127,31,200); break; */
/*         case C_LEMMING:
         case C_LEMMINGF:
          mypixelRGBA(screen,xo+x,yo+y,  168,68,237,255);
          if (tp[x] == C_LEMMINGF && yo+y>2) {
		if (x>0) mypixelRGBA(screen,xo+x-1,yo+y-3,255,0,255,255);
		mypixelRGBA(screen,xo+x,yo+y-3,0,255,255,255);
		if (x+1<SCREENWIDTH) mypixelRGBA(screen,xo+x+1,yo+y-3,255,255,0,255);
	  }
	  break;
         case C_LEMMINGTL:
         case C_LEMMINGTR:
          mypixelRGBA(screen,xo+x,yo+y,  255, 235,223,255);
//	  if (yo+y>0) mypixelRGBA(screen,xo+x,yo+y-1,  0,179,0,255);
	  if (tp[x] == C_LEMMINGTL && x+xo+1 < SCREENWIDTH) {
	    mypixelRGBA(screen,x+xo+1,yo+y,  0,179,0,200);
	    if (y>0) mypixelRGBA(screen,x+xo+1,yo+y-1,  0,179,0,100);
	  } else if (tp[x] == C_LEMMINGTR && x+xo-1 >= 0) {
	    mypixelRGBA(screen,x+xo-1,yo+y,  0,179,0,200);
	    if (y>0) mypixelRGBA(screen,x+xo-1,yo+y-1,  0,179,0,100);
	  }
	  break;
	  */
//      }
//     }
//    }

//    if (!cursor)
    for (int i=0; i<game->nb_water; i++) {
	int ax=abs(game->w[i].x);
        int x=xo+ax;
        int y=yo+game->w[i].y;
        if (!(x >= 0 && game->w[i].y >= 0 && y>0 && x < SCREENWIDTH && y < SCREENHEIGHT)) continue;
//        boxRGBA(screen,x-1,y-1,x+1,y,100,100+rand()%100,200+rand()%50,150);
	int v = (32+abs(128-((ax+2*game->w[i].y+game->time) & 255)));
//	v = v/2 + (rand()%v)/2;
	v &= rand();
//        boxRGBA(screen,x-1,y-1,x+1,y,50+rand()%v,100+v,200+rand()%56,100);

//        boxRGBA(screen,x-1,y-1,x+1,y,2*v/3,v,230,100);
        mypixelRGBA(screen,x,y,2*v/3,v,210+v/4,255);
        if (x>0) mypixelRGBA(screen,x-1,y,2*v/3,v,230,120);
        if (y>0) mypixelRGBA(screen,x,y-1,2*v/3,v,230,120);
        if (x+1<SCREENWIDTH)mypixelRGBA(screen,x+1,y,2*v/3,v,230,120);
        if (y+1<SCREENHEIGHT) mypixelRGBA(screen,x,y+1,2*v/3,v,230,120);
/*        mypixelRGBA(screen,x,y,v,50+v,240,200);
        if (y>0) mypixelRGBA(screen,x,y-1,v,50+v,240,100);
        if (x>0) mypixelRGBA(screen,x-1,y,v,50+v,240,100);
        if (x<SCREENWIDTH-1) mypixelRGBA(screen,x+1,y,v,50+v,240,100);*/

//        mypixelRGBA(screen,x,y,50+rand()%v,100+v,200+rand()%56,255);
//        mypixelRGBA(screen,x,y,100,100+rand()%100,200+rand()%50,255);
    }
//    if (!cursor)
    for (int i=0; i<game->nb_lava; i++) {
	int ax=abs(game->lava[i].x);
        int x=xo+ax;
        int y=yo+game->lava[i].y;
        if (!(x >= 0 && game->lava[i].y >= 0 && y>0 && x < SCREENWIDTH && y < SCREENHEIGHT)) continue;
	int v = (64+abs(128-((5000-game->lava[i].y+2*game->time)&255))/2);
	v &= rand();
//        boxRGBA(screen,x-1,y-1,x+1,y+1,105+v,50+(rand()%v),v/3,100);
        mypixelRGBA(screen,x,y,180+v/2,40+v,v/3,255);
        if (y>0)mypixelRGBA(screen,x,y-1,180+v/2,40+v,v/3,255);
    }
//    if (!cursor)
    for (int i=0; i<game->nb; i++) {

//        if (game->l[i].hp > FPS*6) continue;
//        if (game->l[i].hp == -1) continue;
        int x=xo+game->l[i].x;
        int y=yo+game->l[i].y;
        if (!(x >= 0 && y > 0 && x < SCREENWIDTH && y < SCREENHEIGHT && game->l[i].hp != 0)) continue;
/*        if (!game->l[i].selected) {
          mypixelRGBA(screen,x,y-1,0,0,0,100);
          mypixelRGBA(screen,x,y,0,0,0,100);
        }
        */
        if (game->l[i].hp>0) {
//	  boxRGBA(screen,xo+game->l[i].x-1,yo+game->l[i].y-1,xo+game->l[i].x+1,yo+game->l[i].y+1,100+100*game->l[i].selected,50,200-game->l[i].hp*2,100);
//	  pixelRGBA(screen,xo+game->l[i].x,yo+game->l[i].y,250*game->l[i].selected,100,200-game->l[i].hp*2,255);
	  // normal lemming
	  // body
	  mypixelRGBA(screen,x,y-1,255*game->l[i].selected(), 235,223,255);
	  if ((game->time & 1)) {
	  mypixelRGBA(screen,x,y,  168-100*game->l[i].selected(),68+100*game->l[i].climber(),237,255);
	  } else {
	  if (x>0) mypixelRGBA(screen,x-1,y,  168-100*game->l[i].selected(),68+100*game->l[i].climber(),237,200);
	  if (x+1<SCREENWIDTH) mypixelRGBA(screen,x+1,y,  168-100*game->l[i].selected(),68+100*game->l[i].climber(),237,200);
	  }
	  // hair
	  if (game->l[i].floater() && game->l[i].hp != 1000-FALL_DISTANCE) {
	    if (y>1) mypixelRGBA(screen,x,y-2,  219,239,0,255);
	    if (!game->l[i].turning()) {
	      if (x-game->l[i].dx() >= 0 && x-game->l[i].dx() < SCREENWIDTH) {
	      mypixelRGBA(screen,x-game->l[i].dx(),y-1,  0,239,219,220);
	      if (y>1) mypixelRGBA(screen,x-game->l[i].dx(),y-2,  219,40,219,200);
	      }
	    } else {
//	      if (x>0) mypixelRGBA(screen,x-1,y-1,  40,239,219,170);
//	      if (x<SCREENWIDTH-1) mypixelRGBA(screen,x+1,y-1,  219,40,219,170);
	    }
	  } else {
	    if (y>1) mypixelRGBA(screen,x,y-2,  0,179,0,255);
	    if (!game->l[i].turning()) {
	      if (x-game->l[i].dx() >= 0 && x-game->l[i].dx() < SCREENWIDTH) {
	       mypixelRGBA(screen,x-game->l[i].dx(),y-1,  0,179,0,200);
	       if (y>1) mypixelRGBA(screen,x-game->l[i].dx(),y-2,  0,179,0,100);
	      }
	    } else {
//	      if (x>0) mypixelRGBA(screen,x-1,y-1,  0,179,0,150);
//	      if (x<SCREENWIDTH-1) mypixelRGBA(screen,x+1,y-1,  0,179,0,150);
	    }
	  }


	  if (game->l[i].hp < FPS*6) {
	    if (game->l[i].hp > FPS) { // && game->l[i].x % 8 < 3 && game->l[i].y % 8 < 3)
		boxRGBA(screen,x-2,y-10,x+2,y-4,0,0,0,100);
		int oldscale=scale;
		scale=1;
		draw_digit(game->l[i].hp/FPS,0,x-1,y-9);
		scale=oldscale;
//		char msg[5];
//		snprintf(msg,5,"%i",game->l[i].hp/FPS);
//		text->drawtext_cache(msg,screen,x,y-4,countercolor,text->tiny,A_CENTER,A_BOTTOM, 0);
	    }
	    if (game->l[i].hp < EXPLOSION_SIZE) {
	        //filledCircleRGBA(screen,x,y,40-game->l[i].hp/2,255,200-game->l[i].hp*3,0,180);
	        int br=EXPLOSION_SIZE-game->l[i].hp;
	        boxRGBA(screen,x-br,y-br,x+br,y+br,
	                       255,200-game->l[i].hp*3,0,100-game->l[i].hp*2);
	    }
	  } else if (game->l[i].hp == 1000-FALL_DISTANCE) {
	      if (game->l[i].floater() && y>2) {
		if (x>0) mypixelRGBA(screen,x-1,y-3,255,0,255,255);
		mypixelRGBA(screen,x,y-3,0,255,255,255);
		if (x+1<SCREENWIDTH) mypixelRGBA(screen,x+1,y-3,255,255,0,255);
	      }
	  }
	} else if (game->l[i].hp==-1) {
	    // lemming is being a bridge/stair
	  mypixelRGBA(screen,x,y-1,0,179,0,255);
	  mypixelRGBA(screen,x,y,  170,130,20,255);
	} else if (game->l[i].hp < -1) {
	    // splatter
	    int dropsize= -(100+game->l[i].hp); // from 0 to 10
	    filledCircleRGBA(screen,x+(rand()%(2*dropsize+1))-dropsize,y-(rand()%(dropsize+1)),1+dropsize/2,230,0,0,180-dropsize*10);
	}
    }
/*    if (SDL_MUSTLOCK(screen)) {
	SDL_UnlockSurface(screen);
    }*/
    if (game->nboframes > 0)
    SDL_BlitSurface(game->levelimg_oframe[game->curoframe],&re,screen,&centerre);

    if (game->x1 >= 0) {
	boxRGBA(screen,xo+game->x1,yo+game->y1,xo+game->x2,yo+game->y2,255,255,150,150);
	rectangleRGBA(screen,xo+game->x1,yo+game->y1,xo+game->x2,yo+game->y2,255,255,0,255);
    }
}
void Draw::draw() {
    draw_bare();

    clip(cx,0,SCREENWIDTH);
    clip(cy,0,SCREENHEIGHT);
    if (cursor) {
	if (game->action > 0) {
	    rectangleRGBA(screen,cx-game->brush_size*2-(game->action==1?1:2),cy-game->brush_size*2-(game->action==1?1:2),cx+game->brush_size*2+1,cy+game->brush_size*2+1, 255,255,0,150);
	} else {
	    boxRGBA(screen,cx-1,cy-10,cx+1,cy+10, 255,255,0,150);
	    boxRGBA(screen,cx-10,cy-1,cx+10,cy+1, 255,255,0,150);
	}
    }

    SDL_Rect re;
    re.x = 0;
    re.y = SCREENHEIGHT-BARHEIGHT;
    if (bar[scale-1]) SDL_BlitSurface(bar[scale-1],NULL,screen,&re);
//    boxRGBA(screen,0,re.y-1*scale,SCREENWIDTH-1,re.y-1, 0,0,0,127);
    boxRGBA(screen,0,re.y-1,SCREENWIDTH-1,re.y-1, 255,255,255,127);
    int action=game->action;
    if (action >= 0) {
//	boxRGBA(screen, action*BARHEIGHT, re.y, (action+1)*BARHEIGHT-1, SCREENHEIGHT-1, 255,255,100,150);
//            boxRGBA(screen, (b_x[action]+1)*scale,(b_y[action]+1)*scale+SCREENHEIGHT-BARHEIGHT, (b_x[action]+b_w[action]-1)*scale-1,(b_y[action]+b_h[action]-1)*scale-1+SCREENHEIGHT-BARHEIGHT, 255,255,100,150);
	SDL_Rect sre, dre;
	dre.x = (b_x[action])*scale;
	dre.y = (b_y[action])*scale + SCREENHEIGHT-BARHEIGHT;
	int aframe=(game->time/10)%7;
	sre.x = b_w[action]*scale*aframe;
	sre.y = b_h[action]*scale*action;
	sre.w = b_w[action]*scale;
	sre.h = b_h[action]*scale;
	SDL_BlitSurface(animbar[scale-1],&sre,screen,&dre);
    }
    for (int action=3; action<6; action++) {
	if (game->action_performed[action]>0) {
	    SDL_Rect sre, dre;
	    dre.x = (b_x[action])*scale;
	    dre.y = (b_y[action])*scale + SCREENHEIGHT-BARHEIGHT;
	    int aframe=(game->action_performed[action]/10)%7;
	    sre.x = b_w[action]*scale*aframe;
	    sre.y = b_h[action]*scale*action;
	    sre.w = b_w[action]*scale;
	    sre.h = b_h[action]*scale;
	    SDL_BlitSurface(animbar[scale-1],&sre,screen,&dre);
	    if (++game->action_performed[action] >= 70) game->action_performed[action]=0;
	}
    }
    for (action=0; action<3; action++) {
	if (!game->action_allowed[action]) {
//	    boxRGBA(screen, action*BARHEIGHT, re.y, (action+1)*BARHEIGHT-1, SCREENHEIGHT-1, 0,0,0,255);
            boxRGBA(screen, (b_x[action])*scale,(b_y[action])*scale+SCREENHEIGHT-BARHEIGHT, (b_x[action]+b_w[action])*scale,(b_y[action]+b_h[action])*scale-1+SCREENHEIGHT-BARHEIGHT, 0,0,0,150);
	}
    }
    if (game->action_allowed[1] || game->action_allowed[2]) {
      //action=game->brush_size+9;
      //boxRGBA(screen, action*BARHEIGHT/2, re.y, (action+1)*BARHEIGHT/2-1, re.y+10*scale-1, 255,255,100,150);
      action = game->brush_size+6;
            boxRGBA(screen, (b_x[action]+1)*scale,(b_y[action]+1)*scale+SCREENHEIGHT-BARHEIGHT, (b_x[action]+b_w[action]-1)*scale-1,(b_y[action]+b_h[action]-1)*scale-1+SCREENHEIGHT-BARHEIGHT, 255,255,100,150);
//      boxRGBA(screen, (b_x[action])*scale,(b_y[action])*scale-1+SCREENHEIGHT-BARHEIGHT, (b_x[action]+b_w[action]-1)*scale,(b_y[action]+b_h[action]-1)*scale-1+SCREENHEIGHT-BARHEIGHT, 255,255,100,150);
    } else {
      boxRGBA(screen, (b_x[6])*scale,(b_y[6])*scale-1+SCREENHEIGHT-BARHEIGHT, (b_x[8]+b_w[8])*scale,(b_y[8]+b_h[8])*scale-1+SCREENHEIGHT-BARHEIGHT, 0,0,0,150);
//      boxRGBA(screen, 6*BARHEIGHT/2, re.y, 9*BARHEIGHT/2-1, re.y+10*scale-1, 0,0,0,255);
    }
//    char msg[100];
    for (action=3; action<6; action++) {
	if (!game->action_allowed[action] && (action != 3 || game->alive != game->selected)) {
//            boxRGBA(screen, (3+action)*BARHEIGHT/2, re.y+10*scale, (action+4)*BARHEIGHT/2-1, re.y+20*scale-1, 0,0,0,255);
            boxRGBA(screen, (b_x[action])*scale,(b_y[action])*scale-1+SCREENHEIGHT-BARHEIGHT, (b_x[action]+b_w[action])*scale,(b_y[action]+b_h[action])*scale-1+SCREENHEIGHT-BARHEIGHT, 0,0,0,155);
	} else { //if (game->action_allowed[action]>0) {
	/*
	    if (game->action_allowed[action]>999)
	    snprintf(msg,100,"%iK",game->action_allowed[action]/1000);
	    else
	    snprintf(msg,100,"%i",game->action_allowed[action]);
	    
	    text->drawtext_cache(msg,screen,scale+(3+action)*BARHEIGHT/2+BARHEIGHT/4,SCREENHEIGHT-BARHEIGHT/2,black,text->verysmall,A_CENTER,A_BOTTOM, 4+action*2);
	    text->drawtext_cache(msg,screen,scale+(3+action)*BARHEIGHT/2+BARHEIGHT/4-1,SCREENHEIGHT-BARHEIGHT/2-1,realwhite,text->verysmall,A_CENTER,A_BOTTOM, 5+action*2);
	*/
	    if (game->action_allowed[action])
	    draw_number(game->action_allowed[action],1,(b_x[action]+b_w[action]-1)*scale,(b_y[action]-6)*scale-1+SCREENHEIGHT-BARHEIGHT);
	
	}
    }

    //snprintf(msg,100,"%i%%",game->speed*25/2);
/*    if (game->speed>=8)
      snprintf(msg,100,"x%i",game->speed/8);
    else
      snprintf(msg,100,"x1/%i",8/game->speed);
//    text->drawtext_cache(msg,screen,119*scale,SCREENHEIGHT-5*scale,white,text->small,A_RIGHT,A_CENTER, 3);
    text->drawtext_cache(msg,screen,(b_x[9]+b_w[9])*scale,b_y[9]*scale+SCREENHEIGHT-BARHEIGHT,white,text->small,A_LEFT,A_TOP, 3);
    */
    int speed=0;
    if (game->speed == 16) speed = 2;
    else if (game->speed == 32) speed = 4;
    else if (game->speed == 4) speed = 6;
    else if (game->speed == 2) speed = 8;
    else if (game->speed == 1) speed = 10;

    draw_digit(speed,3,(b_x[9]+b_w[9]+2)*scale,(b_y[9]+2)*scale+SCREENHEIGHT-BARHEIGHT);
    /*
    snprintf(msg,100,"%i/%i",game->selected,game->alive);
    text->drawtext_cache(msg,screen,SCREENWIDTH-12*scale,SCREENHEIGHT-15*scale,white,text->small,A_RIGHT,A_CENTER, 1);
    snprintf(msg,100,"in: %i/%i",game->reached_target,game->min_reached_target);
    text->drawtext_cache(msg,screen,SCREENWIDTH-12*scale,SCREENHEIGHT-5*scale,white,text->small,A_RIGHT,A_CENTER, 2);
    */
    action=0;
//    if (game->selected != game->alive)
    draw_number(game->selected,0,(b_x[action]+b_w[action]-1)*scale,(b_y[action]-6)*scale+SCREENHEIGHT-BARHEIGHT);
    action=1;
    if (game->bridge > 0)
    draw_number(game->bridge,0,(b_x[action]+b_w[action]-1)*scale,(b_y[action]-6)*scale+SCREENHEIGHT-BARHEIGHT);
    action=2;
    if (game->digging > 0)
    draw_number(game->digging,0,(b_x[action]+b_w[action]-1)*scale,(b_y[action]-6)*scale+SCREENHEIGHT-BARHEIGHT);
    draw_number(game->alive,2,SCREENWIDTH-1*scale,SCREENHEIGHT-BARHEIGHT+1*scale);
    draw_number(game->reached_target,2,SCREENWIDTH-1*scale,SCREENHEIGHT-BARHEIGHT+8*scale);
    draw_number(game->min_reached_target,2,SCREENWIDTH-1*scale,SCREENHEIGHT-BARHEIGHT+15*scale);
}


void Draw::draw_stats() {
    char msg[100];
    char msg2[100];
    if (game->reached_target >= game->min_reached_target) {
	if (game->reached_target == game->nb) {
          snprintf(msg,100,"Perfect!");
          text->drawtext_cache(msg,screen,430,70,black2,text->large,A_CENTER,A_CENTER, 4);
	} else if (game->reached_target < game->min_reached_target+(game->nb-game->min_reached_target)/8) {
          snprintf(msg,100,"Close"); snprintf(msg2,100,"one!");
          text->drawtext_cache(msg,screen,430,55,black2,text->big,A_CENTER,A_CENTER, 4);
          text->drawtext_cache(msg2,screen,430,90,black2,text->big,A_CENTER,A_CENTER, 5);
	} else if (game->reached_target < game->min_reached_target+(game->nb-game->min_reached_target)/4) {
          snprintf(msg,100,"Well"); snprintf(msg2,100,"done!");
          text->drawtext_cache(msg,screen,430,55,black2,text->big,A_CENTER,A_CENTER, 4);
          text->drawtext_cache(msg2,screen,430,90,black2,text->big,A_CENTER,A_CENTER, 5);
	} else if (game->reached_target < game->min_reached_target+(game->nb-game->min_reached_target)/2) {
          snprintf(msg,100,"Good"); snprintf(msg2,100,"job!");
          text->drawtext_cache(msg,screen,430,52,black2,text->big,A_CENTER,A_CENTER, 4);
          text->drawtext_cache(msg2,screen,430,93,black2,text->big,A_CENTER,A_CENTER, 5);
	} else if (game->reached_target < game->min_reached_target+(game->nb-game->min_reached_target)*3/4) {
          snprintf(msg,100,"Great"); snprintf(msg2,100,"job!");
          text->drawtext_cache(msg,screen,430,52,black2,text->big,A_CENTER,A_CENTER, 4);
          text->drawtext_cache(msg2,screen,430,93,black2,text->big,A_CENTER,A_CENTER, 5);
	} else {
          snprintf(msg,100,"Excellent!");
          text->drawtext_cache(msg,screen,430,70,black2,text->normal,A_CENTER,A_CENTER, 4);
        }
    } else {
        text->drawtext_cache("Game Over!",screen,400,0,black,text->big,A_CENTER,A_TOP, 4);
    }
    if (game->reached_target>1 && game->reached_target >= game->min_reached_target)
      snprintf(msg,100,"Saved %i nanolemmings!",game->reached_target);
    else if (game->reached_target>1)
      snprintf(msg,100,"Saved %i nanolemmings",game->reached_target);
    else if (game->reached_target==1)
      snprintf(msg,100,"Saved one nanolemming");
    else
      snprintf(msg,100,"Didn't save any nanolemming...");
    text->drawtext_cache(msg,screen,110,160,white2,text->normal,A_LEFT,A_CENTER, 1);
    snprintf(msg,100,"Target was to save %i",game->min_reached_target);
    text->drawtext_cache(msg,screen,110,200,white2,text->normal,A_LEFT,A_CENTER, 2);
    int died=game->died_cliff + game->died_fall + game->died_drown + game->died_lava + game->died_explode;

    if (died > 0) {
     if (game->nb - died - game->reached_target>0) {
      snprintf(msg,100,"Out of %i nanolemmings, %i are still alive and %i died:",game->nb, game->nb - died - game->reached_target, died);
      text->drawtext_cache(msg,screen,110,240,white2,text->tiny,A_LEFT,A_CENTER, 3);
     } else {
      snprintf(msg,100,"Out of %i nanolemmings, %i died:",game->nb, died);
      text->drawtext_cache(msg,screen,110,240,white2,text->small,A_LEFT,A_CENTER, 3);
     }
    } else {
     if (game->nb - died - game->reached_target>0) {
      snprintf(msg,100,"Out of %i nanolemmings, %i were saved.",game->nb, game->reached_target);
      text->drawtext_cache(msg,screen,110,240,white2,text->tiny,A_LEFT,A_CENTER, 3);
     } else {
      snprintf(msg,100,"All %i nanolemmings were saved!",game->nb);
      text->drawtext_cache(msg,screen,110,240,white2,text->small,A_LEFT,A_CENTER, 3);
     }
    }
    int x = 140;
    int y = 270;
    if (game->died_fall > 0) {
	snprintf(msg,100,"%i fell to death",game->died_fall);
	text->drawtext_cache(msg,screen,x,y,white2,text->small,A_LEFT,A_CENTER, 6); y += 30;
    }
    if (game->died_cliff > 0) {
	snprintf(msg,100,"%i fell down a cliff",game->died_cliff);
	text->drawtext_cache(msg,screen,x,y,white2,text->small,A_LEFT,A_CENTER, 7); y += 30;
    }
    if (game->died_drown > 0) {
	snprintf(msg,100,"%i drowned",game->died_drown);
	text->drawtext_cache(msg,screen,x,y,white2,text->small,A_LEFT,A_CENTER, 8); y += 30;
    }
    if (game->died_lava > 0) {
	snprintf(msg,100,"%i burned to death",game->died_lava);
	text->drawtext_cache(msg,screen,x,y,white2,text->small,A_LEFT,A_CENTER, 9); y += 30;
    }
    if (game->died_explode > 0) {
	snprintf(msg,100,"%i exploded!",game->died_explode);
	text->drawtext_cache(msg,screen,x,y,white2,text->small,A_LEFT,A_CENTER, 10); y += 30;
    }
}
