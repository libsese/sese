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

#include <fcntl.h>
#include <unistd.h>

using sese::system::FileLocker;

class FileLocker::Impl {
    int32_t fd;

public:
    Impl(int32_t fd) : fd(fd) {}

    bool lockWrite(int64_t start, int64_t len) {
        struct flock lock {};
        lock.l_type = F_WRLCK;
        lock.l_whence = SEEK_SET;
        lock.l_start = start;
        lock.l_len = len;
        return ::fcntl(fd, F_SETLKW, &lock) == 0;
    }

    bool lockRead(int64_t start, int64_t len) {
        struct flock lock {};
        lock.l_type = F_RDLCK;
        lock.l_whence = SEEK_SET;
        lock.l_start = start;
        lock.l_len = len;
        return ::fcntl(fd, F_SETLKW, &lock) == 0;
    }

    bool lock(int64_t start, int64_t len) {
        struct flock lock {};
        lock.l_type = F_LOCK;
        lock.l_whence = SEEK_SET;
        lock.l_start = start;
        lock.l_len = len;
        return ::fcntl(fd, F_SETLK, &lock) == 0;
    }

    bool unlock() {
        struct flock lock {};
        lock.l_type = F_UNLCK;
        return ::fcntl(fd, F_SETLK, &lock) == 0;
    }
};

FileLocker::FileLocker(int32_t fd) : impl(std::make_unique<Impl>(fd)) {}

FileLocker::~FileLocker() {}

bool FileLocker::lockWrite(int64_t start, int64_t len) {
    return impl->lockWrite(start, len);
}

bool FileLocker::lockRead(int64_t start, int64_t len) {
    return impl->lockRead(start, len);
}

bool FileLocker::lock(int64_t start, int64_t len) {
    return impl->lock(start, len);
}

bool FileLocker::unlock() {
    return impl->unlock();
}