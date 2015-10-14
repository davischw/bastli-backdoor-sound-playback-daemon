#include <SDL/SDL_mixer.h>
#include <SDL/SDL_error.h>
#include <stdio.h>


int main()
{
        int audio_playback_initalization_flags = MIX_INIT_FLAC | MIX_INIT_MP3 | MIX_INIT_OGG | MIX_INIT_MOD;
        if(Mix_Init(audio_playback_initalization_flags) != audio_playback_initalization_flags)
        {
                exit(1);
        }

        if(Mix_OpenAudio(MIX_DEFAULT_FREQUENCY,MIX_DEFAULT_FORMAT,2,4096)<0)
        {
                exit(2);
        }

        Mix_Music *sample;

        sample=Mix_LoadMUS("/home/bastli/soundserver/back_door.wav");
        if(!sample)
        {
                fprintf(stderr, "error: %s\n", Mix_GetError());
                exit(3);
        }

        
        Mix_VolumeMusic(SDL_MIX_MAXVOLUME);
        if(Mix_PlayMusic(sample, 1)==-1)
        {
                exit(4);
        }

        while(Mix_PlayingMusic());

        Mix_FreeMusic(sample);
        Mix_CloseAudio();
        Mix_Quit();

        return 0;
}

