

#include <fcntl.h>
#include <sys/stat.h>

ssize_t read(int fd, void *buf, size_t count);
// for drivers
struct file_ops {
    int (*open)(int oflag, ...);
    int (*close)(int fd);
    size_t (*read)(int fd, void *buf, size_t count);
    ssize_t (*write)(int fd, const void *buf, size_t count);
    int (*poll)(struct pollfd *fds, nfds_t nfds, int timeout);
    int (*ioctl)(int fd, int request, ... /* arg */);
};

/* file handle */
struct file_def {
    char *f_name;
    int oflag;


};

/* mount point */
struct mount_def {
    const char *mount_path;
    // something to forward the file op to
};

int open(const char *path, int oflag, ...)
{

    return -1;
}

/*
FILE *fopen(const char * restrict filename, const char * restrict mode)
{

}
*/

