/* In The Name of Allah */

// libc libraries
#include <stdio.h>

// SDL libraries (sound playback)
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_error.h>

// RabbitMQ libraries (messaging)
#include <amqp.h>
#include <amqp_tcp_socket.h>

// json-c library
#include <json-c/json.h>

// local headers
#include "rmq_interface.h"

// Globals
#define AUDIO_BUF_SIZE 4096


// needs slash at end
const char *sounds_dir = "std_sounds/";


int main()
{
        // SDL audio variables
        Mix_Music *sample;
        int audio_playback_initalization_flags = MIX_INIT_FLAC | MIX_INIT_MP3 | MIX_INIT_OGG | MIX_INIT_MOD;

        // rabbitmq variables
        amqp_connection_state_t conn = NULL;
        amqp_rpc_reply_t res;
        amqp_envelope_t envelope;

        // json-c variables
        json_object *json_msg = NULL;
        json_object *extracted_field = NULL;
        json_object *inner_field = NULL;
        char *msg_cstr = NULL;
        const char *json_filename = NULL;
        char *filename = NULL;
        FILE *audiofile = NULL;

        // SDL mix audio setup

        if(Mix_Init(audio_playback_initalization_flags) != audio_playback_initalization_flags)
        {
                fprintf(stderr,"Could not initialize SDL mixer, error message: %s\n",Mix_GetError());
                exit(1);
        }

        if(Mix_OpenAudio(MIX_DEFAULT_FREQUENCY,MIX_DEFAULT_FORMAT,2,AUDIO_BUF_SIZE)<0)
        {
                fprintf(stderr,"Could not open SDL mixer audio channels, error message: %s\n",Mix_GetError());
                exit(2);
        }

        Mix_VolumeMusic(SDL_MIX_MAXVOLUME);


        // rabbitmq setup
        rmq_setup(&conn);
        
        // end setup rmq



        // TEST SETUP TO READ MESSAGES, NOT FINAL
        while (1) {
                amqp_maybe_release_buffers(conn);
                
                // TODO: is envelope allocated even if error below?
                res = amqp_consume_message(conn, &envelope, NULL, 0);
                
                if (AMQP_RESPONSE_NORMAL != res.reply_type)
                {
                        // TODO: log error
                        continue;
                }
                
                
                // read from message into json object
                msg_cstr = amqp_bytes_to_str(&(envelope.message.body));
                json_msg = json_tokener_parse(msg_cstr);
                free(msg_cstr);
                msg_cstr = NULL;
                
                // search for a filename field
                if(json_object_object_get_ex(json_msg, "cmd", &extracted_field))
                {
                        if(json_object_object_get_ex(extracted_field, "sound", &inner_field) &&
                           json_object_get_string_len(inner_field) > 0)
                        {
                                // memory is somehow managed by json library, so no free()ing
                                json_filename = json_object_get_string(inner_field);
                                
                                // check whether filename has no directories inside
                                if(NULL == strchr(json_filename, '/') &&
                                   NULL == strchr(json_filename, '\\'))
                                {
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

                                        //testing
                                        printf("sounddir=%s\njson_filename=%s\nfilename=%s\n",sounds_dir,json_filename,filename);
                                        
                                        // check whether file exists (and thus filename is valid)
                                        // by trying to read-open it
                                        audiofile = fopen(filename, "r");

                                        // i hope the compiler doesn't optimize this away
                                        if(NULL != audiofile)
                                        {
                                                fclose(audiofile);
                                                // TODO
                                                //play_music(filename);
                                                break;
                                        }
                                        else
                                        {
                                                // TODO
                                                //play_music(fail);
                                        }
                                        
                                        free(filename);
                                        filename = NULL;
                                }
                                else
                                {
                                        // TODO
                                        //play_music(fail);
                                }
                        }
                }
                
                // free the object
                json_object_put(json_msg);
                
                amqp_destroy_envelope(&envelope);
        }


        sample=Mix_LoadMUS("std_sounds/ziegenficker.ogg");
        if(!sample)
        {
                fprintf(stderr, "error: %s\n", Mix_GetError());
                exit(3);
        }


        if(Mix_PlayMusic(sample, 1)==-1)
        {
                exit(4);
        }

        while(Mix_PlayingMusic());

        Mix_FreeMusic(sample);

        Mix_CloseAudio();
        Mix_Quit();

        // free rmq resources
        amqp_connection_close(conn, AMQP_REPLY_SUCCESS);
        
        return 0;
}

