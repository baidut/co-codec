#ifndef IO_H
#define IO_H

ssize_t readx(int fd, void *buf, size_t n);
ssize_t writex(int fd, const void *buf, size_t n);

#endif // io.h
