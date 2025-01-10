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

#include "sese/system/FileNotifier.h"
#include "sese/util/Memory.h"

#include <cstdio>
#include <sys/inotify.h>
#if defined(__GLIBC__)
#include <sys/fcntl.h>
#elif defined(__MUSL__)
#include <fcntl.h>
#include <sys/select.h>
#endif


using namespace sese::system;

class FileNotifier::Impl {
public:
    int inotify_fd = -1;
    int watch_fd = -1;
    std::atomic_bool is_shutdown = false;
    Thread::Ptr th = nullptr;

    OnCreateCallback on_create;
    OnMoveCallback on_move;
    OnModifyCallback on_modify;
    OnDeleteCallback on_delete;

    static std::unique_ptr<Impl> create(const std::string &path) noexcept {
        int inotify_fd = inotify_init1(0);
        if (inotify_fd < 0) {
            return nullptr;
        }

        int opt = fcntl(inotify_fd, F_GETFL);
        if (opt < 0) {
            close(inotify_fd);
            return nullptr;
        }

        int ret = fcntl(inotify_fd, F_SETFL, opt | O_NONBLOCK);
        if (ret < 0) {
            close(inotify_fd);
            return nullptr;
        }

        int watch_fd = inotify_add_watch(inotify_fd, path.c_str(), IN_MODIFY | IN_CREATE | IN_DELETE | IN_MOVE);
        if (watch_fd < 0) {
            close(inotify_fd);
            return nullptr;
        }

        auto impl = std::make_unique<Impl>();
        impl->inotify_fd = inotify_fd;
        impl->watch_fd = watch_fd;
        return impl;
    }

    ~Impl() noexcept {
        if (this->th) {
            shutdown();
        }
    }

    void start() noexcept {
        auto proc = [this]() {
            fd_set set;
            struct timeval timeout {
                1, 0
            };
            char buffer[1024];
            while (!is_shutdown) {
                FD_ZERO(&set);
                FD_SET(inotify_fd, &set);
                select(FD_SETSIZE, &set, nullptr, nullptr, &timeout);
                if (FD_ISSET(inotify_fd, &set)) {
                    auto p_event = (inotify_event *) &buffer;
                    auto len = read(inotify_fd, buffer, sizeof(buffer));
                    auto times = (len / sizeof(inotify_event)) - 1;
                    char *from_string = nullptr;
                    while (times) {
                        if (p_event->wd != watch_fd) {
                            p_event++;
                            times--;
                            continue;
                        } else {
                            if (p_event->mask & IN_CREATE) {
                                if (on_create) {
                                    on_create({p_event->name});
                                }
                            } else if (p_event->mask & IN_MODIFY) {
                                if (on_modify) {
                                    on_modify({p_event->name});
                                }
                            } else if (p_event->mask & IN_DELETE) {
                                if (on_delete) {
                                    on_delete({p_event->name});
                                }
                            } else if (p_event->mask & IN_MOVED_FROM) {
                                from_string = p_event->name;
                            } else if (p_event->mask & IN_MOVED_TO) {
                                if (from_string) {
                                    if (on_move) {
                                        on_move({from_string}, {p_event->name});
                                    }
                                    from_string = nullptr;
                                }
                            }
                            times--;
                            p_event++;
                        }
                    }
                }
            }
        };
        th = std::make_unique<Thread>(proc);
        th->start();
    }

    void shutdown() noexcept {
        is_shutdown = true;
        th->join();
        th = nullptr;
        close(inotify_fd);
    }
};

FileNotifier::Ptr FileNotifier::create(const std::string &path) noexcept {
    auto result = MAKE_UNIQUE_PRIVATE(FileNotifier);
    auto impl = Impl::create(path);
    if (result->impl = std::move(impl); !result->impl) {
        return nullptr;
    }
    return result;
}

FileNotifier::~FileNotifier() noexcept {
}

void FileNotifier::start() const noexcept {
    return impl->start();
}

void FileNotifier::shutdown() const noexcept {
    return impl->shutdown();
}

void FileNotifier::setOnCreate(OnCreateCallback &&callback) const noexcept {
    impl->on_create = std::move(callback);
}

void FileNotifier::setOnMove(OnMoveCallback &&callback) const noexcept {
    impl->on_move = std::move(callback);
}

void FileNotifier::setOnModify(OnModifyCallback &&callback) const noexcept {
    impl->on_modify = std::move(callback);
}

void FileNotifier::setOnDelete(OnDeleteCallback &&callback) const noexcept {
    impl->on_delete = std::move(callback);
}