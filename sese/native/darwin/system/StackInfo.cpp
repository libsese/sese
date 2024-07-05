#include <sese/system/StackInfo.h>
#include <sese/text/StringBuilder.h>
#include <sese/util/Range.h>
#include <sese/io/OutputBufferWrapper.h>

#include <cassert>
#include <execinfo.h>

using namespace sese;


#ifdef SESE_IS_DEBUG
uint16_t system::StackInfo::offset = 2;
#else
uint16_t system::StackInfo::offset = 1;
#endif


inline size_t findPos2(const char *buffer, size_t pos) {
    size_t count = 0;
    for (size_t i = 0; i < pos; ++i) {
        if (buffer[-i] == ' ') {
            count += 1;
        }
        if (count == 3) {
            return pos - i;
        }
    }
    return -1;
}

system::StackInfo::StackInfo(uint16_t limit, uint16_t skip) noexcept {
    void **array = static_cast<void **>(malloc(sizeof(void *) * (limit + skip + offset)));
    int frames = ::backtrace(array, limit + skip + offset);
    char **strings = backtrace_symbols(array, limit + skip + offset);
    for (auto &&i: sese::Range<int>(skip + offset, frames - 1)) {
        /*
         * "1   TestStackInfo                       0x000000010001dcc1 _ZN4sese6system9StackInfoC1Eii + 33"
         *                                         ^                  ^                               ^
         *                                        pos2               pos3                            pos1
         */
        std::string str = strings[i];
        auto pos1 = str.find_last_of('+');
        auto pos2 = findPos2(str.c_str() + pos1, pos1);

        char *end; // pos3
        uint64_t symAddress = std::strtoll(str.c_str() + pos2 + 1, &end, 16);

        auto pos3 = end - str.c_str();
        std::string symName(end + 1, pos1 - pos3 - 2);
        stacks.emplace_back(SubStackInfo{symAddress, decodeSymbolName(symName)});
    }

    free(strings);
    free(array);
}
