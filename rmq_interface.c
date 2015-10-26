

#include "rmq_interface.h"

char *amqp_bytes_to_str(const amqp_bytes_t *msg)
{
        char *str = NULL;
        
        if(NULL == msg)
        {
                return NULL;
        }

        str = (char *) malloc((msg->len + 1) * sizeof(char));

        if(NULL == str)
        {
                fprintf(stderr, "Could not allocate %ld bytes. Exiting.\n", (msg->len + 1));
                exit(7);
        }

        memcpy(str, msg->bytes, msg->len);
        str[msg->len] = '\0';

        return str;
}
