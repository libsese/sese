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

#include <sese/system/ProcessBuilder.h>

sese::system::ProcessBuilder::ProcessBuilder(const std::string &exec) {
    this->exec = exec;
}

sese::system::ProcessBuilder &&sese::system::ProcessBuilder::arg(const std::string &arg) && {
    this->args.emplace_back(arg);
    return std::move(*this);
}

sese::Result<sese::system::Process::Ptr, sese::ErrorCode> sese::system::ProcessBuilder::createEx() {
    return Process::createEx(exec, args);
}
