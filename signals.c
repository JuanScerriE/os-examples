#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  if (argc < 2) {
    printf("Not enough arguements!\n");
    exit(EXIT_SUCCESS);
  }

  switch (strtol(argv[1], NULL, 10)) {
  case 0:
    printf("Waiting for something to happen!\n");
    for (;;)
      ;
    break;

  case 1:
    printf("Generating illegal memory access\n");
    *(int *)0L = 0xDEADBEEF;
    break;

  case 2:
    printf("Dividing by zero!\n");
    int k = rand(), j = 0;
    int r = k / j;
    break;

  default:
    printf("Unkown option!\n");
  }

  return EXIT_SUCCESS;
}
