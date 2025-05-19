#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
  int ret = 0, fd = 0;
  int p_f2s[2], p_s2f[2];
  char buf[8] = {0};
  char pingpong = 'a';

  ret = pipe(p_f2s);
  if (ret != 0) {
    printf("Error: pipe f2s");
    exit(1);
  }
  ret = pipe(p_s2f);
  if (ret != 0) {
    printf("Error: pipe s2f");
    exit(1);
  }

  fd = fork();
  if (fd > 0) {
    /* Father */
    close(p_f2s[0]);
    close(p_s2f[1]);

    write(p_f2s[1], &pingpong, sizeof(pingpong));
    ret = read(p_s2f[0], buf, sizeof(buf));
    if (ret > 0) {
      printf("%d: received pong\n", getpid());
    }
  } else if (fd == 0) {
    /* Son */
    close(p_s2f[0]);
    close(p_f2s[1]);

    ret = read(p_f2s[0], buf, sizeof(buf));
    if (ret > 0) {
      printf("%d: received ping\n", getpid());
      write(p_s2f[1], &pingpong, sizeof(pingpong));
    }
  } else {
    /* Error */
    printf("Error: fork");
    exit(1);
  }

  return 0;
}
