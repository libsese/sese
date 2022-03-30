#include "Test.h"
#include <sstream>
#include <Windows.h>
#ifdef NEED_DBGHELP
#include <DbgHelp.h>
#endif

std::string sese::backtrace2String(int size, int skip, const std::string &prefix) {
#ifdef NEED_DBGHELP
    void **pStack = (void **) malloc(sizeof(void *) * size);

    HANDLE process = GetCurrentProcess();
    SymInitialize(process, nullptr, TRUE);
    WORD frames = CaptureStackBackTrace(skip, size, pStack, NULL);

    std::ostringstream oss;
    for (WORD i = 0; i < frames; ++i) {
        DWORD64 address = (DWORD64) (pStack[i]);

        DWORD64 displacementSym = 0;
        char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
        PSYMBOL_INFO pSymbol = (PSYMBOL_INFO) buffer;
        pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        pSymbol->MaxNameLen = MAX_SYM_NAME;

        DWORD displacementLine = 0;
        IMAGEHLP_LINE64 line;
        //SymSetOptions(SYMOPT_LOAD_LINES);
        line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

        oss << prefix;
        if (SymFromAddr(process, address, &displacementSym, pSymbol) && SymGetLineFromAddr64(process, address, &displacementLine, &line)) {
            oss << " " << pSymbol->Name << " at " << line.FileName << ":" << line.LineNumber << "(0x" << std::hex << pSymbol->Address << std::dec << ")" << std::endl;
        } else {
            oss << " error: " << GetLastError() << std::endl;
        }
    }
    free(pStack);
    return oss.str();
#else
    return "Please set \"NEED_DBGHELP\" to 1 to enable this function";
#endif
}