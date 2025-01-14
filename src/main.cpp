#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <argp.h>
#include <signal.h>
#include <execinfo.h>

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_gfx/SDL_framerate.h"
#include "util.h"
#include "game.h"
#include "draw.h"
#include "text.h"
#include "sound.h"
#include "levels.h"
#include "menu.h"


SDL_Surface *Screen = NULL;


Game *game = NULL;
Draw *draw = NULL;
Sound *sound = NULL;
Text *text = NULL;
LevelLoader *ll = NULL;
Menu *menu = NULL;

bool keydown[500];

SDL_Joystick *nub0 = NULL;
SDL_Joystick *nub1 = NULL;
int nub0idx=0;
int nub1idx=0;

int drag=0;
int mx1=-1;
int my1=-1;

#define NUBSCALE 5000
#define NUBSCALE2 8000

bool make_screenshot=false;


void set_screen_res() {
        draw->SCREENWIDTH = draw->scale*MSCREENWIDTH;
        draw->SCREENHEIGHT = draw->scale*MSCREENHEIGHT;
        draw->BARHEIGHT = draw->scale*MBARHEIGHT;
        Screen = SDL_SetVideoMode(draw->SCREENWIDTH, draw->SCREENHEIGHT, 32, SDL_HWSURFACE | SDL_FULLSCREEN | SDL_DOUBLEBUF);
        text->resize(1.0*draw->scale/DEFAULTSCALE);
}

void do_key(int k, int down) {
    keydown[k]=down;
    if (!down && k == SDLK_END) {
      drag=false;
      game->x1=-1;
    }
    if (down) {
     if (k == SDLK_z) make_screenshot=true;
     if (menu->playing) {
     switch (k) {
      case SDLK_HOME:
        game->select_all();
        break;
      case SDLK_ESCAPE: throw Exiter("Done"); break;
      case SDLK_q:
      case SDLK_LCTRL:
		if (game->reached_target > ll->cl()->saved) {
		    ll->cl()->saved = game->reached_target;
		    ll->save();
		}
		menu->playing=false;
		sound->stoploops();
		sound->playmusic(MENUMUSIC);
		if (Screen->w != MENU_SCREENWIDTH ) {
		    Screen = SDL_SetVideoMode(MENU_SCREENWIDTH, MENU_SCREENHEIGHT, 32, SDL_HWSURFACE | SDL_FULLSCREEN | SDL_DOUBLEBUF);
		}
		if (game->reached_target >= game->min_reached_target) {
		    printf("Won!\n");
		    menu->change_screen(M_GAMEWON);
		} else {
		    printf("Lost!\n");
		    menu->change_screen(M_GAMELOST);
		}
		break;
      case SDLK_1: game->set_action(0); break;
      case SDLK_2: game->set_action(1); break;
      case SDLK_3: game->set_action(2); break;
      case SDLK_4: game->explode(); break;
      case SDLK_5: game->climb(); break;
      case SDLK_6: game->floater(); break;
      case SDLK_7: game->brush_size=0; break;
      case SDLK_8: game->brush_size=1; break;
      case SDLK_9: game->brush_size=2; break;
      case SDLK_s: game->slower(); break;
      case SDLK_f: game->faster(); break;
      case SDLK_d: game->speed=8; break;
      case SDLK_n: draw->cursor = !draw->cursor; 
		   if (draw->cursor && sound) sound->playsoundfile("sound/nubs.ogg");     break;
      case SDLK_PAGEDOWN: game->explode(); break;
      case SDLK_RSHIFT:
        if (draw->scale < MAXSCALE) {
        draw->xoffset -= MSCREENWIDTH/2;
        draw->yoffset -= MSCREENHEIGHT/2;
        draw->cx = draw->cx*(draw->scale+1)/(draw->scale);
        draw->cy = draw->cy*(draw->scale+1)/(draw->scale);
        draw->scale++;
        set_screen_res();
        }
        break;
      case SDLK_RCTRL:
        if (draw->scale > 1) {
        draw->xoffset += MSCREENWIDTH/2;
        draw->yoffset += MSCREENHEIGHT/2;
        draw->cx = draw->cx*(draw->scale)/(draw->scale+1);
        draw->cy = draw->cy*(draw->scale)/(draw->scale+1);
        draw->scale--;
        set_screen_res();
        }
        break;
     }
     } else {
        menu->input(k);
        if (menu->playing) {
	    int bestscale=1;
	    while (bestscale<MAXSCALE && game->levelmap->w > bestscale*MSCREENWIDTH
	                              && game->levelmap->h > bestscale*MSCREENHEIGHT) {
	        bestscale++;
	    }
	    draw->scale=bestscale;
	    if (bestscale == 3) bestscale=2;
	    game->speed=bestscale*2;
	    set_screen_res();
        }
     }
    }
}
void do_held_keys() {
    game->dx=game->dy=0;
    for (int k=0; k<500; k++) {
	if (keydown[k]) {
	    switch (k) {
		case SDLK_UP:    game->dy=-1; break;
		case SDLK_DOWN:  game->dy= 1; break;
		case SDLK_LEFT:  game->dx=-1; break;
		case SDLK_RIGHT: game->dx= 1; break;
		case SDLK_END:
		    {
		    int x=draw->cx - draw->xo;
		    int y=draw->cy - draw->yo;
		    if (!drag) {
			drag=true;
			mx1=x;
			my1=y;
		    }
		    game->select(mx1,my1,x,y); 
		    if (game->action>0) {mx1=x; my1=y;}
		    }
		    break;
		default: break;
	    }
	}
    }
}

