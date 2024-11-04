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

#pragma once

#include <sese/res/ResourceManager.h>
#include <sese/util/Singleton.h>

#define SESE_DEF_RESOURCE(resClass, resName, resSize, resBuf)\
    class resClass final : public sese::res::Resource {      \
    public:                                                  \
        resClass() : sese::res::Resource(resBuf, resSize) {} \
    }

#define SESE_MAKE_RESOURCE(resName, resBuf, resSize) \
    std::make_shared<sese::res::Resource>(resName, resBuf, resSize)

#define SESE_DEF_RES_STREAM(streamClass, streamSize, streamBuf)             \
    class streamClass final : public sese::res::ResourceStream {            \
    public:                                                                 \
        streamClass() : sese::res::ResourceStream(streamBuf, streamSize) {} \
    };

#define SESE_MAKE_RES_STREAM(streamBuf, streamSize) \
    std::make_unique<sese::res::ResourceStream>(streamBuf, streamSize)

#define SESE_ADD_RES(resName, resSize, resBuf) \
    this->resourceMap[resName] = SESE_MAKE_RESOURCE(resName, resBuf, resSize);

#define SESE_DEF_RES_MANAGER(managerName)                         \
    class managerName final : public sese::res::ResourceManager { \
    public:                                                       \
        managerName() : sese::res::ResourceManager() {

#define SESE_DEF_RES_MANAGER_END(managerName) \
        }                                     \
    };                                        \
    using managerName##Instance = sese::SingletonPtr<managerName>;
