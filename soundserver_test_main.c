/* In The Name of Allah */

// libc libraries
#include <stdio.h>

// SDL libraries (sound playback)
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_error.h>

// RabbitMQ libraries (messaging)
#include <amqp.h>
#include <amqp_tcp_socket.h>

// local headers
#include "rmq_interface.h"

#define AUDIO_BUF_SIZE 4096
#define RABBITMQ_VHOST_DEFAULT "/"
#define RABBITMQ_HEARTBEAT_DISABLE 0

#define RABBITMQ_SERVER_HOSTNAME "backdoor.bastli.ch"
#define RABBITMQ_SERVER_PORT 5672
#define RABBITMQ_SERVER_TESTUSER "windows_test"
#define RABBITMQ_SERVER_TESTPW "foobar"
#define RABBITMQ_SERVER_EXCHANGE_NAME "backdoor"
#define RABBITMQ_ROUTING_KEY "lock.open.*"



int main()
{
        Mix_Music *sample;
        int audio_playback_initalization_flags = MIX_INIT_FLAC | MIX_INIT_MP3 | MIX_INIT_OGG | MIX_INIT_MOD;

        // rabbitmq variables
        amqp_connection_state_t conn = amqp_new_connection();
        amqp_socket_t *socket = NULL;
        amqp_rpc_reply_t reply_status;
        int status;
        amqp_channel_t chan = 1;
        amqp_queue_declare_ok_t *r = NULL;
        amqp_bytes_t qname;
        
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

        amqp_channel_open(conn, chan);
        reply_status = amqp_get_rpc_reply(conn);
        if(AMQP_RESPONSE_NORMAL != reply_status.reply_type)
        {
                fprintf(stderr, "rabbitmq: could not open channel, reply_type=%d\n", reply_status.reply_type);
                exit(6);
        }

        // i have no clue what those random 0 and 1 parameters do,
        // they are not documented, but dominik told me it works with those
        r = amqp_queue_declare(conn, chan, amqp_empty_bytes, 0, 0, 0, 1,
                                 amqp_empty_table);
        reply_status = amqp_get_rpc_reply(conn);
        if(AMQP_RESPONSE_NORMAL != reply_status.reply_type)
        {
                fprintf(stderr, "rabbitmq: could not declare queue, reply_type=%d\n", reply_status.reply_type);
                exit(6);
        }

        qname = amqp_bytes_malloc_dup(r->queue);
        if(NULL == qname.bytes)
        {
                fprintf(stderr, "rabbitmq: out of memory while copying queue name\n");
                exit(7);
        }
        
        amqp_queue_bind(conn, chan, qname,
                        amqp_cstring_bytes(RABBITMQ_SERVER_EXCHANGE_NAME),
                        amqp_cstring_bytes(RABBITMQ_ROUTING_KEY),
                        amqp_empty_table);
        reply_status = amqp_get_rpc_reply(conn);
        if(AMQP_RESPONSE_NORMAL != reply_status.reply_type)
        {
                fprintf(stderr, "rabbitmq: could not bind queue, reply_type=%d\n", reply_status.reply_type);
                exit(8);
        }

        // again i have no clue what the parameters do or why i have to call this function
        // in setup but the only documentation is sample code where this line is copied from
        // wtf does consuming basic even mean?
        amqp_basic_consume(conn, chan, qname, amqp_empty_bytes, 0, 1, 0, amqp_empty_table);
        reply_status = amqp_get_rpc_reply(conn);
        if(AMQP_RESPONSE_NORMAL != reply_status.reply_type)
        {
                fprintf(stderr, "rabbitmq: could not consume basic, reply_type=%d\n", reply_status.reply_type);
                exit(9);
        }

        // end setup rmq



        // TEST SETUP TO READ MESSAGES, NOT FINAL
        while (1) {
                amqp_rpc_reply_t res;
                amqp_envelope_t envelope;
                char *str = NULL;

                amqp_maybe_release_buffers(conn);

                res = amqp_consume_message(conn, &envelope, NULL, 0);

                if (AMQP_RESPONSE_NORMAL != res.reply_type) {
                        break;
                }

                printf("Delivery %u, exchange %.*s routingkey %.*s\n",
                       (unsigned) envelope.delivery_tag,
                       (int) envelope.exchange.len, (char *) envelope.exchange.bytes,
                       (int) envelope.routing_key.len, (char *) envelope.routing_key.bytes);

                if (envelope.message.properties._flags & AMQP_BASIC_CONTENT_TYPE_FLAG) {
                        printf("Content-type: %.*s\n",
                               (int) envelope.message.properties.content_type.len,
                               (char *) envelope.message.properties.content_type.bytes);
                }
                printf("----\n");

                str = amqp_bytes_to_str(envelope.message.body);
                printf("body msg:\n%s\nbody len=%d\n%%%%%%%%\n", str, envelope.message.body.len);
                free(str);

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

        return 0;
}

