#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "user/user.h"

void find(char *path, char *file)
{
  char buf[512] = {0}, *p;
  int fd;
  struct dirent de;
  struct stat st;

  if ((fd = open(path, O_RDONLY)) < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot stat %s\n", path);
    goto err;
  }

  if(st.type != T_DIR) {
    fprintf(2, "find: path itn't a directory %s\n", path);
    goto err;
  }

  if(strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
    fprintf(2, "find: path too long\n");
    goto err;
  }

  strcpy(buf, path);
  p = buf+strlen(buf);
  *p++ = '/';

  while(read(fd, &de, sizeof(de)) == sizeof(de)){
    if(de.inum == 0 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) {
      continue;
    }
    strcpy(p, de.name);

    //printf(">>> de.name=%s, buf=%s\n", de.name, buf);

    if(stat(buf, &st) < 0){
      fprintf(2, "find: cannot stat %s\n", buf);
      continue;
    }

    switch(st.type) {
    case T_DEVICE:
    case T_FILE:
      if (strcmp(de.name, file) == 0)
        printf("%s\n", buf);
      break;
    case T_DIR:
      find(buf, file);
      break;
    }
  }

err:
  close(fd);
}

int main(int argc, char *argv[])
{
  if (argc < 3) {
    fprintf(2, "Usage: %s <dir> <file>\n", argv[0]);
    exit(1);
  }

  find(argv[1], argv[2]);

  exit(0);
}
