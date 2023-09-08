#include <sese/system/StackInfo.h>
#include <sese/text/StringBuilder.h>
#include <sese/util/Range.h>
#include <sese/util/OutputBufferWrapper.h>

#include <execinfo.h>

using namespace sese;

int system::StackInfo::WILL_SKIP = 1;

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

system::StackInfo::StackInfo(int limit, int skip) noexcept {
    void **array = (void **) malloc(sizeof(void *) * limit);
    int frames = ::backtrace(array, limit);
    char **strings = backtrace_symbols(array, limit);
    for (auto &&i: sese::Range<int>(skip, frames - 1)) {
        /* 
         * "1   TestStackInfo                       0x000000010001dcc1 _ZN4sese6system9StackInfoC1Eii + 33"
         *                                         ^                  ^                               ^
         *                                        pos2               pos3                            pos1
         */
        std::string str = strings[i];
        auto pos1 = str.find_last_of('+');
        auto pos2 = findPos2(str.c_str() + pos1, pos1);

        char *end;// pos3
        uint64_t symAddress = std::strtoll(str.c_str() + pos2 + 1, &end, 16);

        auto pos3 = end - str.c_str();
        std::string symName(end + 1, pos1 - pos3 - 2);
        stacks.emplace_back(SubStackInfo{symAddress, decodeSymbolName(symName)});
    }

    free(array);
}

std::string system::StackInfo::decodeSymbolName(const std::string &str) noexcept {
    char buffer[2000]{};
    sese::OutputBufferWrapper output(buffer, sizeof(buffer));

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