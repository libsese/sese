#include "Util.h"
#include "Config.h"

#ifdef __linux__
#include <sys/syscall.h>
#include <unistd.h>
#endif

#ifdef _WIN32
#include <Windows.h>
#endif

namespace sese {

    [[maybe_unused]] static struct InitStruct {
        InitStruct() {
            // 初始化 Logger
            auto logger = sese::Singleton<Logger>::getInstance();
            auto formatter = std::make_shared<SimpleFormatter>();
            auto appender = std::make_shared<ConsoleAppender>(formatter);
            logger->addAppender(appender);
        }
        ~InitStruct() {
            // Logger 不需要手动释放
            // delete sese::Singleton<Logger>::getInstance();
        }
    } initStruct; /* NOLINT */

    Logger *getLogger() noexcept {
        return sese::Singleton<Logger>::getInstance();
    }

    bool isSpace(char ch) noexcept {
        auto p = SPACE_CHARS;
        while (*p != '\0') {
            if (*p == ch) {
                return true;
            }
            p++;
        }
        return false;
    }

    pid_t getThreadId() noexcept {
#ifdef __linux__
        return syscall(__NR_gettid);
#endif
#ifdef _WIN32
        return GetCurrentThreadId();
#endif
    }

    static thread_local const char *ThreadName = "Main";

    const char *getThreadName() noexcept {
        return ThreadName;
    }
}// namespace sese
