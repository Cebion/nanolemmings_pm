PROGRAM = lem
OBJECTS = util.o text.o sound.o game.o draw.o levels.o menu.o main.o
CC = g++


# MY_CFLAGS = -I/usr/local/include/SDL -Dmain=SDL_main -DWIN32 -Uunix -mno-cygwin
MY_CFLAGS = `sdl-config --cflags` -Wall -pedantic -D__LINUX__ -pipe -O3 -ggdb -fsigned-char
#-Os -mthumb

%.o:	%.cpp util.h
	$(CC) -c $(MY_CFLAGS) $< -o $@

all:	$(PROGRAM)

$(PROGRAM):	$(OBJECTS)
#	$(CC) -o$(PROGRAM) $(OBJECTS)  -lSDL -lSDL_image -lSDL_ttf -lSDL_mixer -L./ -L/usr/lib/ -L/lib/ -lSDL_gfx
	$(CC) -o$(PROGRAM) $(OBJECTS) -lSDL -lSDL_image -lSDL_ttf -lSDL_mixer -L./ -lSDL_gfx `sdl-config --libs`
#	strip $(PROGRAM)

clean:
	$(RM) *.o
	$(RM) $(PROGRAM)