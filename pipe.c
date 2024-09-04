#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    int fd_track = STDIN_FILENO;

    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            int pipefd[2];

            if (i < argc - 1) {
                if (pipe(pipefd) == -1) {
                    //perror("pipe");
                    exit(EXIT_FAILURE);
                }
            }

            int pid = fork();

            if (pid == 0) {
                // Child process
                if (i > 1) {
                    // If on the second iteration, set input as the last output
                    if (dup2(fd_track, STDIN_FILENO) == -1) {
                        //perror("dup2");
                        exit(EXIT_FAILURE);
                    }
                    close(fd_track);  // Close the old file descriptor
                }

                if (i < argc - 1) {
                    // Redirect output to STDOUT and store output for the next iteration
                    if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
                        //perror("dup2");
                        exit(EXIT_FAILURE);
                    }
                    fd_track = pipefd[0];
                    close(pipefd[0]);  // Close unused read end of the pipe
                }

                close(pipefd[1]);  // Close write end of the pipe in the child

                execlp(argv[i], argv[i], NULL);
                //perror("execlp");
                exit(EXIT_FAILURE);
            } else if (pid > 0) {
                // Parent process
                int new_pid = pid;
                int status = 0;
                if (waitpid(new_pid, &status, 0) == -1) {
    				exit(22);
				}

				if (WIFEXITED(status)) {
    				int exit_status = WEXITSTATUS(status);
    				if (exit_status != 0) {
        				exit(22);
   					}
				} 
				else {
    				exit(EXIT_FAILURE);
				}

                if (i < argc - 1) {
                    close(pipefd[1]);  // Close write end of the pipe in the parent
                }

                // Store the input for the next iteration
                fd_track = pipefd[0];
            } else {
                // Error
                //perror("fork");
                exit(EXIT_FAILURE);
            }
        }
    } else {
        // Not enough arguments
        exit(22);
    }

    return 0;
}
