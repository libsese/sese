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

#include <cstdio>
#include <sys/inotify.h>
#if defined(__GLIBC__)
#include <sys/fcntl.h>
#elif defined(__MUSL__)
#include <fcntl.h>
#include <sys/select.h>
#endif


using namespace sese::system;

FileNotifier::Ptr FileNotifier::create(const std::string &path, FileNotifyOption *option) noexcept {
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

    auto notifier = MAKE_UNIQUE_PRIVATE(FileNotifier);
    notifier->option = option;
    notifier->inotify_fd = inotify_fd;
    notifier->watch_fd = watch_fd;
    return notifier;
}

FileNotifier::~FileNotifier() noexcept {
    if (this->th) {
        shutdown();
    }
}

void FileNotifier::loopNonblocking() noexcept {
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
                            option->onCreate({p_event->name});
                        } else if (p_event->mask & IN_MODIFY) {
                            option->onModify({p_event->name});
                        } else if (p_event->mask & IN_DELETE) {
                            option->onDelete({p_event->name});
                        } else if (p_event->mask & IN_MOVED_FROM) {
                            from_string = p_event->name;
                        } else if (p_event->mask & IN_MOVED_TO) {
                            if (from_string) {
                                option->onMove({from_string}, {p_event->name});
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

void FileNotifier::shutdown() noexcept {
    is_shutdown = true;
    th->join();
    th = nullptr;
    close(inotify_fd);
}