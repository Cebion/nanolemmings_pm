#include "menu.h"
const SDL_Color menutextcolor = {255,235,223};
//green= {24,196,66}; orange= {255,144,0};
const SDL_Color menutextcolor2 = {255,184,40};
const SDL_Color menutextcolor3 = {255,235,223};
const SDL_Color menutextcolornot = {255,144,66};
const SDL_Color scrolltextcolor = {255,235,223};
const SDL_Color scrollshadowcolor = {0,0,0};

const int max_option[10] = {4,1,2};

#define SCROLLBARSTRING "Pandora NanoLemmings v0.1.0 \
      This is an entry in the Alive and Kicking coding contest. \
      Have fun! \
      Concept & coding: _wb_ \
      Sound effects: EvilDragon \
      Pixel art: mcobit \
      Music: Dr.Awesome"

Menu::Menu(LevelLoader * ll, SDL_Surface * scr, Text * t, Sound * s) {
    l = ll;
    screen = scr;
    playing = false;
    text = t;
    sound = s;
    menuscreen = M_MAINMENU;
    helpscreen = 0;
    sel = 0;
    SDL_Surface *img;
    img=IMG_Load("gfx/lemming.png");
    if (!img) {
	printf("Could not load main menu image (gfx/lemming.png)\n");
	throw Exiter("Image not found");
    }
    lemming=SDL_DisplayFormatAlpha(img);
    SDL_FreeSurface(img);
    img=IMG_Load("gfx/floater.png");
    if (!img) {
	printf("Could not load main menu image (gfx/floater.png)\n");
	throw Exiter("Image not found");
    }
    lemmingfl=SDL_DisplayFormatAlpha(img);
    SDL_FreeSurface(img);
    img=IMG_Load("gfx/floateri.png");
    if (!img) {
	printf("Could not load main menu image (gfx/floateri.png)\n");
	throw Exiter("Image not found");
    }
    lemmingfli=SDL_DisplayFormatAlpha(img);
    SDL_FreeSurface(img);
    img=IMG_Load("gfx/twolemmings.png");
    if (!img) {
	printf("Could not load main menu image (gfx/twolemmings.png)\n");
	throw Exiter("Image not found");
    }
    twolemmings=SDL_DisplayFormatAlpha(img);
    SDL_FreeSurface(img);
    img=IMG_Load("gfx/twolemmingsi.png");
    if (!img) {
	printf("Could not load main menu image (gfx/twolemmingsi.png)\n");
	throw Exiter("Image not found");
    }
    twolemmingsi=SDL_DisplayFormatAlpha(img);
    SDL_FreeSurface(img);
    flx=0;
    fly=1000;
    fldx=0;
    tlx=-3000;
    tly=0;
    tldx=0;

    intro=250;

     int i=0;
     img=IMG_Load("gfx/guihints/game-a.png"); guihints[i++]=SDL_DisplayFormatAlpha(img); SDL_FreeSurface(img);
     img=IMG_Load("gfx/guihints/game-b.png"); guihints[i++]=SDL_DisplayFormatAlpha(img); SDL_FreeSurface(img);
     img=IMG_Load("gfx/guihints/game-x.png"); guihints[i++]=SDL_DisplayFormatAlpha(img); SDL_FreeSurface(img);
     img=IMG_Load("gfx/guihints/game-y.png"); guihints[i++]=SDL_DisplayFormatAlpha(img); SDL_FreeSurface(img);
     img=IMG_Load("gfx/guihints/dpad-left.png"); guihints[i++]=SDL_DisplayFormatAlpha(img); SDL_FreeSurface(img);
     img=IMG_Load("gfx/guihints/dpad-right.png"); guihints[i++]=SDL_DisplayFormatAlpha(img); SDL_FreeSurface(img);
     img=IMG_Load("gfx/guihints/dpad-leftright.png"); guihints[i++]=SDL_DisplayFormatAlpha(img); SDL_FreeSurface(img);


    game = new Game(NULL);
    game->begin("gfx/mainmenu-map.png","gfx/mainmenu.png");
    dr = new Draw(game,screen,text);
    dr->yoffset=100;
    dr->BARHEIGHT=0;
    game->action_allowed[5]=10000;
    game->floater();
    game->loopy=true;
    pos=-800;
    draw_obstacles();

}

