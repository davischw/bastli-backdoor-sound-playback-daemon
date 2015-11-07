

#ifndef _SOUNDSERVER_CHILD_MAIN_H_
#define _SOUNDSERVER_CHILD_MAIN_H_

// libc libraries
#include <stdio.h>
#include <stdlib.h>

// UNIX libraries
#include <unistd.h>

// SDL libraries (sound playback)
#include <SDL/SDL_mixer.h>

// local headers
#include "play_sound.h"


// needs slash at end
const char *sounds_dir = "std_sounds/";

const char *failsound = "std_sounds/jungle2.wav";


int sound_playback_proc_main(int pipe_read);


#endif /* _SOUNDSERVER_CHILD_MAIN_H_ */
