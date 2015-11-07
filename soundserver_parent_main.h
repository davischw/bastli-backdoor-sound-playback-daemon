

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

// SDL libraries (sound playback)
#include <SDL/SDL_mixer.h>

// local headers
#include "rmq_interface.h"
#include "play_sound.h"


// needs slash at end
const char *sounds_dir = "std_sounds/";

const char *failsound = "std_sounds/jungle2.wav";


int msg_parse_proc_main();


#endif /* _SOUNDSERVER_PARENT_MAIN_H_ */