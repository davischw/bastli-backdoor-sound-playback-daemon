

#ifndef _SOUNDSERVER_CHILD_MAIN_H_
#define _SOUNDSERVER_CHILD_MAIN_H_

// libc libraries
#include <stdio.h>
#include <stdlib.h>

// UNIX libraries
#include <unistd.h>
#include <poll.h>

// SDL libraries (sound playback)
#include <SDL/SDL_mixer.h>

// local headers
#include "play_sound.h"


// needs slash at end
extern const char *sounds_dir;

extern const char *failsound;


int sound_playback_proc_main(int pipe_read);


#endif /* _SOUNDSERVER_CHILD_MAIN_H_ */
