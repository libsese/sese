#include "sese/net/Socket.h"
#include "sese/Test.h"
#include "sese/record/Logger.h"
#include "sese/Initializer.h"
#include "sese/convert/EncodingConverter.h"

using sese::EncodingConverterInitiateTask;
using sese::Initializer;
using sese::InitiateTask;
using sese::LoggerInitiateTask;
using sese::TestInitiateTask;

static Initializer initializer;// NOLINT

InitiateTask::InitiateTask(std::string name) : name(std::move(name)) {
}

const std::string &sese::InitiateTask::getName() const {
    return name;
}

Initializer::Initializer() {
    buildInLoadTask(std::make_unique<EncodingConverterInitiateTask>());
    buildInLoadTask(std::make_unique<LoggerInitiateTask>());
    buildInLoadTask(std::make_unique<TestInitiateTask>());
#ifdef _WIN32
    buildInLoadTask(std::make_unique<sese::SocketInitiateTask>());
#endif
}

Initializer::~Initializer() {
    /// 保证初始化器按顺序销毁
    InitiateTask::Ptr top;
    while (!tasks.empty()) {
        top = std::move(tasks.top());
        buildInUnloadTask(top);
    }
}

void Initializer::buildInLoadTask(InitiateTask::Ptr &&task) noexcept {
    auto rt = task->init();
    if (rt != 0) {
        printf("Load failed: %32s Exit code %d", task->getName().c_str(), rt);
    } else {
        tasks.emplace(std::move(task));
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
