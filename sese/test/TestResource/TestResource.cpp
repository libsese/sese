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

#include <sese/res/BundlerResource.h>
#include <gtest/gtest.h>

#include "Resource.h"

TEST(TestResource, Bundler) {
    auto res = sese::res::BundlerResource<Resource>();
    auto txt = res.getBinary(Resource::Binaries::hello);
    EXPECT_NE(nullptr, txt);
    char buffer[1024]{};
    auto l = txt->read(buffer, 1024);
    EXPECT_NE(0, l);
    puts(buffer);
}