void Menu::draw_obstacles() {
//    boxRGBA(game->levelmap,250,300,798,579,0,0,0,255);
    SDL_Rect re;
    re.x=250; re.y=300;
    re.w=548; re.h=280;
    if (menuscreen != M_GAMELOST && menuscreen != M_GAMEWON && menuscreen != M_HELP)
    SDL_FillRect(game->levelmap,&re,C_NOTHING);
//    boxRGBA(game->levelimg,250,300,798,579,0,0,0,255);
    draw_options(game->levelmap,100);
    if (rand()%2) {
      if (fly < -200) fly = -200;
    } else {
      if (tlx<-200 || tlx > 800) {
      if (tldx>0) tlx = -200;
      else tlx = 800;
      }
    }
}

void Menu::draw_options(SDL_Surface *surf, int yoffset) {
  char msg[100];
  int i=0;
  SDL_Rect re;
  if (menuscreen == M_MAINMENU) {
    if (sel!=i || yoffset==0) text->drawtext_cache("Play", surf, 770, 240+yoffset+40*i, (sel==i?menutextcolor:menutextcolornot), text->huge, A_RIGHT, A_CENTER, 2+i+5*(i==sel)); i++;
    if (sel!=i || yoffset==0) text->drawtext_cache("Options", surf, 420, 240-3+yoffset+40*i, (sel==i?menutextcolor:menutextcolornot), text->big, A_LEFT, A_CENTER, 2+i+5*(i==sel)); i++;
    if (sel!=i || yoffset==0) text->drawtext_cache("Help", surf, 770, 240+5+yoffset+40*i, (sel==i?menutextcolor:menutextcolornot), text->huge, A_RIGHT, A_CENTER, 2+i+5*(i==sel)); i++;
    if (sel!=i || yoffset==0) text->drawtext_cache("Credits", surf, 425, 240-5+yoffset+40*i, (sel==i?menutextcolor:menutextcolornot), text->big, A_LEFT, A_CENTER, 2+i+5*(i==sel)); i++;
    if (sel!=i || yoffset==0) text->drawtext_cache("Quit", surf, 730, 240-8+yoffset+40*i, (sel==i?menutextcolor:menutextcolornot), text->big, A_RIGHT, A_CENTER, 2+i+5*(i==sel)); i++;
  } else if (menuscreen == M_LEVELSELECT) {
    re.x=760; re.y=240+yoffset; SDL_BlitSurface(guihints[GHDLR],NULL,surf,&re);
    text->drawtext_cache(l->cl()->name, surf, 758, 250+yoffset,
                         (sel==i?menutextcolor:menutextcolornot), text->huge, A_RIGHT, A_CENTER, 2+i+5*(i==sel));
    snprintf(msg,100,"Goal:%i  - Saved:%i",l->cl()->rescue,l->cl()->saved);
    if (sel!=i || yoffset==0) text->drawtext_cache(msg, surf, 780, 300+yoffset,
                         (sel==i?menutextcolor:menutextcolornot), text->normal, A_RIGHT, A_CENTER, 3+i+5*(i==sel));
    i++;
    re.x=760; re.y=380+yoffset; SDL_BlitSurface(guihints[GHA],NULL,surf,&re);
    text->drawtext_cache("Back", surf, 758, 400+yoffset, (sel==i?menutextcolor:menutextcolornot), text->big, A_RIGHT, A_CENTER, 4+i+5*(i==sel));
  } else if (menuscreen == M_OPTIONS) {
    if (sel==i) {re.x=760; re.y=240+yoffset+60*i; SDL_BlitSurface(guihints[(sound->fxvol>0?(sound->fxvol<120?GHDLR:GHDL):GHDR)],NULL,surf,&re);}
    snprintf(msg,100,"Sound: %i",sound->fxvol/10);
    if (sel!=i || yoffset==0) text->drawtext_cache(msg, surf, 758+(sel!=i)*22, 250+yoffset+60*i, (sel==i?menutextcolor:menutextcolornot), text->huge, A_RIGHT, A_CENTER, 2+i+5*(i==sel)); i++;
    if (sel==i) {re.x=760; re.y=240+yoffset+60*i; SDL_BlitSurface(guihints[(sound->musicvol>0?(sound->musicvol<120?GHDLR:GHDL):GHDR)],NULL,surf,&re);}
    snprintf(msg,100,"Music: %i",sound->musicvol/10);
    if (sel!=i || yoffset==0) text->drawtext_cache(msg, surf, 758+(sel!=i)*22, 250+yoffset+60*i, (sel==i?menutextcolor:menutextcolornot), text->huge, A_RIGHT, A_CENTER, 2+i+5*(i==sel)); i++;
    re.x=760; re.y=250+yoffset+0+60*i; SDL_BlitSurface(guihints[GHA],NULL,surf,&re);
    if (sel!=i || yoffset==0) text->drawtext_cache("Back", surf, 758, 250+yoffset+20+60*i, (sel==i?menutextcolor:menutextcolornot), text->big, A_RIGHT, A_CENTER, 2+i+5*(i==sel)); i++;
  } else if (menuscreen == M_CREDITS) {
    text->drawtext_cache("Concept/code", surf, 290, 220+yoffset+50*i, menutextcolor2, text->normal, A_LEFT, A_CENTER, 2+i); i++;
    text->drawtext_cache("Pixel artwork", surf, 300, 220+yoffset+50*i, menutextcolor2, text->normal, A_LEFT, A_CENTER, 2+i); i++;
    text->drawtext_cache("Sound fx", surf, 310, 220+yoffset+50*i, menutextcolor2, text->normal, A_LEFT, A_CENTER, 2+i); i++;
    text->drawtext_cache("Music", surf, 320, 220+yoffset+50*i, menutextcolor2, text->normal, A_LEFT, A_CENTER, 2+i); i++;
    i=0;
    text->drawtext_cache("_wb_", surf, 690, 220+yoffset+50*i, menutextcolor3, text->big, A_RIGHT, A_CENTER, 6+i); i++;
    text->drawtext_cache("mcobit", surf, 710, 220+yoffset+50*i, menutextcolor3, text->big, A_RIGHT, A_CENTER, 6+i); i++;
    text->drawtext_cache("EvilDragon", surf, 760, 220+yoffset+50*i, menutextcolor3, text->big, A_RIGHT, A_CENTER, 6+i); i++;
    text->drawtext_cache("Dr.Awesome", surf, 770, 220+yoffset+50*i, menutextcolor3, text->big, A_RIGHT, A_CENTER, 6+i); i++;
    i=0;
    if (yoffset==0) text->drawtext_cache("(c) Jon '_wb_' Sneyers, 2014. GNU GPL v3.", surf, 790, 400+yoffset+16*i, menutextcolor2, text->tiny, A_RIGHT, A_CENTER, 10+i); i++;
    if (yoffset==0) text->drawtext_cache("This is an entry in the Alive and Kicking coding contest.", surf, 790, 400+yoffset+16*i, menutextcolor, text->tiny, A_RIGHT, A_CENTER, 10+i); i++;
    if (yoffset==0) text->drawtext_cache("Thanks to the awesome Pandora community for various contributions!", surf, 790, 400+yoffset+16*i, menutextcolor, text->tiny, A_RIGHT, A_CENTER, 10+i); i++;
  } else if (menuscreen == M_HELP) {
    snprintf(msg,100,"%i/%i",helpscreen+1,MAXHELPSCREEN+1);
    text->drawtext_cache(msg, surf, 758, 2+yoffset, menutextcolor2, text->normal, A_RIGHT, A_TOP, 6);
    re.x=760; re.y=2+yoffset; SDL_BlitSurface(guihints[(helpscreen>0?(helpscreen<MAXHELPSCREEN?GHDLR:GHDL):GHDR)],NULL,surf,&re);
  }
}

