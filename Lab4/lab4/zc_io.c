#include "zc_io.h"
#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdio.h>
#include  <fcntl.h>

// The zc_file struct is analogous to the FILE struct that you get from fopen.
struct zc_file {
    // Insert the fields you need here.
    size_t offsetl;
    sem_t readSem;
    sem_t writeSem;
};

/**************
 * Exercise 1 *
 **************/

zc_file *zc_open(const char *path) {
    // To implement
    int fd = open(path, O_RDWR);
    if (fd < 0){
        printf("\"%s\" could not be opened\n", path);
        exit(-1);
    }

    struct stat statBuf;
    if (fstat(fd, &statBuf) < 0){
        printf("\"%s\" could not be opened\n", path);
        exit(-1);
    }

    char *ptr = mmap(NULL, statBuf.st_size,
        PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if(ptr == MAP_FAILED){
        printf("Mapping Failed\n");
        exit(-1);
    }
    close(fd);

    return NULL;
}

int zc_close(zc_file *file) {
    // To implement
    return -1;
}

const char *zc_read_start(zc_file *file, size_t *size) {
    // To implement
    return NULL;
}

void zc_read_end(zc_file *file) {
    // To implement
}

/**************
 * Exercise 2 *
 **************/

char *zc_write_start(zc_file *file, size_t size) {
    // To implement
    return NULL;
}

void zc_write_end(zc_file *file) {
    // To implement
}

/**************
 * Exercise 3 *
 **************/

off_t zc_lseek(zc_file *file, long offset, int whence) {
    // To implement
    return -1;
}

/**************
 * Exercise 4 *
 **************/

int zc_copyfile(const char *source, const char *dest) {
    // To implement
    return -1;
}
