#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
  if (argc != 2) {
    printf("Error: parameter error.\n");
    printf("Usage: %s <sec>\n", argv[0]);
    exit(1);
  }

  int sec = atoi(argv[1]);
  return sleep(sec);
}
