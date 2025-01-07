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
#include <sese/record/Logger.h>

#include <gtest/gtest.h>

class Task1 : public sese::InitiateTask {
public:
    Task1() : InitiateTask(__FUNCTION__) {}

    int32_t init() noexcept override {
        sese::record::Logger::info("loading {}", this->getName().c_str());
        return 0;
    }

    int32_t destroy() noexcept override {
        sese::record::Logger::info("unloading {}", this->getName().c_str());
        return -1;
    }
};

class Task2 : public sese::InitiateTask {
public:
    Task2() : InitiateTask(__FUNCTION__) {}

    int32_t init() noexcept override {
        sese::record::Logger::info("loading {}", this->getName().c_str());
        return -1;
    }

    int32_t destroy() noexcept override {
        sese::record::Logger::info("unloading {}", this->getName().c_str());
        return 0;
    }
};

TEST(TestInit, Init) {
    // sese::Initializer::getInitializer();
    sese::Initializer::addTask<Task1>();
    sese::Initializer::addTask(std::make_unique<Task2>());
}