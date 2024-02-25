#include "sese/system/FileNotifier.h"
#include "sese/record/LogHelper.h"

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

    auto notifier = new FileNotifier;
    notifier->option = option;
    notifier->inotifyFd = inotify_fd;
    notifier->watchFd = watch_fd;
    return std::unique_ptr<FileNotifier>(notifier);
}

FileNotifier::~FileNotifier() noexcept {
    if (this->th) {
        shutdown();
    }
}

void FileNotifier::loopNonblocking() noexcept {
    auto proc = [this]() {
        fd_set fd_set;
        struct timeval timeout {
            1, 0
        };
        char buffer[1024];
        while (!isShutdown) {
            FD_ZERO(&fd_set);
            FD_SET(inotifyFd, &fd_set);
            select(FD_SETSIZE, &fd_set, nullptr, nullptr, &timeout);
            if (FD_ISSET(inotifyFd, &fdSet)) {
                auto p_event = (inotify_event *) &buffer;
                auto len = read(inotifyFd, buffer, sizeof(buffer));
                auto times = (len / sizeof(inotify_event)) - 1;
                char *from_string = nullptr;
                while (times) {
                    if (pEvent->wd != watchFd) {
                        p_event++;
                        times--;
                        continue;
                    } else {
                        if (pEvent->mask & IN_CREATE) {
                            option->onCreate({pEvent->name});
                        } else if (pEvent->mask & IN_MODIFY) {
                            option->onModify({pEvent->name});
                        } else if (pEvent->mask & IN_DELETE) {
                            option->onDelete({pEvent->name});
                        } else if (pEvent->mask & IN_MOVED_FROM) {
                            from_string = pEvent->name;
                        } else if (pEvent->mask & IN_MOVED_TO) {
                            if (from_string) {
                                option->onMove({from_string}, {pEvent->name});
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
    isShutdown = true;
    th->join();
    th = nullptr;
    close(inotifyFd);
}