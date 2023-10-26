#include <sese/system/CommandLine.h>

using namespace sese::system;

static int gArgc = 0;
static const char *const *gArgv = nullptr;

CommandLineInitiateTask::CommandLineInitiateTask(const int argc, const char *const *argv) : InitiateTask(__FUNCTION__) {
    gArgc = argc;
    gArgv = argv;
}

int32_t CommandLineInitiateTask::init() noexcept {
    return 0;
}
int32_t CommandLineInitiateTask::destroy() noexcept {
    return 0;
}

int CommandLine::getArgc() {
    return gArgc;
}

const char *const *CommandLine::getArgv() {
    return gArgv;
}
