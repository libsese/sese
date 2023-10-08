#include <sese/system/FileLocker.h>

#include <io.h>

bool sese::system::FileLocker::lockWrite(int64_t start, int64_t len) {
    return lock(start, len);
}

bool sese::system::FileLocker::lockRead(int64_t start, int64_t len) {
    return lock(start, len);
}

bool sese::system::FileLocker::lock(int64_t start, int64_t len) {
    auto handle = (HANDLE) _get_osfhandle(fd);
    auto pStart = (int32_t *) &start;
    auto pLen = (int32_t *) &len;
    return TRUE == ::LockFile(
                           handle,
                           *(pStart + 0),
                           *(pStart + 1),
                           *(pLen + 0),
                           *(pLen + 1)
                   );
}

bool sese::system::FileLocker::unlock() {
    auto handle = (HANDLE) _get_osfhandle(fd);
    return TRUE == UnlockFile(handle, 0, 0, MAXDWORD, MAXDWORD);
}
