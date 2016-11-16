#include <dirent.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/file.h>
#include <fcntl.h>

int
dev_check_pid(const char *daemon_name) 
{
    int pid = 0;
    char path[256] = {0};

    snprintf(path, sizeof(path), "/run/uware_%s.pid", daemon_name);
    FILE *f = fopen(path, "r");
    if (f == NULL)
        return 0;
    int n = fscanf(f, "%d", &pid);
    fclose(f);

    if (n !=1 || pid == 0 || pid == getpid()) {
        return 0;
    }

    if (kill(pid, 0) && errno == ESRCH)
        return 0;

    return pid;
}

static int
dev_pid_output(const char *daemon_name) 
{
    FILE *f;
    int pid = 0;
    char path[256] = {0};
    snprintf(path, sizeof(path), "/run/uware_%s.pid", daemon_name);
    int fd = open(path, O_RDWR|O_CREAT, 0644);
    if (fd == -1) {
        fprintf(stderr, "can't create %s.\n", path);
        return 0;
    }
    f = fdopen(fd, "r+");
    if (f == NULL) {
        fprintf(stderr, "can't open %s.\n", path);
        return 0;
    }

    if (flock(fd, LOCK_EX|LOCK_NB) == -1) {
        int n = fscanf(f, "%d", &pid);
        fclose(f);
        if (n != 1) {
            fprintf(stderr, "can't lock and read path.\n");
        } else {
            fprintf(stderr, "can't lock path, lock is held by pid %d.\n", pid);
        }
        return 0;
    }

    pid = getpid();
    if (!fprintf(f,"%d\n", pid)) {
        fprintf(stderr, "can't write pid.\n");
        close(fd);
        return 0;
    }
    fflush(f);

    return pid;
}

int
dev_daemon_init(const char *daemon_name)
{
    int pid = dev_check_pid(daemon_name);

    if (pid) {
        return 1;
    }

/*    if (daemon(1, 1)) {
        fprintf(stderr, "can't daemonize.\n");
        return 1;
    }
*/
    pid = dev_pid_output(daemon_name);
    if (pid == 0) {
        return 1;
    }

    return 0;
}


int 
dev_find_proc_pid(const char *proc_name)
{
    DIR *dir = NULL;
    struct dirent *next;
    FILE *fp;
    char filename[256];
    char buffer[1024];
    char name[128];
    int first_pid = 0;

    dir = opendir("/proc");
    if (!dir)
        return -1;     
    while ((next = readdir(dir)) != NULL) {
        if (strcmp(next->d_name, "..") == 0)
            continue;
        if (!isdigit(*next->d_name))
            continue;

        snprintf(filename, sizeof(filename), "/proc/%s/status", next->d_name);
        if (!(fp = fopen(filename, "r"))) {
            continue;
        }
        if (fgets(buffer, sizeof(buffer), fp) == NULL) {
            fclose(fp);
            continue;
        }
        fclose(fp);
        sscanf(buffer, "%*s %s", name);
        if (strcmp(name, proc_name) == 0) {
            if (first_pid == 0) {
                first_pid = atoi(next->d_name);
            } else {
                if (dir) {
                    closedir(dir);
                }
                return first_pid;
            }       
        }
    }
    if (dir) {
        closedir(dir);
    }
    return 0;
}
