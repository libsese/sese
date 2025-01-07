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

#include <sese/record/Marco.h>
#include <sese/util/Exception.h>
#include <sese/io/ConsoleOutputStream.h>

#include <gtest/gtest.h>

void event() {
    throw sese::Exception("my exception");
}

void handle() {
    event();
}

TEST(TestException, PrintStacktrace) {
    EXPECT_THROW(
            try {
                handle();
            } catch (sese::Exception &e) {
                e.printStacktrace();
                throw;
            } catch (...) {
                throw;
            },
            sese::Exception
    );

    EXPECT_THROW(
            try {
                handle();
            } catch (sese::Exception &e) {
                sese::io::ConsoleOutputStream console;
                e.printStacktrace(&console);
                throw;
            },
            sese::Exception
    );

    EXPECT_THROW(
            try {
                handle();
            } catch (sese::Exception &e) {
                SESE_EXCEPT(e);
                throw;
            },
            sese::Exception
    );
}