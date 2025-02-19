/*

SDL_framerate.c: framerate manager

Copyright (C) 2001-2011  Andreas Schiffler

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

   3. This notice may not be removed or altered from any source
   distribution.

Andreas Schiffler -- aschiffler at ferzkopp dot net

*/

#include "SDL_framerate.h"

/*!
\brief Initialize the framerate manager.

Initialize the framerate manager, set default framerate of 30Hz and
reset delay interpolation.

\param manager Pointer to the framerate manager.
*/
void SDL_initFramerate(FPSmanager * manager)
{
	/*
	* Store some sane values 
	*/
	manager->framecount = 0;
	manager->rate = FPS_DEFAULT;
	manager->rateticks = (1000.0f / (float) FPS_DEFAULT);
	manager->lastticks = SDL_GetTicks();
}

/*!
\brief Set the framerate in Hz 

Sets a new framerate for the manager and reset delay interpolation.
Rate values must be between FPS_LOWER_LIMIT and FPS_UPPER_LIMIT inclusive to be accepted.

\param manager Pointer to the framerate manager.
\param rate The new framerate in Hz (frames per second).

\return 0 for sucess and -1 for error.
*/
int SDL_setFramerate(FPSmanager * manager, Uint32 rate)
{
	if ((rate >= FPS_LOWER_LIMIT) && (rate <= FPS_UPPER_LIMIT)) {
		manager->framecount = 0;
		manager->rate = rate;
		manager->rateticks = (1000.0f / (float) rate);
		return (0);
	} else {
		return (-1);
	}
}

/*!
\brief Return the current target framerate in Hz 

Get the currently set framerate of the manager.

\param manager Pointer to the framerate manager.

\return Current framerate in Hz or -1 for error.
*/
int SDL_getFramerate(FPSmanager * manager)
{
	if (manager == NULL) {
		return (-1);
	} else {
		return ((int)manager->rate);
	}
}

/*!
\brief Return the current framecount.

Get the current framecount from the framerate manager. 
A frame is counted each time SDL_framerateDelay is called.

\param manager Pointer to the framerate manager.

\return Current frame count or -1 for error.
*/
int SDL_getFramecount(FPSmanager * manager)
{
	if (manager == NULL) {
		return (-1);
	} else {
		return ((int)manager->framecount);
	}
}

/*!
\brief Delay execution to maintain a constant framerate and calculate fps.

Generate a delay to accomodate currently set framerate. Call once in the
graphics/rendering loop. If the computer cannot keep up with the rate (i.e.
drawing too slow), the delay is zero and the delay interpolation is reset.

\param manager Pointer to the framerate manager.
*/
int SDL_framerateDelay(FPSmanager * manager)
{
	int current_ticks;
	int target_ticks;
	int the_delay;

	/*
	* No manager, no delay
	*/
	if (manager == NULL)
		return -1;

	/*
	* Initialize uninitialized manager 
	*/
	if (manager->lastticks == 0)
		SDL_initFramerate(manager);

	/*
	* Next frame 
	*/
	manager->framecount++;

	/*
	* Get/calc ticks 
	*/
	current_ticks = SDL_GetTicks();
	target_ticks = manager->lastticks + (Uint32) ((float) manager->framecount * manager->rateticks);

	if (manager->framecount > 100000) {
	    // reset interpolation once in a while, because I'm paranoid
		manager->framecount = 0;
		manager->lastticks = SDL_GetTicks();
	}
	//printf("diff: %i/%i\n",target_ticks - current_ticks,(int) manager->rateticks);
	if (current_ticks <= target_ticks - 15) {
		// well in time, so we can sleep a bit
		// 60Hz = 16ms per frame, so sleep less and let vsync do the rest
		the_delay = target_ticks - current_ticks - 15;
		//printf("delay: %i\n",the_delay);
		SDL_Delay(the_delay);
		return 0;
	} else if (current_ticks < target_ticks + manager->rateticks/3) {
		// too late for the current frame, but we still have time before the
		// next frame has to be there so let's render now and hope we can
		// still render the next one
		return 0;
	} else if (current_ticks < target_ticks + 3*manager->rateticks) {
		// too late even for the next frame, so let's drop a frame now
		return 1;
	} else {
		// way too late, let's drop a frame and reset interpolation
		manager->framecount = 0;
		manager->lastticks = SDL_GetTicks();
		return 2;
	}
}
