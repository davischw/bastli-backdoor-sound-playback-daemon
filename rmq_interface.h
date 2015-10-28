

#ifndef _RMQ_INTERFACE_H_
#define _RMQ_INTERFACE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// RabbitMQ libraries (messaging)
#include <amqp.h>
#include <amqp_tcp_socket.h>



/**
 * converts amqp_bytes_t into a c string.
 * allocates space for a new string.
 * @NOTE caller has to free alloc'd memory after use.
 * @PARAM[in] bytes pointer to a amqp bytes object.
 * @RETURN a pointer to the allocated string containing
 * the message string.
 * @RETURN NULL if msg is NULL.
 */
char *amqp_bytes_to_str(const amqp_bytes_t *bytes);

/**
 * setup everything to call amqp_consume_message()
 * @param[in] conn pointer to the object to save connection
 * status in
 */
void rmq_setup(ampq_connection_state_t *conn);

#endif /* _RMQ_INTERFACE_H_ */
