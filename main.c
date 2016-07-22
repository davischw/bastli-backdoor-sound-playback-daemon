/* In The Name of Allah */

// libc libraries
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

// UNIX libraries, POSIX.1-2001 conforming
#include <unistd.h>
// both for waitpid
#include <sys/types.h>
#include <sys/wait.h>
// for kill() and signal()
#include <signal.h>

#include "libvlc_playback.h"
#include "soundserver_parent_main.h"



// TODO: terminate child if parent has terminated

int main(int argc, char **argv)
{
	libvlc_init();
	msg_parse_proc_main();
	libvlc_deinit();
}

