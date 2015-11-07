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


#include "soundserver_parent_main.h"
#include "soundserver_child_main.h"


#define STATUS_UNCHANGED 0
#define STATUS_ERROR -1


// global to save child status on SIGCHLD
// set to 1 if SIGCHLD is caught
sig_atomic_t child_exited = 0;

// SIGCHLD signal handler
void sigchld_handler(int sig)
{
        child_exited = 1;
        return;
}


// TODO: terminate child if parent has terminated

int main(int argc, char **argv)
{
        // the child process is for sound playback.
        // we store its pid in this variable.
        pid_t sound_playback_proc;

        // pipe to communicate with child
        int pipefd[2];
        if (pipe(pipefd) == -1)
        {
                perror("pipe");
                exit(-1);
        }
        
        // fork to create child process
        sound_playback_proc = fork();

        // if forking failed...
        if(sound_playback_proc == -1)
        {
                perror("fork");
                exit(16);
        }

        if(sound_playback_proc == 0)
        {
                // Close unused write end of pipe
                close(pipefd[1]);
                
                // if the current proc is the child,
                // return if main function returns
                int child_return_code;

                // MAIN FOR CHILD
                child_return_code = sound_playback_proc_main(pipefd[0]);

                // close read end of pipe and return
                close(pipefd[0]);
                return child_return_code;
        }
        else
        {
                // the child's PID is stored in the fork()
                // return value (sound_playback_proc)

                // if the current proc is the parent,
                // save return code of main, signal child to
                // terminate if necessary, wait for child
                // and then return
                int parent_return_code;
                int wait_status;
                pid_t waitpid_value;

                // Close unused read end of pipe
                close(pipefd[0]);
                
                // variable to save signal() return value
                void (*prev_handler)(int);

                //register signal handler for child
                prev_handler = signal(SIGCHLD, sigchld_handler);

                // check whether handler could not be set
                if(SIG_ERR == prev_handler)
                {
                        fprintf(stderr, "could not set signal handler!\n");
                        // try to execute anyway
                }

                
                // MAIN FOR PARENT
                parent_return_code = msg_parse_proc_main(pipefd[1]);

                
                // child terminated and SIGCHLD was caught
                if(child_exited == 1)
                {
                        return parent_return_code;
                }
                
                // nonblocking-wait for the child
                // this should normally return 0 since child does not terminate
                waitpid_value = waitpid(sound_playback_proc, &wait_status, WNOHANG);
                
                // if the child still exists, signal it to
                // terminate and wait for the child
                if(STATUS_UNCHANGED == waitpid_value ||
                   STATUS_ERROR == waitpid_value)
                {
                        // display error cause
                        if(STATUS_ERROR == waitpid_value)
                        {
                                perror("waitpid()");
                                // on error, still try to kill child
                                // so no exit here
                        }
                        
                        // send termination signal
                        if(STATUS_ERROR == kill(sound_playback_proc, SIGTERM))
                        {
                                // ESRCH means the process does not exist anymore
                                // (or is a zombie because not wait()ed for
                                // but this isn't the case because we called
                                // waitpid() above)

                                // if the process does not exist, we just assume it's already terminated and exit normally
                                if(ESRCH != errno)
                                {
                                        fprintf(stderr, "could not kill child process");
                                        perror("kill");
                                        exit(-1);
                                }
                        }
                        else
                        {
                                // wait for the child process to terminate
                                waitpid_value = wait(NULL);
                                if(STATUS_ERROR == waitpid_value)
                                {
                                        perror("wait()");
                                }
                        }
                }
                else
                {
                        // status has changed, so presumably child has terminated already.

                        // check whether child has terminated because of
                        // an uncaught signal
                        if(WIFSIGNALED(wait_status))
                        {
                                
                        }
                        // notice if child didn't terminate, this shouldn't happen normally
                        else if(!WIFEXITED(wait_status))
                        {
                                fprintf(stderr, "child changed status but didn't exit, this should not happen!\n");
                        }
                }

                // close write end of pipe
                close(pipefd[1]);

                // PARENT RETURN
                return parent_return_code;
        }
        //TODO: Parent needs SIGCHLD handler
        //TODO: how to make sure child exits on term signal?
}

