#include <sese/system/FileLocker.h>

#include <fcntl.h>
#include <unistd.h>

bool sese::system::FileLocker::lockWrite(int64_t start, int64_t len) const {
    struct flock lock {};
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = start;
    lock.l_len = len;
    return ::fcntl(fd, F_SETLKW, &lock) == 0;
}

bool sese::system::FileLocker::lockRead(int64_t start, int64_t len) const {
    struct flock lock {};
    lock.l_type = F_RDLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = start;
    lock.l_len = len;
    return ::fcntl(fd, F_SETLKW, &lock) == 0;
}

bool sese::system::FileLocker::lock(int64_t start, int64_t len) const {
    struct flock lock {};
    lock.l_type = F_LOCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = start;
    lock.l_len = len;
    return ::fcntl(fd, F_SETLK, &lock) == 0;
}

bool sese::system::FileLocker::unlock() const {
    struct flock lock {};
    lock.l_type = F_UNLCK;
    return ::fcntl(fd, F_SETLK, &lock) == 0;
}