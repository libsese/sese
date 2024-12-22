// Copyright 2024 libsese
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

#include "sese/system/Process.h"

#include <csignal>
#include <unistd.h>
#include <sys/wait.h>

using namespace sese::system;

sese::Result<Process::Ptr, sese::ErrorCode> Process::createEx(const std::string &exec, const std::vector<std::string> &args) noexcept {
    auto pid = fork();
    if (pid > 0) {
        // parent process
        auto proc = MAKE_UNIQUE_PRIVATE(Process);
        proc->id = pid;
        return Result<Process::Ptr, ErrorCode>::success(std::move(proc));
    } else if (pid == 0) {
        // client process
        auto c_args = new char *[args.size() + 2];
        auto exec_string = new char[exec.size() + 1];
        memcpy(exec_string, exec.c_str(), exec.size() + 1);
        c_args[0] = exec_string;
        int i = 1;
        for (auto &arg: args) {
            auto arg_string = new char[arg.size() + 1];
            memcpy(arg_string, arg.c_str(), arg.size() + 1);
            c_args[i] = arg_string;
            i++;
        }
        c_args[args.size() + 1] = nullptr;

        if (execvp(exec.c_str(), c_args) == -1) {
            for (int i = 0; i < args.size() + 2; ++i) {
                delete[] c_args[i];
            }
            delete[] c_args;
            return Result<Process::Ptr, ErrorCode>::error({getErrorCode(), getErrorString()}); // GCOVR_EXCL_LINE
        } else {
            // never reach
            return Result<Process::Ptr, ErrorCode>::error({getErrorCode(), getErrorString()}); // GCOVR_EXCL_LINE
        }
    } else {
        // failed to create
        return Result<Process::Ptr, ErrorCode>::error({getErrorCode(), getErrorString()}); // GCOVR_EXCL_LINE
    }
}

sese::pid_t Process::getCurrentProcessId() noexcept {
    return getpid();
}

int Process::wait() const noexcept {
    int status;
    ::waitpid(id, &status, 0);
    if (!WIFEXITED(status)) {
        return WEXITSTATUS(status); // GCOVR_EXCL_LINE
    } else {
        return 0;
    }
}

sese::pid_t Process::getProcessId() const noexcept {
    return id;
}

bool Process::kill() const noexcept {
    return ::kill(id, SIGKILL) == 0;
}
