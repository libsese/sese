#include "Test.h"
#include <dlfcn.h>
#include <iomanip>
#include <unwind.h>
#include <sstream>

namespace sese::_android {
    struct BacktraceState {
        void **current;
        void **end;
    };

    static _Unwind_Reason_Code unwindCallback(struct _Unwind_Context *context, void *arg) {
        auto state = static_cast<BacktraceState *>(arg);
        uintptr_t pc = _Unwind_GetIP(context);
        if (pc) {
            if (state->current == state->end) {
                return _URC_END_OF_STACK;
            } else {
                *state->current++ = reinterpret_cast<void *>(pc);
            }
        }
        return _URC_NO_REASON;
    }

    size_t captureBacktrace(void **buffer, size_t max) {
        BacktraceState state = {buffer, buffer + max};
        _Unwind_Backtrace(unwindCallback, &state);

        return state.current - buffer;
    }
}// namespace sese::_android

std::string sese::backtrace2String(int size, const std::string &prefix, int skip) {
    void **array = (void **) malloc(sizeof(void *) * size);
    size_t s = _android::captureBacktrace(array, size);
    std::stringstream stream;

    for(size_t i = 0; i < size; i++){
        const void *addr = array[i];
        const char* symbol = nullptr;

        Dl_info info;
        if(dladdr(addr, &info) && info.dli_sname){
            symbol = info.dli_sname;
        }
        stream << prefix << " #" << std::setw(2) << i << ": " << addr << " " << symbol << "\n";
    }

    free(array);
    return stream.str();
}