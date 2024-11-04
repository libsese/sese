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

#include <sese/res/Resource.h>
#include <sese/res/Marco.h>

// GCOVR_EXCL_START

sese::res::Resource::Resource(std::string name, const void *buf, size_t size)
    : name(std::move(name)), buf(buf), size(size) {}

// GCOVR_EXCL_STOP

sese::res::ResourceStream::Ptr sese::res::Resource::getStream() {
    return SESE_MAKE_RES_STREAM(buf, size);
}