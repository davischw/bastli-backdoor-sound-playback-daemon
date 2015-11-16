

#ifndef _SOUNDSERVER_CHILD_MAIN_H_
#define _SOUNDSERVER_CHILD_MAIN_H_

// libc libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

// UNIX libraries
#include <unistd.h>
#include <poll.h>

// SDL libraries (sound playback)
#include <SDL/SDL_mixer.h>

// local headers
#include "play_sound.h"

// lookup3.c hash function
extern uint32_t hashlittle(const void *key, size_t length, uint32_t initval);


// limits filename length
#define FNBUF_S 254


// string constants

// contains an array of existing sound files
extern const char *sounds[];
// sound that is played when something goes wrong
extern const char *failsound;
// directory where sounds are in, needs slash at end
extern const char *sounds_dir;


int sound_playback_proc_main(int pipe_read);


#endif /* _SOUNDSERVER_CHILD_MAIN_H_ */