FPSmanager fpsm;
int cfps = 0;
Uint32 ticks_fps_counter = 0;
int frames = 0;
int dropped = 0;

void setFPS(int fps) {
    if (cfps != fps) {
	SDL_setFramerate(&fpsm, fps);
	cfps = fps;
        frames = 0;
        dropped = 0;
        ticks_fps_counter = SDL_GetTicks();
    }
}
void mainloop() {
    SDL_Event event;

    bool dropnext = false;
    int con_drop = 0;
    int fp100s;
    int realfp100s;
    int x=0;
    int y=0;
    int xc=0;
    int yc=0;

    SDL_initFramerate(&fpsm);
    setFPS(FPS);
    ticks_fps_counter = SDL_GetTicks();


    while(1) {
      if (menu->playing) {
       do_held_keys();
       if (!draw->cursor) {
         x+=SDL_JoystickGetAxis(nub0,0);
         y+=SDL_JoystickGetAxis(nub0,1);
       }
       x+=SDL_JoystickGetAxis(nub1,0);
       y+=SDL_JoystickGetAxis(nub1,1);
       draw->xoffset += x/NUBSCALE;
       x = x%NUBSCALE;
       draw->yoffset += y/NUBSCALE;
       y = y%NUBSCALE;
       if (draw->cursor) {
         xc+=SDL_JoystickGetAxis(nub0,0);
         yc+=SDL_JoystickGetAxis(nub0,1);
         draw->cx += xc/NUBSCALE2;
         xc = xc%NUBSCALE2;
         draw->cy += yc/NUBSCALE2;
         yc = yc%NUBSCALE2;
       }
      }

      while(SDL_PollEvent(&event)) {
	switch (event.type) {
		case SDL_KEYDOWN:
			do_key(event.key.keysym.sym,1);
			break;
		case SDL_KEYUP:
			do_key(event.key.keysym.sym,0);
			break;
		case SDL_MOUSEBUTTONDOWN:
		    if (menu->playing) {
			if (event.button.y >= draw->SCREENHEIGHT-draw->BARHEIGHT) {
			    int r=game->set_action(event.button.x / draw->scale,
			                     (event.button.y - (draw->SCREENHEIGHT-draw->BARHEIGHT)) / draw->scale);
			    if (r==1) do_key(SDLK_RCTRL,1);
			    if (r==2) do_key(SDLK_RSHIFT,1);
			} else {
			    mx1=event.button.x-draw->xo;
			    my1=event.button.y-draw->yo;
			    drag=1;
			}
		    }
		    break;
		case SDL_MOUSEMOTION:
		    if (menu->playing) {
			if (drag) {
			    game->select(mx1,my1,event.motion.x-draw->xo,event.motion.y-draw->yo);
			    if (game->action > 0) {
			      mx1=event.button.x-draw->xo;
			      my1=event.button.y-draw->yo;
			    }
			}
		    }
		    break;
		case SDL_MOUSEBUTTONUP:
			if (game->action == 0 && drag == 1 && sound)
			  sound->playsound(14+rand()%4);
			drag=0;
			game->x1=-1;
			game->y1=0;
			game->x2=0;
			game->y2=0;
			break;
		case SDL_QUIT:
			throw Exiter("Quitting.");
	}
      }
      if (menu->playing) {
        int status=GAME_PLAYING;
        if (game->speed >= 8) {
           for (int it=0; it<game->speed/8; it++) status=game->step();
        } else {
           if (frames % (8/game->speed) == 0) status=game->step();
        }
        //if (frames % 60 == 0) game->step();
        if (status==GAME_PLAYING) {
          if (!dropnext) draw->draw();
        } else {
          if (status==GAME_WON) {
           printf("Won!\n");
           menu->change_screen(M_GAMEWON);
          } else {
           printf("Lost!\n");
           menu->change_screen(M_GAMELOST);
          }
          do_key(SDLK_q,1);
        }
      } else {
        menu->step();
        if (!dropnext) {
	    menu->draw();
	    if (menu->menuscreen == M_GAMELOST || menu->menuscreen == M_GAMEWON) draw->draw_stats();
	}
      }

     if (make_screenshot) {
	char sfn[80];
	snprintf(sfn,80,"screenshot.bmp");
	SDL_SaveBMP(Screen,sfn);
	make_screenshot=false;
     }

      if (!dropnext) SDL_Flip(Screen);
      int drop = SDL_framerateDelay(&fpsm);
//      dropnext = false; //(dropnext ? false : drop); // don't drop two in a row
      dropnext = (con_drop <= 4 ? drop : false); // don't drop more than 4 in a row
      // drop;
      //(dropnext ? false : drop); // don't drop two in a row
//      if (drop == 2) printf("Cannot keep up! Please set higher cpu speed!\n");

      frames++;
      if (dropnext) {
        dropped++;
        con_drop++;
      } else con_drop=0;
      if (frames>=FPS*10) {
        fp100s = (frames) * 100000 / (SDL_GetTicks()-ticks_fps_counter);
        realfp100s = (frames-dropped) * 100000 / (SDL_GetTicks()-ticks_fps_counter);
        printf("fps: %.2f (%.2f) /%i  (%i frames drawn, %i frames skipped in %ims)\n",0.01*fp100s,0.01*realfp100s, cfps, frames-dropped,dropped,(SDL_GetTicks()-ticks_fps_counter));
//        if (fp100s < cfps*70) throw Exiter("Game is running way too slow, this is a form of cheating...",100);
        frames = 0;
        dropped = 0;
        ticks_fps_counter = SDL_GetTicks();
      }
    }
}
static const bool kUseExceptionHandler = false;
//static const bool kUseExceptionHandler = true;

