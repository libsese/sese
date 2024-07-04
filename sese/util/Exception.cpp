#include <sese/util/Exception.h>
#include <sese/text/StringBuilder.h>

#include <algorithm>

int sese::Exception::getSkipOffset() {
#if defined(_MSC_VER) 
    return 1;
#elif defined(__APPLE__) && !defined(SESE_IS_DEBUG)
    return 1;
#else
    return 2;
#endif
}

sese::Exception::Exception(const char *message)
    : NativeException(message) {
    stackInfo = new system::StackInfo(16, sese::system::StackInfo::getSkipOffset() + getSkipOffset());
}

sese::Exception::~Exception() {
    delete stackInfo; // GCOVR_EXCL_LINE
}

std::string sese::Exception::buildStacktrace() {
    sese::text::StringBuilder builder{4096};
    builder.append("Exception with description: ");
    builder.append(what());
    builder.append("\n");
    std::for_each(stackInfo->rbegin(), stackInfo->rend(), [&builder](const sese::system::SubStackInfo &info) {
        char address[16]{};
        std::snprintf(address, sizeof(address), "0x%" PRIx64, info.address);

        builder.append("\tat ");
        builder.append(info.func);
        builder.append(" (");
        builder.append(address);
        builder.append(")\n");
    });
    return builder.toString();
}

void sese::Exception::printStacktrace() {
    auto content = buildStacktrace();
    fwrite(content.c_str(), 1, content.length(), stdout);
}

void sese::Exception::printStacktrace(sese::record::Logger *logger) {
    auto content = buildStacktrace();
    logger->dump(content.c_str(), content.length());
}

void sese::Exception::printStacktrace(sese::io::OutputStream *output) {
    auto content = buildStacktrace();
    output->write(content.c_str(), content.length());
}
