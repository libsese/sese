#include <sese/system/StackInfo.h>
#include <sese/util/Range.h>

#include <dbghelp.h>

using namespace sese;

int system::StackInfo::WILL_SKIP = 1;

system::StackInfo::StackInfo(int limit, int skip) noexcept {
    void **pStack = (void **) malloc(sizeof(void *) * limit);
    auto process = GetCurrentProcess();
    SymInitialize(process, nullptr, TRUE);
    auto frames = CaptureStackBackTrace(skip, limit, pStack, nullptr);
    for (auto &&i: sese::Range<WORD>(0, frames - 1)) {
        auto address = (DWORD64) (pStack[i]);

        DWORD64 displacementSym = 0;
        char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)]{};
        auto pSymbol = (PSYMBOL_INFO) buffer;
        pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        pSymbol->MaxNameLen = MAX_SYM_NAME;

        BOOL rt = TRUE;
        rt = SymFromAddr(process, address, &displacementSym, pSymbol);
        if (!rt) {
            stacks.emplace_back(SubStackInfo{});
            continue;
        }

        stacks.emplace_back(SubStackInfo{pSymbol->Address, decodeSymbolName(pSymbol->Name)});
    }

    free(pStack);
}

std::string system::StackInfo::decodeSymbolName(const std::string &str) noexcept {
    auto pos = str.find_first_of("<");
    if (pos == std::string::npos) {
        return str;
    } else {
        return str.substr(0, pos);
    }
}