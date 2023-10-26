#include <sese/system/StackInfo.h>
#include <sese/util/Range.h>
#include <sese/io/OutputBufferWrapper.h>

#include <execinfo.h>

using namespace sese;

int system::StackInfo::WILL_SKIP = 1;

system::StackInfo::StackInfo(int limit, int skip) noexcept {
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

std::string system::StackInfo::decodeSymbolName(const std::string &str) noexcept {
    char buffer[2000]{};
    sese::io::OutputBufferWrapper output(buffer, sizeof(buffer));

    if (!std::equal(str.begin(), str.begin() + 2, "_Z")) {
        return str;
    }

    int pos = str.c_str()[2] == 'N' ? 3 : 2;

    // _ZN1N1C4funcE -> N::C::func(int)
    bool first = true;
    char *pStr = const_cast<char *>(str.data()) + pos;
    char *pNext = nullptr;
    while (true) {
        auto len = std::strtoll(pStr, &pNext, 10);
        if (len == 0) {
            break;
        } else {
            if (first) {
                first = false;
            } else {
                output.write("::", 2);
            }
            output.write(pNext, len);
            pStr = pNext + len;
        }
    }

    return buffer;
}