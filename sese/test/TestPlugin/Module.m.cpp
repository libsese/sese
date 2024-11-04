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

#include "sese/plugin/Marco.h"

class Printable : public sese::plugin::BaseClass {
public:
    virtual void run() = 0;
};

namespace M {
class Bye : public Printable {
public:
    void run() override {
        puts("Bye");
    }
};

class Hello : public Printable {
public:
    void run() override {
        puts("Hello");
    }
};
} // namespace M

DEFINE_CLASS_FACTORY(MyFactory, "0.2.0", "The module for test.") {
    REGISTER_CLASS(M::Bye);
    REGISTER_CLASS(M::Hello);
}