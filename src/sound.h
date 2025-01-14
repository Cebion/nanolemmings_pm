#ifndef SOUND_H
#define SOUND_H

#include "SDL.h"
#include "SDL_mixer.h"
#include "SDL_thread.h"
#include "util.h"

#define MENUMUSIC "music/bg_music_menu.mod"
#define NB_SOUNDS 40

class Sound {
public:
    Mix_Music *music;
    Mix_Music *music_next;
    Mix_Chunk *sounds[NB_SOUNDS];
    Mix_Chunk *tmpsound;
    int loopchan[NB_SOUNDS];
    int loopduration[NB_SOUNDS];
    int loopdecay[NB_SOUNDS];
    int loopattack[NB_SOUNDS];
    Mix_Chunk *speech;
    char soundpath[80];
    char speechmsg[200];
    int speechvol, fxvol, musicvol;
    bool opened;
    Sound();
    void volume(int musicv, int fx, int speechv);
    void playmusic(const char *filename);
    void stopmusic(int fadeout);
    void playmusic(const char *filename, int fadein);

    void loadmusic(const char *filename);
    void playloadedmusic(int fadein);

    void mute();
    void mute_sound();
    void mute_music();
    void unmute();

    void speak(const char *msg);
    void speak(const char *msg,const char *effects);
    void speak(const char *msg,const char *effects, const int pos);
    void playsoundfile(const char *f);
    void playsoundfile(const char *f, int pos);
    void playsoundchan(int nb, int c);
    void playsoundchan(int nb, int c, int pos);
    void playsound(int nb);
    void playsound(int nb, int pos);
    int loopsound(int nb, int pos);
    void stoploopsound(int c);
    void safeloopsound(int nb, int pos, int vol);
    void safeloopsound(int nb, int pos);
    void safestoploopsound(int c);
    void stoploops();
    void loadsounds(const char *filename);
    void exit();
    bool speaking();
    bool speakprepare;
private:
    void freesounds();
    SDL_Thread *speakthread;
};


#endif