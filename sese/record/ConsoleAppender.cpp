#include "sese/record/ConsoleAppender.h"
#include "sese/convert/EncodingConverter.h"

#include <cstdio>

#ifdef _WIN32
#pragma warning(disable : 4996)
/// 控制台句柄由 Windows 负责自动关闭，程序不管理其生命周期
static HANDLE console_handle = INVALID_HANDLE_VALUE;
#endif

using sese::record::ConsoleAppender;

ConsoleAppender::ConsoleAppender(record::Level level) noexcept : record::AbstractAppender(level) {
#ifdef _WIN32
    if (console_handle == INVALID_HANDLE_VALUE) {
        console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    }
#endif
}

void ConsoleAppender::dump(const char *buffer, size_t size) noexcept {
    //_putws(EncodingConverter::toWstring(buffer).c_str());
    fwrite(buffer, 1, size, stdout);
}

#ifdef _WIN32
void ConsoleAppender::setDebugColor() noexcept {
    SetConsoleTextAttribute(console_handle, 0x02);
}

void ConsoleAppender::setInfoColor() noexcept {
    SetConsoleTextAttribute(console_handle, 0x07);
}

void ConsoleAppender::setWarnColor() noexcept {
    SetConsoleTextAttribute(console_handle, 0x06);
}

void ConsoleAppender::setErrorColor() noexcept {
    SetConsoleTextAttribute(console_handle, 0x04);
}

void ConsoleAppender::setCleanColor() noexcept {
    SetConsoleTextAttribute(console_handle, 0x07);
}
#else
void ConsoleAppender::setDebugColor() noexcept {
    fputs("\033[0;32m", stdout);
}

void ConsoleAppender::setInfoColor() noexcept {
    fputs("\033[0m", stdout);
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