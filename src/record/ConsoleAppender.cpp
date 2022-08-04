#include "sese/record/ConsoleAppender.h"
#include "sese/convert/EncodingConverter.h"
#include "sese/thread/Locker.h"

#ifdef _WIN32
#pragma warning(disable : 4996)
/// 控制台句柄由 Windows 负责自动关闭，程序不管理其生命周期
static HANDLE ConsoleHandle = INVALID_HANDLE_VALUE;
#endif

using sese::ConsoleAppender;

ConsoleAppender::ConsoleAppender(const record::AbstractFormatter::Ptr &formatter, record::Level level) noexcept : record::AbstractAppender(formatter, level) {
#ifdef _WIN32
    if (ConsoleHandle == INVALID_HANDLE_VALUE) {
        ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    }
#endif
}

void ConsoleAppender::dump(const record::Event::Ptr &event) noexcept {
    Locker locker(this->mutex);
    setbuf(stdout, nullptr);
    switch (event->getLevel()) {
        case record::Level::DEBUG:
            setDebugColor();
            break;
        case record::Level::INFO:
            setInfoColor();
            break;
        case record::Level::WARN:
            setWarnColor();
            break;
        case record::Level::ERR:
            setErrorColor();
            break;
    }
#ifdef _WIN32
    _putws(EncodingConverter::toWstring(this->formatter->dump(event)).c_str());
#else
    puts(this->formatter->dump(event).c_str());
#endif
    setCleanColor();
    fflush(stdout);
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