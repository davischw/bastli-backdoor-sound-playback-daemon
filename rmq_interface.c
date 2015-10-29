

#include "rmq_interface.h"

char *amqp_bytes_to_str(const amqp_bytes_t *bytes)
{
        char *str = NULL;
        
        if(NULL == bytes)
        {
                return NULL;
        }

        str = (char *) malloc((bytes->len + 1) * sizeof(char));

        if(NULL == str)
        {
                fprintf(stderr, "Could not allocate %zu bytes. Exiting.\n", (bytes->len + 1));
                exit(10);
        }

        memcpy(str, bytes->bytes, bytes->len);
        str[bytes->len] = '\0';

        return str;
}

void rmq_setup(amqp_connection_state_t *conn)
{
        amqp_socket_t *socket = NULL;
        amqp_rpc_reply_t reply_status;
        int status;
        amqp_channel_t chan = 1;
        amqp_queue_declare_ok_t *r = NULL;
        amqp_bytes_t qname;

        *conn = amqp_new_connection();
        
        socket = amqp_tcp_socket_new(*conn);
        if(!socket)
        {
                fprintf(stderr,"rabbitmq: error creating TCP socket\n");
                exit(3);
        }

        status = amqp_socket_open(socket, RABBITMQ_SERVER_HOSTNAME, RABBITMQ_SERVER_PORT);
        if(status)
        {
                fprintf(stderr,"rabbitmq: error opening TCP socket\n");
                exit(4);
        }

        reply_status = amqp_login(
                *conn,
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

        amqp_channel_open(*conn, chan);
        reply_status = amqp_get_rpc_reply(*conn);
        if(AMQP_RESPONSE_NORMAL != reply_status.reply_type)
        {
                fprintf(stderr, "rabbitmq: could not open channel, reply_type=%d\n", reply_status.reply_type);
                exit(6);
        }

        // i have no clue what those random 0 and 1 parameters do,
        // they are not documented, but dominik told me it works with those
        r = amqp_queue_declare(*conn, chan, amqp_empty_bytes, 0, 0, 0, 1,
                                 amqp_empty_table);
        reply_status = amqp_get_rpc_reply(*conn);
        if(AMQP_RESPONSE_NORMAL != reply_status.reply_type)
        {
                fprintf(stderr, "rabbitmq: could not declare queue, reply_type=%d\n", reply_status.reply_type);
                exit(6);
        }

        // i think r does not need to be free()d, but undocumented...
        // todo: how is qname object destroyed?
        qname = amqp_bytes_malloc_dup(r->queue);
        if(NULL == qname.bytes)
        {
                fprintf(stderr, "rabbitmq: out of memory while copying queue name\n");
                exit(7);
        }
        
        amqp_queue_bind(*conn, chan, qname,
                        amqp_cstring_bytes(RABBITMQ_SERVER_EXCHANGE_NAME),
                        amqp_cstring_bytes(RABBITMQ_ROUTING_KEY),
                        amqp_empty_table);
        reply_status = amqp_get_rpc_reply(*conn);
        if(AMQP_RESPONSE_NORMAL != reply_status.reply_type)
        {
                fprintf(stderr, "rabbitmq: could not bind queue, reply_type=%d\n", reply_status.reply_type);
                exit(8);
        }

        // again i have no clue what the parameters do or why i have to call this function
        // in setup but the only documentation is sample code where this line is copied from
        // wtf does consuming basic even mean?
        amqp_basic_consume(*conn, chan, qname, amqp_empty_bytes, 0, 1, 0, amqp_empty_table);
        reply_status = amqp_get_rpc_reply(*conn);
        if(AMQP_RESPONSE_NORMAL != reply_status.reply_type)
        {
                fprintf(stderr, "rabbitmq: could not consume basic, reply_type=%d\n", reply_status.reply_type);
                exit(9);
        }

        // free qname because not needed (?) anymore
        amqp_bytes_free(qname);
}
