/* In The Name of Allah */

#include "soundserver_parent_main.h"


int msg_parse_proc_main(int pipe_write)
{
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

        // sound playback variables
        FILE *audiofile = NULL;
        char *filename = NULL;


        // rabbitmq setup
        rmq_setup(&conn);
        
        // end setup rmq



        // TEST SETUP TO READ MESSAGES, NOT FINAL
        while (1)
        {
                amqp_maybe_release_buffers(conn);
                
                // TODO: is envelope allocated even if error below?
                res = amqp_consume_message(conn, &envelope, NULL, 0);
                
                if(AMQP_RESPONSE_NORMAL != res.reply_type)
                {
                        // TODO: log error
                        amqp_destroy_envelope(&envelope);
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
                                        filename = (char *) malloc(strlen(json_filename)+strlen(sounds_dir_2)+1);

                                        if(filename == NULL)
                                        {
                                                fprintf(stderr, "could not allocate memory for filepath string");
                                                exit(11);
                                        }
                                        
                                        strncpy(filename, sounds_dir_2, strlen(sounds_dir_2));
                                        strncpy(filename+strlen(sounds_dir_2), json_filename, strlen(json_filename));
                                        filename[strlen(json_filename)+strlen(sounds_dir_2)] = '\0';

                                        // check whether file exists (and thus filename is valid)
                                        // by trying to read-open it
                                        audiofile = fopen(filename, "r");

                                        // i hope the compiler doesn't optimize this away
                                        if(NULL != audiofile)
                                        {
                                                fclose(audiofile);

                                                write(pipe_write, filename, strlen(filename));
                                        }
                                        else
                                        {
                                                write(pipe_write, failsound_2, strlen(failsound_2));
                                        }
                                        
                                        free(filename);
                                        filename = NULL;
                                }
                                else
                                {
                                        write(pipe_write, failsound_2, strlen(failsound_2));
                                }
                        }
                }
                
                // free the object
                json_object_put(json_msg);
                
                amqp_destroy_envelope(&envelope);
        }


        // free rmq resources
        amqp_connection_close(conn, AMQP_REPLY_SUCCESS);
        
        return 0;
}

