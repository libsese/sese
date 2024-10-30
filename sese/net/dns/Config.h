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

#include <cstdint>

namespace sese::net::dns {

static constexpr uint16_t TYPE_A = 1;
static constexpr uint16_t TYPE_NS = 2;
static constexpr uint16_t TYPE_CNAME = 5;
static constexpr uint16_t TYPE_SOA = 6;
static constexpr uint16_t TYPE_NULL = 10;
static constexpr uint16_t TYPE_PTR = 12;
static constexpr uint16_t TYPE_HINFO = 13;
static constexpr uint16_t TYPE_MINFO = 14;
static constexpr uint16_t TYPE_MX = 15;
static constexpr uint16_t TYPE_TXT = 16;
static constexpr uint16_t TYPE_RP = 17;
static constexpr uint16_t TYPE_AFSDB = 18;
static constexpr uint16_t TYPE_X25 = 19;
static constexpr uint16_t TYPE_ISDN = 20;
static constexpr uint16_t TYPE_RT = 21;
static constexpr uint16_t TYPE_AAAA = 28;
static constexpr uint16_t TYPE_SRV = 33;
static constexpr uint16_t TYPE_NAPTR = 35;
static constexpr uint16_t TYPE_CAA = 257;

static constexpr uint16_t CLASS_IN = 1;
static constexpr uint16_t CLASS_CS = 2;
static constexpr uint16_t CLASS_CH = 3;
static constexpr uint16_t CLASS_HS = 4;

}