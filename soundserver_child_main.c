/* In The Name of Allah */

#include "soundserver_child_main.h"

// needs slash at end
const char *sounds_dir = "std_sounds/";

const char *failsound = "std_sounds/jungle2.wav";

int sound_playback_proc_main(int pipe_read)
{
        ssize_t num_bytes_read = 0;

        struct pollfd pfd;
        pfd.fd = pipe_read;
        pfd.events = POLLIN;
        
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

                buf[num_bytes_read] = '\0';
                



                // don't play music if something is already playing
                if(Mix_PlayingMusic())
                {
                        continue;                        
                }

                // NOTE: play_sound is blocking!
                if(sound_success != play_sound(buf))
                {
                        play_sound(failsound);
                }


                // TODO: add directory in child process?
                
                /*
                // add directory to filename
                filename = (char *) malloc(strlen(json_filename)+strlen(sounds_dir)+1);

                if(filename == NULL)
                {
                        fprintf(stderr, "could not allocate memory for filepath string");
                        exit(11);
                }
                                        
                strncpy(filename, sounds_dir, strlen(sounds_dir));
                strncpy(filename+strlen(sounds_dir), json_filename, strlen(json_filename));
                filename[strlen(json_filename)+strlen(sounds_dir)] = '\0';

                // testing
                //printf("sounddir=%s\njson_filename=%s\nfilename=%s\n",sounds_dir,json_filename,filename);
                                        
                // check whether file exists (and thus filename is valid)
                // by trying to read-open it
                audiofile = fopen(filename, "r");

                // i hope the compiler doesn't optimize this away
                if(NULL != audiofile)
                {
                        fclose(audiofile);
                        if(sound_success != play_sound(filename))
                        {
                                play_sound(failsound);
                        }
                        //break;
                }
                else
                {
                        play_sound(failsound);
                }
                                        
                free(filename);
                filename = NULL;
                */


                
                // clear remaining buffer
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
