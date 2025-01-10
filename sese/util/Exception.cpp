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

#include <sese/util/Exception.h>
#include <sese/text/StringBuilder.h>

#include <algorithm>
#include <string>
#include <utility>


#if defined(_MSC_VER) && !defined(SESE_IS_DEBUG)
uint16_t sese::Exception::offset = 1;
#elif defined(__APPLE__) && !defined(SESE_IS_DEBUG)
uint16_t sese::Exception::offset = 1;
#else
uint16_t sese::Exception::offset = 2;
#endif


sese::Exception::Exception(std::string message)
    : NativeException("") {
    message_ = new std::string(std::move(message));
    stackInfo = new system::StackInfo(16, offset);
}

sese::Exception::~Exception() {
    delete stackInfo; // GCOVR_EXCL_LINE
    delete message_;  // GCOVR_EXCL_LINE
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

void sese::Exception::printStacktrace(sese::log::Logger *logger) {
    auto content = buildStacktrace();
    logger->dump(content.c_str(), content.length());
}

void sese::Exception::printStacktrace(sese::io::OutputStream *output) {
    auto content = buildStacktrace();
    output->write(content.c_str(), content.length());
}

const char *sese::Exception::what() const noexcept {
    return message_->c_str();
}

std::string sese::Exception::message() const noexcept {
    return *message_;
}
