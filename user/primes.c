#include "kernel/types.h"
#include "user/user.h"

/**
 * Reference: https://swtch.com/~rsc/thread/
 */

#undef __DEBUG__

#ifdef __DEBUG__
#define debug(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define debug(fmt, ...)
#endif

#define NUM_MIN 2
#define NUM_MAX 280

void primes_sieve(int fd_in)
{
  int ret, prime, num;
  int fds[2], pid;

  if (0 >= read(fd_in, &prime, sizeof(prime)))
      return;
  printf("prime %d\n", prime);

  ret = pipe(fds);
  if (ret < 0) {
    printf("Error: pipe2\n");
    return;
  }
  debug("> [%d] fds=%d,%d\n", getpid(), fds[0], fds[1]);

  pid = fork();
  if (pid > 0) {
    close(fds[0]);
    while (0 < read(fd_in, &num, sizeof(num))) {
      debug("> [%d] read, num=%d\n", getpid(), num);

      if (num % prime == 0)
        continue;

      ret = write(fds[1], &num, sizeof(num));
      if (ret < 0) {
        printf("Error: write2\n");
        exit(1);
      }
    }
    close(fd_in);
    close(fds[1]);
    wait((int *)0);
  } else if (pid == 0) {
    /**
     * Notice: Do NOT forget close(fd_in) in son process
     * Because the number of file descriptors that a single process can open is limited (15 in xv6).
     */
    close(fd_in);
    close(fds[1]);
    /* Recursive calls */
    primes_sieve(fds[0]);
  } else {
    printf("Error: fork2\n");
    exit(1);
  }
}

int main(int argc, char *argv[])
{
  int num, ret;
  int fds[2], pid;

  ret = pipe(fds);
  if (ret < 0) {
    printf("Error: pipe\n");
    exit(1);
  }

  debug("> [%d] fds=%d,%d\n", getpid(), fds[0], fds[1]);

  pid = fork();
  if (pid > 0) {
    close(fds[0]);
    for (num = NUM_MIN; num <= NUM_MAX; num++) {
      ret = write(fds[1], &num, sizeof(num));
      if (ret < 0) {
        printf("Error: write\n");
        exit(1);
      }
      debug("> [%d] write, num=%d\n", getpid(), num);
    }
    close(fds[1]);
    wait((int *)0);
  } else if (pid == 0) {
    close(fds[1]);
    primes_sieve(fds[0]);
  } else {
    printf("Error: fork\n");
    exit(1);
  }

  return 0;
}
