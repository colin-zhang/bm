#include <sys/stat.h>
#include <sys/statfs.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>

int 
dev_get_file_size(const char *path)
{  
    int    file_size = -1;      
    struct stat statbuff;  

    if(stat(path, &statbuff) < 0)
    {  
        return file_size;  
    } 
    else
    {  
        file_size = statbuff.st_size;  
        return file_size;  
    }  
}

int 
dev_file_exist(const char *file_path)
{
    struct stat buf;
    int ret = stat(file_path, &buf);
    return (0==ret);
}

bool 
dev_is_dir(const char *path)
{
    struct stat statbuf;
    if(lstat(path, &statbuf) == 0)
    {
        return S_ISDIR(statbuf.st_mode) != 0;
    }
    return false;
}

bool 
dev_is_fifo(const char *file)
{
    struct stat statbuf;
    if(lstat(file,&statbuf) == 0)
    {
        return S_ISFIFO(statbuf.st_mode) != 0;
    }
    return false;
}

bool 
dev_is_file(const char *path)
{
    struct stat statbuf;
    if(lstat(path, &statbuf) == 0)
    {
        return S_ISREG(statbuf.st_mode) != 0;
    }
    return false;
}

bool 
dev_is_special_dir(const char *path)
{
    return strcmp(path, ".") == 0 || strcmp(path, "..") == 0;
}

void 
dev_get_file_path(const char *path, const char *file_name,  char *file_path)
{
    strcpy(file_path, path);
    if(file_path[strlen(path) - 1] != '/')
        strcat(file_path, "/");
    strcat(file_path, file_name);
}

void 
dev_delete_file(const char *path)
{
    DIR* dir;
    char file_path[PATH_MAX];
    struct dirent *dir_info;

    if(dev_is_file(path))
    {
        remove(path);
        return;
    }
    if(dev_is_dir(path))
    {
        if((dir = opendir(path)) == NULL)
        {
            return;
        }
        while((dir_info = readdir(dir)) != NULL)
        {
            dev_get_file_path(path, dir_info->d_name, file_path);
            if(dev_is_special_dir(dir_info->d_name))
            {
                continue;
            }
            dev_delete_file(file_path);
            rmdir(file_path);
        }
        closedir(dir);
    }
    rmdir(path);
}

int 
dev_make_dir(const char* dir, mode_t mode)
{
    const char* p = dir;
    int   len = 0;
    char  tmp[strlen(dir) + 1];

    while((p = strchr(p, '/')) != NULL)
     {
        
        len = p - dir;
        if(len > 0)
        {
            memcpy(tmp, dir, len);
            tmp[len] = '\0';

            if((mkdir(tmp, mode) < 0) && (errno != EEXIST))
            {
                return -3;
            }
        }
        p += 1;
    }

    if((mkdir(dir, mode) < 0) && (errno != EEXIST))
    {
        return -2;
    }
   
    return 0;
}


char *
dev_file_mmap(const char *path , int size)
{
    int fd;  
    void *ptr;

    if((fd = open(path, O_RDWR)) < 0 )   
    {   
        return NULL;  
    }   

    ftruncate(fd, size);
    ptr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);  
    close(fd);
   
    return ptr;
}


//io阻塞时，先获得io的长度再读

ssize_t                     /* Write "n" bytes to a descriptor. */
writen(int fd, const void *vptr, size_t n)
{
    size_t      nleft;
    ssize_t     nwritten;
    const char  *ptr;

    ptr = vptr;
    nleft = n;
    while (nleft > 0) {
        if ( (nwritten = write(fd, ptr, nleft)) <= 0) {
            if (nwritten < 0 && errno == EINTR)
                nwritten = 0;       /* and call write() again */
            else
                return(-1);         /* error */
        }

        nleft -= nwritten;
        ptr   += nwritten;
    }
    return(n);
}


ssize_t                     /* Read "n" bytes from a descriptor. */
readn(int fd, void *vptr, size_t n)
{
    size_t  nleft;
    ssize_t nread;
    char    *ptr;

    ptr = vptr;
    nleft = n;
    while (nleft > 0) {
        if ( (nread = read(fd, ptr, nleft)) < 0) {
            if (errno == EINTR)
                nread = 0;      /* and call read() again */
            else
                return(-1);
        } else if (nread == 0)
            break;              /* EOF */

        nleft -= nread;
        ptr   += nread;
    }
    return(n - nleft);      /* return >= 0 */
}

