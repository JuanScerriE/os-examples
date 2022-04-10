#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Bind to the external signal list.
/* extern const char *const sys_siglist[]; */

// A function pointer typedef.
typedef void (*sig_t)(int);

/* void sigint_handler(int signum) { */
/*   char msg[] = "Trapped SIGINT\n"; */
/*   write(STDOUT_FILENO, msg, sizeof(msg)); */
/* } */

/* void sigkill_handler(int signum) { */
/*   char msg[] = "Trapped SIGKILL\n"; */
/*   write(STDOUT_FILENO, msg, sizeof(msg)); */
/* } */

void sig_handler(int signum) {
  char msg[256];
  snprintf(msg, sizeof(msg), "Trapped %s!\n", strsignal(signum));
  write(STDOUT_FILENO, msg, strnlen(msg, sizeof(msg)));
}

sig_t install_handler(int signum) {
  sig_t old_handler = signal(signum, sig_handler);

  if (old_handler != SIG_ERR)
    printf("Installed %s handler!\n", strsignal(signum));
  else
    printf("Could not install %s handler!\n", strsignal(signum));

  return old_handler;
}

int main(void) {
  sig_t old_sigint = install_handler(SIGINT);
  /* sig_t old_sigsegv = install_handler(SIGSEGV); */
  /* sig_t old_sigkill = install_handler(SIGKILL); */


  sleep(10);
  printf("Restored SIGINT\n");

  signal(SIGINT, old_sigint);

  sleep(10);
  printf("Exiting...\n");

  /* for (;;) { */
    // When we change sigsegv handler it continues looping from
    // the location of the raised exception hence entering into
    // an infinite loop and never actually terminating.
    /* *(int *)0 = 0xDEADBEEF; */
  /* } */

  return EXIT_SUCCESS;
}
