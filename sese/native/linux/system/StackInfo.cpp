#include <sese/system/StackInfo.h>
#include <sese/util/Range.h>
#include <sese/io/OutputBufferWrapper.h>

#include <cassert>

#if defined(USE_LIBUNWIND)
#define UNW_LOCAL_ONLY
#include <libunwind.h>
#elif defined(__GLIBC__)
#include <execinfo.h>
#endif

using namespace sese;


#if defined(USE_LIBUNWIND)
uint16_t system::StackInfo::offset = 1;
#elif defined(__GLIBC__)
#if defined(SESE_IS_DEBUG)
uint16_t system::StackInfo::offset = 1;
#else
uint16_t system::StackInfo::offset = 1;
#endif
#endif


system::StackInfo::StackInfo(uint16_t limit, uint16_t skip) noexcept {
#ifdef USE_LIBUNWIND
    char function_name[1024]{};
    unw_context_t ctx;
    unw_cursor_t curs;
    unw_word_t sp, offset;

    unw_getcontext(&ctx);
    unw_init_local(&curs, &ctx);

    int count = 0;
    while (unw_step(&curs) > 0) {
        if (count < skip + StackInfo::offset) {
            count++;
            continue;
        } else if (count > limit + skip + StackInfo::offset) {
            break;
        } else {
            unw_get_reg(&curs, UNW_REG_SP, &sp);
            unw_get_proc_name(&curs, function_name, sizeof(function_name), &offset);
            stacks.emplace_back(SubStackInfo{sp, decodeSymbolName(function_name)});
            count++;
        }
    }
#elif __GLIBC__
    void **array = (void **) malloc(sizeof(void *) * (limit + skip + StackInfo::offset));
    int frames = ::backtrace(array, limit + skip + StackInfo::offset);
    char **strings = ::backtrace_symbols(array, limit + skip + StackInfo::offset);
    for (auto &&i: sese::Range<int>(skip + StackInfo::offset, frames - 1)) {
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
#endif
}