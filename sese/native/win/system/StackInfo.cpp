#include <sese/system/StackInfo.h>
#include <sese/util/Range.h>

#include <cassert>
#include <dbghelp.h>

using namespace sese;

uint16_t system::StackInfo::offset = 1;

system::StackInfo::StackInfo(uint16_t limit, uint16_t skip) noexcept {
    void **p_stack = static_cast<void **>(malloc(sizeof(void *) * limit));
    auto process = GetCurrentProcess();
    SymInitialize(process, nullptr, TRUE);
    auto frames = CaptureStackBackTrace(skip + offset, limit, p_stack, nullptr);
    for (auto &&i: sese::Range<WORD>(0, frames - 1)) {
        auto address = reinterpret_cast<DWORD64>(p_stack[i]);

        DWORD64 displacement_sym = 0;
        char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)]{};
        auto p_symbol = reinterpret_cast<PSYMBOL_INFO>(buffer);
        p_symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        p_symbol->MaxNameLen = MAX_SYM_NAME;

        BOOL rt = TRUE;
        rt = SymFromAddr(process, address, &displacement_sym, p_symbol);
        if (!rt) {
            stacks.emplace_back(SubStackInfo{});
            continue;
        }

        stacks.emplace_back(SubStackInfo{p_symbol->Address, decodeSymbolName(p_symbol->Name)});
    }

    free(p_stack);
}

std::string system::StackInfo::decodeSymbolName(const std::string &str) noexcept {
    auto pos = str.find_first_of('<');
    if (pos == std::string::npos) {
        return str;
    } else {
        return str.substr(0, pos);
    }
}