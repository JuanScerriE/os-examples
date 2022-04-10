#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv) {
  int child_count = argc - 1;
  int pipe_count = child_count - 1;
  int fd[pipe_count * 2], *current_fd = fd, *previous_fd = NULL;

  char *args[16];
  char arg_buffer[512];

  for (int stage = 0; stage < child_count; stage++) {
    previous_fd = current_fd - 2;

    strcpy(arg_buffer, argv[stage + 1]);
    args[0] = strtok(arg_buffer, " ");
    for (int token = 1; (args[token] = strtok(NULL, " ")) != NULL; token++)
      ;

    if (stage < child_count - 1) {
      pipe(current_fd);
    }

    pid_t pid = fork();
    if (pid == -1) {
      perror("fork");
      return EXIT_FAILURE;
    } else if (pid == 0) {
      if (stage < child_count - 1) {
        close(current_fd[0]);
        dup2(current_fd[1], STDOUT_FILENO);
        close(current_fd[1]);
      }

      if (stage >= 1) {
        close(previous_fd[1]);
        dup2(previous_fd[0], STDIN_FILENO);
        close(previous_fd[0]);
      }

      if (execvp(args[0], args) == -1) {
        perror("execvp");
        return EXIT_FAILURE;
      }

      return EXIT_SUCCESS;
    }

    if (stage >= 1) {
      close(previous_fd[0]);
      close(previous_fd[1]);
    }

    current_fd += 2;
  }

  return EXIT_SUCCESS;
}
