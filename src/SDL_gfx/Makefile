CC = $(CROSS_COMPILE)gcc
CFLAGS += -O3 -fgcse -fearly-inlining -ffinite-math-only -ggdb -fPIC -mcpu=cortex-a8 -mtune=cortex-a8 -mfpu=neon -mfloat-abi=softfp `sdl-config --cflags` -Wall
#CFLAGS += -ftree-vectorize -funsafe-loop-optimizations -funsafe-math-optimizations

#OBJS = SDL_framerate.o SDL_gfxBlitFunc.o SDL_gfxPrimitives.o SDL_imageFilter.o SDL_rotozoom.o neon_helper.o
OBJS = SDL_gfxPrimitives.o neon_helper.o SDL_framerate.o

all: libmySDL_gfx.so

libmySDL_gfx.so: $(OBJS)
	$(CC) -shared -o $@ $^ `sdl-config --libs`

clean:
	$(RM) $(OBJS) libmySDL_gfx.so

%.o: %.S
	$(CC) $(CFLAGS) -c -o $@ $<
