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
        auto p = MAKE_UNIQUE_PRIVATE(Process);
        p->startup_info = startup_info;
        p->process_info = process_info;
        return p;
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
    auto s_info = static_cast<STARTUPINFO *>(startup_info);
    auto p_info = static_cast<PROCESS_INFORMATION *>(process_info);
    delete s_info;
    delete p_info;
    startup_info = nullptr;
    process_info = nullptr;
}

int Process::wait() const noexcept {
    DWORD exit_code;
    auto p_info = static_cast<PROCESS_INFORMATION *>(process_info);
    // pInfo cannot be nullptr
    if (!p_info) return -1;
    WaitForSingleObject(p_info->hProcess, INFINITE);
    GetExitCodeProcess(p_info->hProcess, &exit_code);
    return static_cast<int>(exit_code);
}

bool Process::kill() const noexcept {
    auto p_info = static_cast<PROCESS_INFORMATION *>(process_info);
    if (!p_info) return false;
    return TerminateProcess(p_info->hProcess, -1) != 0;
}

sese::pid_t Process::getProcessId() const noexcept {
    if (!process_info) return 0;
    return GetProcessId(static_cast<PROCESS_INFORMATION *>(process_info)->hProcess);
}