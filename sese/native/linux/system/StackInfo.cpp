#include <sese/system/StackInfo.h>
#include <sese/util/Range.h>
#include <sese/io/OutputBufferWrapper.h>

#include <cassert>
#include <execinfo.h>

using namespace sese;

int system::StackInfo::getSkipOffset() {
    return 2;
}

system::StackInfo::StackInfo(int limit, int skip) noexcept {
    assert(limit > skip);
    void **array = (void **) malloc(sizeof(void *) * limit);
    int frames = ::backtrace(array, limit);
    char **strings = ::backtrace_symbols(array, limit);
    for (auto &&i: sese::Range<int>(skip, frames - 1)) {
        /* /tmp/tmp.zC0rrpZiXB/cmake-build-debug-ubuntu/gtest/TestStackInfo(function2+0x3c) [0x5599188f060d]
         *                                                                 ^         ^      ^              ^
         *                                                                pos1      pos2   pos3           pos4
         */
        std::string str = strings[i];
        auto pos1 = str.find_first_of('(', 0);
        auto pos2 = str.find_first_of('+', pos1);
        auto pos3 = str.find_first_of('[', pos2);
        std::string symName = str.substr(pos1 + 1, pos2 - pos1 - 1);
        char *end; // pos4
        uint64_t symAddress = std::strtoll(strings[i] + pos3 + 1, &end, 16);
        stacks.emplace_back(SubStackInfo{symAddress, decodeSymbolName(symName)});
    }

    free(strings);
    free(array);
}