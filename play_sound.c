/* In The Name of Allah */

#include "play_sound.h"

int play_sound(const char *filename)
{
        // SDL audio variables
        Mix_Music *sample;
        int audio_playback_initalization_flags = MIX_INIT_FLAC | MIX_INIT_MP3 | MIX_INIT_OGG | MIX_INIT_MOD;


        // SDL mix audio setup

        if(Mix_Init(audio_playback_initalization_flags) != audio_playback_initalization_flags)
        {
                fprintf(stderr,"Could not initialize SDL mixer, error message: %s\n",Mix_GetError());
                exit(1);
        }

        if(Mix_OpenAudio(MIX_DEFAULT_FREQUENCY,MIX_DEFAULT_FORMAT,2,AUDIO_BUF_SIZE)<0)
        {
                fprintf(stderr,"Could not open SDL mixer audio channels, error message: %s\n",Mix_GetError());
                Mix_Quit();
                exit(2);
        }

        Mix_VolumeMusic(SDL_MIX_MAXVOLUME);

        // end setup

        sample=Mix_LoadMUS(filename);
        if(!sample)
        {
                fprintf(stderr, "error: %s\n", Mix_GetError());
                Mix_CloseAudio();
                Mix_Quit();
                return sound_sample_load_fail;
        }


        if(Mix_PlayMusic(sample, 1)==-1)
        {
                Mix_FreeMusic(sample);
                Mix_CloseAudio();
                Mix_Quit();
                return sound_sample_playback_fail;
        }

        // NOTE: blocking here
        while(Mix_PlayingMusic());

        Mix_FreeMusic(sample);

        Mix_CloseAudio();
        Mix_Quit();

        return sound_success;
}

