#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

/*
 * We can however implement a more granular way of doing this by
 * interacting with lower level calls.
 */

int reopen(int fd, char *path, int flags, mode_t mode) {
  /*
   * Open a file in the process creating a file descriptor. This
   * requires updating both the file descriptor table of the
   * process and the open file table.
   */
  int open_fd = open(path, flags, mode);
  if (open_fd == fd || open_fd < 0)
    return open_fd;

  /*
   * The fd will be updated to point to the same element in the
   * open file table as open_fd. Obviously, the element in the
   * open file table will be updated as well to realise that is
   * has two active pointers.
   */
  int dup_fd = dup2(open_fd, fd);

  /*
   * We close open_fd because we do not need the file descriptor
   * any more.
   */
  close(open_fd);

  return dup_fd == -1 ? dup_fd : fd;
}

int redirect_input(char *input) {
  printf("Redirecting input from [%s]\n", input);
  return reopen(STDIN_FILENO, input, O_RDONLY, S_IRUSR);
}

int redirect_output(char *output) {
  printf("Redirecting output to [%s]\n", output);
  return reopen(STDOUT_FILENO, output, O_RDWR | O_CREAT | O_TRUNC,
                S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
}

int main(int argc, char **argv) {
  if (argc < 4) {
    fprintf(stderr, "Invalid number of arguments!\n");
    exit(EXIT_FAILURE);
  }

  /*
   * This is simple however streams are buffered which means
   * they might introduce latency. Although they do have the
   * benefit of reducing the number of writes or reads
   * especially for small operations. This is because buffers
   * are flushed into their respective file occasionally, for
   * example when they encounter a \n.
   */

  pid_t pid = fork();

  if (pid == -1) { // Fork error.
    perror("fork");
    exit(EXIT_FAILURE);
  } else if (pid == 0) { // Child.
    redirect_input(argv[2]);
    redirect_output(argv[3]);

    if (execvp(argv[1], argv + 1) == -1) {
      perror("execvp");
      exit(EXIT_FAILURE);
    }
  } // Parent

  printf("Executing: [%s < %s > %s]\n", argv[1], argv[2], argv[3]);

  int status = 0;

  if (waitpid(pid, &status, 0) == -1) {
    perror("waitpid");
    exit(EXIT_FAILURE);
  }

  if (WIFEXITED(status)) {
    printf("Command terminated with status: [%d]\n", WEXITSTATUS(status));
  } else {
    printf("Command terminated abnormally!\n");
  }

  return 0;
}
