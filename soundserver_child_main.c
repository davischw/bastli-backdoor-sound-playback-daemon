/* In The Name of Allah */

#include "soundserver_child_main.h"

const char *failsound = "std_sounds/jungle2.wav";
// needs slash at end
const char *sounds_dir = "std_sounds/";


const char *sounds[] =
{
        "akhi_abligh_hona_alasshab.ogg",
        "habat_ka_sayf.ogg",
        "hebbit_kerrih.ogg",
        "ziegenficker.ogg",
        "qum.ogg",
        "salil_sawarim.ogg",
        "srna.ogg"
        "saraya_dawlati.ogg",
};


int sound_playback_proc_main(int pipe_read)
{
        ssize_t num_bytes_read = 0;

        struct pollfd pfd;
        pfd.fd = pipe_read;
        pfd.events = POLLIN;
        
        // sound playback variables
        FILE *audiofile = NULL;
        char *filename = NULL;

        // lookup3 variable
        uint32_t soundindex = 0;

        char buf[FNBUF_S+1];
        buf[FNBUF_S]='\0';


        while(1)
        {
                num_bytes_read = read(pipe_read, &buf, FNBUF_S);

                if(num_bytes_read == 0)
                {
                        // TODO: use stop/wait?
                        sleep(1);
                        continue;
                }
                
                if(num_bytes_read == 254)
                {
                        //TODO: handle longer filenames
                        fprintf(stderr, "oversize pipe msg\n");
                        exit(-1);
                }

                if(num_bytes_read == -1)
                {
                        fprintf(stderr, "error reading from pipe\n");
                        exit(-1);
                }

                // set terminating nullchar to fix string size to
                // current filename
                buf[num_bytes_read] = '\0';

                // don't play music if something is already playing
                if(Mix_PlayingMusic())
                {
                        continue;                     
                }

                // hash buffer and choose soundfile
                soundindex = hashlittle(buf, strlen(buf), 0) % sizeof(sounds);

                // check whether filename has no directories inside
                if(NULL == strchr(sounds[soundindex], '/') &&
                   NULL == strchr(sounds[soundindex], '\\'))
                {
                        // add directory to filename
                        filename = (char *) malloc(strlen(sounds[soundindex])+strlen(sounds_dir)+1);

                        if(filename == NULL)
                        {
                                fprintf(stderr, "could not allocate memory for filepath string");
                                exit(11);
                        }

                        strncpy(filename, sounds_dir, strlen(sounds_dir));
                        strncpy(filename+strlen(sounds_dir), sounds[soundindex], strlen(sounds[soundindex]));
                        filename[strlen(sounds[soundindex])+strlen(sounds_dir)] = '\0';

                        // testing
                        //printf("sounddir=%s\nbuf=%s\nfilename=%s\n",sounds_dir,buf,filename);
                        
                        // check whether file exists (and thus filename is valid)
                        // by trying to read-open it
                        audiofile = fopen(filename, "r");

                        if(NULL != audiofile)
                        {
                                // i hope the compiler doesn't optimize this check for existing file away
                                fclose(audiofile);

                                // actually play sound
                                // NOTE: play_sound is blocking!
                                if(sound_success != play_sound(filename))
                                {
                                        play_sound(failsound);
                                }
                         }
                        else
                        {
                                play_sound(failsound);
                        }
                                        
                        free(filename);
                        filename = NULL;
                }


                // clear remaining buffer to avoid playing new
                // sounds that we got while playing the sound
                // that just finished
                
                while(poll(&pfd, 1, 0) > 0)
                {
                        if(pfd.revents == POLLIN)
                        {
                                read(pipe_read, &buf,254);
                        }
                        else
                        {
                                // TODO
                                exit(-1);
                        }
                }
        }
}
