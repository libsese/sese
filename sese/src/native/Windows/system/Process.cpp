#include "sese/system/Process.h"

#include <Windows.h>

sese::Process::Ptr sese::Process::create(char *command) noexcept {
    auto startupInfo = new STARTUPINFO{};
    auto processInfo = new PROCESS_INFORMATION{};

    bool rt = CreateProcessA(
            nullptr,
            command,
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

sese::Process::~Process() noexcept {
    auto sInfo = (STARTUPINFO *) startupInfo;
    auto pInfo = (PROCESS_INFORMATION *) processInfo;
    delete sInfo;
    delete pInfo;
    startupInfo = nullptr;
    processInfo = nullptr;
}

int sese::Process::wait() const noexcept {
    DWORD exitCode;
    auto pInfo = (PROCESS_INFORMATION *) processInfo;
    // pInfo cannot be nullptr
    if (!pInfo) return -1;
    WaitForSingleObject(pInfo->hProcess, INFINITE);
    GetExitCodeProcess(pInfo->hProcess, &exitCode);
    return (int) exitCode;
}

bool sese::Process::kill() const noexcept {
    auto pInfo = (PROCESS_INFORMATION *) processInfo;
    if (!pInfo) return false;
    return TerminateProcess(pInfo->hProcess, -1) != 0;
}