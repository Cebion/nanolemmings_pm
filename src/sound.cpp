#include "sound.h"

// how many consecutive frames the action has to be there before the loop starts
#define LOOPATTACK 3
// how many consecutive frames the action can be missing without stopping the loop
#define LOOPDECAY 5

Sound::Sound() {
    opened=false;
//    return;
    if (Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,4096)<0) {
	    printf("Could not open audio\n");
	    return;
	    //throw Exiter("Could not open audio");
    } else {
	printf("Audio opened.\n");
    }
    opened=true;

    Mix_AllocateChannels(8);
    printf("Audio channels allocated.\n");
    Mix_ReserveChannels(5);
//    printf("Speech channel reserved.\n");
    music = NULL; //Mix_LoadMUS(MENUMUSIC);
    music_next = NULL;
    Mix_Volume(-1,120);
    Mix_Volume(0,100);
    Mix_VolumeMusic(70);
    musicvol=70;
    fxvol=120;
//    if (!music) throw Exiter("Could not load music");
//    else {
    if (music) {
	printf("Background music loaded.\n");
	Mix_FadeInMusic(music, -1, 1000);
    }
	
    for (int i=0; i < NB_SOUNDS; i++) {
	    sounds[i] = NULL;
	    loopchan[i] = -1;
	    loopdecay[i] = 0;
	    loopattack[i] = LOOPATTACK;
	    loopduration[i] = 0;
    }
    tmpsound = NULL;
    speech = NULL;
    speakthread = NULL;
    speakprepare = false;
    soundpath[0]=0;
//    printf("Loading sound effects.\n");
//    loadsounds("sound/");
//    printf("Sound effects loaded.\n");
}

void Sound::mute() {
    mute_sound();
    mute_music();
}
void Sound::mute_sound() {
    for (int i=0; i < NB_SOUNDS; i++) {
	if (loopchan[i] != -1) {
	    Mix_FadeOutChannel(loopchan[i],200);
	    loopchan[i] = -1;
	}
    }
}
void Sound::mute_music() {
    Mix_PauseMusic();
//	Mix_VolumeMusic(0);
}
void Sound::unmute() {
    Mix_Volume(-1,fxvol);
    Mix_Volume(0,speechvol);
    Mix_ResumeMusic();
//	Mix_VolumeMusic(musicvol);
}

void Sound::volume(int musicv, int fx, int speechv) {
	clip(musicv,0,120);
	clip(fx,0,120);
	clip(speechv,0,120);
	Mix_Volume(-1,fx);
	Mix_Volume(0,speechv);
	Mix_VolumeMusic(musicv);
	if (musicvol==0 && musicv>0) {
	    musicvol=musicv;
	    playmusic(MENUMUSIC);
	}
	if (musicv==0 && musicvol>0 && music) {
	    Mix_FadeOutMusic(100);
	    Mix_FreeMusic(music);
	    music = NULL;
	}
	speechvol = speechv;
	musicvol = musicv;
	fxvol = fx;
}
void Sound::freesounds() {
	for (int i=0; i < NB_SOUNDS; i++) {
		if(sounds[i]) Mix_FreeChunk(sounds[i]);
	}
}

