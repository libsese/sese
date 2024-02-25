/// \file FileNotifier.h
/// \version 0.1
/// \author kaoru
/// \date 2023年3月11日
/// \brief 文件变更监视器

#pragma once

#include "sese/Config.h"

#include <atomic>
#ifdef __APPLE__
#include <tuple>
#else
#include "sese/thread/Thread.h"
#endif

namespace sese::system {

/// 文件变更回调选项类
struct API FileNotifyOption {
    virtual ~FileNotifyOption() = default;

    /// 创建文件
    virtual void onCreate(std::string_view name) = 0;

    /// 移动文件
    virtual void onMove(std::string_view src_name, std::string_view dst_name) = 0;

    /// 修改文件
    virtual void onModify(std::string_view name) = 0;

    /// 删除文件
    virtual void onDelete(std::string_view name) = 0;
};

/// \brief 文件变更监视器
/// \bug 此实现 Darwin 事件顺序与 Windows 和 Linux 不一致。 <p>
/// Darwin 优先触发同一文件多个事件，Windows 和 Linux 按时间顺序触发。
class API FileNotifier {
public:
    using Ptr = std::unique_ptr<FileNotifier>;

    /// 创建文件监视器
    /// \param path 此处应为[文件夹]的路径
    /// \param option 回调选项
    /// \retval nullptr 创建失败
    static FileNotifier::Ptr create(const std::string &path, FileNotifyOption *option) noexcept;

    virtual ~FileNotifier() noexcept;

    /// 启动线程，开始处理变更事件
    void loopNonblocking() noexcept;

    /// 关闭监视器并阻塞至后台线程退出
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
#endif
};
} // namespace sese::system