#include <unistd.h>
#include <errno.h>
#include "io.h"

ssize_t readx(int fd, void *buf, size_t n)
{
        size_t nread = 0;
        int ret;

        if (fd < 0 || !buf) return -1; 
        while (nread < n) {
                if ((ret = read(fd, buf+nread, n-nread)) < 0) {
                        if (errno == EINTR) continue;
                        return -1;
                }
                if (!ret) break;
                nread += ret;
        }
        return nread;
}

ssize_t writex(int fd, const void *buf, size_t n)
{
        size_t nread = 0;
        int ret;

        if (fd < 0 || !buf) return -1; 
        while (nread < n) {
                if ((ret = write(fd, buf+nread, n-nread)) < 0) {
                        if (errno == EINTR) continue;
                        return -1;
                }
                if (!ret) break;
                nread += ret;
        }
        return nread;
}