void Sound::loadsounds(const char* filename) {
	if (!strcmp(soundpath,filename)) return; // already loaded
        printf("Loading sound effects.\n");

	char f[300];
	freesounds();
	strcpy(f,filename); strcat(f,"Oh-Oh.ogg"); sounds[0] = Mix_LoadWAV(f);
	strcpy(f,filename); strcat(f,"digging.ogg"); sounds[1] = Mix_LoadWAV(f);
	strcpy(f,filename); strcat(f,"building.ogg"); sounds[2] = Mix_LoadWAV(f);
	strcpy(f,filename); strcat(f,"in01.ogg"); sounds[3] = Mix_LoadWAV(f);
	strcpy(f,filename); strcat(f,"in02.ogg"); sounds[4] = Mix_LoadWAV(f);
	strcpy(f,filename); strcat(f,"in03.ogg"); sounds[5] = Mix_LoadWAV(f);
	strcpy(f,filename); strcat(f,"die-fall.ogg"); sounds[9] = Mix_LoadWAV(f);
	strcpy(f,filename); strcat(f,"die-fall2.ogg"); sounds[10] = Mix_LoadWAV(f);
	strcpy(f,filename); strcat(f,"die-lava.ogg"); sounds[11] = Mix_LoadWAV(f);
	strcpy(f,filename); strcat(f,"die-lava2.ogg"); sounds[12] = Mix_LoadWAV(f);
	strcpy(f,filename); strcat(f,"die-loop.ogg"); sounds[13] = Mix_LoadWAV(f);
	strcpy(f,filename); strcat(f,"ack01.ogg"); sounds[14] = Mix_LoadWAV(f);
	strcpy(f,filename); strcat(f,"ack02.ogg"); sounds[15] = Mix_LoadWAV(f);
	strcpy(f,filename); strcat(f,"ack03.ogg"); sounds[16] = Mix_LoadWAV(f);
	strcpy(f,filename); strcat(f,"ack04.ogg"); sounds[17] = Mix_LoadWAV(f);
	strcpy(f,filename); strcat(f,"explosion.ogg"); sounds[18] = Mix_LoadWAV(f);
	strcpy(f,filename); strcat(f,"bridge.ogg"); sounds[19] = Mix_LoadWAV(f);
	strcpy(f,filename); strcat(f,"rain.ogg"); sounds[20] = Mix_LoadWAV(f);
	strcpy(f,filename); strcat(f,"die-cliff.ogg"); sounds[21] = Mix_LoadWAV(f);
	strcpy(f,filename); strcat(f,"die-cliff2.ogg"); sounds[22] = Mix_LoadWAV(f);
	strcpy(f,filename); strcat(f,"die-cliff3.ogg"); sounds[23] = Mix_LoadWAV(f);
	strcpy(f,filename); strcat(f,"neg01.ogg"); sounds[24] = Mix_LoadWAV(f);
	strcpy(f,filename); strcat(f,"neg02.ogg"); sounds[25] = Mix_LoadWAV(f);
	strcpy(f,filename); strcat(f,"neg03.ogg"); sounds[26] = Mix_LoadWAV(f);
	strcpy(f,filename); strcat(f,"neg04.ogg"); sounds[27] = Mix_LoadWAV(f);
	strcpy(f,filename); strcat(f,"neg05.ogg"); sounds[28] = Mix_LoadWAV(f);
//	for (int i=0; i<2; i++)
//	    if (!sounds[i]) throw Exiter("Could not load sound");

	printf("Sounds loaded\n");

/*
    for (int i=0; i<255; i+=3) {
	Mix_SetPanning(MIX_CHANNEL_POST,254-i,i);
	SDL_Delay(100);
    }
    */
	strcpy(soundpath,filename);
}

void Sound::playsound(int nb) {
    playsound(nb,127);
}

void Sound::playsoundchan(int nb, int c) {
    playsoundchan(nb,c,127);
}
void Sound::playsoundchan(int nb, int c, int pos) {
    if (Mix_Playing(c)) return;
    if (!Mix_SetPanning(c, (254-pos), pos)) throw Exiter("Couldn't set panning");
    if (!sounds[nb]) printf("Sound missing: %i\n",nb);
    else Mix_PlayChannel(c, sounds[nb], 0);
}
// pos=0 : left; pos = 254 : right
void Sound::playsound(int nb, int pos) {
    if (sounds[nb] && fxvol>0) {
//	printf("Playing sound at pos %i\n",pos);
	int c = Mix_GroupAvailable(-1);
//	if (c == -1) throw Exiter("Couldn't play sound");
	if (c == -1) return;
//	if (!Mix_SetPanning(c, (254-pos)*fxvol/128, pos*fxvol/128)) throw Exiter("Couldn't set panning");
	if (!Mix_SetPanning(c, (254-pos), pos)) throw Exiter("Couldn't set panning");
	c = Mix_PlayChannel(c, sounds[nb], 0);
    } else if (!sounds[nb]) printf("Sound missing: %i\n",nb);

}
void Sound::playsoundfile(const char *f) {
    playsoundfile(f,127);
}
void Sound::playsoundfile(const char *f, int pos) {
    if (fxvol>0) {
	int c = Mix_GroupAvailable(-1);
	if (c == -1) return;
	if (!Mix_SetPanning(c, (254-pos), pos)) throw Exiter("Couldn't set panning");
	if (tmpsound) Mix_FreeChunk(tmpsound);
	tmpsound=Mix_LoadWAV(f);
	c = Mix_PlayChannel(c, tmpsound, 0);
    }
}
int Sound::loopsound(int nb, int pos) {
    if (sounds[nb] && fxvol>0) {
	int c = Mix_GroupAvailable(-1);
	if (c == -1) return -1;
//	if (c == -1) throw Exiter("Couldn't play sound");
	if (!Mix_SetPanning(c, 254-pos, pos)) throw Exiter("Couldn't set panning");
	c = Mix_PlayChannel(c, sounds[nb], -1);
	return c;
    } else printf("Not playing sound %i\n",nb);
    return -1;
}
void Sound::stoploopsound(int c) {
	Mix_FadeOutChannel(c,200);
}

