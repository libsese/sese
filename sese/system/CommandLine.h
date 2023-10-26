/// \file CommandLine.h
/// \brief 命令行参数类
/// \author kaoru
/// \date 2023年10月26日


#pragma once

#include <sese/Config.h>
#include <sese/util/NotInstantiable.h>
#include <sese/util/Initializer.h>

namespace sese::system {

/// 命令行参数初始化任务
class CommandLineInitiateTask : public InitiateTask {
public:
    explicit CommandLineInitiateTask(const int argc, const char *const *argv);

    int32_t init() noexcept override;

    int32_t destroy() noexcept override;
};

/// 命令行参数类
class API CommandLine : public NotInstantiable {
public:
    static int getArgc();

    static const char *const *getArgv();
};

} // namespace sese::system