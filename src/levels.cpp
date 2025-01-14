#include "levels.h"

void LevelLoader::start() {
    game->begin(cl()->map, cl()->img);
    game->min_reached_target = cl()->rescue;
    for (int i=5; i >= 0; i--) {
	game->action_allowed[i] = cl()->actions[i];
	if (cl()->actions[i] >= game->nb) game->action_allowed[i] = -1;
	if (cl()->actions[i] && i<3) game->action=i;
    }
    if (game->sound) game->sound->playmusic(cl()->bgmusic);
    game->nbframes=cl()->nbframes;
    game->framedelay=cl()->framedelay;
    for (int i=0; i<cl()->nbframes; i++) {
      SDL_Surface *img=IMG_Load(cl()->frame[i]);
      if (!img) {
	printf("Could not load image \"%s\"\n",cl()->frame[i]);
	throw Exiter("Level image frame not found");
      }
      if (game->levelimg_frame[i]) SDL_FreeSurface(game->levelimg_frame[i]);
      game->levelimg_frame[i]=SDL_DisplayFormatAlpha(img);
      SDL_FreeSurface(img);
    }
    game->nboframes=cl()->nboframes;
    game->oframedelay=cl()->oframedelay;
    for (int i=0; i<cl()->nboframes; i++) {
      SDL_Surface *img=IMG_Load(cl()->oframe[i]);
      if (!img) {
	printf("Could not load image \"%s\"\n",cl()->oframe[i]);
	throw Exiter("Level image overlay frame not found");
      }
      if (game->levelimg_oframe[i]) SDL_FreeSurface(game->levelimg_oframe[i]);
      game->levelimg_oframe[i]=SDL_DisplayFormatAlpha(img);
      SDL_FreeSurface(img);
    }
}
void LevelLoader::next() {
    if (current<nb-1) current++;
}
void LevelLoader::prev() {
    if (current>0) current--;
}


LevelLoader::LevelLoader(Game *g) {
	game=g;
	FILE *fp = fopen("maps/maplist", "r");
	nb=0;
	current=0;
	if (fp) {
	    while (!feof(fp)) {
		char buff[100];
		char *buffp, *buffp2;
		level[nb] = new Level();
		fgets(buff, 100, fp);
		level[nb]->id = strtol(buff,NULL,10);
		if (level[nb]->id == 0) break;
		level[nb]->saved = 0;
		fgets(level[nb]->name, 100, fp);
		rmnl(level[nb]->name);
		fgets(level[nb]->map, 100, fp);
		rmnl(level[nb]->map);
		fgets(level[nb]->img, 100, fp);
		rmnl(level[nb]->img);
		fgets(buff, 100, fp);
		level[nb]->nbframes = strtol(buff,&buffp,10);
		level[nb]->framedelay = strtol(buffp,&buffp2,10);
		level[nb]->nboframes = strtol(buffp2,&buffp,10);
		level[nb]->oframedelay = strtol(buffp,NULL,10);
		for (int i=0; i<level[nb]->nbframes; i++) {
		    fgets(level[nb]->frame[i], 100, fp);
		    rmnl(level[nb]->frame[i]);
		}
		for (int i=0; i<level[nb]->nboframes; i++) {
		    fgets(level[nb]->oframe[i], 100, fp);
		    rmnl(level[nb]->oframe[i]);
		}
		fgets(level[nb]->bgmusic, 100, fp);
		rmnl(level[nb]->bgmusic);
		fgets(buff, 100, fp);
		level[nb]->rescue = strtol(buff,&buffp,10);
		level[nb]->actions[0] = strtol(buffp,&buffp2,10);
		level[nb]->actions[1] = strtol(buffp2,&buffp,10);
		level[nb]->actions[2] = strtol(buffp,&buffp2,10);
		level[nb]->actions[3] = strtol(buffp2,&buffp,10);
		level[nb]->actions[4] = strtol(buffp,&buffp2,10);
		level[nb]->actions[5] = strtol(buffp2,NULL,10);
		printf("Map #%i: \"%s\"\n",nb,level[nb]->name);
		nb++;
	    }
	    fclose(fp);
	}
	fp = fopen("savefile", "r");
	if (fp) {
	    while (!feof(fp)) {
		char buff[100];
		char *buffp;
		fgets(buff, 100, fp);
		int id=strtol(buff,&buffp,10);
		if (id) {
		int saved=strtol(buffp,NULL,10);
		for (int i=0; i<nb; i++) {
		    if (id == level[i]->id) {level[i]->saved=saved; break;}
		}
		}
	    }
	    fclose(fp);
	}
}


void LevelLoader::save() {
	FILE *fp = fopen("savefile", "w+");
	if (fp) {
		for (int i=0; i<nb; i++) {
		    if (level[i]->saved>0) {
		      fprintf(fp,"%i %i\n",level[i]->id,level[i]->saved);
		    }
		}
		fclose(fp);
	}
}
