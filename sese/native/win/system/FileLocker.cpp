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
    auto pStart = (DWORD *) &start;
    auto pLen = (DWORD *) &len;
    if (TRUE == ::LockFile(handle, *pStart, *(pStart + 1), *pLen, *(pLen + 1))) {
        offset = start;
        size = len;
        return true;
    } else {
        return false;
    }
}

bool sese::system::FileLocker::unlock() {
    auto handle = (HANDLE) _get_osfhandle(fd);
    auto pStart = (DWORD *) &offset;
    auto pLen = (DWORD *) &size;
    if (TRUE == UnlockFile(handle, *pStart, *(pStart + 1), *pLen, *(pLen + 1))) {
        offset = size = 0;
        return true;
    } else {
        return false;
    }
}
