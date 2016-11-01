#ifndef _DEV_COMMON_H
#define _DEV_COMMON_H 

#include <unistd.h>

int dev_get_file_size(const char *path);
int dev_file_exist(const char *file_path);
void dev_delete_file(const char *path);
int dev_make_dir(const char* dir, int mode);
bool dev_is_fifo(const char *file);
bool dev_is_dir(const char *path);
int dev_get_file_size(const char *path);
char * dev_file_mmap(const char *path , int size);

ssize_t writen(int fd, const void *vptr, size_t n);
ssize_t readn(int fd, void *vptr, size_t n);

#endif