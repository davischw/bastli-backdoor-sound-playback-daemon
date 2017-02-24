

#ifndef _SOUNDSERVER_PARENT_MAIN_H_
#define _SOUNDSERVER_PARENT_MAIN_H_

// libc libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>

// UNIX libraries
#include <unistd.h>

// RabbitMQ libraries (messaging)
#include <amqp.h>
#include <amqp_tcp_socket.h>

// json-c library
#include <json-c/json.h>

// local headers
#include "rmq_interface.h"

#ifdef UMMAH
// lookup3.c hash function
extern uint32_t hashlittle(const void *key, size_t length, uint32_t initval);
#endif /* UMMAH */

// limits filename length
#define FNBUF_S 254

// string constants

#ifdef UMMAH
// contains an array of existing sound files
extern const char *sounds[];
// sound that is played when something goes wrong
extern const char *failsound;
#endif /* UMMAH */

// directory where sounds are in, needs slash at end
extern const char *sounds_dir;

// string constant switch for UMMAH mod
// in UMMAH mod we take the name and hash it
// otherwise we take the passed filename
extern const char *json_file_param;

int msg_parse_proc_main();


#endif /* _SOUNDSERVER_PARENT_MAIN_H_ */
