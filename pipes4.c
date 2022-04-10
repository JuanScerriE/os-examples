#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv) {
  int childCount = 16;
  int pipeCount = childCount - 1;

  int fd[2 * pipeCount];
  int *currentFd = fd;
  int *previousFd = NULL;

  for (int stage = 0; stage < childCount; stage++) {
    if (stage == 0) {
      if (pipe(currentFd) == -1) {
        perror("pipe");
        return EXIT_FAILURE;
      }

      pid_t pid = fork();

      if (pid == -1) {
        perror("fork");
        return EXIT_FAILURE;
      } else if (pid == 0) {
        close(currentFd[0]);
        if (dup2(currentFd[1], STDOUT_FILENO) == -1) {
          perror("dup2");
          return EXIT_FAILURE;
        }
        close(currentFd[1]);

        printf("This is the first message!\n");

        return EXIT_SUCCESS;
      }
    } else if (stage < childCount - 1) {
      if (pipe(currentFd) == -1) {
        perror("pipe");
        return EXIT_FAILURE;
      }

      pid_t pid = fork();

      if (pid == -1) {
        perror("fork");
        return EXIT_FAILURE;
      } else if (pid == 0) {
        close(currentFd[0]);
        if (dup2(currentFd[1], STDOUT_FILENO) == -1) {
          perror("dup2");
          return EXIT_FAILURE;
        }
        close(currentFd[1]);

        close(previousFd[1]);
        if (dup2(previousFd[0], STDIN_FILENO) == -1) {
          perror("dup2");
          return EXIT_FAILURE;
        }
        close(previousFd[0]);

        printf("[Stage: %d]\n", stage);
        for (int c = 0; (c = getchar()) != EOF;)
          putchar(c);

        return EXIT_SUCCESS;
      }

      close(previousFd[0]);
      close(previousFd[1]);
    } else {
      pid_t pid = fork();

      if (pid == -1) {
        perror("fork");
        return EXIT_FAILURE;
      } else if (pid == 0) {
        close(previousFd[1]);
        if (dup2(previousFd[0], STDIN_FILENO) == -1) {
          perror("dup2");
          return EXIT_FAILURE;
        }
        close(previousFd[0]);

        printf("[Stage: %d]\n", stage);
        for (int c = 0; (c = getchar()) != EOF;)
          putchar(c);

        return EXIT_SUCCESS;
      }

      close(previousFd[0]);
      close(previousFd[1]);
    }

    previousFd = currentFd;
    currentFd += 2;
  }

  return EXIT_SUCCESS;
}
