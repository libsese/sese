#include <sese/system/StackInfo.h>
#include <sese/io/OutputBufferWrapper.h>

std::string sese::system::StackInfo::decodeSymbolName(const std::string &str) noexcept {
    char buffer[2000]{};
    sese::io::OutputBufferWrapper output(buffer, sizeof(buffer));

    if (!std::equal(str.begin(), str.begin() + 2, "_Z")) {
        return str;
    }

    int pos = str.c_str()[2] == 'N' ? 3 : 2;

    // _Z9function1v -> function1
    // _ZN1N1C4funcE -> N::C::func
    // _ZN18NamespaceStackInfo6ClassAC1Ev -> NamespaceStackInfo::ClassA::ClassA
    // _ZN18NamespaceStackInfo6ClassAD1Ev -> NamespaceStackInfo::ClassA::~ClassA
    std::string preword;
    bool first = true;
    char *p_str = const_cast<char *>(str.data()) + pos;
    char *p_next = nullptr;
    while (true) {
        auto len = std::strtoll(p_str, &p_next, 10);
        if (len == 0) {
            if (p_next[0] == 'D') {
                preword = "::~" + preword;
                output.write(preword.data(), preword.length());
                break;
            } else if (p_next[0] == 'C') {
                preword = "::" + preword;
                output.write(preword.data(), preword.length());
                break;
            } else {
                break;
            }
        }
        if (first) {
            first = false;
        } else {
            output.write("::", 2);
        }
        preword = std::string(p_next, len);
        output.write(p_next, len);
        p_str = p_next + len;
    }

    return buffer;
}