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

#include "sese/plugin/Module.h"
#include "gtest/gtest.h"

class Printable : public sese::plugin::BaseClass {
public:
    virtual void run() = 0;
};

TEST(TestPlugin, SUCCESS) {
    auto m = sese::plugin::Module::open(PATH_TO_MODULE);
    ASSERT_NE(m, nullptr);
    printf("module info:\n"
           "\tname: %s\n"
           "\tversion: %s\n"
           "\tdescription: %s\n",
           m->getName(),
           m->getVersion(),
           m->getDescription());

    printf("register class:\n");
    printf("\tname%s & raw name\n", "");
    auto info_map = m->getRegisterClassInfo();
    for (const auto &info: info_map) {
        printf("\t%s %s\n", info.first.c_str(), info.second.info->name());
    }

    printf("execute plugin code:\n");
    auto p1 = m->createClassAs<Printable>("M::Bye");
    auto p2 = m->createClassAs<Printable>("M::Hello");
    auto p3 = m->createClassAs<Printable>("M::Undef");
    p1->run();
    p2->run();
    ASSERT_EQ(p3, nullptr);
}

// The file does not exist
TEST(TestPlugin, Error_0) {
    auto m = sese::plugin::Module::open("NullModule");
    ASSERT_EQ(m, nullptr);
}

// The file exists but does not contain module information
TEST(TestPlugin, Error_1) {
    auto m = sese::plugin::Module::open(PATH_TO_CORE);
    ASSERT_EQ(m, nullptr);
}