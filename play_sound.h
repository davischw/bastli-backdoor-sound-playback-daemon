

#ifndef _PLAY_SOUND_H_
#define _PLAY_SOUND_H_

#include <stdio.h>
#include <stdlib.h> // for exit()

// SDL libraries (sound playback)
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_error.h>

// Globals
#define AUDIO_BUF_SIZE 4096

typedef enum sound_status
{
        sound_success = 0,
        sound_sample_load_fail = 3,
        sound_sample_playback_fail = 4
} sound_status;

int play_sound(const char *filename);

#endif /* _PLAY_SOUND_H_ */
