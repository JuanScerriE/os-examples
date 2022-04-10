#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define RD 0
#define WR 1

int main(int argc, char **argv) {
  int fd[2];

  if (pipe(fd) == -1) {
    perror("pipe");
    return EXIT_FAILURE;
  }

  pid_t pid = fork();

  if (pid == -1) {
    perror("fork");
    return EXIT_FAILURE;
  } else if (pid == 0) { // Child.
    close(fd[WR]);
    // What we are going to do knoiw is rewire STDIN_FILENO to
    // be connected to the read-end of the pipe.
    if (dup2(fd[RD], STDIN_FILENO) == -1) {
      perror("dup2");
      return EXIT_FAILURE;
    }
    close(fd[RD]);

    /* char rec_buf[256]; */
    /* int bytes_read = read(fd[RD], rec_buf, sizeof(rec_buf) - 1); */
    /* rec_buf[bytes_read] = '\0'; */

    /* printf("Receive message: [%s]\n", rec_buf); */

    for (int c = 0; (c = getchar()) != EOF;) {
      putchar(toupper(c));
    }
  } else {
    close(fd[RD]);
    // What we are going to do know is rewire STDOUT_FILENO to
    // be connected to the write-end of the pipe.
    if (dup2(fd[WR], STDOUT_FILENO) == -1) {
      perror("dup2");
      return EXIT_FAILURE;
    }
    close(fd[WR]);

    char msg[] = "I am not weird, I'm just different.\n";

    /* write(fd[WR], msg, sizeof(msg)); */
    fprintf(stdout, "%s", msg);
  }

  return EXIT_SUCCESS;
}

#undef WR
#undef RD
