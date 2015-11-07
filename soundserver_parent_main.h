

#ifndef _SOUNDSERVER_PARENT_MAIN_H_
#define _SOUNDSERVER_PARENT_MAIN_H_

// libc libraries
#include <stdio.h>

// UNIX libraries
#include <unistd.h>

// RabbitMQ libraries (messaging)
#include <amqp.h>
#include <amqp_tcp_socket.h>

// json-c library
#include <json-c/json.h>

// local headers
#include "rmq_interface.h"


// needs slash at end
const char *sounds_dir_2 = "std_sounds/";

const char *failsound_2 = "std_sounds/jungle2.wav";


int msg_parse_proc_main(int pipe_write);


#endif /* _SOUNDSERVER_PARENT_MAIN_H_ */
