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
                        if(json_object_object_get_ex(extracted_field, "name", &inner_field) &&
                           json_object_get_string_len(inner_field) > 0)
                        {
                                // memory is somehow managed by json library, so no free()ing
                                json_filename = json_object_get_string(inner_field);

                                fprintf(stderr,"fn=%s\n",json_filename);
                                // notify child of new sound file to be played
                                write(pipe_write, json_filename, strlen(json_filename));
                                fprintf(stderr,"post-write\n");
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

