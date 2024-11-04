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

#include <sese/system/CommandLine.h>

using namespace sese::system;

static int g_argc = 0;
static const char *const *g_argv = nullptr;

CommandLineInitiateTask::CommandLineInitiateTask(const int ARGC, const char *const *argv) : InitiateTask(__FUNCTION__) {
    g_argc = ARGC;
    g_argv = argv;
}

int32_t CommandLineInitiateTask::init() noexcept {
    return 0;
}
int32_t CommandLineInitiateTask::destroy() noexcept {
    return 0;
}

int CommandLine::getArgc() {
    return g_argc;
}

const char *const *CommandLine::getArgv() {
    return g_argv;
}
