#include <sese/util/Exception.h>
#include <sese/text/StringBuilder.h>

#include <algorithm>

#if defined(SESE_PLATFORM_WINDOWS)
int sese::Exception::WILL_SKIP_OFFSET = 1;
#else
int sese::Exception::WILL_SKIP_OFFSET = 2;
#endif

sese::Exception::Exception()
    : NativeException() {
    stackInfo = new system::StackInfo(8, sese::system::StackInfo::WILL_SKIP + WILL_SKIP_OFFSET);
}

sese::Exception::Exception(const char * message)
    : NativeException(message) {
    stackInfo = new system::StackInfo(8, sese::system::StackInfo::WILL_SKIP + WILL_SKIP_OFFSET);
}

sese::Exception::~Exception() {
    delete stackInfo;
}

std::string sese::Exception::buildStacktrace() {
    sese::text::StringBuilder builder{4096};
    builder.append("Exception with description: ");
    builder.append(what());
    builder.append("\n");
    std::for_each(stackInfo->rbegin(), stackInfo->rend(), [&builder](const sese::system::SubStackInfo &info) {
        char address[16]{};
        std::snprintf(address, sizeof(address), "0x%llX", info.address);

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
