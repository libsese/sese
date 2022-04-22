#include "sese/record/ConsoleAppender.h"
#include "sese/convert/EncodingConverter.h"

#ifdef _WIN32
#pragma warning(disable : 4996)
#endif

namespace sese {

    ConsoleAppender::ConsoleAppender(const AbstractFormatter::Ptr &formatter, Level level) noexcept : AbstractAppender(formatter, level) {
    }

    void ConsoleAppender::dump(const sese::Event::Ptr &event) noexcept {
        setbuf(stdout, nullptr);
#ifdef _WIN32
        _putws(EncodingConverter::toWstring(this->formatter->dump(event)).c_str());
#else
        puts(this->formatter->dump(event).c_str());
#endif
        fflush(stdout);
    }

}// namespace sese
