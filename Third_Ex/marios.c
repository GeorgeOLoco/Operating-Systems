/**
 * \file mario.c
 * \author Aggelos Kolaitis <neoaggelos@gmail.com>
 * \date 2020-05-04
 * \brief Example with pipes and input selection
 *
 * This program demonstrates the usage of select() for reading from multiple
 * file descriptors without blocking on either one.
 *
 * The initial process creates an unnamed pipe, and forks a child process. The
 * child process simply writes random number on the pipe. The father process
 * checks for input from both the pipe and the standard input.
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define WHITE "\033[37m"
#define MAX(a, b) ((a) > (b) ? (a) : (b))


int main(int argc, char** argv) {
    // Create pipe, before fork!
    int pd[2];
    if (pipe(pd) != 0) {
        perror("pipe");
    }

    // Fork child
    pid_t pid = fork();
    if (pid == 0) {
        int value = 1000;

        // close pipe read fd, we won't need it
        close(pd[0]);

        for (int i = 0; i < 4; i++) {
            // write one integer to pipe every 5 seconds
            write(pd[1], &value, sizeof(int));
            sleep(5);
        }

        // close pipe write fd and exit()
        close(pd[1]);
        exit(0);
    }

    while (1) {
        fd_set inset;
        int maxfd;

        FD_ZERO(&inset);                // we must initialize before each call to select
        FD_SET(STDIN_FILENO, &inset);   // select will check for input from stdin
        FD_SET(pd[0], &inset);          // select will check for input from pipe

        // select only considers file descriptors that are smaller than maxfd
        maxfd = MAX(STDIN_FILENO, pd[0]) + 1;

        // wait until any of the input file descriptors are ready to receive
        int ready_fds = select(maxfd, &inset, NULL, NULL, NULL);
        if (ready_fds <= 0) {
            perror("select");
            continue;                                       // just try again
        }

        // user has typed something, we can read from stdin without blocking
        if (FD_ISSET(STDIN_FILENO, &inset)) {
            char buffer[101];
            int n_read = read(STDIN_FILENO, buffer, 100);   // error checking!
            buffer[n_read] = '\0';                          // why?

            // New-line is also read from the stream, discard it.
            if (n_read > 0 && buffer[n_read-1] == '\n') {
                buffer[n_read-1] = '\0';
            }

            printf(BLUE"Got user input: '%s'"WHITE"\n", buffer);

            if (n_read >= 4 && strncmp(buffer, "exit", 4) == 0) {
                // user typed 'exit', kill child and exit properly
                kill(pid, SIGTERM);                         // error checking!
                wait(NULL);                                 // error checking!
                exit(0);
            }
        }

        // someone has written bytes to the pipe, we can read without blocking
        if (FD_ISSET(pd[0], &inset)) {
            int val;
            read(pd[0], &val, sizeof(int));                 // error checking!

            printf(MAGENTA"Got input from pipe: '%d'"WHITE"\n", val);
        }
    }
}
