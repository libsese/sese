#pragma once

#include <sese/Config.h>

#include <vector>

#if defined(_MSC_VER)
#pragma warning(disable : 4251)
#endif

namespace sese::system {

    struct SubStackInfo {
        uint64_t address{};
        std::string func{};
        uint64_t line{};
        std::string file{};
    };

    class API StackInfo {
    public:
        explicit StackInfo(int limit, int skip = WILL_SKIP) noexcept;

        SESE_STD_WRAPPER(begin, stacks)
        SESE_STD_WRAPPER(end, stacks)
        SESE_STD_WRAPPER(empty, stacks)
        SESE_STD_WRAPPER(size, stacks)

        static int WILL_SKIP;

    protected:
#if defined(SESE_PLATFORM_LINUX)
        static std::string decodeSymbolName(const std::string &str) noexcept;
#endif

        std::vector<SubStackInfo> stacks;
    };

}// namespace sese::system
