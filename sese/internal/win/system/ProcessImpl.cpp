// Copyright 2025 libsese
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <sese/system/Process.h>
#include <sese/text/StringBuilder.h>
#include <sese/Util.h>

#include <windows.h>

using namespace sese::system;

class Process::Impl {
public:
    using StartupInfo = std::unique_ptr<STARTUPINFO>;
    using ProcessInfo = std::unique_ptr<PROCESS_INFORMATION>;
    StartupInfo startupinfo;
    ProcessInfo process_information;

    static std::unique_ptr<Impl> createEx(const std::string &exec, const std::vector<std::string> &args) noexcept {
        auto startup_info = std::make_unique<STARTUPINFO>();
        auto process_info = std::make_unique<PROCESS_INFORMATION>();

        text::StringBuilder builder;
        builder << exec;
        for (auto &arg: args) {
            builder << " " << arg;
        }
        builder << '\0';
        auto cmd_line = std::make_unique<char[]>(builder.size());
        memcpy(cmd_line.get(), builder.buf(), builder.size());

        bool rt = CreateProcessA(
            nullptr,
            cmd_line.get(),
            nullptr,
            nullptr,
            false,
            0,
            nullptr,
            nullptr,
            startup_info.get(),
            process_info.get()
        );

        if (rt) {
            return std::make_unique<Impl>(std::move(startup_info), std::move(process_info));
        }
        return {};
    }

    static pid_t getCurrentProcessId() noexcept {
        return GetCurrentProcessId();
    }

    int wait() const noexcept {
        DWORD exit_code;
        WaitForSingleObject(process_information->hProcess, INFINITE);
        GetExitCodeProcess(process_information->hProcess, &exit_code);
        return static_cast<int>(exit_code);
    }

    bool kill() const noexcept {
        return TerminateProcess(process_information->hProcess, -1) != 0;
    }

    pid_t getProcessId() const noexcept {
        return GetProcessId(process_information->hProcess);
    }

    Impl(StartupInfo startupinfo, ProcessInfo process_info)
        : startupinfo(std::move(startupinfo)), process_information(std::move(process_info)) {
    }
};

Process::~Process() {
}

sese::Result<Process::Ptr, sese::ErrorCode> Process::createEx(const std::string &exec, const std::vector<std::string> &args) noexcept {
    if (auto impl = Impl::createEx(exec, args)) {
        auto result = MAKE_UNIQUE_PRIVATE(Process);
        result->impl = std::move(impl);
        return Result<Ptr, ErrorCode>::success(std::move(result));
    }
    return Result<Ptr, ErrorCode>::error({getErrorCode(), getErrorString()});
}

sese::pid_t Process::getCurrentProcessId() noexcept {
    return Impl::getCurrentProcessId();
}

int Process::wait() const noexcept {
    return impl->wait();
}

bool Process::kill() const noexcept {
    return impl->kill();
}

sese::pid_t Process::getProcessId() const noexcept {
    return impl->getProcessId();
}