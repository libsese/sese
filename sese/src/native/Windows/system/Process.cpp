#include "sese/system/Process.h"

#include <Windows.h>

using namespace sese::system;

Process::Ptr Process::create(const char *command) noexcept {
    auto startupInfo = new STARTUPINFO{};
    auto processInfo = new PROCESS_INFORMATION{};

    char buffer[1024]{};
    strcpy_s(buffer, command);

    bool rt = CreateProcessA(
            nullptr,
            buffer,
            nullptr,
            nullptr,
            false,
            0,
            nullptr,
            nullptr,
            startupInfo,
            processInfo
    );

    if (rt) {
        auto p = new Process;
        p->startupInfo = startupInfo;
        p->processInfo = processInfo;
        return std::unique_ptr<Process>(p);
    } else {
        delete startupInfo;
        delete processInfo;
        return nullptr;
    }
}

pid_t Process::getCurrentProcessId() noexcept {
    return GetCurrentProcessId();
}

Process::~Process() noexcept {
    auto sInfo = (STARTUPINFO *) startupInfo;
    auto pInfo = (PROCESS_INFORMATION *) processInfo;
    delete sInfo;
    delete pInfo;
    startupInfo = nullptr;
    processInfo = nullptr;
}

int Process::wait() const noexcept {
    DWORD exitCode;
    auto pInfo = (PROCESS_INFORMATION *) processInfo;
    // pInfo cannot be nullptr
    if (!pInfo) return -1;
    WaitForSingleObject(pInfo->hProcess, INFINITE);
    GetExitCodeProcess(pInfo->hProcess, &exitCode);
    return (int) exitCode;
}

bool Process::kill() const noexcept {
    auto pInfo = (PROCESS_INFORMATION *) processInfo;
    if (!pInfo) return false;
    return TerminateProcess(pInfo->hProcess, -1) != 0;
}

pid_t Process::getProcessId() const noexcept {
    if (!processInfo) return 0;
    return GetProcessId(((PROCESS_INFORMATION *) processInfo)->hProcess);
}