void Menu::step() {
    fly += 2;
    if (fly>-200) flx += fldx;
    if (fly > 480) { fly = -3000; fldx = 1-2*(rand()%2); flx = rand()%700 - 200*fldx; }
    tlx += 3*tldx;
    if (tlx > 2000 || tlx < -1400) { tly = 50+rand()%200; tldx = 1-2*(rand()%2); tlx = rand()%1000 - 2000*tldx; }
//    if (rand()%100<4) fldx = -fldx;
    if (intro>0) { intro -= 1+intro/20;
	if (intro==0) {
	    SDL_Rect re;
	    re.y = 110;
	    re.x = 50;
	    SDL_BlitSurface(lemming,NULL,game->levelmap,&re);
	    SDL_BlitSurface(lemming,NULL,game->levelimg,&re);
	    intro--;
	}
    }
    pos += 2;
    if (text && text->cache[0] && pos > text->cache[0]->w) pos=-800;
    game->step();
}
void Menu::draw() {
//    SDL_FillRect(screen, NULL, 0x005500);
    dr->draw_bare();
    SDL_Rect re;

    if (intro>0) {
      re.y=10;
      re.x=50-intro;
      SDL_BlitSurface(lemming,NULL,screen,&re);
    }
//    boxRGBA(screen,0,0,MENU_SCREENWIDTH-1,MENU_SCREENHEIGHT-1-MBARHEIGHT*DEFAULTSCALE,0,0,0,100);
//    boxRGBA(screen,0,439,MENU_SCREENWIDTH-1,439,68,68,237,40);
    if (menuscreen != M_GAMELOST && menuscreen != M_GAMEWON && menuscreen != M_HELP) {
    boxRGBA(screen,0,440,MENU_SCREENWIDTH-1,440,0,0,0,150);
    boxRGBA(screen,0,441,MENU_SCREENWIDTH-1,MENU_SCREENHEIGHT-1,68,68,237,100);
    text->drawtext_cache(SCROLLBARSTRING,
                         screen, 0-pos+1, 460+1,
                         scrollshadowcolor, text->normal, A_LEFT, A_CENTER, 0);
    text->drawtext_cache(SCROLLBARSTRING,
                         screen, 0-pos, 460,
                         scrolltextcolor, text->normal, A_LEFT, A_CENTER, 1);
    }
    draw_options(screen,0);

    if (menuscreen != M_GAMELOST && menuscreen != M_GAMEWON) {
    re.x=flx;
    re.y=fly;
    SDL_BlitSurface((fldx>0?lemmingfli:lemmingfl),NULL,screen,&re);
    re.x=tlx;
    re.y=tly;
    SDL_BlitSurface((tldx>0?twolemmings:twolemmingsi),NULL,screen,&re);
    }
//    printf("(%i,%i) dx=%i\n",tlx,tly,tldx);
}

