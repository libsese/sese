// Copyright 2024 libsese
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
