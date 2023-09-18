#include "sese/net/Socket.h"
#include "sese/util/Test.h"
#include "sese/record/Logger.h"
#include "sese/util/Initializer.h"
#include "sese/convert/EncodingConverter.h"
#include "sese/security/SecurityConfig.h"
#include "sese/thread/Thread.h"

#include <memory>

using sese::EncodingConverterInitiateTask;
using sese::Initializer;
using sese::InitiateTask;
// using sese::TestInitiateTask;

static Initializer initializer;

[[maybe_unused]] void *Initializer::getInitializer() noexcept { return &initializer; }

InitiateTask::InitiateTask(std::string name) : name(std::move(name)) {
}

const std::string &sese::InitiateTask::getName() const {
    return name;
}

Initializer::Initializer() {
    // 不会出错，不需要判断
    // GCOVR_EXCL_START
    buildInLoadTask(std::make_shared<ThreadInitiateTask>());
    buildInLoadTask(std::make_shared<EncodingConverterInitiateTask>());
    buildInLoadTask(std::make_shared<record::LoggerInitiateTask>());
    // 弃用
    // buildInLoadTask(std::make_shared<TestInitiateTask>());
#ifdef _WIN32
    buildInLoadTask(std::make_shared<sese::net::SocketInitiateTask>());
#endif
#ifdef SESE_USE_SSL
    buildInLoadTask(std::make_shared<sese::security::SecurityInitTask>());
#endif
    // GCOVR_EXCL_STOP
}
#undef INIT

Initializer::~Initializer() {
    /// 保证初始化器按顺序销毁
    while (!tasks.empty()) {
        InitiateTask::Ptr &top = tasks.top();
        buildInUnloadTask(top);
    }
}

void Initializer::buildInLoadTask(InitiateTask::Ptr &&task) noexcept {
    auto rt = task->init();
    if (rt != 0) {
        printf("Load failed: %32s Exit code %d", task->getName().c_str(), rt);
    } else {
        tasks.emplace(task);
    }
}

void Initializer::buildInUnloadTask(const InitiateTask::Ptr &task) noexcept {
    auto rt = task->destroy();
    if (rt != 0) {
        printf("Unload failed: %32s Exit code %d", task->getName().c_str(), rt);
    }
    tasks.pop();
}

void Initializer::addTask(InitiateTask::Ptr task) noexcept {
    initializer.buildInLoadTask(std::move(task));
}