// thanks to Steven Craft for this snippet
void handler(int sig) {
    void *array[ 16 ];
    size_t size;
    // get void*'s for all entries on the stack
    size = backtrace(array, sizeof( array ) );
    // print out all the frames to stderr
    fprintf(stderr, "Error: signal %d:\n", sig);
    backtrace_symbols_fd( array, size, 2 );
    SDL_Quit();
    exit( 1 );
}


int main(int argc, char **argv) {

	const bool Fullscreen = true;
	if ( kUseExceptionHandler ) {
		signal(SIGSEGV, handler);
		signal(SIGBUS, handler);
	}
	printf("Initializing SDL: VIDEO...");
	if ( SDL_Init(SDL_INIT_VIDEO) < 0 )
	{
		fprintf(stderr, "Error initializing SDL: %s\n", SDL_GetError());
		return 1;
	}
	int rv = 0;
	try {
		printf("OK.\n");
		
		SDL_WM_SetCaption("NanoLemmings", NULL);
		printf("Setting video mode: 800x480x32...");
		Uint32 flags = SDL_HWSURFACE | SDL_DOUBLEBUF;
		if (Fullscreen)
			flags |= SDL_FULLSCREEN;
		Screen = SDL_SetVideoMode(MSCREENWIDTH*DEFAULTSCALE, MSCREENHEIGHT*DEFAULTSCALE, 32, flags);
		if ( Screen == NULL )
			throw Exiter(SDL_GetError());
		else
			printf("OK\n");

		SDL_InitSubSystem(SDL_INIT_JOYSTICK);
			int numjs=SDL_NumJoysticks();
			printf("num: %i\n",numjs);
			SDL_Joystick *js;
			for(int i=0; i<numjs; i++) {
			  js = SDL_JoystickOpen(i);
			  if (js) {
			    printf("Found joystick %s\n", SDL_JoystickName(i));
			    printf("  axes: %d\n", SDL_JoystickNumAxes(js));
			    if(!strcmp(SDL_JoystickName(i),"nub0")) {
				nub0 = js;
				nub0idx = i;
				printf("   found left nub!\n");
			    } else if(!strcmp(SDL_JoystickName(i),"nub1")) {
				nub1 = js;
				nub1idx = i;
				printf("   found right nub!\n");
			    } else SDL_JoystickClose(js);
			  } else printf("oops\n");
			}


		if (Fullscreen)				// hide mouse cursor in fullscreen
			SDL_ShowCursor(SDL_DISABLE);

		//IMG_Init(IMG_INIT_PNG);
		SDL_Surface *loading=IMG_Load("gfx/pandora.jpg");
		//SDL_FillRect(Screen,NULL,SDL_MapRGB(Screen->format,0,0,0));
		SDL_BlitSurface(loading,NULL,Screen,NULL);
		SDL_Flip(Screen);
		
		printf("Splash screen OK.\n");


		SDL_Event e;
		while (SDL_PollEvent(&e)) {}
		sound = new Sound();

		if (sound) {
		    sound->playsoundfile("sound/boot.ogg");
		    sound->loadsounds("sound/");
		    sound->loadmusic(MENUMUSIC);
		}
		game = new Game(sound);
		text = new Text();
		draw = new Draw(game,Screen,text);
		
		ll = new LevelLoader(game);
		menu = new Menu(ll,Screen,text,sound);

		SDL_Surface *icon=IMG_Load("icon.png");
		SDL_WM_SetIcon(icon, NULL);

		for (int i=10; i<255; i+=10) {
		    SDL_BlitSurface(loading,NULL,Screen,NULL);
		    boxRGBA(Screen,0,0,799,479,0,0,0,i);
		    SDL_Flip(Screen);
		    SDL_Delay(30);
		}
		SDL_FreeSurface(loading);
		loading=IMG_Load("gfx/wb_intro.png");
		//SDL_FillRect(Screen,NULL,SDL_MapRGB(Screen->format,0,0,0));
		SDL_BlitSurface(loading,NULL,Screen,NULL);
		SDL_Flip(Screen);


		if (sound) sound->playloadedmusic(100);

		bool skip=false;
		for (int i=0; i<20; i++) {
		    SDL_Delay(100);
		    if (SDL_PollEvent(&e)) {
			break;
			skip=true;
		    }
		}
		if (!skip)
		for (int i=10; i<255; i+=10) {
		    SDL_BlitSurface(loading,NULL,Screen,NULL);
		    boxRGBA(Screen,0,0,799,479,0,0,0,i);
		    SDL_Flip(Screen);
		    if (SDL_PollEvent(&e)) {
			break;
		    } else SDL_Delay(30);
		}
		SDL_FreeSurface(loading);
		mainloop();
	} catch(Exiter &e) {
		if (sound) sound->playsoundfile("sound/bye.ogg");
		printf("\nExiting: %s\n", e.message.c_str());
		rv = e.value;
		if (ll) ll->save();
		SDL_Delay(200);
	}
	
	//SDL_QuitSubSystem(SDL_INIT_VIDEO);
	if (sound) sound->exit();
	
	SDL_Quit();
	return rv;
}

