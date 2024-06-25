#include "sese/system/FileNotifier.h"

#include <sys/inotify.h>
#include <sys/fcntl.h>

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