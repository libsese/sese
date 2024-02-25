#include "sese/system/Process.h"

#include <windows.h>

using namespace sese::system;

Process::Ptr Process::create(const char *command) noexcept {
    auto startup_info = new STARTUPINFO{};
    auto process_info = new PROCESS_INFORMATION{};

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
            startup_info,
            process_info
    );

    if (rt) {
        auto p = new Process;
        p->startupInfo = startup_info;
        p->processInfo = process_info;
        return std::unique_ptr<Process>(p);
    } else {
        delete startup_info;
        delete process_info;
        return nullptr;
    }
}

sese::pid_t Process::getCurrentProcessId() noexcept {
    return GetCurrentProcessId();
}

Process::~Process() noexcept {
    auto s_info = static_cast<STARTUPINFO *>(startupInfo);
    auto p_info = static_cast<PROCESS_INFORMATION *>(processInfo);
    delete s_info;
    delete p_info;
    startupInfo = nullptr;
    processInfo = nullptr;
}

int Process::wait() const noexcept {
    DWORD exit_code;
    auto p_info = static_cast<PROCESS_INFORMATION *>(processInfo);
    // pInfo cannot be nullptr
    if (!p_info) return -1;
    WaitForSingleObject(p_info->hProcess, INFINITE);
    GetExitCodeProcess(p_info->hProcess, &exit_code);
    return static_cast<int>(exit_code);
}

bool Process::kill() const noexcept {
    auto p_info = static_cast<PROCESS_INFORMATION *>(processInfo);
    if (!p_info) return false;
    return TerminateProcess(p_info->hProcess, -1) != 0;
}

sese::pid_t Process::getProcessId() const noexcept {
    if (!processInfo) return 0;
    return GetProcessId(static_cast<PROCESS_INFORMATION *>(processInfo)->hProcess);
}