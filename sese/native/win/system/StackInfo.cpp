#include <sese/system/StackInfo.h>
#include <sese/util/Range.h>

#include <dbghelp.h>

using namespace sese;

int system::StackInfo::getSkipOffset() {
    return 1;
}

system::StackInfo::StackInfo(int limit, int skip) noexcept {
    void **p_stack = (void **) malloc(sizeof(void *) * limit);
    auto process = GetCurrentProcess();
    SymInitialize(process, nullptr, TRUE);
    auto frames = CaptureStackBackTrace(skip, limit, p_stack, nullptr);
    for (auto &&i: sese::Range<WORD>(0, frames - 1)) {
        auto address = (DWORD64) (p_stack[i]);

        DWORD64 displacement_sym = 0;
        char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)]{};
        auto p_symbol = (PSYMBOL_INFO) buffer;
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