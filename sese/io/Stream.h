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

/**
 * @file Stream.h
 * @date November 14, 2022
 * @author kaoru
 * @brief Stream interface class
 * @version 0.2
 */

#pragma once

#include "sese/io/InputStream.h"
#include "sese/io/OutputStream.h"

namespace sese::io {

/**
 * @brief Stream interface class
 */
class Stream : public InputStream, public OutputStream {
public:
    typedef std::shared_ptr<Stream> Ptr;
};

} // namespace sese::io