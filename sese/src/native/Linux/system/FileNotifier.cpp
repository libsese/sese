#include "sese/system/FileNotifier.h"
#include "sese/record/LogHelper.h"

#include <sys/inotify.h>
#include <sys/fcntl.h>

sese::FileNotifier::Ptr sese::FileNotifier::create(const std::string &path, FileNotifyOption *option) noexcept {
    int inotifyFd = inotify_init1(0);
    if (inotifyFd < 0) {
        return nullptr;
    }

    int opt = fcntl(inotifyFd, F_GETFL);
    if (opt < 0) {
        close(inotifyFd);
        return nullptr;
    }

    int ret = fcntl(inotifyFd, F_SETFL, opt | O_NONBLOCK);
    if (ret < 0) {
        close(inotifyFd);
        return nullptr;
    }

    int watchFd = inotify_add_watch(inotifyFd, path.c_str(), IN_MODIFY | IN_CREATE | IN_DELETE | IN_MOVE);
    if (watchFd < 0) {
        close(inotifyFd);
        return nullptr;
    }

    auto notifier = new FileNotifier;
    notifier->option = option;
    notifier->inotifyFd = inotifyFd;
    notifier->watchFd = watchFd;
    return std::unique_ptr<FileNotifier>(notifier);
}

void sese::FileNotifier::loopNonblocking() noexcept {
    auto proc = [this]() {
        fd_set fdSet;
        struct timeval timeout {
            1, 0
        };
        char buffer[1024];
        while (!isShutdown) {
            FD_ZERO(&fdSet);
            FD_SET(inotifyFd, &fdSet);
            select(FD_SETSIZE, &fdSet, nullptr, nullptr, &timeout);
            if (FD_ISSET(inotifyFd, &fdSet)) {
                auto pEvent = (inotify_event *) &buffer;
                auto len = read(inotifyFd, buffer, sizeof(buffer));
                auto times = (len / sizeof(inotify_event)) - 1;
                char *fromString = nullptr;
                while (times) {
                    if (pEvent->wd != watchFd) {
                        pEvent++;
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
                            fromString = pEvent->name;
                        } else if (pEvent->mask & IN_MOVED_TO) {
                            if (fromString) {
                                option->onMove({fromString}, {pEvent->name});
                                fromString = nullptr;
                            }
                        }
                        times--;
                        pEvent++;
                    }
                }
            }
        }
    };
    th = std::make_unique<Thread>(proc);
    th->start();
}

void sese::FileNotifier::shutdown() noexcept {
    isShutdown = true;
    th->join();
    th = nullptr;
    close(watchFd);
    close(inotifyFd);
}