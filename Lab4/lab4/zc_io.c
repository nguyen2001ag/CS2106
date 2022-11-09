#include "zc_io.h"
#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdio.h>
#include  <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


// The zc_file struct is analogous to the FILE struct that you get from fopen.
struct zc_file {
    // Insert the fields you need here.

    // File attributes
    void* ptr; // Pointer to file system
    off_t fileSize;
    off_t offsetl;
    int fd;

    // Reader-writer synchronization parameters
    int nReaders;
    sem_t mutex;
    sem_t roomEmpty;
    sem_t readMutex;
};

/**************
 * Exercise 1 *
 **************/

zc_file *zc_open(const char *path) {
    // To implement
    int fd = open(path, O_RDWR | O_CREAT , S_IRUSR | S_IWUSR);
    if (fd < 0){
        printf("\"%s\" could not be opened\n", path);
        return NULL;
    }

    struct stat statBuf;
    if (fstat(fd, &statBuf) < 0){
        printf("\"%s\" could not be opened\n", path);
        return NULL;
    }

    off_t fileSize = statBuf.st_size;
    void *ptr;
    if (fileSize > 0){
        ptr = mmap(NULL, fileSize,
            PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    }

    if(ptr == MAP_FAILED){
        printf("Mapping Failed\n");
        return NULL;
    }
    
    zc_file *file = malloc(sizeof(zc_file));

    file->fileSize = fileSize;
    file->offsetl = 0;
    file->ptr = ptr;
    file->fd = fd;

    file->nReaders = 0;
    sem_init(&file->mutex, 0, 1);
    sem_init(&file->roomEmpty, 0, 1);
    sem_init(&file->readMutex, 0, 1);
    return file;
}

int zc_close(zc_file *file) {
    // To implement
    if (close(file->fd) < 0)
        return -1;

    if (munmap(file->ptr, file->fileSize) < 0)
        return -1;

    free(file);
    return 0;
}

const char *zc_read_start(zc_file *file, size_t *size) {
    // To implement
    // Make sure we don't read beyond the file

    sem_wait(&file->mutex);
    file->nReaders++;
    if (file->nReaders == 1)
        sem_wait(&file->roomEmpty);
    sem_post(&file->mutex);

    sem_wait(&file->readMutex);
    if (file->offsetl > file->fileSize){
        return NULL;
    }

    char* readOffset = ((char*) file->ptr) + file->offsetl;

    if (file->offsetl + (off_t)*size > file->fileSize)
        *size = file->fileSize - file->offsetl;

    file->offsetl += *size;
    sem_post(&file->readMutex);

    return readOffset;
}

void zc_read_end(zc_file *file) {
    // To implement
    sem_wait(&file->mutex);
    file->nReaders--;
    if (file->nReaders == 0)
        sem_post(&file->roomEmpty);
    sem_post(&file->mutex);

}

/**************
 * Exercise 2 *
 **************/

char *zc_write_start(zc_file *file, size_t size) {
    // To implement
    sem_wait(&file->roomEmpty);

    if (file->fileSize == 0 && size > 0){
        ftruncate(file->fd, size);
        file->ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, file->fd, 0);
        if (file->ptr == MAP_FAILED){
            return NULL;
        }
        file->fileSize = size;
    }

    // Checking if writing to this file exceed the file size
    if (file->offsetl + (off_t) size > file->fileSize){
        off_t newSize = file->offsetl + size;
        if (ftruncate(file->fd, newSize) < 0){
            printf("Cannot resize file");
            return NULL;
        }

        void* ptr = mremap(file->ptr, file->fileSize, newSize, MREMAP_MAYMOVE);
        if(ptr == MAP_FAILED){
            printf("Mapping Failed\n");
            return NULL;
        }
        file->ptr = ptr;
        file->fileSize = newSize;
    }

    char* writeOffset = ((char*) file->ptr) + file->offsetl;
    file->offsetl += size;
    return writeOffset;
}

void zc_write_end(zc_file *file) {
    // To implement
    sem_post(&file->roomEmpty);
}

/**************
 * Exercise 3 *
 **************/

off_t zc_lseek(zc_file *file, long offset, int whence) {
    // To implement
    sem_wait(&file->roomEmpty);
    off_t newOffset;
    if (whence == SEEK_SET){
        newOffset = offset;
    }
    else if (whence == SEEK_CUR){
        newOffset = file->offsetl + offset;
    }
    else if (whence == SEEK_END){
        newOffset = file->fileSize + offset;
    }

    // Overflow happens
    if (newOffset < 0){
        sem_post(&file->roomEmpty);
        return -1;
    }

    file->offsetl = newOffset;
    sem_post(&file->roomEmpty);
    return file->offsetl;
}

/**************
 * Exercise 4 *
 **************/

int zc_copyfile(const char *source, const char *dest) {
    // To implement
    zc_file *src = zc_open(source);
    zc_file *dst = zc_open(dest);

    if (src->fileSize == 0){
        ftruncate(dst->fd, src->fileSize);
        return 0;
    }

    if (dst->fileSize == 0){
        dst->ptr = mmap(NULL, src->fileSize, PROT_READ | PROT_WRITE, MAP_SHARED, dst->fd, 0);
    }
    else{
        dst->ptr = mremap(dst->ptr, dst->fileSize, src->fileSize, MREMAP_MAYMOVE);
    }

    if (dst->ptr == MAP_FAILED){
        printf("Mapping failed\n");
        return -1;
    }

    ftruncate(dst->fd, src->fileSize);
    dst->fileSize = src->fileSize;
    memcpy(dst->ptr, src->ptr, src->fileSize);
    
    return 0;
}
