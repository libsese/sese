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
 * @file ResponseHeader.h
 * @brief Response Header Class
 * @author kaoru
 * @date May 17, 2022
 */


#pragma once
#include <sese/net/http/Header.h>

namespace sese::net::http {

// GCOVR_EXCL_START

/**
 * @brief Response Header Class
 */
class  ResponseHeader : public Header {
public:
    using Ptr = std::unique_ptr<ResponseHeader>;

    ResponseHeader() = default;

    ResponseHeader(const std::initializer_list<KeyValueType> &initializer_list)
        : Header(initializer_list) {}

    void setCode(uint16_t response_code) noexcept { statusCode = response_code; }
    [[nodiscard]] uint16_t getCode() const noexcept { return statusCode; }

    [[nodiscard]] HttpVersion getVersion() const { return version; }
    void setVersion(HttpVersion new_version) { this->version = new_version; }

protected:
    uint16_t statusCode = 200;
    HttpVersion version = HttpVersion::VERSION_1_1;
};

// GCOVR_EXCL_STOP

} // namespace sese::net::http