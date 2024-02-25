#include <sese/system/CommandLine.h>

using namespace sese::system;

static int g_argc = 0;
static const char *const *g_argv = nullptr;

CommandLineInitiateTask::CommandLineInitiateTask(const int ARGC, const char *const *argv) : InitiateTask(__FUNCTION__) {
    g_argc = ARGC;
    g_argv = argv;
}

int32_t CommandLineInitiateTask::init() noexcept {
    return 0;
}
int32_t CommandLineInitiateTask::destroy() noexcept {
    return 0;
}

int CommandLine::getArgc() {
    return g_argc;
}

const char *const *CommandLine::getArgv() {
    return g_argv;
}
