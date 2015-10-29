

#ifndef _PLAY_SOUND_H_
#define _PLAY_SOUND_H_

#include <stdio.h>
#include <stdlib.h> // for exit()

// SDL libraries (sound playback)
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_error.h>

// Globals
#define AUDIO_BUF_SIZE 4096


int play_sound(const char *filename);

#endif /* _PLAY_SOUND_H_ */
