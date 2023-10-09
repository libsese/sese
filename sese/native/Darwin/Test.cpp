#include "sese/util/Test.h"
#include <execinfo.h>
#include <sstream>
#include <vector>

namespace sese {
namespace _darwin {
    void backtrace(std::vector<std::string> &bt, int size, int skip) {
        void **array = (void **) malloc(sizeof(void *) * size);
        size_t s = ::backtrace(array, size);

        char **strings = backtrace_symbols(array, s);
        if (strings == nullptr) {
            return;
        }

        for (int i = skip; i < s; i++) {
            bt.emplace_back(strings[i]);
        }

        free(strings);
        free(array);
    }
} // namespace _darwin

std::string Test::backtrace2String(int size, const std::string &prefix, int skip) {
    std::vector<std::string> bt;
    _darwin::backtrace(bt, size, skip);
    std::stringstream stream;
    for (auto &i: bt) {
        stream << prefix << i << std::endl;
    }
    return stream.str();
}
} // namespace sese