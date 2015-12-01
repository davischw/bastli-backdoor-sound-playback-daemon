/* In The Name of Allah */

#include "soundserver_child_main.h"

const char *failsound = "std_sounds/jungle2.wav";
// needs slash at end
const char *sounds_dir = "std_sounds/";

#ifdef UMMAH
const char *sounds[] =
{
        "akhi_abligh_hona_alasshab.ogg",
        "habat_ka_sayf.ogg",
        "srna.ogg",
        "ziegenficker.ogg",
        "hebbit_kerrih.ogg",
        "qum.ogg",
        "salil_sawarim.ogg",
        "saraya_dawlati.ogg"
};
#endif


int sound_playback_proc_main(int pipe_read)
{
        // switch parameter that points to the
        // variable where the suggested filename
        // is stored before setting together the path
        // to avoid too many IFDEFs
        const char *fn_parm = NULL;
        
        ssize_t num_bytes_read = 0;

        struct pollfd pfd;
        pfd.fd = pipe_read;
        pfd.events = POLLIN;
        
        // sound playback variables
        FILE *audiofile = NULL;
        char *filename = NULL;

#ifdef UMMAH
        // lookup3 variable
        uint32_t soundindex = 0;
#endif

        char buf[FNBUF_S+1];
        buf[FNBUF_S]='\0';

        // redirect stderr to file
        freopen("debug.txt", "w", stderr);
	setbuf(stderr, NULL);

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


                // make file name operations on correct file name
                // depending on whether UMMAH mode is activated
#ifdef UMMAH
                // hash buffer and choose soundfile from hash
                soundindex = hashlittle(buf, strlen(buf), 0) % sizeof(*sounds);
                fn_parm = sounds[soundindex];
#else /* NOT UMMAH */
                fn_parm = buf;
#endif
                
                // check whether filename has no directories inside
                if(NULL == strchr(fn_parm, '/') &&
                   NULL == strchr(fn_parm, '\\'))
                {
                        // add directory to filename
                        filename = (char *) malloc(strlen(fn_parm)+strlen(sounds_dir)+1);

                        if(filename == NULL)
                        {
                                fprintf(stderr, "could not allocate memory for filepath string");
                                exit(11);
                        }

                        strncpy(filename, sounds_dir, strlen(sounds_dir));
                        strncpy(filename+strlen(sounds_dir), fn_parm, strlen(fn_parm));
                        filename[strlen(fn_parm)+strlen(sounds_dir)] = '\0';

                        // testing
                        //printf("sounddir=%s\nfile parameter=%s\nfilename=%s\n",sounds_dir,fn_parm,filename);
                        
                        // check whether file exists (and thus filename is valid)
                        // by trying to read-open it
                        audiofile = fopen(filename, "r");

                        if(NULL != audiofile)
                        {
                                // i hope the compiler doesn't optimize this check
                                // for existing file away
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
