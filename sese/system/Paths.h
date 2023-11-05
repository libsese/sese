/// \file Paths.h
/// \brief 运行时路径
/// \author kaoru
/// \date 2023年11月5日

#include <sese/system/Path.h>
#include <sese/util/Initializer.h>

namespace sese::system {

/// 运行时路径初始化任务
class PathsInitiateTask : public InitiateTask {
public:
    PathsInitiateTask();

    int32_t init() noexcept override;

    int32_t destroy() noexcept override;
};

/// 运行时路径
class Paths {
public:
    static const Path &getWorkDir();

    static const Path &getExecutablePath();

    static const std::string &getExecutableName();
};
} // namespace sese::system