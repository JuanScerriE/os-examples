#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
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
  if (argc < 3) {
    fprintf(stderr, "Invalid number of arguments!\n");
    exit(EXIT_FAILURE);
  }

  /* printf("Redirecting input from %s and output to %s\n", argv[1], argv[2]);
   */

  /*
   * This is simple however streams are buffered which means
   * they might introduce latency. Although they do have the
   * benefit of reducing the number of writes or reads
   * especially for small operations. This is because buffers
   * are flushed into their respective file occasionally, for
   * example when they encounter a \n.
   */
  /* FILE *fs_in = freopen(argv[1], "r", stdin); */
  /* FILE *fs_out = freopen(argv[2], "w", stdout); */
  redirect_input(argv[1]);
  redirect_output(argv[2]);

  char leet[] = {'4', '8', 'C', 'D', '3', 'F', '9', 'H', 'i',
                 'J', 'k', 'l', 'm', 'n', '0', 'P', 'Q', 'R',
                 '5', '7', 'U', 'v', 'w', 'x', 'Y', '2'};

  int c = 0;

  while ((c = getchar()) != EOF) {
    if (isalpha(c))
      c = leet[tolower(c) - 'a'];

    putchar(c);
  }

  return 0;
}
