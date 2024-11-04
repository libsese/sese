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

#include <sese/util/Initializer.h>
#include <sese/system/Environment.h>

#include <gtest/gtest.h>

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    sese::initCore(argc, argv);
    sese::sys::Environment::setEnv("LANG", "en_US.UTF-8");
    sese::sys::Environment::setEnv("LC_ALL", "en_US.UTF-8");
#if defined(__linux__) || defined(__APPLE__)
    signal(SIGPIPE, SIG_IGN);
#endif
    return RUN_ALL_TESTS();
}