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

#include "sese/util/Uuid.h"

#ifdef WIN32
#pragma warning(disable : 4146)
#pragma warning(disable : 4244)
#endif

sese::Uuid::Uuid(uint8_t self_id, uint64_t timestamp, uint8_t r) noexcept {
    this->selfId = self_id;
    this->timestamp = timestamp;
    this->r = r;
}

uint8_t sese::Uuid::getSelfId() const noexcept {
    return selfId;
}

void sese::Uuid::setSelfId(uint8_t i) noexcept {
    Uuid::selfId = i;
}

uint8_t sese::Uuid::getR() const noexcept {
    return r;
}

void sese::Uuid::setR(uint8_t r) noexcept {
    Uuid::r = r;
}

uint64_t sese::Uuid::getTimestamp() const noexcept {
    return timestamp;
}

void sese::Uuid::setTimestamp(uint64_t i) noexcept {
    Uuid::timestamp = i;
}

uint64_t sese::Uuid::toNumber() const noexcept {
    return ((selfId & -1ULL) << 56) | ((r & -1ULL) << 48) | (timestamp & 0x00FF'FFFF'FFFF'FFFF);
}

void sese::Uuid::parse(uint64_t number) noexcept {
    selfId = (number & 0xFF00'0000'0000'0000) >> 56;
    r = (number & 0x00FF'0000'0000'0000) >> 48;
    timestamp = number & 0x0000'FFFF'FFFF'FFFF;
}
