/* In The Name of Allah */

// libc libraries
#include <stdio.h>

// SDL libraries (sound playback)
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_error.h>

// RabbitMQ libraries (messaging)
#include <amqp.h>
#include <amqp_tcp_socket.h>


#define AUDIO_BUF_SIZE 4096
#define RABBITMQ_VHOST_DEFAULT "/"
#define RABBITMQ_HEARTBEAT_DISABLE 0

#define RABBITMQ_SERVER_HOSTNAME "backdoor.bastli.ch"
#define RABBITMQ_SERVER_PORT 5672
#define RABBITMQ_SERVER_TESTUSER "windows_test"
#define RABBITMQ_SERVER_TESTPW "foobar"


int main()
{
        Mix_Music *sample;
        int audio_playback_initalization_flags = MIX_INIT_FLAC | MIX_INIT_MP3 | MIX_INIT_OGG | MIX_INIT_MOD;

        amqp_connection_state_t conn = amqp_new_connection();
        amqp_socket_t *socket = NULL;
        amqp_rpc_reply_t reply_status;
        int port, status;

        
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

        socket = amqp_tcp_socket_new(conn);
        if(!socket)
        {
                fprintf(stderr,"rabbitmq: error creating TCP socket\n");
        }

        status = amqp_socket_open(socket, RABBITMQ_SERVER_HOSTNAME, RABBITMQ_SERVER_PORT);
        if(status)
        {
                fprintf(stderr,"rabbitmq: error opening TCP socket\n");
        }

        reply_status = amqp_login(
                conn,
                RABBITMQ_VHOST_DEFAULT,
                AMQP_DEFAULT_MAX_CHANNELS,
                AMQP_DEFAULT_FRAME_SIZE,
                RABBITMQ_HEARTBEAT_DISABLE,
                AMQP_SASL_METHOD_PLAIN,
                RABBITMQ_SERVER_TESTUSER,
                RABBITMQ_SERVER_TESTPW);

        if(AMQP_RESPONSE_NORMAL != reply_status.reply_type)
        {
                fprintf(stderr, "rabbitmq: could not login to br0ker, reply_type=%d\n", reply_status.reply_type);
                exit(5);
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

        return 0;
}

