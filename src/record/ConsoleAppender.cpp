#include "sese/record/ConsoleAppender.h"
#include "sese/convert/EncodingConverter.h"
#include "sese/thread/Locker.h"

#ifdef _WIN32
#pragma warning(disable : 4996)
/// 控制台句柄由 Windows 负责自动关闭，程序不管理其生命周期
static HANDLE ConsoleHandle = INVALID_HANDLE_VALUE;
#endif

using sese::record::ConsoleAppender;

ConsoleAppender::ConsoleAppender(record::Level level) noexcept : record::AbstractAppender(level) {
#ifdef _WIN32
    if (ConsoleHandle == INVALID_HANDLE_VALUE) {
        ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    }
#endif
}

void ConsoleAppender::dump(const char *buffer, size_t size) noexcept {
#ifdef _WIN32
    _putws(EncodingConverter::toWstring(buffer).c_str());
#else
    puts(this->formatter->dump(event).c_str());
#endif
}

#ifdef _WIN32
void ConsoleAppender::setDebugColor() noexcept {
    SetConsoleTextAttribute(ConsoleHandle ,0x02);
}

void ConsoleAppender::setInfoColor() noexcept {
    SetConsoleTextAttribute(ConsoleHandle, 0x07);
}

void ConsoleAppender::setWarnColor() noexcept {
    SetConsoleTextAttribute(ConsoleHandle, 0x06);
}

void ConsoleAppender::setErrorColor() noexcept {
    SetConsoleTextAttribute(ConsoleHandle, 0x04);
}

void ConsoleAppender::setCleanColor() noexcept {
    SetConsoleTextAttribute(ConsoleHandle, 0x07);
}
#else
void ConsoleAppender::setDebugColor() noexcept {
    fputs("\033[0;32m", stdout);
}

void ConsoleAppender::setInfoColor() noexcept {
    fputs("\033[0;37m", stdout);
}

void ConsoleAppender::setWarnColor() noexcept {
    fputs("\033[0;33m", stdout);
}

void ConsoleAppender::setErrorColor() noexcept {
    fputs("\033[0;31m", stdout);
}

void ConsoleAppender::setCleanColor() noexcept {
    fputs("\033[0m", stdout);
}
#endif