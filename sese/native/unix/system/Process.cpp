#include "sese/system/Process.h"

#include <csignal>
#include <unistd.h>
#include <sys/wait.h>

using namespace sese::system;

Process::Ptr Process::create(const char *command) noexcept {
    auto pid = fork();
    if (pid > 0) {
        // parent process
        auto proc = MAKE_UNIQUE_PRIVATE(Process);
        proc->id = pid;
        return proc;
    } else if (pid == 0) {
        // client process
        char p_command[1024];
        std::strcpy(p_command, command);
        exec(p_command);
        // never reach
        return nullptr; // GCOVR_EXCL_LINE
    } else {
        // failed to create
        return nullptr; // GCOVR_EXCL_LINE
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

void Process::exec(char *p_command) noexcept {
    auto count = Process::count(p_command);
    auto args = new char *[count + 2];
    char *p = p_command;
    char **p_args = args;
    for (int i = 0; i < count + 1; ++i) {
        auto new_pos = spilt(p);
        *p_args = p;
        p_args++;
        p = new_pos;
    }
    *p_args = nullptr;

    auto code = execvp(args[0], &args[0]);

    // never reach
    delete[] args;

    if (code == -1) {
        exit(errno);
    }
}

sese::pid_t Process::getProcessId() const noexcept {
    return id;
}

bool Process::kill() const noexcept {
    return ::kill(id, SIGKILL) == 0;
}

size_t Process::count(const char *p_command) noexcept {
    const char *p = p_command;
    size_t count = 0;
    size_t space_count = 0;
    while (*p != 0) {
        if (*p == '\"') {
            count++;
            p++;
            continue;
        } else if (*p == ' ') {
            if (count % 2 == 0) {
                space_count++;
                p++;
            } else {
                p++;
            }
        } else {
            p++;
        }
    }
    return space_count;
}

char *Process::spilt(char *p_command) noexcept {
    char *p = p_command;
    int count = 0;
    while (*p != 0) {
        if (*p == '\"') {
            count++;
            p++;
            continue;
        } else if (*p == ' ') {
            if (count % 2 == 0) {
                // 字符串外
                *p = 0;
                p++;
                return p;
            } else {
                // 字符串内
                p++;
            }
        } else {
            p++;
        }
    }
    return nullptr;
}