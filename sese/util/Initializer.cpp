#include "sese/net/Socket.h"
#include "sese/record/Logger.h"
#include "sese/util/Initializer.h"
#include "sese/security/SecurityConfig.h"
#include "sese/system/CommandLine.h"
#include "sese/system/Paths.h"
#include "sese/thread/Thread.h"

#include <memory>
#include <utility>

// using sese::EncodingConverterInitiateTask;
using sese::Initializer;
using sese::InitiateTask;
// using sese::TestInitiateTask;
using sese::system::CommandLineInitiateTask;
using sese::system::PathsInitiateTask;

static Initializer gInitializer;

// GCOVR_EXCL_START
[[maybe_unused]] void *Initializer::getInitializer() noexcept {
    addTask(std::make_shared<PathsInitiateTask>());
    addTask(std::make_shared<ThreadInitiateTask>());
    addTask(std::make_shared<record::LoggerInitiateTask>());
#ifdef _WIN32
    addTask(std::make_shared<net::SocketInitiateTask>());
#endif
    addTask(std::make_shared<security::SecurityInitTask>());
    return &gInitializer;
}
// GCOVR_EXCL_STOP

InitiateTask::InitiateTask(std::string name) : name(std::move(name)) {
}

const std::string &sese::InitiateTask::getName() const {
    return name;
}

Initializer::~Initializer() {
    /// 保证初始化器按顺序销毁
    while (!tasks.empty()) {
        auto &&top = tasks.top();
        auto rt = top->destroy();
        if (rt != 0) {
            printf("Unload failed: %32s Exit code %d", top->getName().c_str(), rt);
        }
        tasks.pop();
    }
}

void Initializer::addTask(const InitiateTask::Ptr &task) noexcept {
    auto rt = task->init();
    if (rt != 0) {
        printf("Load failed: %32s Exit code %d", task->getName().c_str(), rt);
    } else {
        gInitializer.tasks.emplace(task);
    }
}

void sese::initCore(const int argc, const char *const *argv) noexcept {
    // 不会出错，不需要判断
    // GCOVR_EXCL_START
    Initializer::addTask(std::make_shared<system::CommandLineInitiateTask>(argc, argv));
    Initializer::addTask(std::make_shared<PathsInitiateTask>());
    Initializer::addTask(std::make_shared<ThreadInitiateTask>());
    Initializer::addTask(std::make_shared<record::LoggerInitiateTask>());
#ifdef _WIN32
    Initializer::addTask(std::make_shared<net::SocketInitiateTask>());
#endif
    Initializer::addTask(std::make_shared<security::SecurityInitTask>());
    // GCOVR_EXCL_STOP
}