void Sound::safeloopsound(int nb, int pos) {
    safeloopsound(nb,pos,255);
}
void Sound::safeloopsound(int nb, int pos, int vol) {
    if (loopattack[nb] > 0) {
	loopattack[nb]--;
	return;
    }
    loopdecay[nb]=LOOPDECAY; // nb of frames to "bridge"
    loopduration[nb]++;
    if (loopchan[nb] < 0)
	loopchan[nb] = loopsound(nb,pos);
    if (!Mix_SetPanning(loopchan[nb], (254-pos)*vol/255, pos*vol/255)) //throw Exiter("Couldn't set panning");
	printf("Couldn't set panning at pos %i and vol %i\n",pos,vol);
}
void Sound::safestoploopsound(int nb) {
    if (loopchan[nb] != -1 && loopdecay[nb] < 0) {
	     stoploopsound(loopchan[nb]);
	     loopchan[nb] = -1;
	     loopduration[nb]=0;
	     loopattack[nb] = LOOPATTACK; // re-trigger threshold
    } else loopdecay[nb]--;
}
void Sound::stopmusic(int fadeout) {
    if (music) {
	Mix_FadeOutMusic(fadeout);
    }
}
void Sound::stoploops() {
   for (int i=0; i<LOOPDECAY+2; i++){
    safestoploopsound(1);
    safestoploopsound(2);
   }
}

void Sound::loadmusic(const char *filename) {
    if (music_next) {
      Mix_FreeMusic(music_next);
    }
    music_next = Mix_LoadMUS(filename);
    if (!music_next) throw Exiter("Could not load music");
}
void Sound::playloadedmusic(int fadein) {
    if (music) {
      Mix_FreeMusic(music);
    }
    if (!music_next) throw Exiter("No music preloaded");
    music = music_next;
    music_next = NULL;
    Mix_FadeInMusic(music, -1, fadein);
}
void Sound::playmusic(const char *filename, int fadein) {
    if (music) {
//    Mix_FadeOutMusic(10);
//    SDL_Delay(500);
      Mix_FreeMusic(music);
      music = NULL;
    }
//    Mix_FadeOutChannel(-1,500);
    if (musicvol>0) {
//    printf("Loading music: %s\n",filename);
    music = Mix_LoadMUS(filename);
    if (!music) throw Exiter("Could not load music");
//    printf("Music loaded\n");
    Mix_FadeInMusic(music, -1, fadein);
    }
}
void Sound::playmusic(const char *filename) {
    playmusic(filename,2000);
}
int do_speak(void *sound) {
    Sound *s = (Sound *) sound;
    system(s->speechmsg);
    Mix_FreeChunk(s->speech);
    s->speech=Mix_LoadWAV("/tmp/microbes_tmp.wav");
    Mix_Volume(0,s->speechvol);
    Mix_PlayChannel(0, s->speech, 0);
    s->speakprepare=false;
    return 0;
}
void Sound::speak(const char *msg, const char *effects, const int pos) {
    if (speechvol==0) return;
    if (msg[0]==0) return;
    if (Mix_Playing(0)) {
	Mix_FadeOutChannel(0,100);
    }
    snprintf(speechmsg,250,"./speak -v%s -w /tmp/microbes_tmp.wav \"%s\"",effects,msg);
    if (speakthread) SDL_WaitThread(speakthread, NULL);
    speakprepare=true;
    if (pos != 127)
      Mix_SetPanning(0, (254-pos), pos);
    else
      Mix_SetPanning(0, 255, 255);
    speakthread = SDL_CreateThread(do_speak, this);
}
void Sound::speak(const char *msg, const char *effects) {
    speak(msg,effects,127);
}
void Sound::speak(const char *msg) {
    speak(msg,"en-sc");
}
bool Sound::speaking() {
    if (Mix_Playing(0)) return true;
    if (speakprepare) return true;
    return false;
}
void Sound::exit() {
    if (!opened) return;
    while (speakprepare) SDL_Delay(100);
	Mix_FadeOutMusic(1000);
        Mix_FadeOutChannel(-1,1000);
	Mix_FreeMusic(music);
	freesounds();
	Mix_FreeChunk(speech);
	Mix_CloseAudio();
}