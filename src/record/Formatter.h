#pragma once
#include <string>
#include "Event.h"

#ifdef _WIN32
#pragma warning (disable : 4251)
#endif

namespace sese {

    class Formatter {
    public:
        typedef std::shared_ptr<Formatter> Ptr;

        virtual std::string dump(const Event::Ptr &event) noexcept = 0;
    };
}// namespace sese