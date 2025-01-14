
PROGRAM = lem
OBJECTS = util.o text.o sound.o game.o draw.o levels.o menu.o main.o
CC = g++


# MY_CFLAGS = -I/usr/local/include/SDL -Dmain=SDL_main -DWIN32 -Uunix -mno-cygwin
MY_CFLAGS = `sdl-config --cflags` -Wall -pedantic -DPANDORA -D__LINUX__ -pipe -mcpu=cortex-a8 -mfpu=neon -ftree-vectorize -mfloat-abi=softfp -fno-rtti -O3 -ggdb
#-Os -mthumb

%.o:	%.cpp util.h
	$(CC) -c $(MY_CFLAGS) $< -o $@

all:	$(PROGRAM)

$(PROGRAM):	$(OBJECTS)
#	$(CC) -o$(PROGRAM) $(OBJECTS)  -lSDL -lSDL_image -lSDL_ttf -lSDL_mixer -L./ -L/usr/lib/ -L/lib/ -lmySDL_gfx
	$(CC) `sdl-config --libs` -o$(PROGRAM) $(OBJECTS)  -lSDL_image -lSDL_ttf -lSDL_mixer -L./ -lmySDL_gfx
#	strip $(PROGRAM)

clean:
	$(RM) *.o
	$(RM) $(PROGRAM)

