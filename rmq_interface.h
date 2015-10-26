

#ifndef _RMQ_INTERFACE_H_
#define _RMQ_INTERFACE_H_


// for amqp_message_t
#include <amqp.h>

/**
 * converts amqp_bytes_t into a c string.
 * allocates space for a new string.
 * @NOTE caller has to free alloc'd memory after use.
 * @PARAM[in] msg pointer to a amqp bytes object.
 * @RETURN a pointer to the allocated string containing
 * the message string.
 * @RETURN NULL if msg is NULL.
 */
char *amqp_msg_to_str(const amqp_bytes_t *msg);

#endif /* _RMQ_INTERFACE_H_ */