void Menu::change_screen(int s) {
  if (menuscreen == M_GAMELOST || menuscreen == M_GAMEWON || (menuscreen == M_HELP && s != M_HELP)) {
    game->begin("gfx/mainmenu-map.png","gfx/mainmenu.png");
    game->action_allowed[5]=10000;
    game->floater();
    game->loopx=false;
    game->loopy=true;
    pos=-800;
    dr->yoffset=100;
    intro=250;
  }
  if (s == M_GAMELOST) {
    game->begin("gfx/gameover-lost-map.png","gfx/gameover-lost.png");
    game->action_allowed[4]=10000;
    game->climb();
    game->loopx=true;
    dr->yoffset=0;
    intro=-1;
  }
  if (s == M_GAMEWON) {
    game->begin("gfx/gameover-won-map.png","gfx/gameover-won.png");
    game->action_allowed[4]=10000;
    game->climb();
    game->action_allowed[5]=10000;
    game->floater();
    game->loopx=true;
    dr->yoffset=0;
    intro=-1;
  }
  if (s == M_HELP) {
    char mfn[100],ifn[100];
    snprintf(mfn,100,"gfx/help/helpscreen%i-map.png",helpscreen);
    snprintf(ifn,100,"gfx/help/helpscreen%i-img.png",helpscreen);
    game->begin(mfn,ifn);
    game->loopx=true;
    dr->yoffset=0;
    intro=-1;
  }
  menuscreen=s;
  draw_obstacles();
//  printf("changed to menu screen %i\n",s);
}
void Menu::do_action() {
    if (menuscreen==M_LEVELSELECT) {
	if (sel==0) {
	  l->start(); playing=true;
	} else go_back();
    } else if (menuscreen==M_OPTIONS) {
	if (sel==2) go_back();
    } else if (menuscreen==M_MAINMENU) {
	if (sel==4) throw Exiter("Quit from main menu");
	else change_screen(sel+1);
    } else go_back();
}
void Menu::go_back() {
    if (menuscreen==M_LEVELSELECT)  { change_screen(M_MAINMENU); sel=0; return;}
    else if (menuscreen==M_OPTIONS) { change_screen(M_MAINMENU); sel=1; return;}
    else if (menuscreen==M_HELP)    { change_screen(M_MAINMENU); sel=2; return;}
    else if (menuscreen==M_CREDITS) { change_screen(M_MAINMENU); sel=3; return;}
    else if (menuscreen==M_GAMELOST) { change_screen(M_LEVELSELECT); sel=0; return;}
    else if (menuscreen==M_GAMEWON) { change_screen(M_LEVELSELECT); sel=0; return;}
    else { change_screen(M_MAINMENU); sel=4; return;}
}
void Menu::input(int k) {
    switch(k) {
        case SDLK_LEFT: 
	    if (menuscreen==M_LEVELSELECT) { l->prev(); }
	    if (menuscreen==M_OPTIONS && sel==0 && sound) {sound->volume(sound->musicvol,sound->fxvol-10,0);}
	    if (menuscreen==M_OPTIONS && sel==1 && sound) {sound->volume(sound->musicvol-10,sound->fxvol,0);}
	    if (menuscreen==M_HELP && helpscreen>0) {helpscreen--; change_screen(M_HELP);}
	    fldx=tldx=-1;
	    break;
        case SDLK_RIGHT:
	    if (menuscreen==M_LEVELSELECT) { l->next(); }
	    if (menuscreen==M_OPTIONS && sel==0 && sound) {sound->volume(sound->musicvol,sound->fxvol+10,0);}
	    if (menuscreen==M_OPTIONS && sel==1 && sound) {sound->volume(sound->musicvol+10,sound->fxvol,0);}
	    if (menuscreen==M_HELP && helpscreen<MAXHELPSCREEN) {helpscreen++; change_screen(M_HELP);}
	    fldx=tldx=1;
	    break;
	case SDLK_UP:
	    sel--; break;
	case SDLK_DOWN:
	    sel++; break;
        case SDLK_RETURN:
        case SDLK_PAGEDOWN:
        case SDLK_END:
               do_action();
               break;
        case SDLK_HOME:
               go_back();
               break;
        case SDLK_ESCAPE:
        case SDLK_q:
               throw(Exiter("Quit from mainmenu"));
               break;
    }
    if (sel>max_option[menuscreen]) sel=0;
    if (sel<0) sel=max_option[menuscreen];
    draw_obstacles();
}
