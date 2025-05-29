#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

#undef __DEBUG__

#ifdef __DEBUG__
#define debug(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define debug(fmt, ...)
#endif

int main(int argc, char *argv[])
{
  char buf[512] = {0};
  char *sargv[MAXARG];
  int sargc = 0;
  int pid, i;
  int len;

  debug("argc=%d\n", argc);

  for (i = 1; i < argc; i++) {
    sargv[sargc++] = argv[i];
    debug("sargv[%d]=%s\n", sargc-1, sargv[sargc-1]);
  }

  while (1) {
    gets(buf, sizeof(buf));
    if (buf[0] == '\0') {
      break;
    }
    debug("buf=%s, len=%d\n", buf, strlen(buf));

    for (len=strlen(buf)-1; buf[len] == '\r' || buf[len] == '\n'; len--) {
      buf[len] = '\0';
    }
    debug("buf=%s, len=%d\n", buf, strlen(buf));

    sargv[sargc++] = buf;
    sargv[sargc++] = (void *)0;

    pid = fork();
    if (pid > 0) {
      wait((void *)0);
    } else if (pid == 0) {
      exec(sargv[0], sargv);
      exit(0);
    } else {
      printf("Error: fork\n");
      exit(1);
    }
  }

  return 0;
}
