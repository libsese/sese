#include "ConsoleAppender.h"

#ifdef _WIN32
#pragma warning(disable : 4996)
#endif

namespace sese {

    void ConsoleAppender::dump(const sese::Event::Ptr &event) noexcept {
        setbuf(stdout, nullptr);
        puts(this->formatter->dump(event).c_str());
        fflush(stdout);
    }

}// namespace sese
