#include <sese/system/Paths.h>

#ifndef SESE_PLATFORM_WINDOWS
#include <climits>
#include <unistd.h>
#endif

#ifdef SESE_PLATFORM_DARWIN
#include <mach-o/dyld.h>
#endif

static sese::system::Path gWorkDir;

static sese::system::Path gExecutablePath;

static std::string gExecutableName;

sese::system::PathsInitiateTask::PathsInitiateTask() : InitiateTask(__FUNCTION__) {}

int32_t sese::system::PathsInitiateTask::init() noexcept {
#ifdef SESE_PLATFORM_WINDOWS
#define PATH_MAX 4096
    char buffer[PATH_MAX];
    DWORD len;
    len = GetCurrentDirectoryA(sizeof(buffer), buffer);
    if (len == 0) return -1;
    Path::replaceWindowsPathSplitChar(buffer, len);
    gWorkDir = Path::fromNativePath(buffer);
    if (!gWorkDir.isValid()) return -1;

    ZeroMemory(buffer, sizeof(buffer));
    len = GetModuleFileNameA(nullptr, buffer, PATH_MAX);
    if (len == 0) return -1;
    Path::replaceWindowsPathSplitChar(buffer, len);
    auto p = std::strrchr(buffer, '/');
    gExecutablePath = Path::fromNativePath(buffer);
    *p = 0;
    p += 1;
    if (!gExecutablePath.isValid()) return -1;
    gExecutableName = p;

    return 0;
#undef PATH_MAX
#else
    char buffer[PATH_MAX]{};
    auto n = getcwd(buffer, sizeof(buffer));
    if (n == nullptr) return -1;
    gWorkDir = buffer;
    if (!gWorkDir.isValid()) return -1;

#ifdef SESE_PLATFORM_LINUX
    memset(buffer, 0 , sizeof(buffer));
    auto i = readlink("/proc/self/exe", buffer, sizeof(buffer));
    if (i == -1) return -1;
    gExecutablePath = buffer;
    auto p = std::strrchr(buffer, '/');
    *p = 0;
    p += 1;
    gExecutableName = p;
#else
    memset(buffer, 0 , sizeof(buffer));
    uint32_t len = sizeof(buffer);
    if(_NSGetExecutablePath(buffer, &len) != 0) return -1;
    gExecutablePath = buffer;
    auto p = std::strrchr(buffer, '/');
    *p = 0;
    p += 1;
    gExecutableName = p;
#endif

    return 0;
#endif
}

int32_t sese::system::PathsInitiateTask::destroy() noexcept {
    return 0;
}

const sese::system::Path &sese::system::Paths::getWorkDir() {
    return gWorkDir;
}

const sese::system::Path &sese::system::Paths::getExecutablePath() {
    return gExecutablePath;
}

const std::string &sese::system::Paths::getExecutableName() {
    return gExecutableName;
}