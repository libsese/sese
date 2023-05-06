#include "sese/system/Process.h"

#include <csignal>
#include <unistd.h>
#include <sys/wait.h>

using namespace sese::system;

Process::Ptr Process::create(const char *command) noexcept {
    auto pid = fork();
    if (pid > 0) {
        // parent process
        auto proc = new Process;
        proc->id = pid;
        return std::unique_ptr<Process>(proc);
    } else if (pid == 0) {
        // client process
        char pCommand[1024];
        std::strcpy(pCommand, command);
        exec(pCommand);
        exit(0);
    } else {
        // failed to create
        return nullptr;
    }
}

pid_t Process::getCurrentProcessId() noexcept {
    return getpid();
}

int Process::wait() const noexcept {
    int status;
    ::waitpid(id, &status, 0);
    if (!WIFEXITED(status)) {
        return WEXITSTATUS(status);
    } else {
        return 0;
    }
}

void Process::exec(char *pCommand) noexcept {
    auto count = Process::count(pCommand);
    auto args = new char *[count + 2];
    char *p = pCommand;
    char **pArgs = args;
    for (int i = 0; i < count + 1; ++i) {
        auto newPos = spilt(p);
        *pArgs = p;
        pArgs++;
        p = newPos;
    }
    *pArgs = nullptr;

    auto code = execvp(args[0], &args[0]);

    // Never reach
    delete[] args;

    if (code == -1) {
        exit(errno);
    }
}

pid_t Process::getProcessId() const noexcept {
    return id;
}

bool Process::kill() const noexcept {
    return ::kill(id, SIGKILL) == 0;
}

size_t Process::count(const char *pCommand) noexcept {
    const char *p = pCommand;
    size_t count = 0;
    size_t spaceCount = 0;
    while (*p != 0) {
        if (*p == '\"') {
            count++;
            p++;
            continue;
        } else if (*p == ' ') {
            if (count % 2 == 0) {
                spaceCount++;
                p++;
            } else {
                p++;
            }
        } else {
            p++;
        }
    }
    return spaceCount;
}

char *Process::spilt(char *pCommand) noexcept {
    char *p = pCommand;
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