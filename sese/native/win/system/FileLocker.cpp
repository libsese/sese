#include <sese/system/FileLocker.h>

#include <io.h>

bool sese::system::FileLocker::lockWrite(int64_t start, int64_t len) {
    return lock(start, len);
}

bool sese::system::FileLocker::lockRead(int64_t start, int64_t len) {
    return lock(start, len);
}

bool sese::system::FileLocker::lock(int64_t start, int64_t len) {
    auto handle = reinterpret_cast<HANDLE>(_get_osfhandle(fd));
    auto p_start = reinterpret_cast<DWORD *>(&start);
    auto p_len = reinterpret_cast<DWORD *>(&len);
    if (TRUE == ::LockFile(handle, *p_start, *(p_start + 1), *p_len, *(p_len + 1))) {
        offset = start;
        size = len;
        return true;
    } else {
        return false;
    }
}

bool sese::system::FileLocker::unlock() {
    auto handle = reinterpret_cast<HANDLE>(_get_osfhandle(fd));
    auto p_start = reinterpret_cast<DWORD *>(&offset);
    auto p_len = reinterpret_cast<DWORD *>(&size);
    if (TRUE == UnlockFile(handle, *p_start, *(p_start + 1), *p_len, *(p_len + 1))) {
        offset = size = 0;
        return true;
    } else {
        return false;
    }
}
