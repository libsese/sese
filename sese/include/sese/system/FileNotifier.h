#pragma once

#include "sese/Config.h"
#include "sese/record/LogHelper.h"

#include <atomic>

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

        void loop() noexcept;

        void shutdown() noexcept;

    private:
        FileNotifier() = default;

        FileNotifyOption *option = nullptr;
#ifdef WIN32
        void *fileHandle = nullptr;
        void *overlapped = nullptr;
        std::atomic_bool isShutdown = false;
#elif __linux__
        int inotifyFd = -1;
        int watchFd = -1;
        std::atomic_bool isShutdown = false;
#endif
    };
}// namespace sese