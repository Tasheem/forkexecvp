/* Simple program to illustrate the use of fork-exec-wait pattern. 
 * This version uses execvp and command-line arguments to create a new process.
 * To Compile: gcc -Wall forkexecvp.c
 * To Run: ./a.out <command> [args]
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

pid_t pid;

// Invoked when child process completes.
static void sig_child(int signo) {
    int status;

    // Ignore whatever signal is passed as the argument.
    signal(signo, SIG_IGN);
    // Not waiting here, just getting status.
    waitpid(pid, &status, WNOHANG);
    if(WIFEXITED(status)) {
        printf("Child process exited with status = %d\n", WEXITSTATUS(status));
    } else if(signo == SIGINT) {
        printf("Control-C entered!\n");
        // raise(SIGTERM);
    } else if(signo == SIGTSTP) {
        printf("Control-Z entered!\n");
        // raise(SIGTERM);
    } else {
        printf("Child process did not terminate normally!\n");
    }
    fflush(stdout);
    // Restore the signal handler.
    signal(signo, sig_child);
}

int main(int argc, char **argv) {

    if (argc < 2) {
        printf("Usage: %s <command> [args]\n", argv[0]);
        exit(-1);
    }

    pid = fork();
    if (pid == 0) { /* this is child process */
        execvp(argv[1], &argv[1]);
        printf("If you see this statement then execl failed ;-(\n");
        perror("execvp");
        exit(-1);
    } else if (pid > 0) { /* this is the parent process */

        if (signal(SIGTSTP, sig_child) == SIG_ERR) { /* child process terminated normally */
            printf("Unable to catch SIGTSTP\n");
            exit(-1);
        }
        
        if (signal(SIGINT, sig_child) == SIG_ERR) { /* child process terminated normally */
            printf("Unable to catch SIGINT\n");
            exit(-1);
        }

        /*
        if(signal(SIGINT, sig_child) == SIG_ERR) {
            printf("Problem with SIGINT and sig_child func!\n");
        }
        */
        printf("Wait for the child process to terminate\n");
        for( ; ; ) {
            // if(SIGQUIT(status))
            pause();
        }
    } else { /* we have an error */
        perror("fork"); /* use perror to print the system error message */
        exit(EXIT_FAILURE);
    }
    
    printf("[%ld]: Exiting program .....\n", (long)getpid());

    return 0;
}
