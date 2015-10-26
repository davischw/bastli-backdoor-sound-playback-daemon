

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
                exit(7);
        }

        memcpy(str, bytes->bytes, bytes->len);
        str[bytes->len] = '\0';

        return str;
}
