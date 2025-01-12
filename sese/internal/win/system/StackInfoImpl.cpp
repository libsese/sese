// Copyright 2024 libsese
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <sese/system/StackInfo.h>
#include <sese/util/Range.h>

#include <cassert>
#include <dbghelp.h>

using namespace sese;

uint16_t system::StackInfo::offset = 1;

system::StackInfo::StackInfo(uint16_t limit, uint16_t skip) noexcept {
    auto p_stack = static_cast<void **>(malloc(sizeof(void *) * limit));
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
    }
    return str.substr(0, pos);
}