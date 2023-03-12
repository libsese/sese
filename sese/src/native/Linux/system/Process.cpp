#include "sese/system/Process.h"

#include <sys/wait.h>

sese::Process::Ptr sese::Process::create(const char *command) noexcept {
    auto pid = fork();
    if (pid > 0) {
        // parent process
        auto proc = new Process;
        proc->id = pid;
        return std::unique_ptr<Process>(proc);
    } else if (pid == 0) {
        // client process
        exec(command);
        exit(0);
    } else {
        // failed to create
        return nullptr;
    }
}

int sese::Process::wait() const noexcept {
    int status;
    ::waitpid(id, &status, 0);
    if (!WIFEXITED(status)) {
        return WEXITSTATUS(status);
    } else {
        return 0;
    }
}

void sese::Process::exec(const char *pCommand) noexcept {
    auto len = strlen(pCommand);
    char *buffer = new char[len + 1];
    memcpy(buffer, pCommand, len);
    buffer[len] = 0;

    auto count = Process::count(pCommand);
    auto args = new char *[count + 2];
    char *p = buffer;
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
    delete[] buffer;

    if (code == -1) {
        exit(errno);
    }
}

size_t sese::Process::count(const char *pCommand) noexcept {
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

char *sese::Process::spilt(char *pCommand) noexcept {
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