#pragma once

#include "sese/Config.h"
#include "sese/record/LogHelper.h"

#include <atomic>
#ifdef __APPLE__
#include <tuple>
#else
#include "sese/thread/Thread.h"
#endif

namespace sese {

    struct API FileNotifyOption {
        // 创建文件
        virtual void onCreate(std::string_view path) {
            sese::record::LogHelper::d("CREATE %s", path.data());
        };

        // 移动文件
        virtual void onMove(std::string_view srcPath, std::string_view dstPath) {
            sese::record::LogHelper::d("MOVE %s -> %s", srcPath.data(), dstPath.data());
        };

        // 修改文件
        virtual void onModify(std::string_view path) {
            sese::record::LogHelper::d("MODIFY %s", path.data());
        };

        // 删除文件
        virtual void onDelete(std::string_view path) {
            sese::record::LogHelper::d("DELETE %s", path.data());
        };
    };

    class API FileNotifier {
    public:
        using Ptr = std::unique_ptr<FileNotifier>;

        static FileNotifier::Ptr create(const std::string &path, FileNotifyOption *option) noexcept;

        void loopNonblocking() noexcept;

        void shutdown() noexcept;

    private:
        FileNotifier() = default;

#ifdef WIN32
        void *fileHandle = nullptr;
        void *overlapped = nullptr;
        std::atomic_bool isShutdown = false;
        FileNotifyOption *option = nullptr;
        Thread::Ptr th = nullptr;
#elif __linux__
        int inotifyFd = -1;
        int watchFd = -1;
        std::atomic_bool isShutdown = false;
        FileNotifyOption *option = nullptr;
        Thread::Ptr th = nullptr;
#elif __APPLE__
        void *stream = nullptr;
        void *queue = nullptr;
        std::tuple<size_t, FileNotifyOption *> lenAndOption;
#endif
    };
}// namespace sese