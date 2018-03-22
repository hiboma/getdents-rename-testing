#define _GNU_SOURCE
#include <dirent.h>     /* Defines DT_* constants */
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <errno.h>  

#define handle_error(msg) \
        do { perror(msg); exit(EXIT_FAILURE); } while (0)

struct linux_dirent {
    long           d_ino;
    off_t          d_off;
    unsigned short d_reclen;
    char           d_name[];
};

void change_name(char *prefix, struct linux_dirent * d) {
    char old[PATH_MAX+1];
    char new[PATH_MAX+1];

    sprintf(old, "%s/%s",  prefix, d->d_name);
    sprintf(new, "%s/%s+", prefix, d->d_name);
    if (rename(old, new) == -1)
      handle_error("rename");

    struct stat st;
    if (stat(new, &st) == -1)
      handle_error("stat");

    fprintf(stderr ,"rename(2): %-10ld -> %-10ld : %-20s -> %-20s\n", d->d_ino, st.st_ino, old, new);
} 

static int buffer_size = 32;

int
main(int argc, char *argv[])
{
    char *dir = argv[1];

    char *buf = malloc(buffer_size);
    if (buf == NULL)
        handle_error("malloc");
  
    int fd = open(argc > 1 ? argv[1] : ".", O_RDONLY | O_DIRECTORY);
    if (fd == -1)
        handle_error("open");

    for ( ; ; ) {
        int nread = syscall(SYS_getdents, fd, buf, buffer_size);
        if (nread == -1) {

            if (errno == EINVAL) { 
              fprintf(stderr, "getdents(2) returned EINVAL. Try to allocate a bigger buffer\n");
              buffer_size++;
	      buf = realloc(buf, buffer_size);
              if (buf == NULL)
                  handle_error("realloc");

              usleep(1000 * 50);
              continue;
            }  

            handle_error("getdents");
        }

        if (nread == 0) { 
            fprintf(stderr, "[DONE] getdents(2) returned 0\n");
            break;
        }

        for (int bpos = 0; bpos < nread;) {
            struct linux_dirent *d;
            d = (struct linux_dirent *) (buf + bpos);

            bpos += d->d_reclen;
            if (d->d_name[0] == '.')  
              continue;

            change_name(dir, d);
            //usleep(1000 * 50);
        }
    }

    exit(EXIT_SUCCESS);
}
