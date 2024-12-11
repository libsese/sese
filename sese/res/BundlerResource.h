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

#include <sese/Config.h>
#include "ResourceStream.h"

#if defined (SESE_PLATFORM_WINDOWS)
#include <map>
#elif defined (SESE_PLATFORM_APPLE)
#include <mach-o/getsect.h>
#endif

namespace sese::res {

template<class R>
class BundlerResource {
public:
    using BinaryIds = typename R::Binaries;

    explicit BundlerResource();

    virtual ~BundlerResource();

    ResourceStream::Ptr getBinary(BinaryIds id);

private:
#ifdef SESE_PLATFORM_WINDOWS
    struct RC {
        HRSRC hResInfo;
        LPVOID pResData;
        DWORD dwSize;
    };
    std::map<BinaryIds, RC> binariesMap;

    HINSTANCE hInstance;
#endif
};

#if defined (SESE_PLATFORM_WINDOWS)

template<class R>
BundlerResource<R>::BundlerResource() {
    hInstance = nullptr;
}

template<class R>
BundlerResource<R>::~BundlerResource() {}

template<class R>
ResourceStream::Ptr BundlerResource<R>::getBinary(BinaryIds id) {
    if (auto iter = binariesMap.find(id); iter != binariesMap.end()) {
        return std::make_unique<ResourceStream>(iter->second.pResData, iter->second.dwSize);
    }
    HRSRC hResInfo = FindResource(hInstance, MAKEINTRESOURCE(static_cast<int>(id)), RT_RCDATA);
    assert(hResInfo);
    DWORD dwSize = SizeofResource(hInstance, hResInfo);
    LPVOID pResData = LockResource(hResInfo);
    binariesMap[id] = {hResInfo, pResData, dwSize};
    return std::make_unique<ResourceStream>(static_cast<const char *>(pResData) + 32, dwSize);
}

#elif defined (SESE_PLATFORM_LINUX)

template<class R>
BundlerResource<R>::BundlerResource() {}

template<class R>
BundlerResource<R>::~BundlerResource() {}

template<class R>
ResourceStream::Ptr BundlerResource<R>::getBinary(BinaryIds id) {
    auto res = R::syms[static_cast<int>(id)];
    return std::make_unique<ResourceStream>(res.start, res.size);
}

#elif defined (SESE_PLATFORM_APPLE)

template<class R>
BundlerResource<R>::BundlerResource() {}

template<class R>
BundlerResource<R>::~BundlerResource() {}

template<class R>
ResourceStream::Ptr BundlerResource<R>::getBinary(BinaryIds id) {
    auto name = R::syms[static_cast<int>(id)];
    unsigned long size;
    auto start = getsectdata("__DATA", name, &size);
    return std::make_unique<ResourceStream>(start, size);
}

#endif

} // namespace sese::res