#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc, char *argv[])
{
	// TODO: it's all yours

	int prev = STDIN_FILENO;
  	int fds[2];

  	// Check arguments
  	if (argc <= 1) {
    	exit(EINVAL);
  	}

  	// Failed pipe creation
  	if (pipe(fds) != 0) {
    	exit(EXIT_FAILURE);
  	}

  	// Iterate through arguments
  	for (int i = 1; i < argc; i++) {
    	pid_t pid = fork();

    	// Failed fork
    	if (pid == -1) {
      		exit(EXIT_FAILURE);
    	}

    	// Succeeded fork in child process
   		else if (pid == 0) {
      	// arg reads from standard input
      		dup2(prev, STDIN_FILENO);
      		// Set last arg output to standard output
      		if (i != argc - 1) {
			dup2(fds[1], STDOUT_FILENO);
      		}
      // Execute command
      	execlp(argv[i], argv[i], NULL);

      	return 1;
    	}

    	// Parent process
    	else {
      		int st = 0;
      		waitpid(pid, &st, 0);
      		if (!WIFEXITED(st) || WEXITSTATUS(st) != 0) {
				return WEXITSTATUS(st);
      	}

      	if (prev != STDIN_FILENO) {
			close(prev);
      	}

      	// Reset previous file descriptor
      		prev = fds[0];

      	close(fds[1]);

      	// Create pipe for next arg if exists
      	pipe(fds);
    	}
  	}
  	// Close remaining file descriptors
  	close(fds[0]);
  	close(fds[1]);

  	// Final cleanup
  	if (argc >= 2) {
    	close(prev);
  	}
  	else {
    	return 0;
  	}
}
