#pragma once

#include "sese/Config.h"
#include "sese/record/LogHelper.h"

namespace sese {

    struct API FileNotifyOption {
        // 创建文件
        virtual void onCreate(const std::string &path) {
            sese::record::LogHelper::d("CREATE %s", path.c_str());
        };
        // 移动文件
        virtual void onMove(const std::string &srcPath, const std::string &dstPath) {
            sese::record::LogHelper::d("MOVE %s -> %s", srcPath.c_str(), dstPath.c_str());
        };
        // 修改文件
        virtual void onModify(const std::string &path) {
            sese::record::LogHelper::d("MODIFY %s", path.c_str());
        };
        // 删除文件
        virtual void onDelete(const std::string &path) {
            sese::record::LogHelper::d("DELETE %s", path.c_str());
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
#endif
    };
}// namespace sese